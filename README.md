# <img src="./resources/icons/sc-apps-ghostwriter.svg" align="left" width="32" style="padding-right:5px"> ghostwriter

*ghostwriter* is a Windows and Linux text editor for Markdown, which is a plain text markup format created by John Gruber. For more information about Markdown, please visit John Gruber’s website at <http://www.daringfireball.net>.  *ghostwriter* provides a relaxing, distraction-free writing environment, whether your masterpiece be that next blog post, your school paper, or your NaNoWriMo novel.  For a tour of its features, please visit the [*ghostwriter* project site](https://ghostwriter.kde.org).

## Screenshots

You can view screenshots of the application at [*ghostwriter's* project site](https://ghostwriter.kde.org).

## Documentation

A quick reference guide is available [here](https://ghostwriter.kde.org/documentation/).

## Installation

### Windows

An installer will be forthcoming at the [KDE binary factory](https://binary-factory.kde.org/), along with a nightly build.

### Linux

Versions of *ghostwriter* 2.2.0 and above are provided with KDE Gears releases and should be available with your Linux distribution.  For example, on Ubuntu, you can enter the following commands from your terminal:

    $ sudo apt update
    $ sudo apt install ghostwriter

On Fedora, enter the following commands instead:

    $ sudo dnf install ghostwriter

You may also find packages on the author's personal repository locations version 2.1.6 in case your GNU/Linux distribution is behind.  If you are running Ubuntu or one of its derivatives (Linux Mint, Xubuntu, etc.), open a terminal, and enter the following commands:

    $ sudo add-apt-repository ppa:wereturtle/ppa
    $ sudo apt update
    $ sudo apt install ghostwriter

Fedora users can install older version of *ghostwriter* from [Copr](https://copr.fedorainfracloud.org/) by opening a terminal and entering the following commands:

    $ sudo dnf copr enable wereturtle/stable
    $ sudo dnf install ghostwriter

Finally, you may follow the build instructions below to install on Linux with the latest source code.

### MacOS

An installer is planned in the future and will be hosted at the [KDE binary factory](https://binary-factory.kde.org/), along with a nightly build.  If you have any expertise to offer, please consider helping with a [Craft configuration](https://community.kde.org/Craft).

## Build

If you wish to build from the source code, you will need Qt 5, available from <http://www.qt.io/> if you are on Windows, or in your Linux distribution's repository. If you are on MacOS you will need the latest Qt 5 from brew.

This documentation assumes you already have the source code unzipped in a folder.

### Windows

Building on Windows requires Visual Studio.  General instructions for building KDE applications in Windows are available [here](https://community.kde.org/Get_Involved/development/Windows).

**IMPORTANT**: If compiling against Qt 6, note that having OpenGL components (in this case, QWebEngineView) will force the entire window to be rendered in OpenGL.  This triggers a bug in Windows in full screen mode where menus can no longer be displayed, such as the menu bar menus or popup menus.

This issue was not present in Qt 5, since ANGLE was available to bypass the default OpenGL implementation and use DirectX.  With ANGLE having been removed from Qt 6 and the documented solutions not entirely working, you will have to use software rendering instead if you wish to work in full screen mode.  Please see the section below for command line arguments that will disable GPU acceleration.

Obviously, the best option is to continue using Qt 5 on Windows for as long as possible.

### Linux

Before proceeding, ensure that you have the necessary packages installed for Qt 5 and KDE Frameworks.

For Debian or Ubuntu distributions:

    $ sudo apt install g++ qtbase5-dev libqt5svg5-dev qtmultimedia5-dev qtwebengine5-dev pkg-config libqt5concurrent5 qttools5-dev-tools qttools5-dev libkf5coreaddons-dev libkf5xmlgui-dev libkf5configwidgets-dev libkf5sonnet-dev libkf5doctools5 libkf5doctools-dev cmake extra-cmake-modules libhunspell-dev

For Fedora:

    $ sudo dnf install qt-devel qt5-qtbase-devel qt5-qtsvg-devel qt5-qtmultimedia-devel qt5-qtwebengine-devel qt5-linguist kf5-kcoreaddons-devel kf5-kwidgetsaddons-devel kf5-kconfigwidgets-devel kf5-kxmlgui-devel kf5-sonnet-devel kf5-kdoctools kf5-kdoctools-devel cmake extra-cmake-modules

For other Linux flavors, the list will be similar; `cmake` will tell you if you are missing anything.

Next, open a terminal window, and enter the following commands:

    $ cd <your_ghostwriter_folder_location>
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    # make install

### MacOS

Please consult the [KDE development guide](https://community.kde.org/Get_Involved/development/Mac) on how to build KDE applications for MacOS in general.

### FreeBSD

Prerequisites

* Git (`git` or `git-lite`)

Install the dependencies

    sudo pkg inst hunspell en-hunspell pkgconf qt5-svg qt5-multimedia \
    qt5-webengine qt5-concurrent qt5-buildtools qt5-qmake qt5-linguist

You will also need to install QtWebEngine

Get the sources

    git clone https://invent.kde.org/office/ghostwriter

Build

    $ cd ghostwriter
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ sudo make install

## Command Line Usage

For terminal users, *ghostwriter* can be run from the command line.  In your terminal window, simply type the following:

    $ ghostwriter myfile.md

where `myfile.md` is the path to your Markdown text file.

An option to disable GPU acceleration `--disable-gpu` is also available.  Simply type the following:

    $ ghostwriter --disable-gpu

A scenario where you may consider using software rendering would be if compiling against Qt 6 on Windows, and running the application in full screen mode.  See the documented bug under the Windows build instructions above for further details.  Note that the application may inconsistently launch on Windows with GPU acceleration disabled, and it may take several attempts before you can start it successfully.

## Additional Markdown Processors

*ghostwriter* has built-in support for the cmark-gfm processor.  However, it also can auto-detect Pandoc, MultiMarkdown, or cmark processors.  To use any or all of the latter three, simply install them and ensure that their installation locations are added to your system's `PATH` environment variable.  *ghostwriter* will auto-detect their installation on startup, and give you live HTML preview and export options accordingly.

## Contribute

Please read the [contributing guide](https://ghostwriter.kde.org/contribute/) on how to contribute.  Your help would be greatly appreciated!

## Licensing

The source code for *ghostwriter* is licensed under the [GNU General Public License Version 3](http://www.gnu.org/licenses/gpl.html).  However, various icons and third-party FOSS code (i.e., cmark-gfm, MathJax, etc.) have different licenses compatible with GPLv3.  Please read the COPYING or LICENSE files in the respective folders for the different licenses.
