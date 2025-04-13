/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTest>

#include "../../src/document/textdocument.h"

using namespace ghostwriter;

/**
 * Unit test for the TextDocument class.
 *
 * Note: The following file is created as part of the test setup:
 *       valid.txt
 */
class TextDocumentTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void create();
    void openReadable_data();
    void openReadable();
};

void TextDocumentTest::initTestCase()
{
    QFile file("valid.txt");

    file.open(QIODevice::WriteOnly);
    file.write("test file");
    file.close();
    file.setPermissions(QFileDevice::WriteOther | QFileDevice::ReadOther);

    file = QFile("readonly.txt");
    file.open(QIODevice::WriteOnly);
    file.write("test read only file");
    file.close();
    file.setPermissions(QFileDevice::ReadOther);

    file = QFile("writeonly.txt");
    file.open(QIODevice::WriteOnly);
    file.write("test write only file");
    file.close();
    file.setPermissions(QFileDevice::WriteOther);
}

void TextDocumentTest::cleanupTestCase()
{
    QStringList files = {"valid.txt", "readonly.txt", "writeonly.txt"};

    for (auto fileName : files) {
        QFile file(fileName);
        file.remove;
    }
}

void TextDocumentTest::create()
{
    auto result = TextDocument::create("Untitled-1");

    QVERIFY(bool(result))

    TextDocument *document = *result;

    QVERIFY(document != nullptr);
    QVERIFY(document->filePath().isNull());
    QVERIFY(document->fileName().isNull());
    QVERIFY(document->isDraft());
    QVERIFY(!document->backed());
    QVERIFY(!document->modified());
    QVERIFY(!document->hasConflict());
    QVERIFY(document->isWritable());
    QCOMPARE(document->lastModified(), QDateTime());
    QCOMPARE(document->displayName(), "Untitled-1");
}

void TextDocumentTest::openReadable_data()
{
    QTest::addColumn<QString>("inputFilePath");
    QTest::addColumn<bool>("writeable");
    QTest::addColumn<QString>("displayName");
    QTest::addColumn<QString>("contents");

    QTest::newRow("nominal: file is writeable") << "./valid.txt" << true << "valid.txt"
                                                << "test file";

    QTest::newRow("robustness: file is read-only") << "./readonly.txt" << false << "readonly.txt"
                                                   << "test read only file";
}

void TextDocumentTest::openReadable()
{
    QFETCH(QString, inputFilePath);
    QFETCH(bool, writeable);
    QFETCH(QString, displayName);
    QFETCH(QString, contents);

    auto result = TextDocument::open(inputFilePath);

    QVERIFY(bool(result))

    TextDocument *document = *result;
    QFileInfo inputFileInfo(inputFilePath);

    QVERIFY(document != nullptr);
    QCOMPARE(document->filePath(), inputFileInfo.filePath());
    QCOMPARE(document->fileName(), inputFileInfo.fileName());
    QVERIFY(!document->isDraft());
    QVERIFY(!document->modified());
    QVERIFY(!document->hasConflict());
    QCOMPARE(document->isWritable(), writeable);
    QCOMPARE(document->displayName(), displayName);
    QCOMPARE(document->toPlainText(), contents);
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

QTEST_MAIN(TextDocumentTest)
#include "textdocumenttest.moc"
