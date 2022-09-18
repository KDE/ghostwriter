/*
 * SPDX-FileCopyrightText: 2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QSettings>
#include <QString>
#include <QTest>

#include "../../src/library.h"

using namespace ghostwriter;

class BookmarkInputData
{
public:
    QString filePath;
    int cursorPosition;
};

typedef QList<BookmarkInputData> BookmarkInputs;


Q_DECLARE_METATYPE(BookmarkInputData)
Q_DECLARE_METATYPE(Bookmark)

/**
 * Unit test for the Library class.
 * 
 * Note: The following files are created as part of the test setup:
 *       valid1.txt
 *       valid2.txt
 *       valid3.txt
 *       valid4.txt
 *       valid5.txt
 *       valid6.txt
 */
class LibraryTest: public QObject
{
    Q_OBJECT

private:
    void populateRecentHistory(const BookmarkInputs &inputs);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();
    void recentFiles_data();
    void recentFiles();
    void recentFilesMax_data();
    void recentFilesMax();
    void addRecent_data();
    void addRecent();
    void removeRecent_data();
    void removeRecent();
    void sync_data();
    void sync();
    void lookup_data();
    void lookup();
    void clear_data();
    void clear();
};

void LibraryTest::populateRecentHistory(const BookmarkInputs &inputs)
{    
    // Create a settings file with valid bookmark data.
    QSettings settings;

    settings.beginWriteArray("FileHistory", inputs.size());

    for (int i = 0; i < inputs.size(); i++) {
        QString path = inputs.at(i).filePath;
        int pos = inputs.at(i).cursorPosition;
        
        settings.setArrayIndex(i);
        settings.setValue("filePath", path);
        settings.setValue
        (
            "cursorPosition",
            pos
        );
    }

    settings.endArray();
    settings.sync();
}

void LibraryTest::initTestCase()
{
    QCoreApplication::setOrganizationName("ghostwriter");
    QCoreApplication::setApplicationName("ghostwriter");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath
    (
        QSettings::IniFormat,
        QSettings::UserScope,
        "./settings"
    );

    cleanup();
    
    for (int i = 1; i < 7; i++) {
        QString path = QString("./valid%1.txt").arg(i);

        QFile file(path);

        file.open(QIODevice::WriteOnly | QIODevice::ReadOnly);
        file.write("test file");
        file.close();
    }
}

void LibraryTest::cleanupTestCase()
{
    for (int i = 1; i < 7; i++) {
        QString path = QString("./valid%1.txt").arg(i);

        QFile file(path);
        file.remove();
    }
}

void LibraryTest::cleanup()
{
    QSettings settings;
    settings.remove("FileHistory");
}

void LibraryTest::recentFiles_data()
{
    QTest::addColumn<BookmarkInputs>("inputs");
    QTest::addColumn<Bookmarks>("expected");

    QTest::newRow("nominal: no recent files")
        << BookmarkInputs()
        << Bookmarks()
        ;

    QTest::newRow("nominal: all valid bookmarks")
        << BookmarkInputs({
                { "./valid1.txt", 20 },
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            })
        << Bookmarks({
                { "./valid1.txt", 20 },
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            })
        ;

    QTest::newRow("robustness: some bookmarks invalid")
        << BookmarkInputs({
                { "foo.txt", 300 },
                { "./valid1.txt", 20 },
                { "./bar.txt", 400 },
                { "valid2.txt", 80 },
                { "valid3.txt", 40 },
                { "baz.txt", 17 }
            })
        << Bookmarks({
                { "./valid1.txt", 20 },
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            })
        ;
    
    QTest::newRow("robustness: bookmark with invalid cursor position")
        << BookmarkInputs({
                { "./valid1.txt", 20 },
                { "valid2.txt", -78 },
                { "valid3.txt", 40 }
            })
        << Bookmarks({
                { "./valid1.txt", 20 },
                { "valid2.txt", 0 },
                { "valid3.txt", 40 }
            })
        ;
}

