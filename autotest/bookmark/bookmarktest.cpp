/*
 * SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTest>

#include "../../src/bookmark.h"

using namespace ghostwriter;

/**
 * Unit test for the Bookmark class.
 * 
 * Note: The following file is created as part of the test setup:
 *       valid.txt
 */
class BookmarkTest: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void createBookmark_data();
    void createBookmark();
    void copyBookmark();
    void assignBookmark();
    void bookmarksEqual();
    void bookmarksNotEqual();
};

void BookmarkTest::initTestCase()
{
    QFile file("valid.txt");

    file.open(QIODevice::WriteOnly | QIODevice::ReadOnly);
    file.write("test file");
    file.close();
    file.setPermissions(QFileDevice::WriteOther | QFileDevice::ReadOther);
}

void BookmarkTest::cleanupTestCase()
{
    QFile file("valid.txt");
    file.remove();
}

void BookmarkTest::createBookmark_data()
{
    QTest::addColumn<QString>("inputFilePath");
    QTest::addColumn<int>("inputPosition");
    QTest::addColumn<QString>("expectedFilePath");
    QTest::addColumn<int>("expectedPosition");
    QTest::addColumn<bool>("valid");
    QTest::addColumn<bool>("isNull");
    QTest::addColumn<QDateTime>("expectedLastReadTime");

    QFileInfo validFileInfo("valid.txt");

    QDateTime lastReadTime = validFileInfo.lastRead();
    QString path = validFileInfo.absoluteFilePath();

    QTest::newRow("nominal: file path exists, no cursor position specified")
        << "./valid.txt"
        << -99 // Use special value to tell test case to not specify position
        << path
        << 0
        << true
        << false
        << lastReadTime
        ;

    QTest::newRow("nominal: file path exists, cursor position greater than zero")
        << "./valid.txt"
        << 40
        << path
        << 40
        << true
        << false
        << lastReadTime
        ;

    QTest::newRow("robustness: file path does not exist, cursor position is positive")
        << "./foo.txt"
        << 40
        << QFileInfo("foo.txt").absoluteFilePath()
        << 40
        << false
        << false
        << QDateTime()
        ;

    QTest::newRow("robustness: file path is null, cursor position is positive")
        << QString()
        << 40
        << QString()
        << 0
        << false
        << true
        << QDateTime()
        ;

    QTest::newRow("robustness: file path exists, cursor position is negative")
        << "./valid.txt"
        << -700
        << path
        << 0
        << true
        << false
        << lastReadTime
        ;

    QTest::newRow("nominal: no file path or cursor position (null bookmark)")
        << QString()
        << -99
        << QString()
        << 0
        << false
        << true
        << QDateTime()
        ;
}

/**
 * OBJECTIVE:
 *      Create new bookmarks.
 *
 * INPUTS:
 *      1. File path exists, and no cursor position is specified.
 *      2. File path exists, and cursor position is greater than zero.
 *      3. File path does not exist, and cursor position is positive.
 *      4. File path is null, and cursor position is positive.
 *      5. File path exists, and cursor position is negative.
 *      6. No file path or cursor position specified (null bookmark).
 *
 * EXPECTED RESULTS:
 *      1. For the new bookmark:
 *         - cursorPosition() returns 0 (default)
 *         - filePath() returns absolute file path of the input file path
 *         - isValid() returns true
 *         - isNull() returns false
 *      2. For the new bookmark:
 *         - cursorPosition() returns input value
 *         - filePath() returns absolute file path of the input file path
 *         - isValid() returns true
 *         - isNull() returns false
 *      3. For the new bookmark:
 *         - cursorPosition() returns input value
 *         - filePath() returns absolute file path of the input file path
 *         - isValid() returns false
 *         - isNull() returns false
 *      4. For the new bookmark:
 *         - cursorPosition() returns zero
 *         - filePath() returns null
 *         - isValid() returns false
 *         - isNull() returns true
 *      5. For the new bookmark:
 *         - cursorPosition() returns zero
 *         - filePath() returns absolute file path of the input file path
 *         - isValid() returns true
 *         - isNull() returns false
 *      6. For the new bookmark:
 *         - cursorPosition() returns zero
 *         - filePath() returns null
 *         - isValid() returns false
 *         - isNull() returns true
 */
