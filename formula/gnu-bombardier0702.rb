class GnuBombardier < Formula
  desc "The GNU Bombing utility"
  homepage "https://salsa.debian.org/debian/bombardier"
  url "https://salsa.debian.org/debian/bombardier.git",
      tag:      "debian/0.8.3+nmu4",
      using:    :git
  version "0.8.3+nmu4"
  license "GPL-3.0-or-later"

  def install
    # Replace the Latin-2 hof.c with a clean UTF-8 version
    # Move HOF location to match macOS file convention
    rm "hof.c"
    system "curl", "-o", "hof.c", "https://raw.githubusercontent.com/iroll/homebrew-gnu-bombardier-tap/main/files/hof.c"
    # Build and install
    system "make"
    bin.install "bombardier" => "gnu-bombardier"
  end
end
