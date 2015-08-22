![ghostwriter](http://wereturtle.github.io/images/logo.png)

About *ghostwriter*
===================

*ghostwriter* is a Windows and Linux text editor for Markdown, which is a plain text markup format created by John Gruber. For more information and help, please visit John Gruber’s website at <http://www.daringfireball.net>.  *ghostwriter* provides a relaxing environment for getting your work done in style, whether it be that next blog post, your school paper, or your next novel.  Here are just a few of its features:

* Syntax highlighting of Markdown
* Navigation of document headings
* Full-screen mode
* Focus mode that highlights the current
    - Sentence
    - Line
    - Three lines
    - Or paragraph
* Two built-in themes, one light and one dark
* Theme creator for custom colors and background images
* Spell checking with Hunspell
* A live HTML preview as you type
* Use of custom CSS style sheets for HTML preview
* Preview and export with the [Sundown](http://github.com/vmg/sundown/) processor
* Interoperability with other popular Markdown processors:
    - [Pandoc](http://johnmacfarlane.net/pandoc/)
    - [MultiMarkdown](http://fletcherpenney.net/multimarkdown/)
    - [Discount](http://www.pell.portland.or.us/~orc/Code/discount/)

The best part is that *ghostwriter* is entirely free and open source.  No longer do you have to look at your friend's stylish Mac OS X Markdown editors with envy.  *ghostwriter* competitively provides the same style and functionality as many other famous editors for none of the cost.

Screenshots
===========

*ghostwriter* comes with both classic light and dark themes, but you can also create your own.

![ghostwriter light and dark themes](http://wereturtle.github.io/images/ghostwriterlightdarkthemes.png)

The HTML Preview is in it's own window, so you can place it anywhere and not only side by side.

![ghostwriter light and dark themes](http://wereturtle.github.io/images/ghostwriterhtmlpreview.png)

Use the focus mode to better concentrate on the current sentence, line, three lines, or paragraph you are editing.

![ghostwriter light and dark themes](http://wereturtle.github.io/images/ghostwriterfocusmode.png)

Installation
============

Windows
-------

On Windows, you can either download the setup.exe and go through the setup wizard (recommended), or you can run the portable version (advanced users).  To install via the setup wizard, download `setup.exe` to a location of your choice, double click on its icon to run, and follow the installation steps.  Note that you must have administrator rights to install.

The portable version is a Zip file which you can unzip to any location on your computer.  Within the Zip file is a simple folder containing the `ghostwriter.exe` file.  Double click on this file to run the program.  The `data` sub-folder contains the location of your Hunspell dictionaries, themes, and application settings.  The portable version is an excellent alternative for when you do not have administrative rights to install the application.  You can take it anywhere on a thumb drive and run it--at school, a friend's house, etc.

Linux
-----

An Ubuntu PPA for *ghostwriter* will be coming soon!  I also plan on begging the Debian maintainers to either adopt *ghostwriter* or my own packaging of it.  If you are a repository maintainer of any Linux distribution, I would appreciate your help in getting *ghostwriter* packaged.

Mac OS X - Help Wanted!
-----------------------

Unfortunately, since I do not own a Mac computer, I cannot reasonably create a Mac version of this application.  However, if you happen to be a Mac user yourself and have the knowledge, you can build the source code on your Mac.  Theoretically, it should work;  but you may run into some bugs since *ghostwriter* is untested on this platform.

If you are willing to test *ghostwriter* on Mac OS X and provide bug fixes through GitHub, your help would be very much appreciated.  Should you choose to do so, you will want to use Qt 5 rather than 4.8, since certain QGraphicsEffects may not work on 4.8 on Mac OS X, whereas there is hope that they will on version 5, based on the documentation.  Spell-checking with Hunspell and Voikko will need careful testing as well.  You may wish to consult the source code for [FocusWriter](http://gottcode.org/focuswriter/) to get the latest patches, since *ghostwriter* reused and modified it.

Finally, I based *ghostwriter's* packaging structure off of FocusWriter, including for Mac OS X, to give you a head start.  You will want to once again look at FocusWriter's .pro file and resource files to ensure everything is in place within *ghostwriter* for a successful OS X build and installation.

Build
=====

If you wish to build from the source code, you will need either Qt 4.8 or Qt 5, available from <http://www.qt.io/> if you are on Windows or Mac OS X, or in your Linux OS repository. This documentation assumes you already have the source code unzipped in a folder.

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

I typically use [Inno Setup](http://www.jrsoftware.org/isinfo.php) to create a `setup.exe` Windows installer for *ghostwriter*.  However, you can also run *ghostwriter* in portable mode.  See the section below for details.

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

**Note:** If you are a hobbyist OS X developer and can get *ghostwriter* to build, install, and run successfully, please put in a pull request through GitHub with improved instructions for this process.  Your help would be greatly appreciated.

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

Please submit any bugs you find through GitHub, with a detailed description on how to replicate the problem.  Also, if you happen to be a C++/Qt wizard yourself, feel free to submit pull requests for bug fixes.  I would also be grateful to anyone who can translate *ghostwriter* into other languages via *Qt Linguist*.

Finally, if you wish to thank me by donating to the tip jar, you can flattr me from GitHub.  Simply sign up for [Flattr](http://flattr.com) if you haven't already, connect your Flattr account to your GitHub account, and then Star my repository from GitHub.  I'll use your generous tip to treat my family to fish tacos!  Yum!

Licensing
=========

The source code for *ghostwriter* is licensed under the [GNU General Public License Version 3](http://www.gnu.org/licenses/gpl.html).  However, various icons and third-party FOSS code (i.e., Hunspell and Sundown) have different licenses compatible with GPLv3.  Please read the COPYING files in the respective folders for the different licenses.
