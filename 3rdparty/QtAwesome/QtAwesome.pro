#-------------------------------------------------
#
# Project created by QtCreator 2013-04-18T13:28:42
#
#-------------------------------------------------

TARGET = QtAwesome
TEMPLATE = lib
CONFIG += staticlib c++11
QT += widgets

# only one option must be enabled
#CONFIG += fontAwesomePro
CONFIG += fontAwesomeFree
include(QtAwesome.pri)

isEmpty(PREFIX) {
    unix {
        PREFIX = /usr
    } else {
        PREFIX = $$[QT_INSTALL_PREFIX]
    }
}

install_headers.files = QtAwesome.h QtAwesomeAnim.h
install_headers.path = $$PREFIX/include
target.path = $$PREFIX/lib
INSTALLS += install_headers target
