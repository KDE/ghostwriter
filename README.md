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

On Windows, you can either download the setup.exe and go through the setup wizard (recommended), or you can run the portable version (advanced users).  To install via the setup wizard, download `ghostwriter-setup.exe` to a location of your choice, double click on its icon to run, and follow the installation steps.  Note that you must have administrator rights to install.

The portable version is a zip file which you can unzip to any location on your computer.  Within the zip file is a simple folder containing the `ghostwriter.exe` file.  Double click on this file to run the program.  The `dictionaries` subfolder contains your Hunspell dictionaries.  The `data` subfolder contains the location of your personal dictionaries, themes, and application settings.  The portable version is an excellent alternative for when you do not have administrative rights to install the application.  You can take it anywhere on a thumb drive and run it--at school, a friend's house, etc.

Linux
-----

If you are running Ubuntu or one of its derivatives (Linux Mint, Xubuntu, etc.), simply add the [wereturtle Launchpad PPA](http://launchpad.net/~wereturtle/+archive/ubuntu/ppa) to your system.  Open a terminal, and enter the following:

    $ sudo add-apt-repository ppa:wereturtle/ppa
    $ sudo apt-get update

You can now install the ghostwriter package.  Please consult the Launchpad guide, [*Installing software from a PPA*](https://help.launchpad.net/Packaging/PPA/InstallingSoftware), for further details.

Also, be on the lookout for *ghostwriter* making its debut in the Debian and Ubuntu repositories in the future.  In the meantime, if you are a repository maintainer of any Linux distribution, I would appreciate your help in getting *ghostwriter* packaged.

Finally, you may follow the build instructions below to install on Linux with the latest source code.

**Note to Unity Users:** If you are running Ubuntu with the Unity desktop environment, you should be aware of a bug with how Qt 5 application menus are displayed in the global menu.  Please see [this bug in Launchpad](https://bugs.launchpad.net/appmenu-qt5/+bug/1380702).  At the moment, the shortcuts (such as Ctrl+C, Ctrl+V, etc.) in *ghostwriter's* menus are not available within Unity.  This is true of other Qt 5 applications.  A common workaround is to remove the appmenu-qt5 package.  To do so, open a terminal window, and type the following:

    sudo apt-get remove appmenu-qt5

This will fix the issue for all your Qt 5 applications, including *ghostwriter*.  However, it also places the menus for the application into the application's window, rather than into the global menu.

An alternative workaround is to compile ghostwriter for yourself using Qt 4.8.  Shortcuts should properly work in the global menu for Qt 4.8 applications.  Regardless of your preferred workaround, this appmenu-qt5 bug should be fixed in the near future.  You can use the Launchpad link above to monitor the status of this bug.

**GNU/Linux Help:**  For further help with troubleshooting issues for Qt5 applications, including *ghostwriter*, please see the [GNU/Linux Troubleshooting for Q5 Applications](https://github.com/wereturtle/ghostwriter/wiki/GNU---Linux-Troubleshooting-for-Qt5-Applications) topic in the community wiki.

MacOS - Help Wanted!
-----------------------

Unfortunately, since I do not own an Apple computer, I cannot reasonably create a Mac version of this application.  However, if you happen to be a Mac user yourself and have the knowledge, you can build the source code.  Theoretically, it should work;  but you may run into some bugs since *ghostwriter* is untested on this platform.

If you are willing to test *ghostwriter* on MacOS and provide bug fixes through GitHub, your help would be very much appreciated.  Should you choose to do so, you will want to use Qt 5.5 rather than 4.8, since certain QGraphicsEffects may not work on 4.8 on MacOS, whereas there is hope that they will on version 5, based on the documentation.  Also, versions greater than Qt 5.5 do not come with QtWebkit pre-built.  Spell checking with Hunspell and Voikko will need careful testing as well.  You may wish to consult the source code for [FocusWriter](http://gottcode.org/focuswriter/) to get the latest patches, since *ghostwriter* lifted and modified parts of it.

Finally, I based *ghostwriter's* packaging structure on FocusWriter, including for MacOS, to give you a head start.  You will want to once again look at FocusWriter's .pro file and resource files to ensure everything is in place within *ghostwriter* for a successful MacOS build and installation.

Build
=====

If you wish to build from the source code, you will need either Qt 4.8 or Qt 5, available from <http://www.qt.io/> if you are on Windows or MacOS, or in your Linux distribution's repository. This documentation assumes you already have the source code unzipped in a folder.

Windows
-------

Open a DOS terminal window, and enter the following commands:

    > cd <your_ghostwriter_folder_location>
    > qmake

The next command depends on whether you have chosen to use Qt with MinGW or with Microsoft's compiler.  If you are using MinGW, enter the following:

    > mingw32-make

If you are using Microsoft's tools, enter the following:

    > nmake release

Unless you have built *ghostwriter* as a standalone executable statically linked to your own build of Qt's source code, you will need to copy the necessary Qt (and MinGW) .dll files to the same location as `ghostwriter.exe` so that the executable can find the required libraries.

Linux
-----

These instructions are for Debian or Ubuntu distributions.  However, they will be similar for other Linux flavors.

Before proceeding, ensure that you have the following packages installed for Qt 5:

* qt5-default,
* qtbase5-dev,
* libqt5svg5-dev,
* qtmultimedia5-dev,
* libqt5webkit5-dev,
* libhunspell-dev,
* pkg-config
* libqt5concurrent5
* libqt5printsupport5
* libqt5svg5

Note that you may also compile and run against Qt 4.8;  however, Qt 5 is optimal for for its newer features.  Qt 4.8 has similar package dependencies to what is listed above.  You will have to find their Qt 4.8 equivalents in your Linux distribution's repository.

Next, open a terminal window, and enter the following commands:

    $ cd <your_ghostwriter_folder_location>
    $ qmake
    $ make
    $ make install

The last command will install *ghostwriter* on your machine.  If you need to install the application in an alternative location to `/usr/local`, enter the following command in lieu of the second command above, passing in the desired value for `PREFIX`:

    $ qmake PREFIX=<your_install_path_here>

For example, to install under `/opt`, you would enter:

    $ qmake PREFIX=/opt

**Note:**  If you see blank areas where there should be icons, then you are missing the Qt dependency for the SVG images.  On Debian and Ubuntu, this is libqt5svg5.  Other Linux distributions may vary on the exact package name.

MacOS - Help Wanted!
-----------------------

Install [homebrew](http://brew.sh).  In a terminal:

``` shell
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Then:

``` shell
$ brew install qt5 --with-qtwebkit # Compication takes a while, and drains the battery
$ resources/mac/Info.plist resources/
$ qmake -spec macx-g++
$ make
```

If you see:

```
fatal: Not a git repository (or any of the parent directories): .git
```

Make sure you're cloned the repo, not just downloaded the src tarball.

If you want *ghostwriter* in your applications folder, from the repo root do:

``` shell
$ sudo cp -R ./build/release/ghostwriter.app /Applications
```

To use *ghostwriter* from the command line (assuming `/usr/local/bin` is in your path and you've moved *ghostwriter* to the `/Applications` folder):

``` shell
$ sudo ln -s /Applications/ghostwriter.app/Contents/MacOS/ghostwriter /usr/local/bin
```


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

- *ghostwriter* added into the various Linux distribution repositories (Debian, Fedora, etc.).  **Help wanted!**
- A fully-tested MacOS port. **Help wanted!**
- Translation of *ghostwriter* into other languages via *Qt Linguist*.  **Help wanted!**
- A new Windows build maintainer **Help wanted!**

Licensing
=========

The source code for *ghostwriter* is licensed under the [GNU General Public License Version 3](http://www.gnu.org/licenses/gpl.html).  However, various icons and third-party FOSS code (i.e., Hunspell and Sundown) have different licenses compatible with GPLv3.  Please read the COPYING files in the respective folders for the different licenses.