/**
 * OBJECTIVE:
 *      Call recentFiles() with no max specified (defaults to -1).
 *
 * INPUTS:
 *      1. An empty settings file.
 *      2. A settings file containing valid bookmark data (files exist and
 *         cursor postions positive).
 *      3. A settings file with some bookmarks valid (files exist, cursor
 *         positions positive) and some invalid (files do not exist, but
 *         cursor positions still positive).
 *      4. A settings file with some valid bookmarks and one valid bookmark
 *         with a negative cursor position.
 *
 * EXPECTED RESULTS:
 *      1. Call to recentFiles() returns an empty list of bookmarks.
 *      2. Call to recentFiles() returns a matching list of the bookmarks as of
 *         those in the settings file (all valid).
 *      3. Call to recentFiles() returns only the valid the bookmarks from the
 *         settings file.
 *      4. Call to recentFiles() returns a matching list of the the bookmarks
 *         from the settings file, but with the invalid cursor position set to
 *         zero.
 */
void LibraryTest::recentFiles()
{
    QFETCH(BookmarkInputs, inputs);
    QFETCH(Bookmarks, expected);

    if (!inputs.isEmpty()) {
        populateRecentHistory(inputs);
    }

    Library library;
    Bookmarks recentFiles = library.recentFiles();

    QCOMPARE(expected.size(), recentFiles.size());
    
    for (int i = 0; i < recentFiles.size(); i++) {
        QCOMPARE(expected.at(i).filePath(), recentFiles.at(i).filePath());
        QCOMPARE(expected.at(i).cursorPosition(), recentFiles.at(i).cursorPosition());
        QVERIFY(recentFiles.at(i).isValid());
    }
}

void LibraryTest::recentFilesMax_data()
{
    QTest::addColumn<int>("max");
    QTest::addColumn<BookmarkInputs>("inputs");
    QTest::addColumn<Bookmarks>("expected");
    
    QTest::newRow("nominal: max equal to total bookmarks")
        << 3
        << BookmarkInputs({
                {"./valid1.txt", 20},
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            })
        << Bookmarks({
                { "./valid1.txt", 20 },
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            })
        ;

    QTest::newRow("nominal: max less than total bookmarks")
        << 3
        << BookmarkInputs({
                {"./valid1.txt", 20},
                { "valid2.txt", 80 },
                { "valid3.txt", 40 },
                { "valid4.txt", 60 },
                { "valid5.txt", 120 }
            })
        << Bookmarks({
                { "./valid1.txt", 20 },
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            })
        ;
    
    QTest::newRow("robustness: max greater than total bookmarks")
        << 5
        << BookmarkInputs({
                {"./valid1.txt", 20},
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            })
        << Bookmarks({
                { "./valid1.txt", 20 },
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            })
        ;
}

/**
 * OBJECTIVE:
 *      Call recentFiles() with a maximum value specified.
 *
 * INPUTS:
 *      1. A settings file containing valid bookmarks, and a max value passed
 *         to recentFiles() matching the number of bookmarks in the file.
 *      2. A settings file containing valid bookmarks, and a max value passed
 *         to recentFiles() less than the number of bookmarks in the file.
 *      3. A settings file containing valid bookmarks, and a max value passed
 *         to recentFiles() greater than the number of bookmarks in the file.
 *
 * EXPECTED RESULTS:
 *      1. Call to recentFiles() returns all the bookmarks in the settings
 *         file (all valid).
 *      2. Call to recentFiles() returns the bookmarks in the settings
 *         file (all valid) up to the max value specified, most recent first.
 *      3. Call to recentFiles() returns all the bookmarks in the settings
 *         file (all valid).
 */
void LibraryTest::recentFilesMax()
{
    QFETCH(int, max);
    QFETCH(BookmarkInputs, inputs);
    QFETCH(Bookmarks, expected);

    populateRecentHistory(inputs);

    Library library;
    Bookmarks recentFiles = library.recentFiles(max);

    QCOMPARE(expected.size(), recentFiles.size());
    
    for (int i = 0; i < recentFiles.size() - max; i++) {
        QCOMPARE(expected.at(i).filePath(), recentFiles.at(i).filePath());
        QCOMPARE(expected.at(i).cursorPosition(), recentFiles.at(i).cursorPosition());
        QVERIFY(recentFiles.at(i).isValid());
    }
}

