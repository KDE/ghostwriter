message("Building ghostwriter translations, this might take some time")

# Build app translations from .ts sources using lrelease.
APP_LANGUAGES = cs de en es fr it ja pl pt_BR ru zh 
APP_TRANSLATIONS_DIR = $$PWD/translations

# Build qt translations from source, if present.
QT_LANGUAGES = cs de es fr it ja pl ru
QT_TRANSLATIONS_DIR = $$PWD/qttranslations/translations

# Target dir for translations (inside build dir)
TRANSLATION_TARGET_DIR = $$OUT_PWD/$$DESTDIR/translations
!exists($$TRANSLATION_TARGET_DIR) {
    mkpath($$TRANSLATION_TARGET_DIR)
}

# set lrelease binary.
isEmpty(QMAKE_LRELEASE) {
    win32:L_RELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:L_RELEASE = $$[QT_INSTALL_BINS]/lrelease
}

# set lrelease options.
OPTIONS = "-compress -removeidentical -silent"

# prepend translation name to language.
defineReplace(prependAll) {
    for(a,$$1):result += $$2$${a}$$3
    return($$result)
}

# application translations
TRANSLATIONS = $$prependAll(APP_LANGUAGES, $${APP_TRANSLATIONS_DIR}/ghostwriter_, .ts)

# qt translations
exists($$QT_TRANSLATIONS_DIR) {
  message("Building qt translations from $$QT_TRANSLATIONS_DIR")
  TRANSLATIONS += $$prependAll(QT_LANGUAGES, $${QT_TRANSLATIONS_DIR}/qt_, .ts)
  TRANSLATIONS += $$prependAll(QT_LANGUAGES, $${QT_TRANSLATIONS_DIR}/qtbase_, .ts)
} else {
  PATH = $$[QT_INSTALL_TRANSLATIONS]
  message("Using qt translations from $$PATH")
  BINARIES += $$prependAll(QT_LANGUAGES, $$PATH/qt_, .qm)
  BINARIES += $$prependAll(QT_LANGUAGES, $$PATH/qtbase_, .qm)
}

# build translations using lrelease.
for(tsfile, TRANSLATIONS) {
    qmfile = $$TRANSLATION_TARGET_DIR/$$basename(tsfile)
    qmfile ~= s/.ts$/.qm/
    system($$L_RELEASE $$OPTIONS $$tsfile -qm $$qmfile)
}

# copying system binaries.
for(bin, BINARIES) {
    qmfile = $$TRANSLATION_TARGET_DIR/$$basename(bin)
    system(cp -v $$bin $$qmfile)
}