void BookmarkTest::createBookmark()
{
    QFETCH(QString, inputFilePath);
    QFETCH(int, inputPosition);
    QFETCH(QString, expectedFilePath);
    QFETCH(int, expectedPosition);
    QFETCH(bool, valid);
    QFETCH(bool, isNull);
    QFETCH(QDateTime, expectedLastReadTime);

    Bookmark *bookmark = nullptr;

    if (inputFilePath.isNull()) {
        if (-99 == inputPosition) {
            bookmark = new Bookmark();
        } else {
            bookmark = new Bookmark(inputFilePath, inputPosition);
        }
    } else {
        if (-99 == inputPosition) { // Use default position
            bookmark = new Bookmark(inputFilePath);
        } else {
            bookmark = new Bookmark(inputFilePath, inputPosition);
        }
    }

    QFileInfo fileInfo(inputFilePath);

    QCOMPARE(valid, fileInfo.exists());

    QCOMPARE(expectedFilePath, bookmark->filePath());
    QCOMPARE(expectedPosition, bookmark->cursorPosition());
    QCOMPARE(valid, bookmark->isValid());
    QCOMPARE(isNull, bookmark->isNull());
    QCOMPARE(expectedLastReadTime, bookmark->lastRead());

    if (nullptr != bookmark) {
        delete bookmark;
        bookmark = nullptr;
    }
}

/**
 * OBJECTIVE: (nominal case)
 *      Use the copy constructor to copy a bookmark.
 *
 * INPUTS:
 *      - A bookmark to be copied with a valid file path (i.e., file exists)
 *        and a cursor position value of 18.
 *
 * EXPECTED RESULTS:
 *      - New bookmark's cursor position and file path matches the original.
 *      - New bookmark's isValid() method returns the same value as the
 *        original.
 */
void BookmarkTest::copyBookmark()
{
    QString filePath = "./valid.txt";
    QFileInfo fileInfo(filePath);

    QVERIFY(fileInfo.exists());

    Bookmark bookmark1(filePath, 18);
    Bookmark bookmark2(bookmark1);

    QCOMPARE(bookmark2.filePath(), bookmark1.filePath());
    QCOMPARE(bookmark2.cursorPosition(), bookmark1.cursorPosition());
    QCOMPARE(bookmark2.isValid(), bookmark1.isValid());
    QCOMPARE(bookmark2.isNull(), bookmark1.isNull());
    QCOMPARE(bookmark2.lastRead(), bookmark1.lastRead());
}

/**
 * OBJECTIVE: (nominal case)
 *      Assign one bookmark to another with the overloaded = operator.
 *
 * INPUTS:
 *      - A bookmark (assignee) with a valid file path (i.e., file exists) and
 *        a cursor position value of 5000.
 *      - A bookmark to be assigned to the other bookmark with an invalid file
 *        path (does not exist) and a cursor position of 800.
 *
 * EXPECTED RESULTS:
 *      - The assignee bookmark's fields are updated to match those of the
 *        assigned bookmark's.
 */
void BookmarkTest::assignBookmark()
{
    QString filePath1 = "./valid.txt";
    QString filePath2 = "./foo.txt";
    QFileInfo fileInfo1(filePath1);
    QFileInfo fileInfo2(filePath2);

    QVERIFY(fileInfo1.exists());
    QCOMPARE(fileInfo2.exists(), false);

    Bookmark bookmark1(filePath1, 5000);
    Bookmark bookmark2(filePath2, 800);

    bookmark1 = bookmark2;

    QCOMPARE(bookmark1.filePath(), bookmark2.filePath());
    QCOMPARE(bookmark1.cursorPosition(), bookmark2.cursorPosition());
    QCOMPARE(bookmark1.isValid(), bookmark2.isValid());
    QCOMPARE(bookmark1.isNull(), bookmark2.isNull());
    QCOMPARE(bookmark1.lastRead(), bookmark2.lastRead());
}

/**
 * OBJECTIVE: (nominal case)
 *      Call == operator for Bookmark.
 *
 * INPUTS:
 *      None
 *
 * EXPECTED RESULTS:
 *      - Comparison returns true when bookmarks have the same file path.
 *      - Comparison returns false when bookmarks have different file paths.
 */
void BookmarkTest::bookmarksEqual()
{
    Bookmark bookmark1("./valid.txt");
    Bookmark bookmark2("valid.txt", 5);

    QVERIFY(bookmark1 == bookmark2);

    Bookmark bookmark3("./valid.txt", 8);
    Bookmark bookmark4("foo.txt", 8);

    QCOMPARE(bookmark3 == bookmark4, false);
}

/**
 * OBJECTIVE: (nominal case)
 *      Call != operator for Bookmark.
 *
 * INPUTS:
 *      None
 *
 * EXPECTED RESULTS:
 *      - Comparison returns true when bookmarks have different file paths.
 *      - Comparison returns false when bookmarks have the same file paths.
 */
void BookmarkTest::bookmarksNotEqual()
{
    Bookmark bookmark1("./valid.txt");
    Bookmark bookmark2("valid.txt", 5);

    QCOMPARE(bookmark1 != bookmark2, false);

    Bookmark bookmark3("./valid.txt", 8);
    Bookmark bookmark4("foo.txt", 8);

    QVERIFY(bookmark3 != bookmark4);
}

QTEST_MAIN(BookmarkTest)
#include "bookmarktest.moc"