void LibraryTest::addRecent_data()
{
    QTest::addColumn<int>("method");
    QTest::addColumn<BookmarkInputs>("existing");
    QTest::addColumn<BookmarkInputData>("added");
    QTest::addColumn<Bookmarks>("expected");

    for (int i = 1; i <= 2; i++) {
        char title[120];

        sprintf(title, "method %d: nominal: add new valid bookmark to empty history", i);

        QTest::newRow(title)
            << i
            << BookmarkInputs { }
            << BookmarkInputData { "./valid1.txt", 5000 }
            << Bookmarks { { "./valid1.txt", 5000 } }
            ;
        
        sprintf(title, "method %d: nominal: add new valid bookmark to existing history", i);
        
        QTest::newRow(title)
            << i
            << BookmarkInputs {
                    {"./valid1.txt", 20},
                    { "valid2.txt", 80 },
                    { "valid3.txt", 40 }
                }
            << BookmarkInputData { "./valid4.txt", 5000 }
            << Bookmarks {
                    { "./valid4.txt", 5000 },
                    {"./valid1.txt", 20},
                    { "valid2.txt", 80 },
                    { "valid3.txt", 40 }
                }
            ;
        
        sprintf(title, "method %d: nominal: update bookmark in history", i);
        
        QTest::newRow(title)
            << i
            << BookmarkInputs {
                    {"./valid1.txt", 20},
                    { "valid2.txt", 80 },
                    { "valid3.txt", 40 }
                }
            << BookmarkInputData { "./valid2.txt", 2700 }
            << Bookmarks {
                    { "./valid2.txt", 2700 },
                    {"./valid1.txt", 20},
                    { "valid3.txt", 40 }
                }
            ;
        
        sprintf(title, "method %d: robustness: add bookmark where file does not exist in history", i);
        
        QTest::newRow(title)
            << i
            << BookmarkInputs { }
            << BookmarkInputData { "foo.txt", 3000 }
            << Bookmarks { }
            ;
        
        sprintf(title, "method %d: robustness: add bookmark with negative cursor position to history", i);
        
        QTest::newRow(title)
            << i
            << BookmarkInputs { }
            << BookmarkInputData { "valid1.txt", -3000 }
            << Bookmarks { { "valid1.txt", 0 } }
            ;
        
        sprintf(title, "method %d: robustness: add null bookmark to history", i);
        
        QTest::newRow(title)
            << i
            << BookmarkInputs { }
            << BookmarkInputData { QString(), -3000 }
            << Bookmarks { }
            ;
    }
}

/**
 * OBJECTIVE:
 *      Call addRecent() to add new valid bookmarks to history.
 *
 * INPUTS:
 *      For each addRecent() method:
 * 
 *      1. An empty settings file, and a valid bookmark passed to
 *         addRecent().
 *      2. A settings file with existing bookmarks (all valid), and a valid
 *         bookmark passed to addRecent().
 *      3. A settings file with existing bookmarks (all valid), and a valid
 *         bookmark passed to addRecent() with a file path that already
 *         exists in the settings file but with a different cursor position.
 *      4. An empty settings file, and an invalid bookmark (file does not
 *         exist) passed to addRecent().
 *      5. An empty settings file, and a valid bookmark with a negative
 *         cursor position passed to addRecent().
 *      6. An empty settings file, and a null bookmark.
 *
 * EXPECTED RESULTS:
 *      For each addRecent() method:
 * 
 *      1. Call to recentFiles() returns the bookmark added to the settings
 *         file.
 *      2. Call to recentFiles() returns the new bookmark with the bookmarks
 *         in the settings file, in last in, first out order (i.e., the new
 *         bookmark is first in the list).
 *      3. Call to recentFiles() returns the bookmarks in the settings file,
 *         but with the added bookmark promoted to be the first in the list
 *         with its cursor position updated to the new bookmark's value.
 *      4. Call to recentFiles() returns an empty list.
 *      5. Call to recentFiles() returns the new bookmark with the cursor
 *         position set to zero.
 *      6. Call to recentFiles() returns an empty list.
 */
