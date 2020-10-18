# QtAwesome - Font Awesome support for Qt applications

## Description

QtAwesome is a simple library that can be used to add [Font Awesome](http://fortawesome.github.io/Font-Awesome/) icons to your [Qt application](http://qt-project.org/).

NOTE: Though the name is QtAwesome and currently it's very Font Awesome based, you can use every other icon/glyph font you want.

The class can also be used to manage your own dynamic code-drawn icons, by adding named icon-painters.

## Changes in FontAwesome 5.10.2

New version of this library added support to Font Awesome version **5.10.2**.

* This library supports Free and Pro versions of Font Awesome, however only the pro font files are included in this repository, but if you have a pro license of the icons you can use it.
* It support all icons styles (solid, regular, brand, light, duotone), including the duotone icons in pro version.

## Changes in FontAwesome 4.7.0

Previous version of this library added support to Font Awesome version **4.7.0**.
You can find the font-aweomse 4 version in the branch. (https://github.com/gamecreature/QtAwesome/tree/fontawesome-4)

* In the 4.5.0 version the _linux name has been changed to fa_linux. (Makes the naming of conflicting/invalid names more consistent, like fa_try and fa_500px)
* You can find the previous FontAwesome 4 c++11 library in the [c++11 branch](https://github.com/gamecreature/QtAwesome/tree/c++11).
* You can find the previous FontAwesome 3 library in the [fontawesome-3 branch](https://github.com/gamecreature/QtAwesome/tree/fontawesome-3).

**Note about previous c++11**

I removed the C++11 requirement. And moved the c++11 code to a c++11 branch.
It's not that I don't like c++11, but the typed enum made the code less flexible then it is now.
Just integers it is. Simpler is better.

## Installation

The easiest way to include QtAweome **5.10.2** in your project is to copy the QtAwesome directory to your
project tree and add the following `include()` to your Qt project file:

```bash
CONFIG+=fontAwesomeFree #or CONFIG+=fontAwesomePro for pro version
include(QtAwesome/QtAwesome.pri)
```

Now you are good to go!

## Usage

You probably want to create a single QtAwesome object for your whole application:

```c++
QtAwesome* awesome = new QtAwesome( qApp )
awesome->initFontAwesome();     // This line is important as it loads the font and initializes the named icon map
```

* Add an accessor to this object (i.e. a global function, member of your application object, or whatever you like).
* Use an icon name from the [Font Awesome Cheatsheet](http://fortawesome.github.io/Font-Awesome/cheatsheet/).

## Example

```c++
// You should create a single object of QtAwesome.
QtAwesome* awesome = new QtAwesome( qApp );
awesome->initFontAwesome();

// Next create your icon with the help of the icon-enumeration (no dashes):
QPushButton* beerButton new QPushButton( awesome->icon( style::fas, fa::beer ), "Cheers!" );

// You can also use 'string' names to access the icons. (The string version omits the 'fa-' prefix)
QPushButton* coffeeButton new QPushButton( awesome->icon( "fas coffee" ), "Black please!" );

// When you create an icon you can supply some options for your icons:
// The available options can be found at the "Default options"-section

QVariantMap options;
options.insert( "color" , QColor(255,0,0) );
QPushButton* musicButton = new QPushButton( awesome->icon( style::fas, fa::music, options ), "Music" );

// You can also change the default options.
// for example if you always would like to have green icons you could call)
awesome->setDefaultOption( "color-disabled", QColor(0,255,0) );

// You can also directly render a label with this font
QLabel* label = new QLabel( QChar( fa::rocketchat ) );
label->setFont( awesome->font(style::fab, 16) );

```

## Example custom painter

This example registers a custom painter for supporting a duplicate icon (it draws 2 "plus marks"):

```c++
class DuplicateIconPainter : public QtAwesomeIconPainter
{
public:
    virtual void paint( QtAwesome* awesome, QPainter* painter, const QRect& rectIn, QIcon::Mode mode, QIcon::State state, const QVariantMap& options  )
    {
        int drawSize = qRound(rectIn.height()*0.5);
        int offset = rectIn.height() / 4;
        QChar chr = QChar( static_cast<int>(fa::plus) );
        int st = style::fas;

        painter->setFont( st, awesome->font( drawSize ) );

        painter->setPen( QColor(100,100,100) );
        painter->drawText( QRect( QPoint(offset*2, offset*2), QSize(drawSize, drawSize) ), chr , QTextOption( Qt::AlignCenter|Qt::AlignVCenter ) );

        painter->setPen( QColor(50,50,50) );
        painter->drawText( QRect( QPoint(rectIn.width()-drawSize-offset, rectIn.height()-drawSize-offset), QSize(drawSize, drawSize) ), chr , QTextOption( Qt::AlignCenter|Qt::AlignVCenter ) );

    }
};

awesome->give("duplicate", new DuplicateIconPainter() );
```

## Default options

  The following options are default in the QtAwesome class.

```c++
setDefaultOption( "color", QColor(50,50,50) );
setDefaultOption( "color-disabled", QColor(70,70,70,60));
setDefaultOption( "color-active", QColor(10,10,10));
setDefaultOption( "color-selected", QColor(10,10,10));

setDefaultOption( "text", QString() );      // internal option
setDefaultOption( "text-disabled", QString() );
setDefaultOption( "text-active", QString() );
setDefaultOption( "text-selected", QString() );

setDefaultOption( "scale-factor", 0.9 );
```

In pro version

```c++
setDefaultOption( "duotone-color", QColor(50,50,50,127) );
setDefaultOption( "duotone-color-disabled", QColor(70,70,70,50));
setDefaultOption( "duotone-color-active", QColor(10,10,10, 00));
setDefaultOption( "duotone-color-selected", QColor(10,10,10,210));
```

When creating an icon, it first populates the options-map with the default options from the QtAwesome object.
After that the options are expanded/overwritten by the options supplied to the icon.

It is possible to use another glyph per icon-state. For example to make an icon-unlock symbol switch to locked when selected,
you could supply the following option:

```c++
options.insert("text-selected", QString( fa::lock ) );
```

Color and text options have the following structure:
`keyname-iconmode-iconstate`

Where iconmode normal is empty
And iconstate On is off.

So the list of items used is:

* color
* color-disabled
* color-active
* color-selected
* color-off
* color-disabled-off
* color-active-off
* color-selected-off
* duotone-color (only in pro mode)
* duotone-color-disabled (only in pro mode)
* duotone-color-active (only in pro mode)
* duotone-color-selected (only in pro mode)
* duotone-color-off (only in pro mode)
* duotone-color-disabled-off (only in pro mode)
* duotone-color-active-off (only in pro mode)
* duotone-color-selected-off (only in pro mode)
* text
* text-disabled
* text-active
* text-selected
* text-off
* text-disabled-off
* text-active-off
* text-selected-off
* style
* style-disabled
* style-active
* style-selected
* style-off
* style-disabled-off
* style-active-off
* style-selected-off

## License


MIT License. Copyright 2013 - Reliable Bits Software by Blommers IT. [http://blommersit.nl/](http://blommersit.nl)

The Font Awesome font is licensed under the SIL Open Font License - [http://scripts.sil.org/OFL](http://scripts.sil.org/OFL)
The Font Awesome pictograms are licensed under the CC BY 3.0 License - [http://creativecommons.org/licenses/by/3.0/](http://creativecommons.org/licenses/by/3.0/)
"Font Awesome by Dave Gandy - http://fortawesome.github.com/Font-Awesome"

## Contact

* email: <rick@blommersit.nl>
* twitter: [https://twitter.com/gamecreature](https://twitter.com/gamecreature)
* website: [http://blommersit.nl](http://blommersit.nl)  (warning Dutch content ahead)
* github: [https://github.com/gamecreature/QtAwesome](https://github.com/gamecreature/QtAwesome)

## Known issues and workarounds

On Mac OS X, placing an qtAwesome icon in QMainWindow menu, doesn't work directly.
See the following issue: [https://github.com/gamecreature/QtAwesome/issues/10]

A workaround for this problem is converting it to a Pixmap icon like this:

```c++
QAction* menuAction = new QAction("test");
menuAction->setIcon( awesome->icon(fa::beer).pixmap(32,32) );
```

## Remarks

I've created this project because I needed some nice icons for my own Qt project. After doing a lot of
css/html5 work and being spoiled by the ease of twitter bootstrap with Font Awesome,
I thought it would be nice to be able to use these icons for my Qt project.

I've slightly changed the code from the original, added some more documentation, but it's still
a work in progress. So feel free to drop me an e-mail for your suggestions and improvements!

There are still some things todo, like:

* document the usage of another icon font
* add some tests
* do some code cleanup

Thanks go to the contributors of this project!

And of course last but not least,

Many thanks go to Dave Gandy an the other Font Awesome contributors!! [http://fortawesome.github.com/Font-Awesome](http://fortawesome.github.com/Font-Awesome)
And of course to the Qt team/contributors for supplying this great cross-platform c++ library.

Contributions are welcome! Feel free to fork and send a pull request through Github.
