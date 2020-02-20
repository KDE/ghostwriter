![ghostwriter](http://wereturtle.github.io/ghostwriter/images/logo.png)

About *ghostwriter*
===================

*ghostwriter* is a Windows and Linux text editor for Markdown, which is a plain text markup format created by John Gruber. For more information about Markdown, please visit John Gruber’s website at <http://www.daringfireball.net>.  *ghostwriter* provides a relaxing, distraction-free writing environment, whether your masterpiece be that next blog post, your school paper, or your NaNoWriMo novel.  For a tour of its features, please visit the [*ghostwriter* project page](http://wereturtle.github.io/ghostwriter/).

Screenshots
===========

You can view screenshots of the application at [*ghostwriter's* project page](http://wereturtle.github.io/ghostwriter).

Installation
============

Windows
-------

On Windows, you can either download the setup.exe and go through the setup wizard (recommended), or you can run the portable version (advanced users).  To install via the setup wizard, download `ghostwriter-x64_installer.exe` from [the Windows repository here](https://github.com/michelolvera/vs-ghostwriter) to the location of your choice, double click on its icon to run, and follow the installation steps.  Note that you must have administrator rights to install.

The portable version is a zip file which you can unzip to any location on your computer.  Within the zip file is a simple folder containing the `ghostwriter.exe` file.  Double click on this file to run the program.  The `dictionaries` subfolder contains your Hunspell dictionaries.  The `data` subfolder contains the location of your personal dictionaries, themes, and application settings.  The portable version is an excellent alternative for when you do not have administrative rights to install the application.  You can take it anywhere on a thumb drive and run it--at school, a friend's house, etc.

Linux
-----

If you are running Ubuntu or one of its derivatives (Linux Mint, Xubuntu, etc.), open a terminal, and enter the following commands:

    $ sudo add-apt-repository ppa:wereturtle/ppa
    $ sudo apt update
    $ sudo apt install ghostwriter

There is also now a staging PPA for release candidate builds for advanced users who wish to test drive the latest features/fixes.  To install the latest release candidate, enter the following commands from a terminal:

    $ sudo add-apt-repository ppa:wereturtle/staging
    $ sudo apt-get update
    $ sudo apt install ghostwriter

Fedora users can install ghostwriter from (Copr)[https://copr.fedorainfracloud.org/] by opening a terminal and entering the following commands:

    $ sudo dnf copr enable wereturtle/stable
    $ sudo dnf install ghostwriter

There is likewise a staging repository for (Copr)[https://copr.fedorainfracloud.org/], where the latest release candidate can be installed with the following commands:

    $ sudo dnf copr enable wereturtle/staging
    $ sudo dnf install ghostwriter
    
openSUSE users can visit https://software.opensuse.org/package/ghostwriter to install the latest stable version.  Note that the openSUSE version is maintained by volunteers.

Finally, you may follow the build instructions below to install on Linux with the latest source code.

MacOS
-----

You can download an application bundle for MacOS and copy it under /Applications (for all the users) or $HOME/Applications (for the current user). That application should work on osx 10.10+, but was tested only on macOS 10.13. Please remember that this build is experimental and you'll find some bugs. Please report those on the issue tracker.

FreeBSD
-------

To install ghostwriter on FreeBSD, open a terminal, and enter the following command:

    $ sudo pkg install ghostwriter

Build
=====

If you wish to build from the source code, you will need Qt 5, available from <http://www.qt.io/> if you are on Windows, or in your Linux distribution's repository. If you are on MacOS you will need the latest Qt 5 from brew.

This documentation assumes you already have the source code unzipped in a folder.

Windows
-------

Building on Windows requires Visual Studio.  Open a terminal window, and enter the following commands:

    > cd <your_ghostwriter_folder_location>
    > qmake
    > nmake release

Unless you have built *ghostwriter* as a standalone executable statically linked to your own build of Qt's source code, you will need to copy the necessary Qt and Windows .dll files to the same location as `ghostwriter.exe` so that the executable can find the required libraries.

Linux
-----

Before proceeding, ensure that you have the necessary packages installed for Qt 5.

For Debian or Ubuntu distributions:

    $ sudo apt install g++ qt5-default qtbase5-dev libqt5svg5-dev qtmultimedia5-dev qtwebengine5-dev libhunspell-dev pkg-config libqt5concurrent5 qttools5-dev-tools

For Fedora:

    $ sudo dnf install qt-devel qt5-qtbase-devel qt5-qtsvg-devel qt5-qtmultimedia-devel qt5-qtwebengine-devel hunspell-devel

For other Linux flavors, the list will be similar; `qmake` will tell you if you are missing anything.

Next, open a terminal window, and enter the following commands:

    $ cd <your_ghostwriter_folder_location>
    $ qmake
    $ make
    # make install

The last command will install *ghostwriter* on your machine.  If you need to install the application in an alternative location to `/usr/local`, enter the following command in lieu of the second command above, passing in the desired value for `PREFIX`:

    $ qmake PREFIX=<your_install_path_here>

For example, to install under `/opt`, you would enter:

    $ qmake PREFIX=/opt

**Note:**  If you see blank areas where there should be icons, then you are missing the Qt dependency for the SVG images.  On Debian and Ubuntu, this is libqt5svg5.  Other Linux distributions may vary on the exact package name.

MacOS
-----

1. You need either *XCode* or *XCode command line tools* to install and run brew and to build ghostwriter and other Qt applications.

- You can install XCode from Mac App Store
- You can install XCode command line tools from your terminal typing `xcode-select --install`

2. Install [homebrew](http://brew.sh).  In a terminal:

``` shell
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

3. Install Qt 5 from brew.

``` shell
$ brew install qt5
```

*Note:* if you installed XCode command line tools qt will fail building the application with an error like `Project ERROR: Could not resolve SDK Path for 'macosx' Error while parsing file <ghostwriter.pro>. Giving up.`. Please follow the [instructions to build Qt applications without installing XCode](doc/BUILD_MAC.md)

4. run `brew link` 

``` shell
brew link qt5 --force
```
*Note:* This command allows you to use `qmake` from the CLI. You'll need it for the final step.

5. Clone this repository and build ghostwriter:

``` shell
git clone https://github.com/wereturtle/ghostwriter
cd ghostwriter
qmake
make

```

Make sure you're cloned the repo, not just downloaded the src tarball.

5. If you want *ghostwriter* in your applications folder, from the repo root do:

``` shell
$ sudo cp -R ./build/release/ghostwriter.app /Applications
```

6. To use *ghostwriter* from the command line, do:

``` shell
$ sudo ln -s /Applications/ghostwriter.app/Contents/MacOS/ghostwriter /usr/local/bin
```

FreeBSD
-------

Prerequisites

* Git (`git` or `git-lite`)

Install the dependencies

    sudo pkg inst hunspell en-hunspell pkgconf qt5-svg qt5-multimedia \
    qt5-webengine qt5-concurrent qt5-buildtools qt5-qmake qt5-linguist

You will also need to install QtWebEngine

Get the sources

    git clone https://github.com/wereturtle/ghostwriter

Build

    cd ghostwriter
    qmake
    make
    sudo make install

Command Line Usage
==================

For terminal users, *ghostwriter* can be run from the command line.  In your terminal window, simply type the following:

    $ ghostwriter myfile.md

where `myfile.md` is the path to your Markdown text file.

Portable Mode
=============

You can download the Windows Portable version of *ghostwriter*, or make your own on any OS.  Just as with FocusWriter, simply create a folder named `data` in the same folder as the `ghostwriter.exe` or `ghostwriter` executable file (depending on the OS).  The application will now use settings and themes in this folder.  If you need to migrate existing themes you created while running in non-portable mode, simply copy them from the relevant folder below:

- Windows:  C:\\Users\\\<your_user_name\>\\AppData\\Roaming\\ghostwriter\\themes
- Linux:  /home/\<your_user_name\>/.config/ghostwriter/themes
- MacOS:  ~/Library/Application Support/ghostwriter/themes

**Note:**  The MacOS settings location needs to be confirmed.  A full sample application path would also be helpful (instead of listing `~/`).  If you are a hobbyist MacOS developer and if you can confirm where *ghostwriter* stores it's settings, please put in a pull request with your revisions to this README file.

Themes
======
José Geraldo Gouvêa has generously provided a GitHub theme repository for *ghostwriter* [here](https://github.com/jggouvea/ghostwriter-themes). You may download new themes here, or create your own and put in a pull request to add a new theme to the repository.

Wiki
====
*ghostwriter* has a community wiki [here](https://github.com/wereturtle/ghostwriter/wiki).  You can read up on help topics or contribute your own.

Contribute
==========

Please submit any bugs you find through [GitHub](http://github.com/wereturtle/ghostwriter) with a detailed description on how to replicate the problem.  New translations are also welcome. However, please note that new feature requests are no longer being accepted.  Please read the [contributing guide](https://github.com/wereturtle/ghostwriter/blob/master/CONTRIBUTING.md) for further details on how you can contribute to the project.

Roadmap
========

- Port from qmake to cmake in preparation for Qt 6.
- Add session and document goals.
- Updating only the changed HTML in live preview to improve rendering speed.
- Add quick preview popup of equations and images.
- Add quick preview popup of current paragraph/section.
- Go to corresponding line in editor on `Ctrl+left click` over text in live preview.
- Replace HUDs with popup menus or sidebar.
- Provide a fully-tested MacOS port with installer. **Help wanted!**
- Translate *ghostwriter* into other languages via *Qt Linguist*.  **Help wanted!**

Licensing
=========

The source code for *ghostwriter* is licensed under the [GNU General Public License Version 3](http://www.gnu.org/licenses/gpl.html).  However, various icons and third-party FOSS code (i.e., Hunspell and Sundown) have different licenses compatible with GPLv3.  Please read the COPYING files in the respective folders for the different licenses.