void LibraryTest::addRecent()
{
    QFETCH(int, method);
    QFETCH(BookmarkInputs, existing);
    QFETCH(BookmarkInputData, added);
    QFETCH(Bookmarks, expected);

    populateRecentHistory(existing);
    
    Library library;

    if (1 == method) {
        Bookmark bookmark;

        if (!added.filePath.isNull()) {
            bookmark = Bookmark(added.filePath, added.cursorPosition);
        }

        library.addRecent(bookmark);
    } else {
        library.addRecent(added.filePath,
            added.cursorPosition);
    }

    Bookmarks recentFiles = library.recentFiles();

    QCOMPARE(expected.size(), recentFiles.size());
    
    for (int i = 0; i < recentFiles.size(); i++) {
        QCOMPARE(expected.at(i).filePath(), recentFiles.at(i).filePath());
        QCOMPARE(expected.at(i).cursorPosition(), recentFiles.at(i).cursorPosition());
        QVERIFY(recentFiles.at(i).isValid());
    }
}

void LibraryTest::removeRecent_data()
{
    QTest::addColumn<int>("method");
    QTest::addColumn<BookmarkInputs>("existing");
    QTest::addColumn<QString>("removed");
    QTest::addColumn<Bookmarks>("expected");

    for (int i = 1; i <= 2; i++) {
        char title[120];

        sprintf(title, "method %d: nominal: remove valid bookmark from empty history", i);

        QTest::newRow(title)
            << i
            << BookmarkInputs { }
            << "./valid1.txt"
            << Bookmarks { }
            ;
        
        sprintf(title, "method %d: nominal: remove valid bookmark from existing history", i);
        
        QTest::newRow(title)
            << i
            << BookmarkInputs {
                    {"./valid1.txt", 20},
                    { "valid2.txt", 80 },
                    { "valid3.txt", 40 }
                }
            << "./valid2.txt"
            << Bookmarks {
                    {"./valid1.txt", 20},
                    { "valid3.txt", 40 }
                }
            ;
        
        sprintf(title, "method %d: robustness: remove valid bookmark that does not exist in history", i);
        
        QTest::newRow(title)
            << i
            << BookmarkInputs {
                    {"./valid1.txt", 20},
                    { "valid2.txt", 80 },
                    { "valid3.txt", 40 }
                }
            << "./valid5.txt"
            << Bookmarks {
                    {"./valid1.txt", 20},
                    { "valid2.txt", 80 },
                    { "valid3.txt", 40 }
                }
            ;
        
        sprintf(title, "method %d: robustness: remove invalid bookmark", i);
        
        QTest::newRow(title)
            << i
            << BookmarkInputs {
                    {"./valid1.txt", 20},
                    { "valid2.txt", 80 },
                    { "valid3.txt", 40 }
                }
            << "foo.txt"
            << Bookmarks {
                    {"./valid1.txt", 20},
                    { "valid2.txt", 80 },
                    { "valid3.txt", 40 }
                }
            ;
        
        sprintf(title, "method %d: robustness: remove null bookmark", i);
        
        QTest::newRow(title)
            << i
            << BookmarkInputs {
                    {"./valid1.txt", 20},
                    { "valid2.txt", 80 },
                    { "valid3.txt", 40 }
                }
            << QString()
            << Bookmarks {
                    {"./valid1.txt", 20},
                    { "valid2.txt", 80 },
                    { "valid3.txt", 40 }
                }
            ;
    }
}

/**
 * OBJECTIVE:
 *      Call removeRecent() to remove bookmarks from the history.
 *
 * INPUTS:
 *      For each removeRecent() method:
 * 
 *      1. An empty settings file, and a valid bookmark passed to
 *         removeRecent().
 *      2. A settings file with existing bookmarks (all valid), and a valid
 *         bookmark passed to removeRecent() that exists in the settings
 *         file.
 *      3. A settings file with existing bookmarks (all valid), and a valid
 *         bookmark passed to removeRecent() that does not exist in the
 *         settings file.
 *      4. A settings file with existing bookmarks (all valid), and an invalid
 *         bookmark (file does not exist) passed to removeRecent().
 *      5. A settings file with existing bookmarks (all valid), and a null
 *         bookmark / null file path passed to removeRecent().
 *
 * EXPECTED RESULTS:
 *      For each removeRecent() method:
 * 
 *      1. Call to recentFiles() returns an empty list.
 *      2. Call to recentFiles() returns the same files as those in the
 *         settings file, minus the removed bookmark.
 *      3. Call to recentFiles() returns the bookmarks in the settings file.
 *      4. Call to recentFiles() returns the bookmarks in the settings file.
 *      5. Call to recentFiles() returns the bookmarks in the settings file.
 */
