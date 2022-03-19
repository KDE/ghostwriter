/**
 * QtAwesome - use font-awesome (or other font icons) in your c++ / Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2013-2016 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "QtAwesome.h"
#include "QtAwesomeAnim.h"

#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QFontMetrics>

QtAwesomeIconPainter::~QtAwesomeIconPainter()
{

}

/// The font-awesome icon painter
class QtAwesomeCharIconPainter: public QtAwesomeIconPainter
{
protected:

    virtual ~QtAwesomeCharIconPainter();

    QStringList optionKeysForModeAndState( const QString& key, QIcon::Mode mode, QIcon::State state)
    {
        QString modePostfix;
        switch(mode) {
            case QIcon::Disabled:
                modePostfix = "-disabled";
                break;
            case QIcon::Active:
                modePostfix = "-active";
                break;
            case QIcon::Selected:
                modePostfix = "-selected";
                break;
            default: // QIcon::Normal:
                // modePostfix = "";
                break;
        }

        QString statePostfix;
        if( state == QIcon::Off) {
            statePostfix = "-off";
        }

        // the keys that need to bet tested:   key-mode-state | key-mode | key-state | key
        QStringList result;
        if( !modePostfix.isEmpty() ) {
            if( !statePostfix.isEmpty() ) {
                result.push_back( key + modePostfix + statePostfix );
            }
            result.push_back( key + modePostfix );
        }
        if( !statePostfix.isEmpty() ) {
            result.push_back( key + statePostfix );
        }
        return result;
    }


    QVariant optionValueForModeAndState( const QString& baseKey, QIcon::Mode mode, QIcon::State state, const QVariantMap& options )
    {
        foreach( QString key, optionKeysForModeAndState(baseKey, mode, state) ) {
            //if ( options.contains(key) && options.value(key).toString().isEmpty()) qDebug() << "Not found:" << key;
            if( options.contains(key) && !(options.value(key).toString().isEmpty()) )
                return options.value(key);
        }

        return options.value(baseKey);
    }

public:


    virtual void paint( QtAwesome* awesome, QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state, const QVariantMap& options  )
    {
        painter->save();

        painter->setRenderHint(QPainter::Antialiasing);

        QVariant var =options.value("anim");
        QtAwesomeAnimation* anim = var.value<QtAwesomeAnimation*>();
        if( anim ) {
            anim->setup( *painter, rect );
        }

        // set the default options
        QColor color = optionValueForModeAndState("color", mode, state, options).value<QColor>();
        QString text = optionValueForModeAndState("text", mode, state, options).toString();
        int st = optionValueForModeAndState("style", mode, state, options).toInt();

        Q_ASSERT(color.isValid());
        Q_ASSERT(!text.isEmpty());

        painter->setPen(color);
        QRectF textRect(rect);
        int flags = Qt::AlignHCenter | Qt::AlignVCenter;

        // ajust font size depending on the rectangle
        int drawSize = qRound(textRect.height() * options.value("scale-factor").toDouble());
        QFont ft = awesome->font(static_cast<style::styles>(st), drawSize);
        QFontMetricsF fm(ft);
        QRectF tbr = fm.boundingRect(textRect,flags,text);
        if(tbr.width() > textRect.width()) {
            drawSize = static_cast<int>(ft.pixelSize() * qMin(textRect.width() *
                                        0.95/tbr.width(),textRect.height() * 0.95/tbr.height()));
            ft.setPixelSize(drawSize);
        }

        painter->setFont( ft );
        painter->drawText( textRect, flags, text);

#ifdef FONT_AWESOME_PRO
        if(st == style::stfad){
            QColor dcolor = optionValueForModeAndState("duotone-color", mode, state, options).value<QColor>();
            int dcharacter = text.at(0).unicode() | QtAwesome::DUOTONE_HEX_ICON_VALUE;
            //Duotone is a 21-bits character, we need to use surrogate pairs
            int high = ((dcharacter - 0x10000) >> 10) + 0xD800;
            int low = (dcharacter & 0x3FF) + 0xDC00;
            text = QString("%1%2").arg(QChar(high), QChar(low));

            painter->setPen(dcolor);
            painter->drawText( textRect, Qt::AlignHCenter | Qt::AlignVCenter, text);
        }
#endif

        painter->restore();
    }
};

QtAwesomeCharIconPainter::~QtAwesomeCharIconPainter(){

}

//---------------------------------------------------------------------------------------


/// The painter icon engine.
class QtAwesomeIconPainterIconEngine : public QIconEngine
{

public:

    QtAwesomeIconPainterIconEngine( QtAwesome* awesome, QtAwesomeIconPainter* painter, const QVariantMap& options  )
        : awesomeRef_(awesome)
        , iconPainterRef_(painter)
        , options_(options)
    {
    }

    virtual ~QtAwesomeIconPainterIconEngine();

    QtAwesomeIconPainterIconEngine* clone() const
    {
        return new QtAwesomeIconPainterIconEngine( awesomeRef_, iconPainterRef_, options_ );
    }

    virtual void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
    {
        Q_UNUSED( mode );
        Q_UNUSED( state );
        iconPainterRef_->paint( awesomeRef_, painter, rect, mode, state, options_ );
    }

    virtual QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
    {
        QPixmap pm(size);
        pm.fill( Qt::transparent ); // we need transparency
        {
            QPainter p(&pm);
            paint(&p, QRect(QPoint(0,0),size), mode, state);
        }
        return pm;
    }

private:

    QtAwesome* awesomeRef_;                  ///< a reference to the QtAwesome instance
    QtAwesomeIconPainter* iconPainterRef_;   ///< a reference to the icon painter
    QVariantMap options_;                    ///< the options for this icon painter
};

QtAwesomeIconPainterIconEngine::~QtAwesomeIconPainterIconEngine(){

}

//---------------------------------------------------------------------------------------

const QString QtAwesome::FAB_FONT_FILENAME = "Font Awesome 5 Brands-Regular-400.otf";
#ifdef FONT_AWESOME_PRO
const QString QtAwesome::FAD_FONT_FILENAME = "Font Awesome 5 Duotone-Solid-900.otf";
const QString QtAwesome::FAL_FONT_FILENAME = "Font Awesome 5 Pro-Light-300.otf";
const QString QtAwesome::FAR_FONT_FILENAME = "Font Awesome 5 Pro-Regular-400.otf";
const QString QtAwesome::FAS_FONT_FILENAME = "Font Awesome 5 Pro-Solid-900.otf";
#else
const QString QtAwesome::FAR_FONT_FILENAME = "Font Awesome 5 Free-Regular-400.otf";
const QString QtAwesome::FAS_FONT_FILENAME = "Font Awesome 5 Free-Solid-900.otf";
#endif

/// The default icon colors
QtAwesome::QtAwesome( QObject* parent )
    : QObject( parent )
    , _namedCodepoints()
{
    // initialize the default options
    setDefaultOption( "color", QColor(50,50,50) );
    setDefaultOption( "color-disabled", QColor(70,70,70,60));
    setDefaultOption( "color-active", QColor(10,10,10));
    setDefaultOption( "color-selected", QColor(10,10,10));
    setDefaultOption( "scale-factor", 1.0 );

#ifdef FONT_AWESOME_PRO
    setDefaultOption( "duotone-color", QColor(50,50,50,127) );
    setDefaultOption( "duotone-color-disabled", QColor(70,70,70,50));
    setDefaultOption( "duotone-color-active", QColor(10,10,10,200));
    setDefaultOption( "duotone-color-selected", QColor(10,10,10,210));
#endif

    setDefaultOption( "text", QVariant() );
    setDefaultOption( "text-disabled", QVariant() );
    setDefaultOption( "text-active", QVariant() );
    setDefaultOption( "text-selected", QVariant() );

    _fontIconPainter = new QtAwesomeCharIconPainter();

    _fontDetails.insert(style::stfas, FontData(FAS_FONT_FILENAME, FAS_FONT_WEIGHT));
    _fontDetails.insert(style::stfar, FontData(FAR_FONT_FILENAME, FAR_FONT_WEIGHT));
    _fontDetails.insert(style::stfab, FontData(FAB_FONT_FILENAME, FAB_FONT_WEIGHT));
#ifdef FONT_AWESOME_PRO
    _fontDetails.insert(style::stfal, FontData(FAL_FONT_FILENAME, FAL_FONT_WEIGHT));
    _fontDetails.insert(style::stfad, FontData(FAD_FONT_FILENAME, FAD_FONT_WEIGHT));
#endif
}


QtAwesome::~QtAwesome()
{
    delete _fontIconPainter;
    qDeleteAll(_painterMap);

    if(_namedCodepoints.contains(style::stfab))
        delete _namedCodepoints[style::stfab];
#ifdef FONT_AWESOME_PRO
    if(_namedCodepoints.contains(style::stfas))
        delete _namedCodepoints[style::stfas];
#else
    if(_namedCodepoints.contains(style::stfas))
        delete _namedCodepoints[style::stfas];

    if(_namedCodepoints.contains(style::stfar))
        delete _namedCodepoints[style::stfar];
#endif
}

struct FANameIcon {
  const char *name;
  int icon;
};

static const FANameIcon faBrandsIconArray[] = {
    { "500px"                          , fa::fa_500px                    },
    { "accessible-icon"                , fa::accessibleicon              },
    { "accusoft"                       , fa::accusoft                    },
    { "acquisitions-incorporated"      , fa::acquisitionsincorporated    },
    { "adn"                            , fa::adn                         },
    { "adobe"                          , fa::adobe                       },
    { "adversal"                       , fa::adversal                    },
    { "affiliatetheme"                 , fa::affiliatetheme              },
    { "airbnb"                         , fa::airbnb                      },
    { "algolia"                        , fa::algolia                     },
    { "alipay"                         , fa::alipay                      },
    { "amazon"                         , fa::amazon                      },
    { "amazon-pay"                     , fa::amazonpay                   },
    { "amilia"                         , fa::amilia                      },
    { "android"                        , fa::android                     },
    { "angellist"                      , fa::angellist                   },
    { "angrycreative"                  , fa::angrycreative               },
    { "angular"                        , fa::angular                     },
    { "app-store"                      , fa::appstore                    },
    { "app-store-ios"                  , fa::appstoreios                 },
    { "apper"                          , fa::apper                       },
    { "apple"                          , fa::apple                       },
    { "apple-pay"                      , fa::applepay                    },
    { "artstation"                     , fa::artstation                  },
    { "asymmetrik"                     , fa::asymmetrik                  },
    { "atlassian"                      , fa::atlassian                   },
    { "audible"                        , fa::audible                     },
    { "autoprefixer"                   , fa::autoprefixer                },
    { "avianex"                        , fa::avianex                     },
    { "aviato"                         , fa::aviato                      },
    { "aws"                            , fa::aws                         },
    { "bandcamp"                       , fa::bandcamp                    },
    { "battle-net"                     , fa::battlenet                   },
    { "behance"                        , fa::behance                     },
    { "behance-square"                 , fa::behancesquare               },
    { "bimobject"                      , fa::bimobject                   },
    { "bitbucket"                      , fa::bitbucket                   },
    { "bitcoin"                        , fa::bitcoin                     },
    { "bity"                           , fa::bity                        },
    { "black-tie"                      , fa::blacktie                    },
    { "blackberry"                     , fa::blackberry                  },
    { "blogger"                        , fa::blogger                     },
    { "blogger-b"                      , fa::bloggerb                    },
    { "bluetooth"                      , fa::bluetooth                   },
    { "bluetooth-b"                    , fa::bluetoothb                  },
    { "bootstrap"                      , fa::bootstrap                   },
    { "btc"                            , fa::btc                         },
    { "buffer"                         , fa::buffer                      },
    { "buromobelexperte"               , fa::buromobelexperte            },
    { "buysellads"                     , fa::buysellads                  },
    { "canadian-maple-leaf"            , fa::canadianmapleleaf           },
    { "cc-amazon-pay"                  , fa::ccamazonpay                 },
    { "cc-amex"                        , fa::ccamex                      },
    { "cc-apple-pay"                   , fa::ccapplepay                  },
    { "cc-diners-club"                 , fa::ccdinersclub                },
    { "cc-discover"                    , fa::ccdiscover                  },
    { "cc-jcb"                         , fa::ccjcb                       },
    { "cc-mastercard"                  , fa::ccmastercard                },
    { "cc-paypal"                      , fa::ccpaypal                    },
    { "cc-stripe"                      , fa::ccstripe                    },
    { "cc-visa"                        , fa::ccvisa                      },
    { "centercode"                     , fa::centercode                  },
    { "centos"                         , fa::centos                      },
    { "chrome"                         , fa::chrome                      },
    { "chromecast"                     , fa::chromecast                  },
    { "cloudscale"                     , fa::cloudscale                  },
    { "cloudsmith"                     , fa::cloudsmith                  },
    { "cloudversify"                   , fa::cloudversify                },
    { "codepen"                        , fa::codepen                     },
    { "codiepie"                       , fa::codiepie                    },
    { "confluence"                     , fa::confluence                  },
    { "connectdevelop"                 , fa::connectdevelop              },
    { "contao"                         , fa::contao                      },
    { "cotton-bureau"                  , fa::cottonbureau                },
    { "cpanel"                         , fa::cpanel                      },
    { "creative-commons"               , fa::creativecommons             },
    { "creative-commons-by"            , fa::creativecommonsby           },
    { "creative-commons-nc"            , fa::creativecommonsnc           },
    { "creative-commons-nc-eu"         , fa::creativecommonsnceu         },
    { "creative-commons-nc-jp"         , fa::creativecommonsncjp         },
    { "creative-commons-nd"            , fa::creativecommonsnd           },
    { "creative-commons-pd"            , fa::creativecommonspd           },
    { "creative-commons-pd-alt"        , fa::creativecommonspdalt        },
    { "creative-commons-remix"         , fa::creativecommonsremix        },
    { "creative-commons-sa"            , fa::creativecommonssa           },
    { "creative-commons-sampling"      , fa::creativecommonssampling     },
    { "creative-commons-sampling-plus" , fa::creativecommonssamplingplus },
    { "creative-commons-share"         , fa::creativecommonsshare        },
    { "creative-commons-zero"          , fa::creativecommonszero         },
    { "critical-role"                  , fa::criticalrole                },
    { "css3"                           , fa::css3                        },
    { "css3-alt"                       , fa::css3alt                     },
    { "cuttlefish"                     , fa::cuttlefish                  },
    { "d-and-d"                        , fa::dandd                       },
    { "d-and-d-beyond"                 , fa::danddbeyond                 },
    { "dashcube"                       , fa::dashcube                    },
    { "delicious"                      , fa::delicious                   },
    { "deploydog"                      , fa::deploydog                   },
    { "deskpro"                        , fa::deskpro                     },
    { "dev"                            , fa::dev                         },
    { "deviantart"                     , fa::deviantart                  },
    { "dhl"                            , fa::dhl                         },
    { "diaspora"                       , fa::diaspora                    },
    { "digg"                           , fa::digg                        },
    { "digital-ocean"                  , fa::digitalocean                },
    { "discord"                        , fa::discord                     },
    { "discourse"                      , fa::discourse                   },
    { "dochub"                         , fa::dochub                      },
    { "docker"                         , fa::docker                      },
    { "draft2digital"                  , fa::draft2digital               },
    { "dribbble"                       , fa::dribbble                    },
    { "dribbble-square"                , fa::dribbblesquare              },
    { "dropbox"                        , fa::dropbox                     },
    { "drupal"                         , fa::drupal                      },
    { "dyalog"                         , fa::dyalog                      },
    { "earlybirds"                     , fa::earlybirds                  },
    { "ebay"                           , fa::ebay                        },
    { "edge"                           , fa::edge                        },
    { "elementor"                      , fa::elementor                   },
    { "ello"                           , fa::ello                        },
    { "ember"                          , fa::ember                       },
    { "empire"                         , fa::empire                      },
    { "envira"                         , fa::envira                      },
    { "erlang"                         , fa::erlang                      },
    { "ethereum"                       , fa::ethereum                    },
    { "etsy"                           , fa::etsy                        },
    { "evernote"                       , fa::evernote                    },
    { "expeditedssl"                   , fa::expeditedssl                },
    { "facebook"                       , fa::facebook                    },
    { "facebook-f"                     , fa::facebookf                   },
    { "facebook-messenger"             , fa::facebookmessenger           },
    { "facebook-square"                , fa::facebooksquare              },
    { "fantasy-flight-games"           , fa::fantasyflightgames          },
    { "fedex"                          , fa::fedex                       },
    { "fedora"                         , fa::fedora                      },
    { "figma"                          , fa::figma                       },
    { "firefox"                        , fa::firefox                     },
    { "first-order"                    , fa::firstorder                  },
    { "first-order-alt"                , fa::firstorderalt               },
    { "firstdraft"                     , fa::firstdraft                  },
    { "flickr"                         , fa::flickr                      },
    { "flipboard"                      , fa::flipboard                   },
    { "fly"                            , fa::fly                         },
    { "font-awesome"                   , fa::fontawesome                 },
    { "font-awesome-alt"               , fa::fontawesomealt              },
    { "font-awesome-flag"              , fa::fontawesomeflag             },
    { "fonticons"                      , fa::fonticons                   },
    { "fonticons-fi"                   , fa::fonticonsfi                 },
    { "fort-awesome"                   , fa::fortawesome                 },
    { "fort-awesome-alt"               , fa::fortawesomealt              },
    { "forumbee"                       , fa::forumbee                    },
    { "foursquare"                     , fa::foursquare                  },
    { "free-code-camp"                 , fa::freecodecamp                },
    { "freebsd"                        , fa::freebsd                     },
    { "fulcrum"                        , fa::fulcrum                     },
    { "galactic-republic"              , fa::galacticrepublic            },
    { "galactic-senate"                , fa::galacticsenate              },
    { "get-pocket"                     , fa::getpocket                   },
    { "gg"                             , fa::gg                          },
    { "gg-circle"                      , fa::ggcircle                    },
    { "git"                            , fa::git                         },
    { "git-alt"                        , fa::gitalt                      },
    { "git-square"                     , fa::gitsquare                   },
    { "github"                         , fa::github                      },
    { "github-alt"                     , fa::githubalt                   },
    { "github-square"                  , fa::githubsquare                },
    { "gitkraken"                      , fa::gitkraken                   },
    { "gitlab"                         , fa::gitlab                      },
    { "gitter"                         , fa::gitter                      },
    { "glide"                          , fa::glide                       },
    { "glide-g"                        , fa::glideg                      },
    { "gofore"                         , fa::gofore                      },
    { "goodreads"                      , fa::goodreads                   },
    { "goodreads-g"                    , fa::goodreadsg                  },
    { "google"                         , fa::google                      },
    { "google-drive"                   , fa::googledrive                 },
    { "google-play"                    , fa::googleplay                  },
    { "google-plus"                    , fa::googleplus                  },
    { "google-plus-g"                  , fa::googleplusg                 },
    { "google-plus-square"             , fa::googleplussquare            },
    { "google-wallet"                  , fa::googlewallet                },
    { "gratipay"                       , fa::gratipay                    },
    { "grav"                           , fa::grav                        },
    { "gripfire"                       , fa::gripfire                    },
    { "grunt"                          , fa::grunt                       },
    { "gulp"                           , fa::gulp                        },
    { "hacker-news"                    , fa::hackernews                  },
    { "hacker-news-square"             , fa::hackernewssquare            },
    { "hackerrank"                     , fa::hackerrank                  },
    { "hips"                           , fa::hips                        },
    { "hire-a-helper"                  , fa::hireahelper                 },
    { "hooli"                          , fa::hooli                       },
    { "hornbill"                       , fa::hornbill                    },
    { "hotjar"                         , fa::hotjar                      },
    { "houzz"                          , fa::houzz                       },
    { "html5"                          , fa::html5                       },
    { "hubspot"                        , fa::hubspot                     },
    { "imdb"                           , fa::imdb                        },
    { "instagram"                      , fa::instagram                   },
    { "intercom"                       , fa::intercom                    },
    { "internet-explorer"              , fa::internetexplorer            },
    { "invision"                       , fa::invision                    },
    { "ioxhost"                        , fa::ioxhost                     },
    { "itch-io"                        , fa::itchio                      },
    { "itunes"                         , fa::itunes                      },
    { "itunes-note"                    , fa::itunesnote                  },
    { "java"                           , fa::java                        },
    { "jedi-order"                     , fa::jediorder                   },
    { "jenkins"                        , fa::jenkins                     },
    { "jira"                           , fa::jira                        },
    { "joget"                          , fa::joget                       },
    { "joomla"                         , fa::joomla                      },
    { "js"                             , fa::js                          },
    { "js-square"                      , fa::jssquare                    },
    { "jsfiddle"                       , fa::jsfiddle                    },
    { "kaggle"                         , fa::kaggle                      },
    { "keybase"                        , fa::keybase                     },
    { "keycdn"                         , fa::keycdn                      },
    { "kickstarter"                    , fa::kickstarter                 },
    { "kickstarter-k"                  , fa::kickstarterk                },
    { "korvue"                         , fa::korvue                      },
    { "laravel"                        , fa::laravel                     },
    { "lastfm"                         , fa::lastfm                      },
    { "lastfm-square"                  , fa::lastfmsquare                },
    { "leanpub"                        , fa::leanpub                     },
    { "less"                           , fa::less                        },
    { "line"                           , fa::line                        },
    { "linkedin"                       , fa::linkedin                    },
    { "linkedin-in"                    , fa::linkedinin                  },
    { "linode"                         , fa::linode                      },
    { "linux"                          , fa::fa_linux                    },
    { "lyft"                           , fa::lyft                        },
    { "magento"                        , fa::magento                     },
    { "mailchimp"                      , fa::mailchimp                   },
    { "mandalorian"                    , fa::mandalorian                 },
    { "markdown"                       , fa::markdown                    },
    { "mastodon"                       , fa::mastodon                    },
    { "maxcdn"                         , fa::maxcdn                      },
    { "medapps"                        , fa::medapps                     },
    { "medium"                         , fa::medium                      },
    { "medium-m"                       , fa::mediumm                     },
    { "medrt"                          , fa::medrt                       },
    { "meetup"                         , fa::meetup                      },
    { "megaport"                       , fa::megaport                    },
    { "mendeley"                       , fa::mendeley                    },
    { "microsoft"                      , fa::microsoft                   },
    { "mix"                            , fa::mix                         },
    { "mixcloud"                       , fa::mixcloud                    },
    { "mizuni"                         , fa::mizuni                      },
    { "modx"                           , fa::modx                        },
    { "monero"                         , fa::monero                      },
    { "napster"                        , fa::napster                     },
    { "neos"                           , fa::neos                        },
    { "nimblr"                         , fa::nimblr                      },
    { "node"                           , fa::node                        },
    { "node-js"                        , fa::nodejs                      },
    { "npm"                            , fa::npm                         },
    { "ns8"                            , fa::ns8                         },
    { "nutritionix"                    , fa::nutritionix                 },
    { "odnoklassniki"                  , fa::odnoklassniki               },
    { "odnoklassniki-square"           , fa::odnoklassnikisquare         },
    { "old-republic"                   , fa::oldrepublic                 },
    { "opencart"                       , fa::opencart                    },
    { "openid"                         , fa::openid                      },
    { "opera"                          , fa::opera                       },
    { "optin-monster"                  , fa::optinmonster                },
    { "osi"                            , fa::osi                         },
    { "page4"                          , fa::page4                       },
    { "pagelines"                      , fa::pagelines                   },
    { "palfed"                         , fa::palfed                      },
    { "patreon"                        , fa::patreon                     },
    { "paypal"                         , fa::paypal                      },
    { "penny-arcade"                   , fa::pennyarcade                 },
    { "periscope"                      , fa::periscope                   },
    { "phabricator"                    , fa::phabricator                 },
    { "phoenix-framework"              , fa::phoenixframework            },
    { "phoenix-squadron"               , fa::phoenixsquadron             },
    { "php"                            , fa::php                         },
    { "pied-piper"                     , fa::piedpiper                   },
    { "pied-piper-alt"                 , fa::piedpiperalt                },
    { "pied-piper-hat"                 , fa::piedpiperhat                },
    { "pied-piper-pp"                  , fa::piedpiperpp                 },
    { "pinterest"                      , fa::pinterest                   },
    { "pinterest-p"                    , fa::pinterestp                  },
    { "pinterest-square"               , fa::pinterestsquare             },
    { "playstation"                    , fa::playstation                 },
    { "product-hunt"                   , fa::producthunt                 },
    { "pushed"                         , fa::pushed                      },
    { "python"                         , fa::python                      },
    { "qq"                             , fa::qq                          },
    { "quinscape"                      , fa::quinscape                   },
    { "quora"                          , fa::quora                       },
    { "r-project"                      , fa::rproject                    },
    { "raspberry-pi"                   , fa::raspberrypi                 },
    { "ravelry"                        , fa::ravelry                     },
    { "react"                          , fa::react                       },
    { "reacteurope"                    , fa::reacteurope                 },
    { "readme"                         , fa::readme                      },
    { "rebel"                          , fa::rebel                       },
    { "red-river"                      , fa::redriver                    },
    { "reddit"                         , fa::reddit                      },
    { "reddit-alien"                   , fa::redditalien                 },
    { "reddit-square"                  , fa::redditsquare                },
    { "redhat"                         , fa::redhat                      },
    { "renren"                         , fa::renren                      },
    { "replyd"                         , fa::replyd                      },
    { "researchgate"                   , fa::researchgate                },
    { "resolving"                      , fa::resolving                   },
    { "rev"                            , fa::rev                         },
    { "rocketchat"                     , fa::rocketchat                  },
    { "rockrms"                        , fa::rockrms                     },
    { "safari"                         , fa::safari                      },
    { "salesforce"                     , fa::salesforce                  },
    { "sass"                           , fa::sass                        },
    { "schlix"                         , fa::schlix                      },
    { "scribd"                         , fa::scribd                      },
    { "searchengin"                    , fa::searchengin                 },
    { "sellcast"                       , fa::sellcast                    },
    { "sellsy"                         , fa::sellsy                      },
    { "servicestack"                   , fa::servicestack                },
    { "shirtsinbulk"                   , fa::shirtsinbulk                },
    { "shopware"                       , fa::shopware                    },
    { "simplybuilt"                    , fa::simplybuilt                 },
    { "sistrix"                        , fa::sistrix                     },
    { "sith"                           , fa::sith                        },
    { "sketch"                         , fa::sketch                      },
    { "skyatlas"                       , fa::skyatlas                    },
    { "skype"                          , fa::skype                       },
    { "slack"                          , fa::slack                       },
    { "slack-hash"                     , fa::slackhash                   },
    { "slideshare"                     , fa::slideshare                  },
    { "snapchat"                       , fa::snapchat                    },
    { "snapchat-ghost"                 , fa::snapchatghost               },
    { "snapchat-square"                , fa::snapchatsquare              },
    { "soundcloud"                     , fa::soundcloud                  },
    { "sourcetree"                     , fa::sourcetree                  },
    { "speakap"                        , fa::speakap                     },
    { "speaker-deck"                   , fa::speakerdeck                 },
    { "spotify"                        , fa::spotify                     },
    { "squarespace"                    , fa::squarespace                 },
    { "stack-exchange"                 , fa::stackexchange               },
    { "stack-overflow"                 , fa::stackoverflow               },
    { "stackpath"                      , fa::stackpath                   },
    { "staylinked"                     , fa::staylinked                  },
    { "steam"                          , fa::steam                       },
    { "steam-square"                   , fa::steamsquare                 },
    { "steam-symbol"                   , fa::steamsymbol                 },
    { "sticker-mule"                   , fa::stickermule                 },
    { "strava"                         , fa::strava                      },
    { "stripe"                         , fa::stripe                      },
    { "stripe-s"                       , fa::stripes                     },
    { "studiovinari"                   , fa::studiovinari                },
    { "stumbleupon"                    , fa::stumbleupon                 },
    { "stumbleupon-circle"             , fa::stumbleuponcircle           },
    { "superpowers"                    , fa::superpowers                 },
    { "supple"                         , fa::supple                      },
    { "suse"                           , fa::suse                        },
    { "symfony"                        , fa::symfony                     },
    { "teamspeak"                      , fa::teamspeak                   },
    { "telegram"                       , fa::telegram                    },
    { "telegram-plane"                 , fa::telegramplane               },
    { "tencent-weibo"                  , fa::tencentweibo                },
    { "the-red-yeti"                   , fa::theredyeti                  },
    { "themeco"                        , fa::themeco                     },
    { "themeisle"                      , fa::themeisle                   },
    { "think-peaks"                    , fa::thinkpeaks                  },
    { "trade-federation"               , fa::tradefederation             },
    { "trello"                         , fa::trello                      },
    { "tripadvisor"                    , fa::tripadvisor                 },
    { "tumblr"                         , fa::tumblr                      },
    { "tumblr-square"                  , fa::tumblrsquare                },
    { "twitch"                         , fa::twitch                      },
    { "twitter"                        , fa::twitter                     },
    { "twitter-square"                 , fa::twittersquare               },
    { "typo3"                          , fa::typo3                       },
    { "uber"                           , fa::uber                        },
    { "ubuntu"                         , fa::ubuntu                      },
    { "uikit"                          , fa::uikit                       },
    { "uniregistry"                    , fa::uniregistry                 },
    { "untappd"                        , fa::untappd                     },
    { "ups"                            , fa::ups                         },
    { "usb"                            , fa::usb                         },
    { "usps"                           , fa::usps                        },
    { "ussunnah"                       , fa::ussunnah                    },
    { "vaadin"                         , fa::vaadin                      },
    { "viacoin"                        , fa::viacoin                     },
    { "viadeo"                         , fa::viadeo                      },
    { "viadeo-square"                  , fa::viadeosquare                },
    { "viber"                          , fa::viber                       },
    { "vimeo"                          , fa::vimeo                       },
    { "vimeo-square"                   , fa::vimeosquare                 },
    { "vimeo-v"                        , fa::vimeov                      },
    { "vine"                           , fa::vine                        },
    { "vk"                             , fa::vk                          },
    { "vnv"                            , fa::vnv                         },
    { "vuejs"                          , fa::vuejs                       },
    { "waze"                           , fa::waze                        },
    { "weebly"                         , fa::weebly                      },
    { "weibo"                          , fa::weibo                       },
    { "weixin"                         , fa::weixin                      },
    { "whatsapp"                       , fa::whatsapp                    },
    { "whatsapp-square"                , fa::whatsappsquare              },
    { "whmcs"                          , fa::whmcs                       },
    { "wikipedia-w"                    , fa::wikipediaw                  },
    { "windows"                        , fa::windows                     },
    { "wix"                            , fa::wix                         },
    { "wizards-of-the-coast"           , fa::wizardsofthecoast           },
    { "wolf-pack-battalion"            , fa::wolfpackbattalion           },
    { "wordpress"                      , fa::wordpress                   },
    { "wordpress-simple"               , fa::wordpresssimple             },
    { "wpbeginner"                     , fa::wpbeginner                  },
    { "wpexplorer"                     , fa::wpexplorer                  },
    { "wpforms"                        , fa::wpforms                     },
    { "wpressr"                        , fa::wpressr                     },
    { "xbox"                           , fa::xbox                        },
    { "xing"                           , fa::xing                        },
    { "xing-square"                    , fa::xingsquare                  },
    { "y-combinator"                   , fa::ycombinator                 },
    { "yahoo"                          , fa::yahoo                       },
    { "yammer"                         , fa::yammer                      },
    { "yandex"                         , fa::yandex                      },
    { "yandex-international"           , fa::yandexinternational         },
    { "yarn"                           , fa::yarn                        },
    { "yelp"                           , fa::yelp                        },
    { "yoast"                          , fa::yoast                       },
    { "youtube"                        , fa::youtube                     },
    { "youtube-square"                 , fa::youtubesquare               },
    { "zhihu"                          , fa::zhihu                       }
};


#ifdef FONT_AWESOME_PRO
static const FANameIcon faProIconArray[] = {
    { "abacus"                              , fa::abacus                          },
    { "acorn"                               , fa::acorn                           },
    { "alarm-clock"                         , fa::alarmclock                      },
    { "alarm-exclamation"                   , fa::alarmexclamation                },
    { "alarm-plus"                          , fa::alarmplus                       },
    { "alarm-snooze"                        , fa::alarmsnooze                     },
    { "alicorn"                             , fa::alicorn                         },
    { "align-slash"                         , fa::alignslash                      },
    { "analytics"                           , fa::analytics                       },
    { "angel"                               , fa::angel                           },
    { "apple-crate"                         , fa::applecrate                      },
    { "arrow-alt-down"                      , fa::arrowaltdown                    },
    { "arrow-alt-from-bottom"               , fa::arrowaltfrombottom              },
    { "arrow-alt-from-left"                 , fa::arrowaltfromleft                },
    { "arrow-alt-from-right"                , fa::arrowaltfromright               },
    { "arrow-alt-from-top"                  , fa::arrowaltfromtop                 },
    { "arrow-alt-left"                      , fa::arrowaltleft                    },
    { "arrow-alt-right"                     , fa::arrowaltright                   },
    { "arrow-alt-square-down"               , fa::arrowaltsquaredown              },
    { "arrow-alt-square-left"               , fa::arrowaltsquareleft              },
    { "arrow-alt-square-right"              , fa::arrowaltsquareright             },
    { "arrow-alt-square-up"                 , fa::arrowaltsquareup                },
    { "arrow-alt-to-bottom"                 , fa::arrowalttobottom                },
    { "arrow-alt-to-left"                   , fa::arrowalttoleft                  },
    { "arrow-alt-to-right"                  , fa::arrowalttoright                 },
    { "arrow-alt-to-top"                    , fa::arrowalttotop                   },
    { "arrow-alt-up"                        , fa::arrowaltup                      },
    { "arrow-from-bottom"                   , fa::arrowfrombottom                 },
    { "arrow-from-left"                     , fa::arrowfromleft                   },
    { "arrow-from-right"                    , fa::arrowfromright                  },
    { "arrow-from-top"                      , fa::arrowfromtop                    },
    { "arrows"                              , fa::arrows                          },
    { "arrows-h"                            , fa::arrowsh                         },
    { "arrow-square-down"                   , fa::arrowsquaredown                 },
    { "arrow-square-left"                   , fa::arrowsquareleft                 },
    { "arrow-square-right"                  , fa::arrowsquareright                },
    { "arrow-square-up"                     , fa::arrowsquareup                   },
    { "arrows-v"                            , fa::arrowsv                         },
    { "arrow-to-bottom"                     , fa::arrowtobottom                   },
    { "arrow-to-left"                       , fa::arrowtoleft                     },
    { "arrow-to-right"                      , fa::arrowtoright                    },
    { "arrow-to-top"                        , fa::arrowtotop                      },
    { "atom-alt"                            , fa::atomalt                         },
    { "axe"                                 , fa::axe                             },
    { "axe-battle"                          , fa::axebattle                       },
    { "backpack"                            , fa::backpack                        },
    { "badge"                               , fa::badge                           },
    { "badge-check"                         , fa::badgecheck                      },
    { "badge-dollar"                        , fa::badgedollar                     },
    { "badge-percent"                       , fa::badgepercent                    },
    { "badger-honey"                        , fa::badgerhoney                     },
    { "bags-shopping"                       , fa::bagsshopping                    },
    { "ballot"                              , fa::ballot                          },
    { "ballot-check"                        , fa::ballotcheck                     },
    { "ball-pile"                           , fa::ballpile                        },
    { "barcode-alt"                         , fa::barcodealt                      },
    { "barcode-read"                        , fa::barcoderead                     },
    { "barcode-scan"                        , fa::barcodescan                     },
    { "baseball"                            , fa::baseball                        },
    { "basketball-hoop"                     , fa::basketballhoop                  },
    { "bat"                                 , fa::bat                             },
    { "battery-bolt"                        , fa::batterybolt                     },
    { "battery-slash"                       , fa::batteryslash                    },
    { "bell-exclamation"                    , fa::bellexclamation                 },
    { "bell-plus"                           , fa::bellplus                        },
    { "bells"                               , fa::bells                           },
    { "bell-school"                         , fa::bellschool                      },
    { "bell-school-slash"                   , fa::bellschoolslash                 },
    { "biking-mountain"                     , fa::bikingmountain                  },
    { "blanket"                             , fa::blanket                         },
    { "bone-break"                          , fa::bonebreak                       },
    { "book-alt"                            , fa::bookalt                         },
    { "book-heart"                          , fa::bookheart                       },
    { "books"                               , fa::books                           },
    { "books-medical"                       , fa::booksmedical                    },
    { "book-spells"                         , fa::bookspells                      },
    { "book-user"                           , fa::bookuser                        },
    { "boot"                                , fa::boot                            },
    { "booth-curtain"                       , fa::boothcurtain                    },
    { "border-bottom"                       , fa::borderbottom                    },
    { "border-center-h"                     , fa::bordercenterh                   },
    { "border-center-v"                     , fa::bordercenterv                   },
    { "border-inner"                        , fa::borderinner                     },
    { "border-left"                         , fa::borderleft                      },
    { "border-outer"                        , fa::borderouter                     },
    { "border-right"                        , fa::borderright                     },
    { "border-style-alt"                    , fa::borderstylealt                  },
    { "border-top"                          , fa::bordertop                       },
    { "bow-arrow"                           , fa::bowarrow                        },
    { "bowling-pins"                        , fa::bowlingpins                     },
    { "box-alt"                             , fa::boxalt                          },
    { "box-ballot"                          , fa::boxballot                       },
    { "box-check"                           , fa::boxcheck                        },
    { "boxes-alt"                           , fa::boxesalt                        },
    { "box-fragile"                         , fa::boxfragile                      },
    { "box-full"                            , fa::boxfull                         },
    { "box-heart"                           , fa::boxheart                        },
    { "boxing-glove"                        , fa::boxingglove                     },
    { "box-up"                              , fa::boxup                           },
    { "box-usd"                             , fa::boxusd                          },
    { "brackets"                            , fa::brackets                        },
    { "brackets-curly"                      , fa::bracketscurly                   },
    { "bread-loaf"                          , fa::breadloaf                       },
    { "bring-forward"                       , fa::bringforward                    },
    { "bring-front"                         , fa::bringfront                      },
    { "browser"                             , fa::browser                         },
    { "bullseye-arrow"                      , fa::bullseyearrow                   },
    { "bullseye-pointer"                    , fa::bullseyepointer                 },
    { "burger-soda"                         , fa::burgersoda                      },
    { "burrito"                             , fa::burrito                         },
    { "bus-school"                          , fa::busschool                       },
    { "cabinet-filing"                      , fa::cabinetfiling                   },
    { "calculator-alt"                      , fa::calculatoralt                   },
    { "calendar-edit"                       , fa::calendaredit                    },
    { "calendar-exclamation"                , fa::calendarexclamation             },
    { "calendar-star"                       , fa::calendarstar                    },
    { "camera-alt"                          , fa::cameraalt                       },
    { "campfire"                            , fa::campfire                        },
    { "candle-holder"                       , fa::candleholder                    },
    { "candy-corn"                          , fa::candycorn                       },
    { "car-building"                        , fa::carbuilding                     },
    { "car-bump"                            , fa::carbump                         },
    { "car-bus"                             , fa::carbus                          },
    { "caret-circle-down"                   , fa::caretcircledown                 },
    { "caret-circle-left"                   , fa::caretcircleleft                 },
    { "caret-circle-right"                  , fa::caretcircleright                },
    { "caret-circle-up"                     , fa::caretcircleup                   },
    { "car-garage"                          , fa::cargarage                       },
    { "car-mechanic"                        , fa::carmechanic                     },
    { "cars"                                , fa::cars                            },
    { "car-tilt"                            , fa::cartilt                         },
    { "car-wash"                            , fa::carwash                         },
    { "cauldron"                            , fa::cauldron                        },
    { "chair-office"                        , fa::chairoffice                     },
    { "chart-line-down"                     , fa::chartlinedown                   },
    { "chart-network"                       , fa::chartnetwork                    },
    { "chart-pie-alt"                       , fa::chartpiealt                     },
    { "chart-scatter"                       , fa::chartscatter                    },
    { "cheeseburger"                        , fa::cheeseburger                    },
    { "cheese-swiss"                        , fa::cheeseswiss                     },
    { "chess-bishop-alt"                    , fa::chessbishopalt                  },
    { "chess-clock"                         , fa::chessclock                      },
    { "chess-clock-alt"                     , fa::chessclockalt                   },
    { "chess-king-alt"                      , fa::chesskingalt                    },
    { "chess-knight-alt"                    , fa::chessknightalt                  },
    { "chess-pawn-alt"                      , fa::chesspawnalt                    },
    { "chess-queen-alt"                     , fa::chessqueenalt                   },
    { "chess-rook-alt"                      , fa::chessrookalt                    },
    { "chevron-double-down"                 , fa::chevrondoubledown               },
    { "chevron-double-left"                 , fa::chevrondoubleleft               },
    { "chevron-double-right"                , fa::chevrondoubleright              },
    { "chevron-double-up"                   , fa::chevrondoubleup                 },
    { "chevron-square-down"                 , fa::chevronsquaredown               },
    { "chevron-square-left"                 , fa::chevronsquareleft               },
    { "chevron-square-right"                , fa::chevronsquareright              },
    { "chevron-square-up"                   , fa::chevronsquareup                 },
    { "chimney"                             , fa::chimney                         },
    { "claw-marks"                          , fa::clawmarks                       },
    { "clipboard-list-check"                , fa::clipboardlistcheck              },
    { "clipboard-prescription"              , fa::clipboardprescription           },
    { "clipboard-user"                      , fa::clipboarduser                   },
    { "cloud-download"                      , fa::clouddownload                   },
    { "cloud-drizzle"                       , fa::clouddrizzle                    },
    { "cloud-hail"                          , fa::cloudhail                       },
    { "cloud-hail-mixed"                    , fa::cloudhailmixed                  },
    { "cloud-rainbow"                       , fa::cloudrainbow                    },
    { "clouds"                              , fa::clouds                          },
    { "cloud-showers"                       , fa::cloudshowers                    },
    { "cloud-sleet"                         , fa::cloudsleet                      },
    { "clouds-moon"                         , fa::cloudsmoon                      },
    { "cloud-snow"                          , fa::cloudsnow                       },
    { "clouds-sun"                          , fa::cloudssun                       },
    { "cloud-upload"                        , fa::cloudupload                     },
    { "club"                                , fa::club                            },
    { "code-commit"                         , fa::codecommit                      },
    { "code-merge"                          , fa::codemerge                       },
    { "coffee-togo"                         , fa::coffeetogo                      },
    { "coffin"                              , fa::coffin                          },
    { "coin"                                , fa::coin                            },
    { "comment-alt-check"                   , fa::commentaltcheck                 },
    { "comment-alt-dollar"                  , fa::commentaltdollar                },
    { "comment-alt-dots"                    , fa::commentaltdots                  },
    { "comment-alt-edit"                    , fa::commentaltedit                  },
    { "comment-alt-exclamation"             , fa::commentaltexclamation           },
    { "comment-alt-lines"                   , fa::commentaltlines                 },
    { "comment-alt-medical"                 , fa::commentaltmedical               },
    { "comment-alt-minus"                   , fa::commentaltminus                 },
    { "comment-alt-plus"                    , fa::commentaltplus                  },
    { "comment-alt-slash"                   , fa::commentaltslash                 },
    { "comment-alt-smile"                   , fa::commentaltsmile                 },
    { "comment-alt-times"                   , fa::commentalttimes                 },
    { "comment-check"                       , fa::commentcheck                    },
    { "comment-edit"                        , fa::commentedit                     },
    { "comment-exclamation"                 , fa::commentexclamation              },
    { "comment-lines"                       , fa::commentlines                    },
    { "comment-minus"                       , fa::commentminus                    },
    { "comment-plus"                        , fa::commentplus                     },
    { "comments-alt"                        , fa::commentsalt                     },
    { "comments-alt-dollar"                 , fa::commentsaltdollar               },
    { "comment-smile"                       , fa::commentsmile                    },
    { "comment-times"                       , fa::commenttimes                    },
    { "compass-slash"                       , fa::compassslash                    },
    { "compress-alt"                        , fa::compressalt                     },
    { "compress-wide"                       , fa::compresswide                    },
    { "construction"                        , fa::construction                    },
    { "container-storage"                   , fa::containerstorage                },
    { "conveyor-belt"                       , fa::conveyorbelt                    },
    { "conveyor-belt-alt"                   , fa::conveyorbeltalt                 },
    { "corn"                                , fa::corn                            },
    { "cow"                                 , fa::cow                             },
    { "credit-card-blank"                   , fa::creditcardblank                 },
    { "credit-card-front"                   , fa::creditcardfront                 },
    { "cricket"                             , fa::cricket                         },
    { "croissant"                           , fa::croissant                       },
    { "crutches"                            , fa::crutches                        },
    { "curling"                             , fa::curling                         },
    { "dagger"                              , fa::dagger                          },
    { "debug"                               , fa::debug                           },
    { "deer"                                , fa::deer                            },
    { "deer-rudolph"                        , fa::deerrudolph                     },
    { "desktop-alt"                         , fa::desktopalt                      },
    { "dewpoint"                            , fa::dewpoint                        },
    { "diamond"                             , fa::diamond                         },
    { "dice-d10"                            , fa::diced10                         },
    { "dice-d12"                            , fa::diced12                         },
    { "dice-d4"                             , fa::diced4                          },
    { "dice-d8"                             , fa::diced8                          },
    { "digging"                             , fa::digging                         },
    { "diploma"                             , fa::diploma                         },
    { "disease"                             , fa::disease                         },
    { "dog-leashed"                         , fa::dogleashed                      },
    { "dolly-empty"                         , fa::dollyempty                      },
    { "dolly-flatbed-alt"                   , fa::dollyflatbedalt                 },
    { "dolly-flatbed-empty"                 , fa::dollyflatbedempty               },
    { "do-not-enter"                        , fa::donotenter                      },
    { "draw-circle"                         , fa::drawcircle                      },
    { "draw-square"                         , fa::drawsquare                      },
    { "dreidel"                             , fa::dreidel                         },
    { "drone"                               , fa::drone                           },
    { "drone-alt"                           , fa::dronealt                        },
    { "drumstick"                           , fa::drumstick                       },
    { "dryer"                               , fa::dryer                           },
    { "dryer-alt"                           , fa::dryeralt                        },
    { "duck"                                , fa::duck                            },
    { "ear"                                 , fa::ear                             },
    { "ear-muffs"                           , fa::earmuffs                        },
    { "eclipse"                             , fa::eclipse                         },
    { "eclipse-alt"                         , fa::eclipsealt                      },
    { "egg-fried"                           , fa::eggfried                        },
    { "elephant"                            , fa::elephant                        },
    { "ellipsis-h-alt"                      , fa::ellipsishalt                    },
    { "ellipsis-v-alt"                      , fa::ellipsisvalt                    },
    { "empty-set"                           , fa::emptyset                        },
    { "engine-warning"                      , fa::enginewarning                   },
    { "envelope-open-dollar"                , fa::envelopeopendollar              },
    { "exchange"                            , fa::exchange                        },
    { "exclamation-square"                  , fa::exclamationsquare               },
    { "expand-alt"                          , fa::expandalt                       },
    { "expand-arrows"                       , fa::expandarrows                    },
    { "expand-wide"                         , fa::expandwide                      },
    { "external-link"                       , fa::externallink                    },
    { "external-link-square"                , fa::externallinksquare              },
    { "eye-evil"                            , fa::eyeevil                         },
    { "farm"                                , fa::farm                            },
    { "field-hockey"                        , fa::fieldhockey                     },
    { "file-certificate"                    , fa::filecertificate                 },
    { "file-chart-line"                     , fa::filechartline                   },
    { "file-chart-pie"                      , fa::filechartpie                    },
    { "file-check"                          , fa::filecheck                       },
    { "file-edit"                           , fa::fileedit                        },
    { "file-exclamation"                    , fa::fileexclamation                 },
    { "file-minus"                          , fa::fileminus                       },
    { "file-plus"                           , fa::fileplus                        },
    { "file-search"                         , fa::filesearch                      },
    { "files-medical"                       , fa::filesmedical                    },
    { "file-spreadsheet"                    , fa::filespreadsheet                 },
    { "file-times"                          , fa::filetimes                       },
    { "file-user"                           , fa::fileuser                        },
    { "film-alt"                            , fa::filmalt                         },
    { "fireplace"                           , fa::fireplace                       },
    { "fire-smoke"                          , fa::firesmoke                       },
    { "fish-cooked"                         , fa::fishcooked                      },
    { "flag-alt"                            , fa::flagalt                         },
    { "flame"                               , fa::flame                           },
    { "flask-poison"                        , fa::flaskpoison                     },
    { "flask-potion"                        , fa::flaskpotion                     },
    { "flower"                              , fa::flower                          },
    { "flower-daffodil"                     , fa::flowerdaffodil                  },
    { "flower-tulip"                        , fa::flowertulip                     },
    { "fog"                                 , fa::fog                             },
    { "folders"                             , fa::folders                         },
    { "folder-times"                        , fa::foldertimes                     },
    { "folder-tree"                         , fa::foldertree                      },
    { "font-case"                           , fa::fontcase                        },
    { "football-helmet"                     , fa::footballhelmet                  },
    { "forklift"                            , fa::forklift                        },
    { "fragile"                             , fa::fragile                         },
    { "french-fries"                        , fa::frenchfries                     },
    { "frosty-head"                         , fa::frostyhead                      },
    { "function"                            , fa::function                        },
    { "game-board"                          , fa::gameboard                       },
    { "game-board-alt"                      , fa::gameboardalt                    },
    { "gas-pump-slash"                      , fa::gaspumpslash                    },
    { "gift-card"                           , fa::giftcard                        },
    { "gingerbread-man"                     , fa::gingerbreadman                  },
    { "glass"                               , fa::glass                           },
    { "glass-champagne"                     , fa::glasschampagne                  },
    { "glass-citrus"                        , fa::glasscitrus                     },
    { "glasses-alt"                         , fa::glassesalt                      },
    { "glass-whiskey-rocks"                 , fa::glasswhiskeyrocks               },
    { "globe-snow"                          , fa::globesnow                       },
    { "globe-stand"                         , fa::globestand                      },
    { "golf-club"                           , fa::golfclub                        },
    { "h1"                                  , fa::h1                              },
    { "h2"                                  , fa::h2                              },
    { "h3"                                  , fa::h3                              },
    { "h4"                                  , fa::h4                              },
    { "hammer-war"                          , fa::hammerwar                       },
    { "hand-heart"                          , fa::handheart                       },
    { "hand-holding-box"                    , fa::handholdingbox                  },
    { "hand-holding-magic"                  , fa::handholdingmagic                },
    { "hand-holding-seedling"               , fa::handholdingseedling             },
    { "hand-holding-water"                  , fa::handholdingwater                },
    { "hand-receiving"                      , fa::handreceiving                   },
    { "handshake-alt"                       , fa::handshakealt                    },
    { "hands-heart"                         , fa::handsheart                      },
    { "hands-usd"                           , fa::handsusd                        },
    { "hat-chef"                            , fa::hatchef                         },
    { "hat-santa"                           , fa::hatsanta                        },
    { "hat-winter"                          , fa::hatwinter                       },
    { "hat-witch"                           , fa::hatwitch                        },
    { "head-side"                           , fa::headside                        },
    { "head-side-brain"                     , fa::headsidebrain                   },
    { "head-side-medical"                   , fa::headsidemedical                 },
    { "head-vr"                             , fa::headvr                          },
    { "heart-circle"                        , fa::heartcircle                     },
    { "heart-rate"                          , fa::heartrate                       },
    { "heart-square"                        , fa::heartsquare                     },
    { "helmet-battle"                       , fa::helmetbattle                    },
    { "hexagon"                             , fa::hexagon                         },
    { "hockey-mask"                         , fa::hockeymask                      },
    { "hockey-sticks"                       , fa::hockeysticks                    },
    { "home-alt"                            , fa::homealt                         },
    { "home-heart"                          , fa::homeheart                       },
    { "home-lg"                             , fa::homelg                          },
    { "home-lg-alt"                         , fa::homelgalt                       },
    { "hood-cloak"                          , fa::hoodcloak                       },
    { "horizontal-rule"                     , fa::horizontalrule                  },
    { "hospitals"                           , fa::hospitals                       },
    { "hospital-user"                       , fa::hospitaluser                    },
    { "house-flood"                         , fa::houseflood                      },
    { "humidity"                            , fa::humidity                        },
    { "hurricane"                           , fa::hurricane                       },
    { "ice-skate"                           , fa::iceskate                        },
    { "icons-alt"                           , fa::iconsalt                        },
    { "inbox-in"                            , fa::inboxin                         },
    { "inbox-out"                           , fa::inboxout                        },
    { "industry-alt"                        , fa::industryalt                     },
    { "info-square"                         , fa::infosquare                      },
    { "inhaler"                             , fa::inhaler                         },
    { "integral"                            , fa::integral                        },
    { "intersection"                        , fa::intersection                    },
    { "inventory"                           , fa::inventory                       },
    { "island-tropical"                     , fa::islandtropical                  },
    { "jack-o-lantern"                      , fa::jackolantern                    },
    { "kerning"                             , fa::kerning                         },
    { "keynote"                             , fa::keynote                         },
    { "key-skeleton"                        , fa::keyskeleton                     },
    { "kidneys"                             , fa::kidneys                         },
    { "kite"                                , fa::kite                            },
    { "knife-kitchen"                       , fa::knifekitchen                    },
    { "lambda"                              , fa::lambda                          },
    { "lamp"                                , fa::lamp                            },
    { "landmark-alt"                        , fa::landmarkalt                     },
    { "layer-minus"                         , fa::layerminus                      },
    { "layer-plus"                          , fa::layerplus                       },
    { "leaf-heart"                          , fa::leafheart                       },
    { "leaf-maple"                          , fa::leafmaple                       },
    { "leaf-oak"                            , fa::leafoak                         },
    { "level-down"                          , fa::leveldown                       },
    { "level-up"                            , fa::levelup                         },
    { "lightbulb-dollar"                    , fa::lightbulbdollar                 },
    { "lightbulb-exclamation"               , fa::lightbulbexclamation            },
    { "lightbulb-on"                        , fa::lightbulbon                     },
    { "lightbulb-slash"                     , fa::lightbulbslash                  },
    { "lights-holiday"                      , fa::lightsholiday                   },
    { "line-columns"                        , fa::linecolumns                     },
    { "line-height"                         , fa::lineheight                      },
    { "lips"                                , fa::lips                            },
    { "location"                            , fa::location                        },
    { "location-circle"                     , fa::locationcircle                  },
    { "location-slash"                      , fa::locationslash                   },
    { "lock-alt"                            , fa::lockalt                         },
    { "lock-open-alt"                       , fa::lockopenalt                     },
    { "long-arrow-down"                     , fa::longarrowdown                   },
    { "long-arrow-left"                     , fa::longarrowleft                   },
    { "long-arrow-right"                    , fa::longarrowright                  },
    { "long-arrow-up"                       , fa::longarrowup                     },
    { "loveseat"                            , fa::loveseat                        },
    { "luchador"                            , fa::luchador                        },
    { "lungs"                               , fa::lungs                           },
    { "mace"                                , fa::mace                            },
    { "mailbox"                             , fa::mailbox                         },
    { "mandolin"                            , fa::mandolin                        },
    { "map-marker-alt-slash"                , fa::mapmarkeraltslash               },
    { "map-marker-check"                    , fa::mapmarkercheck                  },
    { "map-marker-edit"                     , fa::mapmarkeredit                   },
    { "map-marker-exclamation"              , fa::mapmarkerexclamation            },
    { "map-marker-minus"                    , fa::mapmarkerminus                  },
    { "map-marker-plus"                     , fa::mapmarkerplus                   },
    { "map-marker-question"                 , fa::mapmarkerquestion               },
    { "map-marker-slash"                    , fa::mapmarkerslash                  },
    { "map-marker-smile"                    , fa::mapmarkersmile                  },
    { "map-marker-times"                    , fa::mapmarkertimes                  },
    { "meat"                                , fa::meat                            },
    { "megaphone"                           , fa::megaphone                       },
    { "mind-share"                          , fa::mindshare                       },
    { "minus-hexagon"                       , fa::minushexagon                    },
    { "minus-octagon"                       , fa::minusoctagon                    },
    { "mistletoe"                           , fa::mistletoe                       },
    { "mobile-android"                      , fa::mobileandroid                   },
    { "mobile-android-alt"                  , fa::mobileandroidalt                },
    { "money-check-edit"                    , fa::moneycheckedit                  },
    { "money-check-edit-alt"                , fa::moneycheckeditalt               },
    { "monitor-heart-rate"                  , fa::monitorheartrate                },
    { "monkey"                              , fa::monkey                          },
    { "moon-cloud"                          , fa::mooncloud                       },
    { "moon-stars"                          , fa::moonstars                       },
    { "mountains"                           , fa::mountains                       },
    { "mug"                                 , fa::mug                             },
    { "mug-marshmallows"                    , fa::mugmarshmallows                 },
    { "mug-tea"                             , fa::mugtea                          },
    { "narwhal"                             , fa::narwhal                         },
    { "octagon"                             , fa::octagon                         },
    { "oil-temp"                            , fa::oiltemp                         },
    { "omega"                               , fa::omega                           },
    { "ornament"                            , fa::ornament                        },
    { "overline"                            , fa::overline                        },
    { "page-break"                          , fa::pagebreak                       },
    { "paint-brush-alt"                     , fa::paintbrushalt                   },
    { "pallet-alt"                          , fa::palletalt                       },
    { "paragraph-rtl"                       , fa::paragraphrtl                    },
    { "parking-circle"                      , fa::parkingcircle                   },
    { "parking-circle-slash"                , fa::parkingcircleslash              },
    { "parking-slash"                       , fa::parkingslash                    },
    { "paw-alt"                             , fa::pawalt                          },
    { "paw-claws"                           , fa::pawclaws                        },
    { "pegasus"                             , fa::pegasus                         },
    { "pencil"                              , fa::pencil                          },
    { "pencil-paintbrush"                   , fa::pencilpaintbrush                },
    { "pennant"                             , fa::pennant                         },
    { "person-carry"                        , fa::personcarry                     },
    { "person-dolly"                        , fa::persondolly                     },
    { "person-dolly-empty"                  , fa::persondollyempty                },
    { "person-sign"                         , fa::personsign                      },
    { "phone-laptop"                        , fa::phonelaptop                     },
    { "phone-office"                        , fa::phoneoffice                     },
    { "phone-plus"                          , fa::phoneplus                       },
    { "pi"                                  , fa::pi                              },
    { "pie"                                 , fa::pie                             },
    { "pig"                                 , fa::pig                             },
    { "pizza"                               , fa::pizza                           },
    { "plane-alt"                           , fa::planealt                        },
    { "plus-hexagon"                        , fa::plushexagon                     },
    { "plus-octagon"                        , fa::plusoctagon                     },
    { "podium"                              , fa::podium                          },
    { "podium-star"                         , fa::podiumstar                      },
    { "poll-people"                         , fa::pollpeople                      },
    { "popcorn"                             , fa::popcorn                         },
    { "presentation"                        , fa::presentation                    },
    { "print-search"                        , fa::printsearch                     },
    { "print-slash"                         , fa::printslash                      },
    { "pumpkin"                             , fa::pumpkin                         },
    { "question-square"                     , fa::questionsquare                  },
    { "rabbit"                              , fa::rabbit                          },
    { "rabbit-fast"                         , fa::rabbitfast                      },
    { "racquet"                             , fa::racquet                         },
    { "raindrops"                           , fa::raindrops                       },
    { "ram"                                 , fa::ram                             },
    { "ramp-loading"                        , fa::ramploading                     },
    { "rectangle-landscape"                 , fa::rectanglelandscape              },
    { "rectangle-portrait"                  , fa::rectangleportrait               },
    { "rectangle-wide"                      , fa::rectanglewide                   },
    { "repeat"                              , fa::repeat                          },
    { "repeat-1"                            , fa::repeat1                         },
    { "repeat-1-alt"                        , fa::repeat1alt                      },
    { "repeat-alt"                          , fa::repeatalt                       },
    { "retweet-alt"                         , fa::retweetalt                      },
    { "rings-wedding"                       , fa::ringswedding                    },
    { "route-highway"                       , fa::routehighway                    },
    { "route-interstate"                    , fa::routeinterstate                 },
    { "ruler-triangle"                      , fa::rulertriangle                   },
    { "rv"                                  , fa::rv                              },
    { "sack"                                , fa::sack                            },
    { "sack-dollar"                         , fa::sackdollar                      },
    { "salad"                               , fa::salad                           },
    { "sandwich"                            , fa::sandwich                        },
    { "sausage"                             , fa::sausage                         },
    { "scalpel"                             , fa::scalpel                         },
    { "scalpel-path"                        , fa::scalpelpath                     },
    { "scanner"                             , fa::scanner                         },
    { "scanner-keyboard"                    , fa::scannerkeyboard                 },
    { "scanner-touchscreen"                 , fa::scannertouchscreen              },
    { "scarecrow"                           , fa::scarecrow                       },
    { "scarf"                               , fa::scarf                           },
    { "scroll-old"                          , fa::scrollold                       },
    { "scrubber"                            , fa::scrubber                        },
    { "scythe"                              , fa::scythe                          },
    { "send-back"                           , fa::sendback                        },
    { "send-backward"                       , fa::sendbackward                    },
    { "share-all"                           , fa::shareall                        },
    { "sheep"                               , fa::sheep                           },
    { "shield"                              , fa::shield                          },
    { "shield-check"                        , fa::shieldcheck                     },
    { "shield-cross"                        , fa::shieldcross                     },
    { "shipping-timed"                      , fa::shippingtimed                   },
    { "shish-kebab"                         , fa::shishkebab                      },
    { "shovel"                              , fa::shovel                          },
    { "shovel-snow"                         , fa::shovelsnow                      },
    { "shredder"                            , fa::shredder                        },
    { "shuttlecock"                         , fa::shuttlecock                     },
    { "sickle"                              , fa::sickle                          },
    { "sigma"                               , fa::sigma                           },
    { "signal-1"                            , fa::signal1                         },
    { "signal-2"                            , fa::signal2                         },
    { "signal-3"                            , fa::signal3                         },
    { "signal-4"                            , fa::signal4                         },
    { "signal-alt"                          , fa::signalalt                       },
    { "signal-alt-1"                        , fa::signalalt1                      },
    { "signal-alt-2"                        , fa::signalalt2                      },
    { "signal-alt-3"                        , fa::signalalt3                      },
    { "signal-alt-slash"                    , fa::signalaltslash                  },
    { "signal-slash"                        , fa::signalslash                     },
    { "sign-in"                             , fa::signin                          },
    { "sign-out"                            , fa::signout                         },
    { "skeleton"                            , fa::skeleton                        },
    { "ski-jump"                            , fa::skijump                         },
    { "ski-lift"                            , fa::skilift                         },
    { "sledding"                            , fa::sledding                        },
    { "sliders-h-square"                    , fa::slidershsquare                  },
    { "sliders-v"                           , fa::slidersv                        },
    { "sliders-v-square"                    , fa::slidersvsquare                  },
    { "smile-plus"                          , fa::smileplus                       },
    { "smoke"                               , fa::smoke                           },
    { "snake"                               , fa::snake                           },
    { "snooze"                              , fa::snooze                          },
    { "snow-blowing"                        , fa::snowblowing                     },
    { "snowflakes"                          , fa::snowflakes                      },
    { "snowmobile"                          , fa::snowmobile                      },
    { "sort-alt"                            , fa::sortalt                         },
    { "sort-shapes-down"                    , fa::sortshapesdown                  },
    { "sort-shapes-down-alt"                , fa::sortshapesdownalt               },
    { "sort-shapes-up"                      , fa::sortshapesup                    },
    { "sort-shapes-up-alt"                  , fa::sortshapesupalt                 },
    { "sort-size-down"                      , fa::sortsizedown                    },
    { "sort-size-down-alt"                  , fa::sortsizedownalt                 },
    { "sort-size-up"                        , fa::sortsizeup                      },
    { "sort-size-up-alt"                    , fa::sortsizeupalt                   },
    { "soup"                                , fa::soup                            },
    { "spade"                               , fa::spade                           },
    { "sparkles"                            , fa::sparkles                        },
    { "spider-black-widow"                  , fa::spiderblackwidow                },
    { "spider-web"                          , fa::spiderweb                       },
    { "spinner-third"                       , fa::spinnerthird                    },
    { "square-root"                         , fa::squareroot                      },
    { "squirrel"                            , fa::squirrel                        },
    { "staff"                               , fa::staff                           },
    { "star-christmas"                      , fa::starchristmas                   },
    { "star-exclamation"                    , fa::starexclamation                 },
    { "stars"                               , fa::stars                           },
    { "steak"                               , fa::steak                           },
    { "steering-wheel"                      , fa::steeringwheel                   },
    { "stocking"                            , fa::stocking                        },
    { "stomach"                             , fa::stomach                         },
    { "stretcher"                           , fa::stretcher                       },
    { "sun-cloud"                           , fa::suncloud                        },
    { "sun-dust"                            , fa::sundust                         },
    { "sunglasses"                          , fa::sunglasses                      },
    { "sun-haze"                            , fa::sunhaze                         },
    { "sunrise"                             , fa::sunrise                         },
    { "sunset"                              , fa::sunset                          },
    { "sword"                               , fa::sword                           },
    { "swords"                              , fa::swords                          },
    { "tablet-android"                      , fa::tabletandroid                   },
    { "tablet-android-alt"                  , fa::tabletandroidalt                },
    { "tablet-rugged"                       , fa::tabletrugged                    },
    { "tachometer"                          , fa::tachometer                      },
    { "tachometer-alt-average"              , fa::tachometeraltaverage            },
    { "tachometer-alt-fast"                 , fa::tachometeraltfast               },
    { "tachometer-alt-fastest"              , fa::tachometeraltfastest            },
    { "tachometer-alt-slow"                 , fa::tachometeraltslow               },
    { "tachometer-alt-slowest"              , fa::tachometeraltslowest            },
    { "tachometer-average"                  , fa::tachometeraverage               },
    { "tachometer-fast"                     , fa::tachometerfast                  },
    { "tachometer-fastest"                  , fa::tachometerfastest               },
    { "tachometer-slow"                     , fa::tachometerslow                  },
    { "tachometer-slowest"                  , fa::tachometerslowest               },
    { "taco"                                , fa::taco                            },
    { "tally"                               , fa::tally                           },
    { "tanakh"                              , fa::tanakh                          },
    { "tasks-alt"                           , fa::tasksalt                        },
    { "temperature-frigid"                  , fa::temperaturefrigid               },
    { "temperature-hot"                     , fa::temperaturehot                  },
    { "tennis-ball"                         , fa::tennisball                      },
    { "text"                                , fa::text                            },
    { "text-size"                           , fa::textsize                        },
    { "theta"                               , fa::theta                           },
    { "thunderstorm"                        , fa::thunderstorm                    },
    { "thunderstorm-moon"                   , fa::thunderstormmoon                },
    { "thunderstorm-sun"                    , fa::thunderstormsun                 },
    { "ticket"                              , fa::ticket                          },
    { "tilde"                               , fa::tilde                           },
    { "times-hexagon"                       , fa::timeshexagon                    },
    { "times-octagon"                       , fa::timesoctagon                    },
    { "times-square"                        , fa::timessquare                     },
    { "tire"                                , fa::tire                            },
    { "tire-flat"                           , fa::tireflat                        },
    { "tire-pressure-warning"               , fa::tirepressurewarning             },
    { "tire-rugged"                         , fa::tirerugged                      },
    { "toilet-paper-alt"                    , fa::toiletpaperalt                  },
    { "tombstone"                           , fa::tombstone                       },
    { "tombstone-alt"                       , fa::tombstonealt                    },
    { "toothbrush"                          , fa::toothbrush                      },
    { "tornado"                             , fa::tornado                         },
    { "traffic-cone"                        , fa::trafficcone                     },
    { "traffic-light-go"                    , fa::trafficlightgo                  },
    { "traffic-light-slow"                  , fa::trafficlightslow                },
    { "traffic-light-stop"                  , fa::trafficlightstop                },
    { "trash-undo"                          , fa::trashundo                       },
    { "trash-undo-alt"                      , fa::trashundoalt                    },
    { "treasure-chest"                      , fa::treasurechest                   },
    { "tree-alt"                            , fa::treealt                         },
    { "tree-christmas"                      , fa::treechristmas                   },
    { "tree-decorated"                      , fa::treedecorated                   },
    { "tree-large"                          , fa::treelarge                       },
    { "tree-palm"                           , fa::treepalm                        },
    { "trees"                               , fa::trees                           },
    { "triangle"                            , fa::triangle                        },
    { "trophy-alt"                          , fa::trophyalt                       },
    { "truck-container"                     , fa::truckcontainer                  },
    { "truck-couch"                         , fa::truckcouch                      },
    { "truck-plow"                          , fa::truckplow                       },
    { "truck-ramp"                          , fa::truckramp                       },
    { "turkey"                              , fa::turkey                          },
    { "turtle"                              , fa::turtle                          },
    { "tv-retro"                            , fa::tvretro                         },
    { "unicorn"                             , fa::unicorn                         },
    { "union"                               , fa::unionp                          },
    { "usd-circle"                          , fa::usdcircle                       },
    { "usd-square"                          , fa::usdsquare                       },
    { "user-chart"                          , fa::userchart                       },
    { "user-crown"                          , fa::usercrown                       },
    { "user-hard-hat"                       , fa::userhardhat                     },
    { "user-headset"                        , fa::userheadset                     },
    { "user-md-chat"                        , fa::usermdchat                      },
    { "users-class"                         , fa::usersclass                      },
    { "users-crown"                         , fa::userscrown                      },
    { "users-medical"                       , fa::usersmedical                    },
    { "utensil-fork"                        , fa::utensilfork                     },
    { "utensil-knife"                       , fa::utensilknife                    },
    { "utensils-alt"                        , fa::utensilsalt                     },
    { "value-absolute"                      , fa::valueabsolute                   },
    { "video-plus"                          , fa::videoplus                       },
    { "volcano"                             , fa::volcano                         },
    { "volume"                              , fa::volume                          },
    { "volume-slash"                        , fa::volumeslash                     },
    { "vote-nay"                            , fa::votenay                         },
    { "walker"                              , fa::walker                          },
    { "wand"                                , fa::wand                            },
    { "wand-magic"                          , fa::wandmagic                       },
    { "warehouse-alt"                       , fa::warehousealt                    },
    { "washer"                              , fa::washer                          },
    { "watch"                               , fa::watch                           },
    { "watch-fitness"                       , fa::watchfitness                    },
    { "water-lower"                         , fa::waterlower                      },
    { "water-rise"                          , fa::waterrise                       },
    { "wave-sine"                           , fa::wavesine                        },
    { "wave-triangle"                       , fa::wavetriangle                    },
    { "webcam"                              , fa::webcam                          },
    { "webcam-slash"                        , fa::webcamslash                     },
    { "whale"                               , fa::whale                           },
    { "wheat"                               , fa::wheat                           },
    { "whistle"                             , fa::whistle                         },
    { "wifi-1"                              , fa::wifi1                           },
    { "wifi-2"                              , fa::wifi2                           },
    { "wifi-slash"                          , fa::wifislash                       },
    { "window"                              , fa::window                          },
    { "window-alt"                          , fa::windowalt                       },
    { "windsock"                            , fa::windsock                        },
    { "wind-turbine"                        , fa::windturbine                     },
    { "wind-warning"                        , fa::windwarning                     },
    { "wreath"                              , fa::wreath                          }
};
#endif
static const FANameIcon faCommonIconArray[] = {
    { "address-book"                        , fa::addressbook                      }, //First free-regular icon
    { "address-card"                        , fa::addresscard                      },
    { "angry"                               , fa::angry                            },
    { "arrow-alt-circle-down"               , fa::arrowaltcircledown               },
    { "arrow-alt-circle-left"               , fa::arrowaltcircleleft               },
    { "arrow-alt-circle-right"              , fa::arrowaltcircleright              },
    { "arrow-alt-circle-up"                 , fa::arrowaltcircleup                 },
    { "bell"                                , fa::bell                             },
    { "bell-slash"                          , fa::bellslash                        },
    { "bookmark"                            , fa::bookmark                         },
    { "building"                            , fa::building                         },
    { "calendar"                            , fa::calendar                         },
    { "calendar-alt"                        , fa::calendaralt                      },
    { "calendar-check"                      , fa::calendarcheck                    },
    { "calendar-minus"                      , fa::calendarminus                    },
    { "calendar-plus"                       , fa::calendarplus                     },
    { "calendar-times"                      , fa::calendartimes                    },
    { "caret-square-down"                   , fa::caretsquaredown                  },
    { "caret-square-left"                   , fa::caretsquareleft                  },
    { "caret-square-right"                  , fa::caretsquareright                 },
    { "caret-square-up"                     , fa::caretsquareup                    },
    { "chart-bar"                           , fa::chartbar                         },
    { "check-circle"                        , fa::checkcircle                      },
    { "check-square"                        , fa::checksquare                      },
    { "circle"                              , fa::circle                           },
    { "clipboard"                           , fa::clipboard                        },
    { "clock"                               , fa::clock                            },
    { "clone"                               , fa::clone                            },
    { "closed-captioning"                   , fa::closedcaptioning                 },
    { "comment"                             , fa::comment                          },
    { "comment-alt"                         , fa::commentalt                       },
    { "comment-dots"                        , fa::commentdots                      },
    { "comments"                            , fa::comments                         },
    { "compass"                             , fa::compass                          },
    { "copy"                                , fa::copy                             },
    { "copyright"                           , fa::copyright                        },
    { "credit-card"                         , fa::creditcard                       },
    { "dizzy"                               , fa::dizzy                            },
    { "dot-circle"                          , fa::dotcircle                        },
    { "edit"                                , fa::edit                             },
    { "envelope"                            , fa::envelope                         },
    { "envelope-open"                       , fa::envelopeopen                     },
    { "eye"                                 , fa::eye                              },
    { "eye-slash"                           , fa::eyeslash                         },
    { "file"                                , fa::file                             },
    { "file-alt"                            , fa::filealt                          },
    { "file-archive"                        , fa::filearchive                      },
    { "file-audio"                          , fa::fileaudio                        },
    { "file-code"                           , fa::filecode                         },
    { "file-excel"                          , fa::fileexcel                        },
    { "file-image"                          , fa::fileimage                        },
    { "file-pdf"                            , fa::filepdf                          },
    { "file-powerpoint"                     , fa::filepowerpoint                   },
    { "file-video"                          , fa::filevideo                        },
    { "file-word"                           , fa::fileword                         },
    { "flag"                                , fa::flag                             },
    { "flushed"                             , fa::flushed                          },
    { "folder"                              , fa::folder                           },
    { "folder-open"                         , fa::folderopen                       },
    { "frown"                               , fa::frown                            },
    { "frown-open"                          , fa::frownopen                        },
    { "futbol"                              , fa::futbol                           },
    { "gem"                                 , fa::gem                              },
    { "grimace"                             , fa::grimace                          },
    { "grin"                                , fa::grin                             },
    { "grin-alt"                            , fa::grinalt                          },
    { "grin-beam"                           , fa::grinbeam                         },
    { "grin-beam-sweat"                     , fa::grinbeamsweat                    },
    { "grin-hearts"                         , fa::grinhearts                       },
    { "grin-squint"                         , fa::grinsquint                       },
    { "grin-squint-tears"                   , fa::grinsquinttears                  },
    { "grin-stars"                          , fa::grinstars                        },
    { "grin-tears"                          , fa::grintears                        },
    { "grin-tongue"                         , fa::grintongue                       },
    { "grin-tongue-squint"                  , fa::grintonguesquint                 },
    { "grin-tongue-wink"                    , fa::grintonguewink                   },
    { "grin-wink"                           , fa::grinwink                         },
    { "hand-lizard"                         , fa::handlizard                       },
    { "hand-paper"                          , fa::handpaper                        },
    { "hand-peace"                          , fa::handpeace                        },
    { "hand-point-down"                     , fa::handpointdown                    },
    { "hand-point-left"                     , fa::handpointleft                    },
    { "hand-point-right"                    , fa::handpointright                   },
    { "hand-point-up"                       , fa::handpointup                      },
    { "hand-pointer"                        , fa::handpointer                      },
    { "hand-rock"                           , fa::handrock                         },
    { "hand-scissors"                       , fa::handscissors                     },
    { "hand-spock"                          , fa::handspock                        },
    { "handshake"                           , fa::handshake                        },
    { "hdd"                                 , fa::hdd                              },
    { "heart"                               , fa::heart                            },
    { "hospital"                            , fa::hospital                         },
    { "hourglass"                           , fa::hourglass                        },
    { "id-badge"                            , fa::idbadge                          },
    { "id-card"                             , fa::idcard                           },
    { "image"                               , fa::image                            },
    { "images"                              , fa::images                           },
    { "keyboard"                            , fa::keyboard                         },
    { "kiss"                                , fa::kiss                             },
    { "kiss-beam"                           , fa::kissbeam                         },
    { "kiss-wink-heart"                     , fa::kisswinkheart                    },
    { "laugh"                               , fa::laugh                            },
    { "laugh-beam"                          , fa::laughbeam                        },
    { "laugh-squint"                        , fa::laughsquint                      },
    { "laugh-wink"                          , fa::laughwink                        },
    { "lemon"                               , fa::lemon                            },
    { "life-ring"                           , fa::lifering                         },
    { "lightbulb"                           , fa::lightbulb                        },
    { "list-alt"                            , fa::listalt                          },
    { "map"                                 , fa::map                              },
    { "meh"                                 , fa::meh                              },
    { "meh-blank"                           , fa::mehblank                         },
    { "meh-rolling-eyes"                    , fa::mehrollingeyes                   },
    { "minus-square"                        , fa::minussquare                      },
    { "money-bill-alt"                      , fa::moneybillalt                     },
    { "moon"                                , fa::moon                             },
    { "newspaper"                           , fa::newspaper                        },
    { "object-group"                        , fa::objectgroup                      },
    { "object-ungroup"                      , fa::objectungroup                    },
    { "paper-plane"                         , fa::paperplane                       },
    { "pause-circle"                        , fa::pausecircle                      },
    { "play-circle"                         , fa::playcircle                       },
    { "plus-square"                         , fa::plussquare                       },
    { "question-circle"                     , fa::questioncircle                   },
    { "registered"                          , fa::registered                       },
    { "sad-cry"                             , fa::sadcry                           },
    { "sad-tear"                            , fa::sadtear                          },
    { "save"                                , fa::save                             },
    { "share-square"                        , fa::sharesquare                      },
    { "smile"                               , fa::smile                            },
    { "smile-beam"                          , fa::smilebeam                        },
    { "smile-wink"                          , fa::smilewink                        },
    { "snowflake"                           , fa::snowflake                        },
    { "square"                              , fa::square                           },
    { "star"                                , fa::star                             },
    { "star-half"                           , fa::starhalf                         },
    { "sticky-note"                         , fa::stickynote                       },
    { "stop-circle"                         , fa::stopcircle                       },
    { "sun"                                 , fa::sun                              },
    { "surprise"                            , fa::surprise                         },
    { "thumbs-down"                         , fa::thumbsdown                       },
    { "thumbs-up"                           , fa::thumbsup                         },
    { "times-circle"                        , fa::timescircle                      },
    { "tired"                               , fa::tired                            },
    { "trash-alt"                           , fa::trashalt                         },
    { "user"                                , fa::user                             },
    { "user-circle"                         , fa::usercircle                       },
    { "window-close"                        , fa::windowclose                      },
    { "window-maximize"                     , fa::windowmaximize                   },
    { "window-minimize"                     , fa::windowminimize                   },
    { "window-restore"                      , fa::windowrestore                    }, //last free-regular icon
    { "ad"                                  , fa::ad                               },
    { "adjust"                              , fa::adjust                           },
    { "air-freshener"                       , fa::airfreshener                     },
    { "align-center"                        , fa::aligncenter                      },
    { "align-justify"                       , fa::alignjustify                     },
    { "align-left"                          , fa::alignleft                        },
    { "align-right"                         , fa::alignright                       },
    { "allergies"                           , fa::allergies                        },
    { "ambulance"                           , fa::ambulance                        },
    { "american-sign-language-interpreting" , fa::americansignlanguageinterpreting },
    { "anchor"                              , fa::anchor                           },
    { "angle-double-down"                   , fa::angledoubledown                  },
    { "angle-double-left"                   , fa::angledoubleleft                  },
    { "angle-double-right"                  , fa::angledoubleright                 },
    { "angle-double-up"                     , fa::angledoubleup                    },
    { "angle-down"                          , fa::angledown                        },
    { "angle-left"                          , fa::angleleft                        },
    { "angle-right"                         , fa::angleright                       },
    { "angle-up"                            , fa::angleup                          },
    { "ankh"                                , fa::ankh                             },
    { "apple-alt"                           , fa::applealt                         },
    { "archive"                             , fa::archive                          },
    { "archway"                             , fa::archway                          },
    { "arrow-circle-down"                   , fa::arrowcircledown                  },
    { "arrow-circle-left"                   , fa::arrowcircleleft                  },
    { "arrow-circle-right"                  , fa::arrowcircleright                 },
    { "arrow-circle-up"                     , fa::arrowcircleup                    },
    { "arrow-down"                          , fa::arrowdown                        },
    { "arrow-left"                          , fa::arrowleft                        },
    { "arrow-right"                         , fa::arrowright                       },
    { "arrows-alt"                          , fa::arrowsalt                        },
    { "arrows-alt-h"                        , fa::arrowsalth                       },
    { "arrows-alt-v"                        , fa::arrowsaltv                       },
    { "arrow-up"                            , fa::arrowup                          },
    { "assistive-listening-systems"         , fa::assistivelisteningsystems        },
    { "asterisk"                            , fa::asterisk                         },
    { "at"                                  , fa::at                               },
    { "atlas"                               , fa::atlas                            },
    { "atom"                                , fa::atom                             },
    { "audio-description"                   , fa::audiodescription                 },
    { "award"                               , fa::award                            },
    { "baby"                                , fa::baby                             },
    { "baby-carriage"                       , fa::babycarriage                     },
    { "backspace"                           , fa::backspace                        },
    { "backward"                            , fa::backward                         },
    { "bacon"                               , fa::bacon                            },
    { "balance-scale"                       , fa::balancescale                     },
    { "balance-scale-left"                  , fa::balancescaleleft                 },
    { "balance-scale-right"                 , fa::balancescaleright                },
    { "ban"                                 , fa::ban                              },
    { "band-aid"                            , fa::bandaid                          },
    { "barcode"                             , fa::barcode                          },
    { "bars"                                , fa::bars                             },
    { "baseball-ball"                       , fa::baseballball                     },
    { "basketball-ball"                     , fa::basketballball                   },
    { "bath"                                , fa::bath                             },
    { "battery-empty"                       , fa::batteryempty                     },
    { "battery-full"                        , fa::batteryfull                      },
    { "battery-half"                        , fa::batteryhalf                      },
    { "battery-quarter"                     , fa::batteryquarter                   },
    { "battery-three-quarters"              , fa::batterythreequarters             },
    { "bed"                                 , fa::bed                              },
    { "beer"                                , fa::beer                             },
    { "bezier-curve"                        , fa::beziercurve                      },
    { "bible"                               , fa::bible                            },
    { "bicycle"                             , fa::bicycle                          },
    { "biking"                              , fa::biking                           },
    { "binoculars"                          , fa::binoculars                       },
    { "biohazard"                           , fa::biohazard                        },
    { "birthday-cake"                       , fa::birthdaycake                     },
    { "blender"                             , fa::blender                          },
    { "blender-phone"                       , fa::blenderphone                     },
    { "blind"                               , fa::blind                            },
    { "blog"                                , fa::blog                             },
    { "bold"                                , fa::bold                             },
    { "bolt"                                , fa::bolt                             },
    { "bomb"                                , fa::bomb                             },
    { "bone"                                , fa::bone                             },
    { "bong"                                , fa::bong                             },
    { "book"                                , fa::book                             },
    { "book-dead"                           , fa::bookdead                         },
    { "book-medical"                        , fa::bookmedical                      },
    { "book-open"                           , fa::bookopen                         },
    { "book-reader"                         , fa::bookreader                       },
    { "border-all"                          , fa::borderall                        },
    { "border-none"                         , fa::bordernone                       },
    { "border-style"                        , fa::borderstyle                      },
    { "bowling-ball"                        , fa::bowlingball                      },
    { "box"                                 , fa::box                              },
    { "boxes"                               , fa::boxes                            },
    { "box-open"                            , fa::boxopen                          },
    { "braille"                             , fa::braille                          },
    { "brain"                               , fa::brain                            },
    { "bread-slice"                         , fa::breadslice                       },
    { "briefcase"                           , fa::briefcase                        },
    { "briefcase-medical"                   , fa::briefcasemedical                 },
    { "broadcast-tower"                     , fa::broadcasttower                   },
    { "broom"                               , fa::broom                            },
    { "brush"                               , fa::brush                            },
    { "bug"                                 , fa::bug                              },
    { "bullhorn"                            , fa::bullhorn                         },
    { "bullseye"                            , fa::bullseye                         },
    { "burn"                                , fa::burn                             },
    { "bus"                                 , fa::bus                              },
    { "bus-alt"                             , fa::busalt                           },
    { "business-time"                       , fa::businesstime                     },
    { "calculator"                          , fa::calculator                       },
    { "calendar-day"                        , fa::calendarday                      },
    { "calendar-week"                       , fa::calendarweek                     },
    { "camera"                              , fa::camera                           },
    { "camera-retro"                        , fa::cameraretro                      },
    { "campground"                          , fa::campground                       },
    { "candy-cane"                          , fa::candycane                        },
    { "cannabis"                            , fa::cannabis                         },
    { "capsules"                            , fa::capsules                         },
    { "car"                                 , fa::car                              },
    { "car-alt"                             , fa::caralt                           },
    { "car-battery"                         , fa::carbattery                       },
    { "car-crash"                           , fa::carcrash                         },
    { "caret-down"                          , fa::caretdown                        },
    { "caret-left"                          , fa::caretleft                        },
    { "caret-right"                         , fa::caretright                       },
    { "caret-up"                            , fa::caretup                          },
    { "carrot"                              , fa::carrot                           },
    { "car-side"                            , fa::carside                          },
    { "cart-arrow-down"                     , fa::cartarrowdown                    },
    { "cart-plus"                           , fa::cartplus                         },
    { "cash-register"                       , fa::cashregister                     },
    { "cat"                                 , fa::cat                              },
    { "certificate"                         , fa::certificate                      },
    { "chair"                               , fa::chair                            },
    { "chalkboard"                          , fa::chalkboard                       },
    { "chalkboard-teacher"                  , fa::chalkboardteacher                },
    { "charging-station"                    , fa::chargingstation                  },
    { "chart-area"                          , fa::chartarea                        },
    { "chart-line"                          , fa::chartline                        },
    { "chart-pie"                           , fa::chartpie                         },
    { "check"                               , fa::check                            },
    { "check-double"                        , fa::checkdouble                      },
    { "cheese"                              , fa::cheese                           },
    { "chess"                               , fa::chess                            },
    { "chess-bishop"                        , fa::chessbishop                      },
    { "chess-board"                         , fa::chessboard                       },
    { "chess-king"                          , fa::chessking                        },
    { "chess-knight"                        , fa::chessknight                      },
    { "chess-pawn"                          , fa::chesspawn                        },
    { "chess-queen"                         , fa::chessqueen                       },
    { "chess-rook"                          , fa::chessrook                        },
    { "chevron-circle-down"                 , fa::chevroncircledown                },
    { "chevron-circle-left"                 , fa::chevroncircleleft                },
    { "chevron-circle-right"                , fa::chevroncircleright               },
    { "chevron-circle-up"                   , fa::chevroncircleup                  },
    { "chevron-down"                        , fa::chevrondown                      },
    { "chevron-left"                        , fa::chevronleft                      },
    { "chevron-right"                       , fa::chevronright                     },
    { "chevron-up"                          , fa::chevronup                        },
    { "child"                               , fa::child                            },
    { "church"                              , fa::church                           },
    { "circle-notch"                        , fa::circlenotch                      },
    { "city"                                , fa::city                             },
    { "clinic-medical"                      , fa::clinicmedical                    },
    { "clipboard-check"                     , fa::clipboardcheck                   },
    { "clipboard-list"                      , fa::clipboardlist                    },
    { "cloud"                               , fa::cloud                            },
    { "cloud-download-alt"                  , fa::clouddownloadalt                 },
    { "cloud-meatball"                      , fa::cloudmeatball                    },
    { "cloud-moon"                          , fa::cloudmoon                        },
    { "cloud-moon-rain"                     , fa::cloudmoonrain                    },
    { "cloud-rain"                          , fa::cloudrain                        },
    { "cloud-showers-heavy"                 , fa::cloudshowersheavy                },
    { "cloud-sun"                           , fa::cloudsun                         },
    { "cloud-sun-rain"                      , fa::cloudsunrain                     },
    { "cloud-upload-alt"                    , fa::clouduploadalt                   },
    { "cocktail"                            , fa::cocktail                         },
    { "code"                                , fa::code                             },
    { "code-branch"                         , fa::codebranch                       },
    { "coffee"                              , fa::coffee                           },
    { "cog"                                 , fa::cog                              },
    { "cogs"                                , fa::cogs                             },
    { "coins"                               , fa::coins                            },
    { "columns"                             , fa::columns                          },
    { "comment-dollar"                      , fa::commentdollar                    },
    { "comment-medical"                     , fa::commentmedical                   },
    { "comments-dollar"                     , fa::commentsdollar                   },
    { "comment-slash"                       , fa::commentslash                     },
    { "compact-disc"                        , fa::compactdisc                      },
    { "compress"                            , fa::compress                         },
    { "compress-arrows-alt"                 , fa::compressarrowsalt                },
    { "concierge-bell"                      , fa::conciergebell                    },
    { "cookie"                              , fa::cookie                           },
    { "cookie-bite"                         , fa::cookiebite                       },
    { "couch"                               , fa::couch                            },
    { "crop"                                , fa::crop                             },
    { "crop-alt"                            , fa::cropalt                          },
    { "cross"                               , fa::cross                            },
    { "crosshairs"                          , fa::crosshairs                       },
    { "crow"                                , fa::crow                             },
    { "crown"                               , fa::crown                            },
    { "crutch"                              , fa::crutch                           },
    { "cube"                                , fa::cube                             },
    { "cubes"                               , fa::cubes                            },
    { "cut"                                 , fa::cut                              },
    { "database"                            , fa::database                         },
    { "deaf"                                , fa::deaf                             },
    { "democrat"                            , fa::democrat                         },
    { "desktop"                             , fa::desktop                          },
    { "dharmachakra"                        , fa::dharmachakra                     },
    { "diagnoses"                           , fa::diagnoses                        },
    { "dice"                                , fa::dice                             },
    { "dice-d20"                            , fa::diced20                          },
    { "dice-d6"                             , fa::diced6                           },
    { "dice-five"                           , fa::dicefive                         },
    { "dice-four"                           , fa::dicefour                         },
    { "dice-one"                            , fa::diceone                          },
    { "dice-six"                            , fa::dicesix                          },
    { "dice-three"                          , fa::dicethree                        },
    { "dice-two"                            , fa::dicetwo                          },
    { "digital-tachograph"                  , fa::digitaltachograph                },
    { "directions"                          , fa::directions                       },
    { "divide"                              , fa::divide                           },
    { "dna"                                 , fa::dna                              },
    { "dog"                                 , fa::dog                              },
    { "dollar-sign"                         , fa::dollarsign                       },
    { "dolly"                               , fa::dolly                            },
    { "dolly-flatbed"                       , fa::dollyflatbed                     },
    { "donate"                              , fa::donate                           },
    { "door-closed"                         , fa::doorclosed                       },
    { "door-open"                           , fa::dooropen                         },
    { "dove"                                , fa::dove                             },
    { "download"                            , fa::download                         },
    { "drafting-compass"                    , fa::draftingcompass                  },
    { "dragon"                              , fa::dragon                           },
    { "draw-polygon"                        , fa::drawpolygon                      },
    { "drum"                                , fa::drum                             },
    { "drum-steelpan"                       , fa::drumsteelpan                     },
    { "drumstick-bite"                      , fa::drumstickbite                    },
    { "dumbbell"                            , fa::dumbbell                         },
    { "dumpster"                            , fa::dumpster                         },
    { "dumpster-fire"                       , fa::dumpsterfire                     },
    { "dungeon"                             , fa::dungeon                          },
    { "egg"                                 , fa::egg                              },
    { "eject"                               , fa::eject                            },
    { "ellipsis-h"                          , fa::ellipsish                        },
    { "ellipsis-v"                          , fa::ellipsisv                        },
    { "envelope-open-text"                  , fa::envelopeopentext                 },
    { "envelope-square"                     , fa::envelopesquare                   },
    { "equals"                              , fa::equals                           },
    { "eraser"                              , fa::eraser                           },
    { "ethernet"                            , fa::ethernet                         },
    { "euro-sign"                           , fa::eurosign                         },
    { "exchange-alt"                        , fa::exchangealt                      },
    { "exclamation"                         , fa::exclamation                      },
    { "exclamation-circle"                  , fa::exclamationcircle                },
    { "exclamation-triangle"                , fa::exclamationtriangle              },
    { "expand"                              , fa::expand                           },
    { "expand-arrows-alt"                   , fa::expandarrowsalt                  },
    { "external-link-alt"                   , fa::externallinkalt                  },
    { "external-link-square-alt"            , fa::externallinksquarealt            },
    { "eye-dropper"                         , fa::eyedropper                       },
    { "fan"                                 , fa::fan                              },
    { "fast-backward"                       , fa::fastbackward                     },
    { "fast-forward"                        , fa::fastforward                      },
    { "fax"                                 , fa::fax                              },
    { "feather"                             , fa::feather                          },
    { "feather-alt"                         , fa::featheralt                       },
    { "female"                              , fa::female                           },
    { "fighter-jet"                         , fa::fighterjet                       },
    { "file-contract"                       , fa::filecontract                     },
    { "file-csv"                            , fa::filecsv                          },
    { "file-download"                       , fa::filedownload                     },
    { "file-export"                         , fa::fileexport                       },
    { "file-import"                         , fa::fileimport                       },
    { "file-invoice"                        , fa::fileinvoice                      },
    { "file-invoice-dollar"                 , fa::fileinvoicedollar                },
    { "file-medical"                        , fa::filemedical                      },
    { "file-medical-alt"                    , fa::filemedicalalt                   },
    { "file-prescription"                   , fa::fileprescription                 },
    { "file-signature"                      , fa::filesignature                    },
    { "file-upload"                         , fa::fileupload                       },
    { "fill"                                , fa::fill                             },
    { "fill-drip"                           , fa::filldrip                         },
    { "film"                                , fa::film                             },
    { "filter"                              , fa::filter                           },
    { "fingerprint"                         , fa::fingerprint                      },
    { "fire"                                , fa::fire                             },
    { "fire-alt"                            , fa::firealt                          },
    { "fire-extinguisher"                   , fa::fireextinguisher                 },
    { "first-aid"                           , fa::firstaid                         },
    { "fish"                                , fa::fish                             },
    { "fist-raised"                         , fa::fistraised                       },
    { "flag-checkered"                      , fa::flagcheckered                    },
    { "flag-usa"                            , fa::flagusa                          },
    { "flask"                               , fa::flask                            },
    { "folder-minus"                        , fa::folderminus                      },
    { "folder-plus"                         , fa::folderplus                       },
    { "font"                                , fa::font                             },
    { "football-ball"                       , fa::footballball                     },
    { "forward"                             , fa::forward                          },
    { "frog"                                , fa::frog                             },
    { "funnel-dollar"                       , fa::funneldollar                     },
    { "gamepad"                             , fa::gamepad                          },
    { "gas-pump"                            , fa::gaspump                          },
    { "gavel"                               , fa::gavel                            },
    { "genderless"                          , fa::genderless                       },
    { "ghost"                               , fa::ghost                            },
    { "gift"                                , fa::gift                             },
    { "gifts"                               , fa::gifts                            },
    { "glass-cheers"                        , fa::glasscheers                      },
    { "glasses"                             , fa::glasses                          },
    { "glass-martini"                       , fa::glassmartini                     },
    { "glass-martini-alt"                   , fa::glassmartinialt                  },
    { "glass-whiskey"                       , fa::glasswhiskey                     },
    { "globe"                               , fa::globe                            },
    { "globe-africa"                        , fa::globeafrica                      },
    { "globe-americas"                      , fa::globeamericas                    },
    { "globe-asia"                          , fa::globeasia                        },
    { "globe-europe"                        , fa::globeeurope                      },
    { "golf-ball"                           , fa::golfball                         },
    { "gopuram"                             , fa::gopuram                          },
    { "graduation-cap"                      , fa::graduationcap                    },
    { "greater-than"                        , fa::greaterthan                      },
    { "greater-than-equal"                  , fa::greaterthanequal                 },
    { "grip-horizontal"                     , fa::griphorizontal                   },
    { "grip-lines"                          , fa::griplines                        },
    { "grip-lines-vertical"                 , fa::griplinesvertical                },
    { "grip-vertical"                       , fa::gripvertical                     },
    { "guitar"                              , fa::guitar                           },
    { "hamburger"                           , fa::hamburger                        },
    { "hammer"                              , fa::hammer                           },
    { "hamsa"                               , fa::hamsa                            },
    { "hand-holding"                        , fa::handholding                      },
    { "hand-holding-heart"                  , fa::handholdingheart                 },
    { "hand-holding-usd"                    , fa::handholdingusd                   },
    { "hand-middle-finger"                  , fa::handmiddlefinger                 },
    { "hands"                               , fa::hands                            },
    { "hands-helping"                       , fa::handshelping                     },
    { "hanukiah"                            , fa::hanukiah                         },
    { "hard-hat"                            , fa::hardhat                          },
    { "hashtag"                             , fa::hashtag                          },
    { "hat-wizard"                          , fa::hatwizard                        },
    { "haykal"                              , fa::haykal                           },
    { "heading"                             , fa::heading                          },
    { "headphones"                          , fa::headphones                       },
    { "headphones-alt"                      , fa::headphonesalt                    },
    { "headset"                             , fa::headset                          },
    { "heartbeat"                           , fa::heartbeat                        },
    { "heart-broken"                        , fa::heartbroken                      },
    { "helicopter"                          , fa::helicopter                       },
    { "highlighter"                         , fa::highlighter                      },
    { "hiking"                              , fa::hiking                           },
    { "hippo"                               , fa::hippo                            },
    { "history"                             , fa::history                          },
    { "hockey-puck"                         , fa::hockeypuck                       },
    { "holly-berry"                         , fa::hollyberry                       },
    { "home"                                , fa::home                             },
    { "horse"                               , fa::horse                            },
    { "horse-head"                          , fa::horsehead                        },
    { "hospital-alt"                        , fa::hospitalalt                      },
    { "hospital-symbol"                     , fa::hospitalsymbol                   },
    { "hotdog"                              , fa::hotdog                           },
    { "hotel"                               , fa::hotel                            },
    { "hot-tub"                             , fa::hottub                           },
    { "hourglass-end"                       , fa::hourglassend                     },
    { "hourglass-half"                      , fa::hourglasshalf                    },
    { "hourglass-start"                     , fa::hourglassstart                   },
    { "house-damage"                        , fa::housedamage                      },
    { "hryvnia"                             , fa::hryvnia                          },
    { "h-square"                            , fa::hsquare                          },
    { "ice-cream"                           , fa::icecream                         },
    { "icicles"                             , fa::icicles                          },
    { "icons"                               , fa::icons                            },
    { "i-cursor"                            , fa::icursor                          },
    { "id-card-alt"                         , fa::idcardalt                        },
    { "igloo"                               , fa::igloo                            },
    { "inbox"                               , fa::inbox                            },
    { "indent"                              , fa::indent                           },
    { "industry"                            , fa::industry                         },
    { "infinity"                            , fa::infinity                         },
    { "info"                                , fa::info                             },
    { "info-circle"                         , fa::infocircle                       },
    { "italic"                              , fa::italic                           },
    { "jedi"                                , fa::jedi                             },
    { "joint"                               , fa::joint                            },
    { "journal-whills"                      , fa::journalwhills                    },
    { "kaaba"                               , fa::kaaba                            },
    { "key"                                 , fa::key                              },
    { "khanda"                              , fa::khanda                           },
    { "kiwi-bird"                           , fa::kiwibird                         },
    { "landmark"                            , fa::landmark                         },
    { "language"                            , fa::language                         },
    { "laptop"                              , fa::laptop                           },
    { "laptop-code"                         , fa::laptopcode                       },
    { "laptop-medical"                      , fa::laptopmedical                    },
    { "layer-group"                         , fa::layergroup                       },
    { "leaf"                                , fa::leaf                             },
    { "less-than"                           , fa::lessthan                         },
    { "less-than-equal"                     , fa::lessthanequal                    },
    { "level-down-alt"                      , fa::leveldownalt                     },
    { "level-up-alt"                        , fa::levelupalt                       },
    { "link"                                , fa::link                             },
    { "lira-sign"                           , fa::lirasign                         },
    { "list"                                , fa::list                             },
    { "list-ol"                             , fa::listol                           },
    { "list-ul"                             , fa::listul                           },
    { "location-arrow"                      , fa::locationarrow                    },
    { "lock"                                , fa::lock                             },
    { "lock-open"                           , fa::lockopen                         },
    { "long-arrow-alt-down"                 , fa::longarrowaltdown                 },
    { "long-arrow-alt-left"                 , fa::longarrowaltleft                 },
    { "long-arrow-alt-right"                , fa::longarrowaltright                },
    { "long-arrow-alt-up"                   , fa::longarrowaltup                   },
    { "low-vision"                          , fa::lowvision                        },
    { "luggage-cart"                        , fa::luggagecart                      },
    { "magic"                               , fa::magic                            },
    { "magnet"                              , fa::magnet                           },
    { "mail-bulk"                           , fa::mailbulk                         },
    { "male"                                , fa::male                             },
    { "map-marked"                          , fa::mapmarked                        },
    { "map-marked-alt"                      , fa::mapmarkedalt                     },
    { "map-marker"                          , fa::mapmarker                        },
    { "map-marker-alt"                      , fa::mapmarkeralt                     },
    { "map-pin"                             , fa::mappin                           },
    { "map-signs"                           , fa::mapsigns                         },
    { "marker"                              , fa::marker                           },
    { "mars"                                , fa::mars                             },
    { "mars-double"                         , fa::marsdouble                       },
    { "mars-stroke"                         , fa::marsstroke                       },
    { "mars-stroke-h"                       , fa::marsstrokeh                      },
    { "mars-stroke-v"                       , fa::marsstrokev                      },
    { "mask"                                , fa::mask                             },
    { "medal"                               , fa::medal                            },
    { "medkit"                              , fa::medkit                           },
    { "memory"                              , fa::memory                           },
    { "menorah"                             , fa::menorah                          },
    { "mercury"                             , fa::mercury                          },
    { "meteor"                              , fa::meteor                           },
    { "microchip"                           , fa::microchip                        },
    { "microphone"                          , fa::microphone                       },
    { "microphone-alt"                      , fa::microphonealt                    },
    { "microphone-alt-slash"                , fa::microphonealtslash               },
    { "microphone-slash"                    , fa::microphoneslash                  },
    { "microscope"                          , fa::microscope                       },
    { "minus"                               , fa::minus                            },
    { "minus-circle"                        , fa::minuscircle                      },
    { "mitten"                              , fa::mitten                           },
    { "mobile"                              , fa::mobile                           },
    { "mobile-alt"                          , fa::mobilealt                        },
    { "money-bill"                          , fa::moneybill                        },
    { "money-bill-wave"                     , fa::moneybillwave                    },
    { "money-bill-wave-alt"                 , fa::moneybillwavealt                 },
    { "money-check"                         , fa::moneycheck                       },
    { "money-check-alt"                     , fa::moneycheckalt                    },
    { "monument"                            , fa::monument                         },
    { "mortar-pestle"                       , fa::mortarpestle                     },
    { "mosque"                              , fa::mosque                           },
    { "motorcycle"                          , fa::motorcycle                       },
    { "mountain"                            , fa::mountain                         },
    { "mouse-pointer"                       , fa::mousepointer                     },
    { "mug-hot"                             , fa::mughot                           },
    { "music"                               , fa::music                            },
    { "network-wired"                       , fa::networkwired                     },
    { "neuter"                              , fa::neuter                           },
    { "not-equal"                           , fa::notequal                         },
    { "notes-medical"                       , fa::notesmedical                     },
    { "oil-can"                             , fa::oilcan                           },
    { "om"                                  , fa::om                               },
    { "otter"                               , fa::otter                            },
    { "outdent"                             , fa::outdent                          },
    { "pager"                               , fa::pager                            },
    { "paint-brush"                         , fa::paintbrush                       },
    { "paint-roller"                        , fa::paintroller                      },
    { "palette"                             , fa::palette                          },
    { "pallet"                              , fa::pallet                           },
    { "paperclip"                           , fa::paperclip                        },
    { "parachute-box"                       , fa::parachutebox                     },
    { "paragraph"                           , fa::paragraph                        },
    { "parking"                             , fa::parking                          },
    { "passport"                            , fa::passport                         },
    { "pastafarianism"                      , fa::pastafarianism                   },
    { "paste"                               , fa::paste                            },
    { "pause"                               , fa::pause                            },
    { "paw"                                 , fa::paw                              },
    { "peace"                               , fa::peace                            },
    { "pen"                                 , fa::pen                              },
    { "pen-alt"                             , fa::penalt                           },
    { "pencil-alt"                          , fa::pencilalt                        },
    { "pencil-ruler"                        , fa::pencilruler                      },
    { "pen-fancy"                           , fa::penfancy                         },
    { "pen-nib"                             , fa::pennib                           },
    { "pen-square"                          , fa::pensquare                        },
    { "people-carry"                        , fa::peoplecarry                      },
    { "pepper-hot"                          , fa::pepperhot                        },
    { "percent"                             , fa::percent                          },
    { "percentage"                          , fa::percentage                       },
    { "person-booth"                        , fa::personbooth                      },
    { "phone"                               , fa::phone                            },
    { "phone-alt"                           , fa::phonealt                         },
    { "phone-slash"                         , fa::phoneslash                       },
    { "phone-square"                        , fa::phonesquare                      },
    { "phone-square-alt"                    , fa::phonesquarealt                   },
    { "phone-volume"                        , fa::phonevolume                      },
    { "photo-video"                         , fa::photovideo                       },
    { "piggy-bank"                          , fa::piggybank                        },
    { "pills"                               , fa::pills                            },
    { "pizza-slice"                         , fa::pizzaslice                       },
    { "place-of-worship"                    , fa::placeofworship                   },
    { "plane"                               , fa::plane                            },
    { "plane-arrival"                       , fa::planearrival                     },
    { "plane-departure"                     , fa::planedeparture                   },
    { "play"                                , fa::play                             },
    { "plug"                                , fa::plug                             },
    { "plus"                                , fa::plus                             },
    { "plus-circle"                         , fa::pluscircle                       },
    { "podcast"                             , fa::podcast                          },
    { "poll"                                , fa::poll                             },
    { "poll-h"                              , fa::pollh                            },
    { "poo"                                 , fa::poo                              },
    { "poop"                                , fa::poop                             },
    { "poo-storm"                           , fa::poostorm                         },
    { "portrait"                            , fa::portrait                         },
    { "pound-sign"                          , fa::poundsign                        },
    { "power-off"                           , fa::poweroff                         },
    { "pray"                                , fa::pray                             },
    { "praying-hands"                       , fa::prayinghands                     },
    { "prescription"                        , fa::prescription                     },
    { "prescription-bottle"                 , fa::prescriptionbottle               },
    { "prescription-bottle-alt"             , fa::prescriptionbottlealt            },
    { "print"                               , fa::print                            },
    { "procedures"                          , fa::procedures                       },
    { "project-diagram"                     , fa::projectdiagram                   },
    { "puzzle-piece"                        , fa::puzzlepiece                      },
    { "qrcode"                              , fa::qrcode                           },
    { "question"                            , fa::question                         },
    { "quidditch"                           , fa::quidditch                        },
    { "quote-left"                          , fa::quoteleft                        },
    { "quote-right"                         , fa::quoteright                       },
    { "quran"                               , fa::quran                            },
    { "radiation"                           , fa::radiation                        },
    { "radiation-alt"                       , fa::radiationalt                     },
    { "rainbow"                             , fa::rainbow                          },
    { "random"                              , fa::random                           },
    { "receipt"                             , fa::receipt                          },
    { "recycle"                             , fa::recycle                          },
    { "redo"                                , fa::redo                             },
    { "redo-alt"                            , fa::redoalt                          },
    { "remove-format"                       , fa::removeformat                     },
    { "reply"                               , fa::reply                            },
    { "reply-all"                           , fa::replyall                         },
    { "republican"                          , fa::republican                       },
    { "restroom"                            , fa::restroom                         },
    { "retweet"                             , fa::retweet                          },
    { "ribbon"                              , fa::ribbon                           },
    { "ring"                                , fa::ring                             },
    { "road"                                , fa::road                             },
    { "robot"                               , fa::robot                            },
    { "rocket"                              , fa::rocket                           },
    { "route"                               , fa::route                            },
    { "rss"                                 , fa::rss                              },
    { "rss-square"                          , fa::rsssquare                        },
    { "ruble-sign"                          , fa::rublesign                        },
    { "ruler"                               , fa::ruler                            },
    { "ruler-combined"                      , fa::rulercombined                    },
    { "ruler-horizontal"                    , fa::rulerhorizontal                  },
    { "ruler-vertical"                      , fa::rulervertical                    },
    { "running"                             , fa::running                          },
    { "rupee-sign"                          , fa::rupeesign                        },
    { "satellite"                           , fa::satellite                        },
    { "satellite-dish"                      , fa::satellitedish                    },
    { "school"                              , fa::school                           },
    { "screwdriver"                         , fa::screwdriver                      },
    { "scroll"                              , fa::scroll                           },
    { "sd-card"                             , fa::sdcard                           },
    { "search"                              , fa::search                           },
    { "search-dollar"                       , fa::searchdollar                     },
    { "search-location"                     , fa::searchlocation                   },
    { "search-minus"                        , fa::searchminus                      },
    { "search-plus"                         , fa::searchplus                       },
    { "seedling"                            , fa::seedling                         },
    { "server"                              , fa::server                           },
    { "shapes"                              , fa::shapes                           },
    { "share"                               , fa::share                            },
    { "share-alt"                           , fa::sharealt                         },
    { "share-alt-square"                    , fa::sharealtsquare                   },
    { "shekel-sign"                         , fa::shekelsign                       },
    { "shield-alt"                          , fa::shieldalt                        },
    { "ship"                                , fa::ship                             },
    { "shipping-fast"                       , fa::shippingfast                     },
    { "shoe-prints"                         , fa::shoeprints                       },
    { "shopping-bag"                        , fa::shoppingbag                      },
    { "shopping-basket"                     , fa::shoppingbasket                   },
    { "shopping-cart"                       , fa::shoppingcart                     },
    { "shower"                              , fa::shower                           },
    { "shuttle-van"                         , fa::shuttlevan                       },
    { "sign"                                , fa::sign                             },
    { "signal"                              , fa::signal                           },
    { "signature"                           , fa::signature                        },
    { "sign-in-alt"                         , fa::signinalt                        },
    { "sign-language"                       , fa::signlanguage                     },
    { "sign-out-alt"                        , fa::signoutalt                       },
    { "sim-card"                            , fa::simcard                          },
    { "sitemap"                             , fa::sitemap                          },
    { "skating"                             , fa::skating                          },
    { "skiing"                              , fa::skiing                           },
    { "skiing-nordic"                       , fa::skiingnordic                     },
    { "skull"                               , fa::skull                            },
    { "skull-crossbones"                    , fa::skullcrossbones                  },
    { "slash"                               , fa::slash                            },
    { "sleigh"                              , fa::sleigh                           },
    { "sliders-h"                           , fa::slidersh                         },
    { "smog"                                , fa::smog                             },
    { "smoking"                             , fa::smoking                          },
    { "smoking-ban"                         , fa::smokingban                       },
    { "sms"                                 , fa::sms                              },
    { "snowboarding"                        , fa::snowboarding                     },
    { "snowman"                             , fa::snowman                          },
    { "snowplow"                            , fa::snowplow                         },
    { "socks"                               , fa::socks                            },
    { "solar-panel"                         , fa::solarpanel                       },
    { "sort"                                , fa::sort                             },
    { "sort-alpha-down"                     , fa::sortalphadown                    },
    { "sort-alpha-down-alt"                 , fa::sortalphadownalt                 },
    { "sort-alpha-up"                       , fa::sortalphaup                      },
    { "sort-alpha-up-alt"                   , fa::sortalphaupalt                   },
    { "sort-amount-down"                    , fa::sortamountdown                   },
    { "sort-amount-down-alt"                , fa::sortamountdownalt                },
    { "sort-amount-up"                      , fa::sortamountup                     },
    { "sort-amount-up-alt"                  , fa::sortamountupalt                  },
    { "sort-down"                           , fa::sortdown                         },
    { "sort-numeric-down"                   , fa::sortnumericdown                  },
    { "sort-numeric-down-alt"               , fa::sortnumericdownalt               },
    { "sort-numeric-up"                     , fa::sortnumericup                    },
    { "sort-numeric-up-alt"                 , fa::sortnumericupalt                 },
    { "sort-up"                             , fa::sortup                           },
    { "spa"                                 , fa::spa                              },
    { "space-shuttle"                       , fa::spaceshuttle                     },
    { "spell-check"                         , fa::spellcheck                       },
    { "spider"                              , fa::spider                           },
    { "spinner"                             , fa::spinner                          },
    { "splotch"                             , fa::splotch                          },
    { "spray-can"                           , fa::spraycan                         },
    { "square-full"                         , fa::squarefull                       },
    { "square-root-alt"                     , fa::squarerootalt                    },
    { "stamp"                               , fa::stamp                            },
    { "star-and-crescent"                   , fa::starandcrescent                  },
    { "star-half-alt"                       , fa::starhalfalt                      },
    { "star-of-david"                       , fa::starofdavid                      },
    { "star-of-life"                        , fa::staroflife                       },
    { "step-backward"                       , fa::stepbackward                     },
    { "step-forward"                        , fa::stepforward                      },
    { "stethoscope"                         , fa::stethoscope                      },
    { "stop"                                , fa::stop                             },
    { "stopwatch"                           , fa::stopwatch                        },
    { "store"                               , fa::store                            },
    { "store-alt"                           , fa::storealt                         },
    { "stream"                              , fa::stream                           },
    { "street-view"                         , fa::streetview                       },
    { "strikethrough"                       , fa::strikethrough                    },
    { "stroopwafel"                         , fa::stroopwafel                      },
    { "subscript"                           , fa::subscript                        },
    { "subway"                              , fa::subway                           },
    { "suitcase"                            , fa::suitcase                         },
    { "suitcase-rolling"                    , fa::suitcaserolling                  },
    { "superscript"                         , fa::superscript                      },
    { "swatchbook"                          , fa::swatchbook                       },
    { "swimmer"                             , fa::swimmer                          },
    { "swimming-pool"                       , fa::swimmingpool                     },
    { "synagogue"                           , fa::synagogue                        },
    { "sync"                                , fa::sync                             },
    { "sync-alt"                            , fa::syncalt                          },
    { "syringe"                             , fa::syringe                          },
    { "table"                               , fa::table                            },
    { "tablet"                              , fa::tablet                           },
    { "tablet-alt"                          , fa::tabletalt                        },
    { "table-tennis"                        , fa::tabletennis                      },
    { "tablets"                             , fa::tablets                          },
    { "tachometer-alt"                      , fa::tachometeralt                    },
    { "tag"                                 , fa::tag                              },
    { "tags"                                , fa::tags                             },
    { "tape"                                , fa::tape                             },
    { "tasks"                               , fa::tasks                            },
    { "taxi"                                , fa::taxi                             },
    { "teeth"                               , fa::teeth                            },
    { "teeth-open"                          , fa::teethopen                        },
    { "temperature-high"                    , fa::temperaturehigh                  },
    { "temperature-low"                     , fa::temperaturelow                   },
    { "tenge"                               , fa::tenge                            },
    { "terminal"                            , fa::terminal                         },
    { "text-height"                         , fa::textheight                       },
    { "text-width"                          , fa::textwidth                        },
    { "th"                                  , fa::th                               },
    { "theater-masks"                       , fa::theatermasks                     },
    { "thermometer"                         , fa::thermometer                      },
    { "thermometer-empty"                   , fa::thermometerempty                 },
    { "thermometer-full"                    , fa::thermometerfull                  },
    { "thermometer-half"                    , fa::thermometerhalf                  },
    { "thermometer-quarter"                 , fa::thermometerquarter               },
    { "thermometer-three-quarters"          , fa::thermometerthreequarters         },
    { "th-large"                            , fa::thlarge                          },
    { "th-list"                             , fa::thlist                           },
    { "thumbtack"                           , fa::thumbtack                        },
    { "ticket-alt"                          , fa::ticketalt                        },
    { "times"                               , fa::times                            },
    { "tint"                                , fa::tint                             },
    { "tint-slash"                          , fa::tintslash                        },
    { "toggle-off"                          , fa::toggleoff                        },
    { "toggle-on"                           , fa::toggleon                         },
    { "toilet"                              , fa::toilet                           },
    { "toilet-paper"                        , fa::toiletpaper                      },
    { "toolbox"                             , fa::toolbox                          },
    { "tools"                               , fa::tools                            },
    { "tooth"                               , fa::tooth                            },
    { "torah"                               , fa::torah                            },
    { "torii-gate"                          , fa::toriigate                        },
    { "tractor"                             , fa::tractor                          },
    { "trademark"                           , fa::trademark                        },
    { "traffic-light"                       , fa::trafficlight                     },
    { "train"                               , fa::train                            },
    { "tram"                                , fa::tram                             },
    { "transgender"                         , fa::transgender                      },
    { "transgender-alt"                     , fa::transgenderalt                   },
    { "trash"                               , fa::trash                            },
    { "trash-restore"                       , fa::trashrestore                     },
    { "trash-restore-alt"                   , fa::trashrestorealt                  },
    { "tree"                                , fa::tree                             },
    { "trophy"                              , fa::trophy                           },
    { "truck"                               , fa::truck                            },
    { "truck-loading"                       , fa::truckloading                     },
    { "truck-monster"                       , fa::truckmonster                     },
    { "truck-moving"                        , fa::truckmoving                      },
    { "truck-pickup"                        , fa::truckpickup                      },
    { "tshirt"                              , fa::tshirt                           },
    { "tty"                                 , fa::tty                              },
    { "tv"                                  , fa::tv                               },
    { "umbrella"                            , fa::umbrella                         },
    { "umbrella-beach"                      , fa::umbrellabeach                    },
    { "underline"                           , fa::underline                        },
    { "undo"                                , fa::undo                             },
    { "undo-alt"                            , fa::undoalt                          },
    { "universal-access"                    , fa::universalaccess                  },
    { "university"                          , fa::university                       },
    { "unlink"                              , fa::unlink                           },
    { "unlock"                              , fa::unlock                           },
    { "unlock-alt"                          , fa::unlockalt                        },
    { "upload"                              , fa::upload                           },
    { "user-alt"                            , fa::useralt                          },
    { "user-alt-slash"                      , fa::useraltslash                     },
    { "user-astronaut"                      , fa::userastronaut                    },
    { "user-check"                          , fa::usercheck                        },
    { "user-clock"                          , fa::userclock                        },
    { "user-cog"                            , fa::usercog                          },
    { "user-edit"                           , fa::useredit                         },
    { "user-friends"                        , fa::userfriends                      },
    { "user-graduate"                       , fa::usergraduate                     },
    { "user-injured"                        , fa::userinjured                      },
    { "user-lock"                           , fa::userlock                         },
    { "user-md"                             , fa::usermd                           },
    { "user-minus"                          , fa::userminus                        },
    { "user-ninja"                          , fa::userninja                        },
    { "user-nurse"                          , fa::usernurse                        },
    { "user-plus"                           , fa::userplus                         },
    { "users"                               , fa::users                            },
    { "users-cog"                           , fa::userscog                         },
    { "user-secret"                         , fa::usersecret                       },
    { "user-shield"                         , fa::usershield                       },
    { "user-slash"                          , fa::userslash                        },
    { "user-tag"                            , fa::usertag                          },
    { "user-tie"                            , fa::usertie                          },
    { "user-times"                          , fa::usertimes                        },
    { "utensils"                            , fa::utensils                         },
    { "utensil-spoon"                       , fa::utensilspoon                     },
    { "vector-square"                       , fa::vectorsquare                     },
    { "venus"                               , fa::venus                            },
    { "venus-double"                        , fa::venusdouble                      },
    { "venus-mars"                          , fa::venusmars                        },
    { "vial"                                , fa::vial                             },
    { "vials"                               , fa::vials                            },
    { "video"                               , fa::video                            },
    { "video-slash"                         , fa::videoslash                       },
    { "vihara"                              , fa::vihara                           },
    { "voicemail"                           , fa::voicemail                        },
    { "volleyball-ball"                     , fa::volleyballball                   },
    { "volume-down"                         , fa::volumedown                       },
    { "volume-mute"                         , fa::volumemute                       },
    { "volume-off"                          , fa::volumeoff                        },
    { "volume-up"                           , fa::volumeup                         },
    { "vote-yea"                            , fa::voteyea                          },
    { "vr-cardboard"                        , fa::vrcardboard                      },
    { "walking"                             , fa::walking                          },
    { "wallet"                              , fa::wallet                           },
    { "warehouse"                           , fa::warehouse                        },
    { "water"                               , fa::water                            },
    { "wave-square"                         , fa::wavesquare                       },
    { "weight"                              , fa::weight                           },
    { "weight-hanging"                      , fa::weighthanging                    },
    { "wheelchair"                          , fa::wheelchair                       },
    { "wifi"                                , fa::wifi                             },
    { "wind"                                , fa::wind                             },
    { "wine-bottle"                         , fa::winebottle                       },
    { "wine-glass"                          , fa::wineglass                        },
    { "wine-glass-alt"                      , fa::wineglassalt                     },
    { "won-sign"                            , fa::wonsign                          },
    { "wrench"                              , fa::wrench                           },
    { "x-ray"                               , fa::xray                             },
    { "yen-sign"                            , fa::yensign                          },
    { "yin-yang"                            , fa::yinyang                          }
};


/// a specialized init function so font-awesome is loaded and initialized
/// this method return true on success, it will return false if the fnot cannot be initialized
/// To initialize QtAwesome with font-awesome you need to call this method
bool QtAwesome::initFontAwesome( )
{
    bool errors = false;
    // The macro below internally calls "qInitResources_QtAwesome()". this initializes
    // the resource system. For a .pri project this isn't required, but when building and using a
    // static library the resource need to initialized first.
    ///
    // I've checked th qInitResource_* code and calling this method mutliple times shouldn't be any problem
    // (More info about this subject:  http://qt-project.org/wiki/QtResources)
#ifdef FONT_AWESOME_PRO
        Q_INIT_RESOURCE(QtAwesomePro);
#else
        Q_INIT_RESOURCE(QtAwesomeFree);
#endif

    for(FontData &fd : _fontDetails ) {
    // only load font-awesome once
        if( fd.fontId() < 0 ) {
            // load the font file
            QFile res(":/fonts/" + fd.fontFilename());
            if(!res.open(QIODevice::ReadOnly)) {
                qDebug() << "Font awesome font" << fd.fontFilename() << "could not be loaded!";
                errors = true;
                continue;
            }
            QByteArray fontData( res.readAll() );
            res.close();

            // fetch the given font
            fd.setFontId(QFontDatabase::addApplicationFontFromData(fontData));
        }

        QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(fd.fontId());
        if( !loadedFontFamilies.empty() ) {
            fd.setFontFamily( loadedFontFamilies.at(0) );
        } else {
            qDebug() << "Font awesome" << fd.fontFilename() << " font is empty?!";
            fd.setFontId( -1 ); // restore the font-awesome id
            return false;
        }
    }

    // intialize the brands icon map
    QHash<QString, int> *brands = new QHash<QString, int>();
    for (unsigned i = 0; i < sizeof(faBrandsIconArray)/sizeof(FANameIcon); ++i) {
        brands->insert(faBrandsIconArray[i].name, faBrandsIconArray[i].icon);
    }
    _namedCodepoints.insert(style::stfab, brands);

    //initialize others code icons maps
#ifdef FONT_AWESOME_PRO
    QHash<QString, int> *commonMap = new QHash<QString, int>();
    for (unsigned i = 0; i < sizeof(faCommonIconArray)/sizeof(FANameIcon); ++i) {
        commonMap->insert(faCommonIconArray[i].name, faCommonIconArray[i].icon);
    }

    for (unsigned i = 0; i < sizeof(faProIconArray)/sizeof(FANameIcon); ++i) {
        commonMap->insert(faProIconArray[i].name, faProIconArray[i].icon);
    }

    _namedCodepoints.insert(style::stfar, commonMap);
    _namedCodepoints.insert(style::stfas, commonMap);
    _namedCodepoints.insert(style::stfal, commonMap);
    _namedCodepoints.insert(style::stfad, commonMap);
#else
    QHash<QString, int> *farMap = new QHash<QString, int>();
    for (unsigned i = 0; i < sizeof(faCommonIconArray)/sizeof(FANameIcon) && i < FREE_REGULAR_ICON_SIZE; ++i) {
        farMap->insert(faCommonIconArray[i].name, faCommonIconArray[i].icon);
    }
    _namedCodepoints.insert(style::stfar, farMap);

    QHash<QString, int> *fasMap = new QHash<QString, int>();
    for (unsigned i = 0; i < sizeof(faCommonIconArray)/sizeof(FANameIcon); ++i) {
        fasMap->insert(faCommonIconArray[i].name, faCommonIconArray[i].icon);
    }
    _namedCodepoints.insert(style::stfas, fasMap);
#endif

    return errors;
}

const QHash<QString, int> QtAwesome::namedCodePoints(style::styles st) const
{
    if(!_namedCodepoints.contains(st))
        return QHash<QString, int>();

    return *_namedCodepoints[st];
}


/// Sets a default option. These options are passed on to the icon painters
void QtAwesome::setDefaultOption(const QString& name, const QVariant& value)
{
    _defaultOptions.insert( name, value );
}


/// Returns the default option for the given name
QVariant QtAwesome::defaultOption(const QString& name)
{
    return _defaultOptions.value( name );
}


// internal helper method to merge to option maps
static QVariantMap mergeOptions( const QVariantMap& defaults, const QVariantMap& override )
{
    QVariantMap result= defaults;
    if( !override.isEmpty() ) {
        QMapIterator<QString,QVariant> itr(override);
        while( itr.hasNext() ) {
            itr.next();
            result.insert( itr.key(), itr.value() );
        }
    }
    return result;
}


/// Creates an icon with the given code-point for st style
/// <code>
///     awesome->icon( icon_group )
/// </code>
QIcon QtAwesome::icon(int st, int character, const QVariantMap &options)
{
    // create a merged QVariantMap to have default options and icon-specific options
    QVariantMap optionMap = mergeOptions( _defaultOptions, options );
    optionMap.insert("text", QString( QChar(character)) );
    optionMap.insert("style", st);

    return icon( _fontIconPainter, optionMap );
}



/// Creates an icon with the given name
///
/// You can use the icon names as defined on https://fontawesome.com/cheatsheet/free and
/// https://fontawesome.com/cheatsheet/pro adding the style prefix, e.g. "fas address-book"
/// @param name the name of the icon
/// @param options extra option to pass to the icon renderer
QIcon QtAwesome::icon(const QString& name, const QVariantMap& options)
{
    //Split name
    QStringList params = name.split(" ");
    int st;
    QString ic;

    if(params.size() == 2){
        st = stringToStyleEnum(params[0]);
        ic = params[1];
    } else if (params.size() == 1) {
        st = style::stfas;
        ic = params[0];
    } else {
        return QIcon();
    }

    // when it's a named codepoint
    if(_namedCodepoints.contains(st) && _namedCodepoints[st]->contains(ic)) {
        return icon( st, _namedCodepoints[st]->value(ic), options );
    }

    // create a merged QVariantMap to have default options and icon-specific options
    QVariantMap optionMap = mergeOptions( _defaultOptions, options );
    optionMap.insert("style", st);

    // this method first tries to retrieve the icon
    QtAwesomeIconPainter* painter = _painterMap.value(name);
    if( !painter ) {
        return QIcon();
    }

    return icon( painter, optionMap );
}

/// Create a dynamic icon by simlpy supplying a painter object
/// The ownership of the painter is NOT transfered.
/// @param painter a dynamic painter that is going to paint the icon
/// @param optionmap the options to pass to the painter
QIcon QtAwesome::icon(QtAwesomeIconPainter* painter, const QVariantMap& optionMap )
{
    // Warning, when you use memoryleak detection. You should turn it off for the next call
    // QIcon's placed in gui items are often cached and not deleted when my memory-leak detection checks for leaks.
    // I'm not sure if it's a Qt bug or something I do wrong
    QtAwesomeIconPainterIconEngine* engine = new QtAwesomeIconPainterIconEngine( this, painter, optionMap  );
    return QIcon( engine );
}

/// Adds a named icon-painter to the QtAwesome icon map
/// As the name applies the ownership is passed over to QtAwesome
///
/// @param name the name of the icon including the style
/// @param painter the icon painter to add for this name
void QtAwesome::give(const QString& name, QtAwesomeIconPainter* painter)
{
    delete _painterMap.value( name );   // delete the old one
    _painterMap.insert( name, painter );
}


/// \brief QtAwesome::font Creates/Gets the icon font with a given size in pixels. This can be usefull to use a label for displaying icons
///
/// \param st Font Awesome style such as fas, fal, fab, fad or far
/// \param size point size of the font
/// \return the corresponding font
/// Example:
///
///    QLabel* label = new QLabel( QChar( icon_group ) );
///    label->setFont( awesome->font(style::fas, 16) )
QFont QtAwesome::font(style::styles st, int size ) const
{
    if(!_fontDetails.contains(st))
        return QFont();

    QFont ft( _fontDetails[st].fontFamily());
    ft.setPixelSize(size);
    ft.setWeight((QFont::Weight)_fontDetails[st].fontWeight());

    return ft;
}

QString QtAwesome::fontName(style::styles st) const
{
    if(_fontDetails.contains(st))
        return _fontDetails[st].fontFamily();

    return "";
}

int QtAwesome::stringToStyleEnum(const QString st) const
{
    if(st == "fas")
        return style::stfas;
    else if (st == "far")
        return style::stfar;
    else if (st == "fab")
        return style::stfab;
#ifdef FONT_AWESOME_PRO
    else if (st == "fal")
        return style::stfal;
    else if (st == "fad")
        return style::stfad;
#endif

    return style::stfas;
}

const QString QtAwesome::styleEnumToString(int st) const
{
    switch(st){
    case style::stfab:
        return "fab";
    case style::stfar:
        return "far";
    case style::stfas:
        return "fas";
#ifdef FONT_AWESOME_PRO
    case style::stfal:
        return "fal";
    case style::stfad:
        return "fad";
#endif
    }

    return "fas";
}
