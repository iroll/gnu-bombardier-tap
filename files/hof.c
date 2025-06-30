/* Bombardier */
/* The GNU Bombing utility */
/* Copyright (C) 2001, 2009 Gergely Risko */
/* Can be licensed under the terms of GPL v3 or above */

#include "bombardier.h"
#include "fdgetline.c"
#include "date.h"
#include <string.h>
#include <unistd.h>


# define MAX_NAME_DISPLAY 20 - switches byte counting in old versions to character counting

void truncate_utf8(char *dst, const char *src, int max_chars) {
    int i = 0, j = 0, chars = 0;
    while (src[i] && chars < max_chars) {
        unsigned char c = src[i];
        int len = (c & 0x80) == 0 ? 1 :
                  (c & 0xE0) == 0xC0 ? 2 :
                  (c & 0xF0) == 0xE0 ? 3 : 4;
        if (!src[i + len - 1]) break;
        for (int k = 0; k < len; k++) dst[j++] = src[i++];
        chars++;
    }
    // Pad with spaces if shorter than max_chars visually
    while (chars++ < max_chars) dst[j++] = ' ';
    dst[j] = '\0';
}

char * printhof(char names[9][21], char dates[9][11], int scores[9], unsigned char numoffame, unsigned char nowres, int score)
{
    unsigned char i;
    WINDOW *act;
    static char *name;

    name=NULL;
    mvprintw(gy(0),gx(0),"Your score is: %7d\n", score);
    mvprintw(gy(5),gx(15),"/----------------- HALL OF FAME -------------------\\");
    for (i=0;i<numoffame;i++) {
        char namebuf[64];
        truncate_utf8(namebuf, names[i], 20);
        mvprintw(gy(i+6),gx(15),"| %d. | %-20s | %10s | %7d |", i+1, namebuf, dates[i], scores[i]);
    }
    fillspace();
    if (nowres)
    {
        name=malloc(256);
        act=derwin(stdscr, 1, 40, gy(nowres+5), gx(22));
        wclrtoeol(act);
        fillspace();
        refresh();
        echo();
        curs_set(1);
        wgetnstr(act, name, 255);
        curs_set(0);
        noecho();
        delwin(act);
    }
    if ((!name) || (strlen(name)==0))
        return "Someone";
    return name;
}

    /* HOF initialized in UTF-8 instead of ISO-8859-2 */
void defhof(int fd)
{
    dprintf(fd, "Teller Ede          |1908-01-15|  16384\n");
    dprintf(fd, "Szil\303\241rd Le\303\263         |1898-02-11|   8192\n");
    dprintf(fd, "Neumann J\303\241nos       |1903-12-28|   4096\n");
    dprintf(fd, "G\303\241bor D\303\251nes         |1900-06-05|   2048\n");
    dprintf(fd, "Bolyai J\303\241nos        |1802-12-15|   1024\n");
    dprintf(fd, "E\303\266tv\303\266s Lor\303\241nd       |1848-07-27|    512\n");
    dprintf(fd, "Horthy Mikl\303\263s       |1800-00-00|    256\n");
    dprintf(fd, "K\303\241d\303\241r J\303\241nos         |1800-00-00|    128\n");
    dprintf(fd, "R\303\241kosi M\303\241ty\303\241s       |1892-00-00|     64\n");
}

void hof(struct struc_state *state)
{
    /* HOF defaults to Debian but safe for macOS file convention */
#ifdef __APPLE__
    char path[4096];
    char dirpath[4096];
    snprintf(path, sizeof(path), "%s/.bombardier/bdscore", getenv("HOME"));
    snprintf(dirpath, sizeof(dirpath), "%s/.bombardier", getenv("HOME"));
    mkdir(dirpath, 0755);
#else
    const char *path = "/var/games/bombardier/bdscore";
#endif
    int fd;
    struct flock lock;
    char names[9][21], dates[9][11];
    int scores[9];
    char *line;
    unsigned char numoffame=0;
    unsigned char shift=0;
    unsigned char nowres=0;

    fd=open(path, O_RDWR);
    if (fd<0)
    {
        fd=open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        if (fd>0)
        {
            defhof(fd);
            close(fd);
        }
    }
    fd=open(path, O_RDWR);
    if (fd>0)
    {
        refresh();
        lock.l_type=F_WRLCK;
        lock.l_whence=SEEK_SET;
        lock.l_start=0;
        lock.l_len=0;
        lock.l_pid=getpid();
        if (fcntl(fd, F_SETLKW, &lock)!=-1)
        {
            unsigned char i;

            refresh();
            line=fdgetline(fd);
            while((line) && (numoffame+shift<9))
            {
                gerase();
                strncpy(names[numoffame+shift], line, 20);
                strncpy(dates[numoffame+shift], line+21, 10);
                names[numoffame+shift][20]=0;
                dates[numoffame+shift][10]=0;
                sscanf(line+32, "%d", &scores[numoffame+shift]);
                if ((!shift) && (scores[numoffame]<state->score))
                {
                    if (numoffame<8)
                    {
                        strcpy(names[numoffame+1],names[numoffame]);
                        strcpy(dates[numoffame+1],dates[numoffame]);
                        scores[numoffame+1]=scores[numoffame];
                        shift=1;
                    }
                    scores[numoffame]=state->score;
                    strcpy(dates[numoffame],ascdate());
                    strcpy(names[numoffame],"Someone");
                    nowres=numoffame+1;
                }
                numoffame++;
                free(line);
                line=fdgetline(fd);
            }
            numoffame+=shift;
            if ((!shift) && (numoffame<9))
            {
                scores[numoffame]=state->score;
                strcpy(dates[numoffame],ascdate());
                strcpy(names[numoffame],"Someone");
                nowres=++numoffame;
            }
            if (nowres)
            {
                /* Write out with someone instead the real name which
                   will be given by printhof */
                lseek(fd, 0, SEEK_SET);
                for (i=0;i<numoffame;i++)
                {
                    char linewrite[40];

                    sprintf(linewrite, "%-20s|%10s|%7d", names[i], dates[i], scores[i]);
                    linewrite[39]=10;
                    write(fd, linewrite, 40);
                }
                lseek(fd, 0, SEEK_SET);
                fsync(fd); /* Because user can tricky, and he can kill his process */
                strcpy(names[nowres-1],printhof(names, dates, scores, numoffame, nowres, state->score));
                /* Write out with the real name */
                for (i=0;i<numoffame;i++)
                {
                    char linewrite[40];

                    sprintf(linewrite, "%-20s|%10s|%7d", names[i], dates[i], scores[i]);
                    linewrite[39]=10;
                    write(fd, linewrite, 40);
                }
                lock.l_type=F_UNLCK;
                fcntl(fd, F_SETLKW, &lock);
                close(fd);
            }
            else
            {
                lock.l_type=F_UNLCK;
                fcntl(fd, F_SETLKW, &lock);
                close(fd);
                printhof(names, dates, scores, numoffame, nowres, state->score);
            }
        }
        else
        {
            close(fd);
        }
    }
}