void LibraryTest::removeRecent()
{
    QFETCH(int, method);
    QFETCH(BookmarkInputs, existing);
    QFETCH(QString, removed);
    QFETCH(Bookmarks, expected);

    populateRecentHistory(existing);
    
    Library library;

    if (1 == method) {
        Bookmark bookmark;

        if (!removed.isNull()) {
            bookmark = Bookmark(removed);
        }

        library.removeRecent(bookmark);
    } else {
        library.removeRecent(removed);
    }

    // Force write to disk.
    library.sync();

    // Force fresh read from disk.
    Bookmarks recentFiles = Library().recentFiles();

    QCOMPARE(expected.size(), recentFiles.size());
    
    for (int i = 0; i < recentFiles.size(); i++) {
        QCOMPARE(expected.at(i).filePath(), recentFiles.at(i).filePath());
        QCOMPARE(expected.at(i).cursorPosition(), recentFiles.at(i).cursorPosition());
        QCOMPARE(expected.at(i).isValid(), recentFiles.at(i).isValid());
        QCOMPARE(expected.at(i).isNull(), recentFiles.at(i).isNull());
    }
}

void LibraryTest::sync_data()
{
    QTest::addColumn<BookmarkInputs>("existing");
    QTest::addColumn<BookmarkInputs>("added");
    QTest::addColumn<Bookmarks>("expected");
    
    QTest::newRow("nominal: store empty recent bookmarks list")
        << BookmarkInputs { }
        << BookmarkInputs { }
        << Bookmarks { }
        ;

    QTest::newRow("nominal: store new bookmarks added to empty history")
        << BookmarkInputs { }
        << BookmarkInputs {
                { "valid3.txt", 40 },
                { "valid2.txt", 80 },
                {"./valid1.txt", 20}
            }
        << Bookmarks {
                { "./valid1.txt", 20 },
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            }
        ;

    QTest::newRow("nominal: store new bookmarks added to existing history")
        << BookmarkInputs {
                { "./valid1.txt", 20 },
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            }
        << BookmarkInputs {
                { "valid6.txt", 140 },
                { "valid5.txt", 180 },
                {"./valid4.txt", 120}
            }
        << Bookmarks {
                { "./valid4.txt", 120 },
                { "valid5.txt", 180 },
                { "valid6.txt", 140 },
                { "./valid1.txt", 20 },
                { "valid2.txt", 80 },
                { "valid3.txt", 40 }
            }
        ;
}

/**
 * OBJECTIVE:
 *      Call sync() to store new bookmarks.
 *
 * INPUTS:
 *      1. An empty settings file, and no new bookmarks added.
 *      2. An empty settings file, and a valid bookmarks passed to
 *         addRecent().
 *      3. A settings file with valid bookmarks, and valid bookmarks
 *         passed to addRecent().
 *
 * EXPECTED RESULTS:
 *      1. Call to recentFiles() from a new Library instance after the
 *         call to sync() returns an empty list.
 *      2. Call to recentFiles() from a new Library instance after the
 *         call to sync() returns the bookmarks passed to addRecent().
 *      3. Call to recentFiles() from a new Library instance after the
 *         call to sync() returns the bookmarks passed to addRecent(),
 *         plus the original bookmarks from the settings file, in last in
 *         first out order (i.e., most recently added files are first).
 */
void LibraryTest::sync()
{
    QFETCH(BookmarkInputs, existing);
    QFETCH(BookmarkInputs, added);
    QFETCH(Bookmarks, expected);

    populateRecentHistory(existing);
    
    Library library;

    for (BookmarkInputData b : added) {
        library.addRecent(b.filePath, b.cursorPosition);
    }

    // Force write to settings file.
    library.sync();

    // Force fresh read from disk.
    Bookmarks recentFiles = Library().recentFiles();

    QCOMPARE(expected.size(), recentFiles.size());
    
    for (int i = 0; i < recentFiles.size(); i++) {
        QCOMPARE(expected.at(i).filePath(), recentFiles.at(i).filePath());
        QCOMPARE(expected.at(i).cursorPosition(), recentFiles.at(i).cursorPosition());
        QVERIFY(recentFiles.at(i).isValid());
    }
}

