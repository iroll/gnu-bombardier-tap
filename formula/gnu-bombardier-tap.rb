class GnuBombardier < Formula
  desc "The GNU Bombing utility"
  homepage "https://salsa.debian.org/debian/bombardier"
  url "https://salsa.debian.org/debian/bombardier.git",
      tag:      "debian/0.8.3+nmu4",
      using:    :git
  version "0.8.3+nmu4"
  license "GPL-3.0-or-later"

end

__END__
diff --git a/hof.c b/hof.c
index deadbeef..cafebabe 100644
--- a/hof.c
+++ b/hof.c

@@ -100,6 +100,12 @@ void hof(struct struc_state *state)
     unsigned char nowres=0;

+/*Debian and macOS have different /var/ permissions
+  so this patch adds a macOS-safe save location*/
+#ifdef __APPLE__
+    const char *home = getenv("HOME");
+    char fullpath[4096];
+    snprintf(fullpath, sizeof(fullpath), "%s/.bombardier/bdscore", home ? home : ".");
+    const char *hofpath = fullpath;
+#else
+    const char *hofpath = "/var/games/bombardier/bdscore";
+#endif
-
-    fd=open("/var/games/bombardier/bdscore", O_RDWR);
+    fd = open(hofpath, O_RDWR);
     if (fd<0)
     {
-        fd=open("/var/games/bombardier/bdscore", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
+        fd = open(hofpath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
         if (fd>0)
         {
             defhof(fd);
             close(fd);
         }
     }
-    fd=open("/var/games/bombardier/bdscore", O_RDWR);
+    fd = open(hofpath, O_RDWR);

@@ void defhof(int fd)
{
+/*Debian and others still gracefully handle ISO-8859-2; macOS does not.
+  Unfortunately, trying to make hof.c UTF-8 safe breaks everything,
+  so a fallback to ASCII is provided for macOS*/
+#ifdef __APPLE__
+    write(fd, "Teller Ede          |1908-01-15|  16384\\n", 40);
+    write(fd, "Szilard Leo         |1898-02-11|   8192\\n", 40);
+    write(fd, "Neumann Janos       |1903-12-28|   4096\\n", 40);
+    write(fd, "Gabor Denes         |1900-06-05|   2048\\n", 40);
+    write(fd, "Bolyai Janos        |1802-12-15|   1024\\n", 40);
+    write(fd, "Eotvos Lorand       |1848-07-27|    512\\n", 40);
+    write(fd, "Horthy Miklos       |1800-00-00|    256\\n", 40);
+    write(fd, "Kadar Janos         |1800-00-00|    128\\n", 40);
+    write(fd, "Rakosi Matyas       |1892-00-00|     64\\n", 40);
+#else
    write(fd, "Teller Ede          |1908-01-15|  16384\n", 40);
    write(fd, "Szilárd Leó         |1898-02-11|   8192\n", 40);
    write(fd, "Neumann János       |1903-12-28|   4096\n", 40);
    write(fd, "Gábor Dénes         |1900-06-05|   2048\n", 40);
    write(fd, "Bolyai János        |1802-12-15|   1024\n", 40);
    write(fd, "Eötvös Loránd       |1848-07-27|    512\n", 40);
    write(fd, "Horthy Miklós       |1800-00-00|    256\n", 40);
    write(fd, "Kádár János         |1800-00-00|    128\n", 40);
    write(fd, "Rákosi Mátyás       |1892-00-00|     64\n", 40);
+#endif
}

