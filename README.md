#  Cursorial Change Directory

A tool to expand the utility of cmd's built in `cd`:

![Screen Shot](binaries/example_run.png)

Usage: `ccd "partial directory name"`

    `..` = Go up one level (`...` for two levels, etc)
    `-c` = Create ccd helper batch files for `..`, `...`, etc
    `-f` = Don't use smart directory matching
    `-n` = Don't change folder, just show matches
    `-r` = Force a rescan of all directories
    `-s` = Show stats about the AllDirs.db file
    `-u` = Only consider directories under this one

Data files:

    AllDirs.db       = Database of all dirs, created during a "-r" scan
    IgnoreDirs.txt   = Directories to ignore during a "-r" scan
    ShortcutDirs.txt = Each line contains a shortcut of the
                        format "shortcut=fullpath"

Data directory:

The first directory found of the following list will be used to
store data files for the current drive:

    "C:\Users\Scott\AppData\Local\CCD\C"
    "C:\CCD"
    "C:\"

Any characters pressed during the display of potential directories
will be used to filter the list.

Build 38, created by Scott Seligman <scott@scottandmichelle.net>
