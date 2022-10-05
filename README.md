Installation of PiKdev 1.4.x
============================

PiKdev V1.4.x is only provided as source code, because I don't have enough time
to package executable versions. However, building and installing the program is
really simple. You will probably have to install pk2cmd, because modern
computers only offer USB interfaces.

Needed software:

    - The gcc C++ compiler (g++)
    - Qt 5.x development tools (available with all Linux distributions). A
    simple way to get them is to install the package Qt Creator.
    - Nothing else

Build and installation procedure for pikdev-1.4.2:

    1. Unpack the archive (tar xzvf pikdev-1.4.2.tar.gz)
    2. Go to the src sub directory (cd src/)
    3. Generate the Makefile (qmake -o Makefile pikdev142.pro)
    4. Build the application (make)
    5. Become root (su root)
    6. Install in /usr/bin (make install)

Important: The name of the executable is pikdev-1.4.2. The installation phase
is not mandatory : you can execute pikdev from anywhere without being
installed, so you don't need the root privilege. PiKdev 1.4.2 is
self-contained: you don't need to copy cryptic files to cryptic location to get
it working. Because PiKdev is now a pure Qt5 application, it uses a
configuration file ~/.config/GoldenBugSoftware/pikdev.conf


Build and installation procedure for pkp-1.4.0:
===============================================

pkp may be built from a separate project file, which can be found in the same
source directory than pikdev.

    1. Unpack the archive (tar xzvf pikdev-1.4.2.tar.gz) if this is not already
    done
    2. Go to the src sub directory (cd src/)
    3. Generate the Makefile (qmake -o Makefile pkp.pro)
    4. Build the application (make)
    5. Become root (su root)
    6. Install in /usr/bin (make install)

Important: The name of the executable is pkp-1.4.2.


Installation of Microchip's pk2cmd tool for PicKit2
===================================================

Many people have trouble to get a working pk2cmd. Here are my advices to
install pk2cmd properly.

    1. Verify that the libusb and libusb-dev are installed in your system (the
    actual name of the needed packages may vary from one distro to another)
    2. Get pk2cmd V1.20.0 (or higher) source code from Microchip's website, or
    from my download page
    3. Unpack the archive
    4. Enter the created directory
    5. Build the application with make linux (no ./configure needed .. NICE ! )
    6. Become root with su
    7. Install with make linux install (please do not try to bypass this step)
    8. Return to user mode with exit
    9. Make sure that your PATH environment variable contains /usr/share/pk2.
    If you use bash, you must add the following at the end of the .bashrc AND
    .bash_profile files :

    export PATH=$PATH:/usr/share/pk2

    10. Before trying to use your pickit2 programmer from pikdev, try to use it
    from command line interpreter:

    pk2cmd -P

    Programmer's LED should quickly flash, and pk2cmd should issue a
    Auto-Detect: No known part found. message. If this is not the case, the
    pikdev's picKit2 support will not work.