void LibraryTest::lookup_data()
{
    QTest::addColumn<BookmarkInputs>("existing");
    QTest::addColumn<QString>("inputFilePath");
    QTest::addColumn<Bookmark>("expected");
    
    QTest::newRow("nominal: find existing bookmark")
        << BookmarkInputs {
                { "valid1.txt", 40 },
                { "valid2.txt", 80 },
                {"./valid3.txt", 20}
            }
        << "./valid2.txt"
        << Bookmark("valid2.txt", 80)
        ;

    QTest::newRow("nominal: find non-existant bookmark")
        << BookmarkInputs {
                { "valid1.txt", 40 },
                { "valid2.txt", 80 },
                {"./valid3.txt", 20}
            }
        << "./valid4.txt"
        << Bookmark() // null (not found)
        ;

    QTest::newRow("robustness: find null file path")
        << BookmarkInputs {
                { "valid1.txt", 40 },
                { "valid2.txt", 80 },
                {"./valid3.txt", 20}
            }
        << QString()
        << Bookmark() // null (not found)
        ;
}

/**
 * OBJECTIVE:
 *      Call lookup() to find bookmark data for a file path.
 *
 * INPUTS:
 *      1. A settings file with valid bookmarks, and a file path
 *         passed to addRecents() that is within the settings file.
 *      2. A settings file with valid bookmarks, and a file path
 *         passed to addRecents() that is NOT within the settings file.
 *      3. A settings file with valid bookmarks, and a file path
 *         passed to addRecents() that is null.
 *
 * EXPECTED RESULTS:
 *      1. Call to lookup() returns matching bookmark from settings file with
 *         the following properties:
 *         - filePath() returns is the same absolute file path as the input
 *           file path.
 *         - cursorPosition() returns the same value in the settings file for
 *           the same path.
 *         - isValid() returns true.
 *         - isNull() returns false.
 *         - lastRead() return value is last read time of the file path.
 *      2. Call to lookup() returns a null bookmark with the following
 *         properties:
 *         - filePath() returns null.
 *         - cursorPosition() returns zero.
 *         - isValid() returns false.
 *         - isNull() returns true.
 *         - lastRead() null date/time.
 *      3. Call to lookup() returns a null bookmark with the following
 *         properties:
 *         - filePath() returns null.
 *         - cursorPosition() returns zero.
 *         - isValid() returns false.
 *         - isNull() returns true.
 *         - lastRead() null date/time.
 */
void LibraryTest::lookup()
{
    QFETCH(QString, inputFilePath);
    QFETCH(BookmarkInputs, existing);
    QFETCH(Bookmark, expected);

    populateRecentHistory(existing);
    
    Library library;

    Bookmark bookmark = library.lookup(inputFilePath);
    
    QCOMPARE(expected.filePath(), bookmark.filePath());
    QCOMPARE(expected.cursorPosition(), bookmark.cursorPosition());
    QCOMPARE(expected.isValid(), bookmark.isValid());
    QCOMPARE(expected.isNull(), bookmark.isNull());
    QCOMPARE(expected.lastRead(), bookmark.lastRead());
}

void LibraryTest::clear_data()
{
    QTest::addColumn<BookmarkInputs>("existing");
    
    QTest::newRow("nominal: clear history with existing bookmarks")
        << BookmarkInputs {
                { "valid1.txt", 40 },
                { "valid2.txt", 80 },
                {"./valid3.txt", 20}
            }
        ;
    
    QTest::newRow("robustness: clear already empty history")
        << BookmarkInputs { }
        ;
}

/**
 * OBJECTIVE:
 *      Call clear() to remove all bookmarks from the recent
 *      files history.
 *
 * INPUTS:
 *      1. A settings file with valid bookmarks.
 *      2. An empty settings file.
 *
 * EXPECTED RESULTS:
 *      1. Call to recentFiles() returns an empty list.
 *      1. Call to recentFiles() returns an empty list.
 */
void LibraryTest::clear()
{
    QFETCH(BookmarkInputs, existing);

    populateRecentHistory(existing);
    
    Library library;

    library.clear();
    library.sync(); // Force write to disk.
    
    // Fetch recent files fresh from the disk.
    Bookmarks recentFiles = Library().recentFiles();

    QVERIFY(recentFiles.isEmpty());
}

QTEST_MAIN(LibraryTest)
#include "librarytest.moc"
