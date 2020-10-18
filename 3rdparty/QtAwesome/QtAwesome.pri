defineReplace( resourcesForConfig ){
    return($$PWD/QtAwesome$$eval($$1).qrc)
}

options = $$find(CONFIG, fontAwesomePro) $$find(CONFIG, fontAwesomeFree)
count(options, 0) { error("fontAwesomePro or fontAwesomeFree should be defined") }
count(options, 2) { error("fontAwesomePro and fontAwesomeFree were defined, only one config is accepted") }

INCLUDEPATH += $$PWD

SOURCES += $$PWD/QtAwesome.cpp \  
    $$PWD/QtAwesomeAnim.cpp

HEADERS += $$PWD/QtAwesome.h \   
    $$PWD/QtAwesomeAnim.h

CONFIG( fontAwesomePro ){
    config = Pro
    RESOURCES += $$resourcesForConfig(config)
    DEFINES += FONT_AWESOME_PRO=1
    !build_pass:message(using font awesome pro)
}

CONFIG( fontAwesomeFree ){
    config = Free
    RESOURCES += $$resourcesForConfig(config)
    !build_pass:message(using font awesome free)
}


