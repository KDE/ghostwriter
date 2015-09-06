![ghostwriter](http://wereturtle.github.io/images/logo.png)

About *ghostwriter*
===================

*ghostwriter* is a Windows and Linux text editor for Markdown, which is a plain text markup format created by John Gruber. For more information about Markdown, please visit John Gruber’s website at <http://www.daringfireball.net>.  *ghostwriter* provides a relaxing, distraction-free writing environment, whether your masterpiece be that next blog post, your school paper, or your NaNoWriMo novel.  Here are just a few of its features:

* Syntax highlighting of Markdown
* Navigation of document headings
* Full-screen mode
* Focus mode that highlights the current sentence, line, three lines, or paragraph
* Two built-in themes, one light and one dark
* Theme creator for custom colors and background images
* Spell checking with Hunspell
* A live HTML preview as you type
* Use of custom CSS style sheets for HTML preview
* [Sundown](http://github.com/vmg/sundown/) processor built in for preview and export to HTML
* Interoperability with the following Markdown processors (if installed) for preview and export to popular document formats, such as PDF, ODT, and RTF:
    - [Pandoc](http://johnmacfarlane.net/pandoc/)
    - [MultiMarkdown](http://fletcherpenney.net/multimarkdown/)
    - [Discount](http://www.pell.portland.or.us/~orc/Code/discount/)
    - [cmark](https://github.com/jgm/cmark)

The best part is that *ghostwriter* is entirely free and open source.  No longer do you have to look at your friend's stylish Mac OS X Markdown editors with envy.  *ghostwriter* competitively provides the same style and functionality as many other famous editors for none of the cost.

Screenshots
===========

You can view screenshots of the application at [*ghostwriter's* project page](http://wereturtle.github.io/ghostwriter).

Installation
============

Windows
-------

On Windows, you can either download the setup.exe and go through the setup wizard (recommended), or you can run the portable version (advanced users).  To install via the setup wizard, download `ghostwriter-setup.exe` to a location of your choice, double click on its icon to run, and follow the installation steps.  Note that you must have administrator rights to install.

The portable version is a Zip file which you can unzip to any location on your computer.  Within the Zip file is a simple folder containing the `ghostwriter.exe` file.  Double click on this file to run the program.  The `dictionaries` subfolder contains your Hunspell dictionaries.  The `data` subfolder contains the location of your personal dictionaries, themes, and application settings.  The portable version is an excellent alternative for when you do not have administrative rights to install the application.  You can take it anywhere on a thumb drive and run it--at school, a friend's house, etc.

Linux
-----

An Ubuntu PPA for *ghostwriter* will be coming soon!  I also plan on begging the Debian maintainers to either adopt *ghostwriter* or my own packaging of it.  If you are a repository maintainer of any Linux distribution, I would appreciate your help in getting *ghostwriter* packaged.

In the meantime, follow the build instructions below to install on Linux.

Mac OS X - Help Wanted!
-----------------------

Unfortunately, since I do not own a Mac computer, I cannot reasonably create a Mac version of this application.  However, if you happen to be a Mac user yourself and have the knowledge, you can build the source code on your Mac.  Theoretically, it should work;  but you may run into some bugs since *ghostwriter* is untested on this platform.

If you are willing to test *ghostwriter* on Mac OS X and provide bug fixes through GitHub, your help would be very much appreciated.  Should you choose to do so, you will want to use Qt 5 rather than 4.8, since certain QGraphicsEffects may not work on 4.8 on Mac OS X, whereas there is hope that they will on version 5, based on the documentation.  Spell checking with Hunspell and Voikko will need careful testing as well.  You may wish to consult the source code for [FocusWriter](http://gottcode.org/focuswriter/) to get the latest patches, since *ghostwriter* reused and modified it.

Finally, I based *ghostwriter's* packaging structure off of FocusWriter, including for Mac OS X, to give you a head start.  You will want to once again look at FocusWriter's .pro file and resource files to ensure everything is in place within *ghostwriter* for a successful OS X build and installation.

Build
=====

If you wish to build from the source code, you will need either Qt 4.8 or Qt 5, available from <http://www.qt.io/> if you are on Windows or Mac OS X, or in your Linux distribution's repository. This documentation assumes you already have the source code unzipped in a folder.

Windows
-------

Open a DOS terminal window, and enter the following commands:

    > cd <your_ghostwriter_folder_location>
    > qmake

The next command depends on whether you have chosen to use Qt with MinGW or with Microsoft's compiler.  If you are using MinGW, enter the following:

    > mingw32-make

If you are using Microsoft's tools, enter the following:

    > nmake release

Unless you have built a static version of Qt from it's source to be statically linked to *ghostwriter* as a standalone executable, you will need to copy the necessary Qt (and MinGW) .dll files to the same location as `ghostwriter.exe` so that the executable can find the required libraries.

Linux
-----

Open a terminal window, and enter the following commands:

    $ cd <your_ghostwriter_folder_location>
    $ qmake
    $ make
    $ make install

The last command will formally install *ghostwriter* on your machine.  If you need to install the application in an alternate location from `/usr/local`, enter the following command in lieu of the second command above, passing in the desired value for `PREFIX`:

    $ qmake PREFIX=<your_install_path_here>

For example, t install under `/opt`, you would enter:

    $ qmake PREFIX=/opt

Mac OS X - Help Wanted!
-----------------------

Open a terminal window, and enter the following commands:

    $ qmake -spec macx-g++
    $ make

**Note:** If you are a hobbyist OS X developer and can get *ghostwriter* to build, install, and run successfully, please put in a pull request for this README file through GitHub with improved instructions for this process.  Your help would be greatly appreciated.

Command Line Usage
==================

For terminal users, *ghostwriter* can be run from the command line.  In your terminal window, simply type the following:

    $ ghostwriter myfile.md

where `myfile.md` is the path to your Markdown text file.

Portable Mode
=============

You can download the Windows Portable version of *ghostwriter*, or make your own on any OS.  Just as with FocusWriter, simply create a folder named `data` in the same folder as the `ghostwriter.exe` or `ghostwriter` executable file (depending on the OS).  The application will now use settings and themes in this folder.  If you need to migrate existing themes you created while running in non-portable mode, simply copy them from the relevant folder below:

- Windows:  C:\\Users\\\<your_user_name\>\\AppData\\Remote\\ghostwriter\\themes

- Linux:  /home/\<your_user_name\>/.config/ghostwriter/themes

- Mac OS X:  ~/Library/Application Support/ghostwriter/themes

**Note:**  The Mac OS X settings location needs to be confirmed.  A full sample application path would also be helpful (instead of listing `~/`).  If you are a hobbyist OS X developer and if you can confirm where *ghostwriter* stores it's settings, please put in a pull request with your revisions to this README file.

Contribute
==========

Please submit any bugs you find through GitHub, with a detailed description on how to replicate the problem.  New feature requests are also welcome.  Please see the roadmap below for ideas for which you can submit pull requests.  Also, if you happen to be a C++/Qt wizard yourself, feel free to submit pull requests for bug fixes.

Finally, if you wish to thank me by donating to the tip jar, you can flattr me from GitHub.  Simply sign up for [Flattr](http://flattr.com) if you haven't already, connect your Flattr account to your GitHub account, and then Star my repository from GitHub.  You can also simply visit the [*ghostwriter* project page](http://wereturtle.github.io/ghostwriter) and click on the Flattr button at the bottom of the page.  I'll use your generous tip to treat my family to fish tacos!  Yum!

Roadmap
=======

- Ability to store custom Markdown processor configurations.  This way, other Markdown processors can be used, or custom options be passed in.  This would take the form of a command, such as what you would type in a command line.
- Build cmark or some other Markdown processor that implements [CommonMark](http://commonmark.org/) directly into *ghostwriter* the same way Sundown is.  Note that this will take some tweaking to the build process to incorporate cmake.
- deb and RPM packages for Linux distributions.  **Help wanted!**
- *ghostwriter* added into the various Linux distribution repositories (Debian, Fedora, etc.).  **Help wanted!**
- A fully-tested Mac OS X port. **Help wanted!**
- Translatation of *ghostwriter* into other languages via *Qt Linguist*.  **Help wanted!**

Licensing
=========

The source code for *ghostwriter* is licensed under the [GNU General Public License Version 3](http://www.gnu.org/licenses/gpl.html).  However, various icons and third-party FOSS code (i.e., Hunspell and Sundown) have different licenses compatible with GPLv3.  Please read the COPYING files in the respective folders for the different licenses.
