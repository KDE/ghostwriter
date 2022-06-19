/***********************************************************************
 *
 * Copyright (C) 2022 wereturtle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <QApplication>
#include <QTest>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QString>

#include "../src/asynctextwriter.h"

/**
 * Unit test for the AsyncTextWriter class.
 */
class AsyncTextWriterTest: public QObject
{
    Q_OBJECT

private:
    /**
     * Write helper method (nominal cases).
     * Can use to specify different file name and codec combinations.
     * Set successExpected parameter to verify whether the test is expected
     * to complete the write successfully (true) or else have an
     * error (false).
     */
    void write(const QString &fileName,
        QTextCodec *codec,
        bool successExpected);

private slots:
    void constructor1();
    void setFileName();
    void setCodec();
    void emptyFileName();
    void nullFileName();
    void writeWithCodec();
    void writeWithoutCodec();
    void writeToReadOnlyFile();
    void writeToReadOnlyDirectory();
    void writeAlreadyInProgress();
};

void AsyncTextWriterTest::write(const QString &fileName,
        QTextCodec *codec,
        bool successExpected)
{
    bool writeCompleted = false;
    bool noErrors = true;
    QString expectedContents = "abcdefg\nxyz\n";
    QString actualContents;
    QTextCodec *expectedCodec = codec;

    if (nullptr == codec) {
        // If null input codec, expect default codec.
        expectedCodec = QTextCodec::codecForLocale();
    }

    ghostwriter::AsyncTextWriter writer(fileName);
    writer.setCodec(codec);

    this->connect(
        &writer,
        &ghostwriter::AsyncTextWriter::writeComplete,
        [this, &writer, &writeCompleted]() {
            QCOMPARE(writer.writeInProgress(), false);
            writeCompleted = true;
        }
    );

    this->connect(
        &writer,
        &ghostwriter::AsyncTextWriter::writeError,
        [this, &writer, &noErrors](const QString &err) {
            noErrors = false;
            qWarning() << QString("Error writing to file: ") + err;
        }
    );

    // Verify write is not in progress before call to write().
    QCOMPARE(writer.writeInProgress(), false);

    bool status = writer.write(expectedContents);
    QCOMPARE(status, true);

    // Give the thread time to complete the write.
    QThread::sleep(1);

    // Verify write is in progress before signals are received.
    QVERIFY(writer.writeInProgress());

    // Force signal/event processing.
    qApp->processEvents();

    // Verify whether writeError() signal was received.
    QCOMPARE(noErrors, successExpected);

    // Verify whether writeCompleted() signal was received.
    QCOMPARE(writeCompleted, successExpected);

    // Verify write is no longer in progress.
    QCOMPARE(writer.writeInProgress(), false);

    if (successExpected) {
        QFile file(writer.fileName());
        bool fileReadable = file.open(QIODevice::ReadOnly | QIODevice::Text);

        QVERIFY(fileReadable);

        if (fileReadable) {
            QTextStream stream(&file);

            QString actualContents;
            actualContents = stream.readAll();

            // Verify codec.
            QCOMPARE(stream.codec(), expectedCodec);

            file.close();

            // Verify file contents read from disk.
            QCOMPARE(actualContents, expectedContents);

            // Cleanup.
            file.remove();
        }
    }
}

void AsyncTextWriterTest::constructor1()
{
    QString fileName = "constructor.txt";
    QString expectedFileName = QFileInfo(fileName).absoluteFilePath();

    ghostwriter::AsyncTextWriter writer(fileName, this);

    QCOMPARE(writer.fileName(), expectedFileName);
    QCOMPARE(writer.codec(), nullptr);
    QCOMPARE(writer.parent(), this);
}

void AsyncTextWriterTest::setFileName()
{
    QString oldfileName = "oldname.txt";
    QString expectedFileName = QFileInfo(oldfileName).absoluteFilePath();

    ghostwriter::AsyncTextWriter writer(oldfileName);
    QCOMPARE(writer.fileName(), expectedFileName);

    QString newFileName = "newname.txt";
    expectedFileName = QFileInfo(newFileName).absoluteFilePath();
    writer.setFileName(newFileName);
    QCOMPARE(writer.fileName(), expectedFileName);
}

void AsyncTextWriterTest::setCodec()
{
    QString fileName = "codectest.txt";

    ghostwriter::AsyncTextWriter writer("codectest.txt");
    QCOMPARE(writer.codec(), nullptr);

    QTextCodec *expectedCodec = QTextCodec::codecForName("UTF-8");
    writer.setCodec(QTextCodec::codecForName("UTF-8"));
    QCOMPARE(writer.codec()->name(), expectedCodec->name());
}

/**
 * OBJECTIVE:
 *
 *      Call write() with codec specified (nominal case).
 *
 * INPUTS:
 *
 *      - Valid new file name.
 *      - String to write to the file.
 *      - UTF-8 codec.
 *
 * EXPECTED RESULTS:
 *
 *      - Write is NOT in progress before call to write().
 *      - Write is in progress after call to write() before writeComplete()
 *        signal is received.
 *      - write() returns true.
 *      - Write is NOT in progress after writeComplete() signal is received.
 *      - writeComplete() signal is received.
 *      - writeError() signal is NOT received.
 *      - File is created and written to successfully.
 *      - File contents match input string.
 *      - QTextStream detects UTF-8 codec when reading the file.
 */
void AsyncTextWriterTest::writeWithCodec()
{
    write("write.txt", QTextCodec::codecForName("UTF-8"), true);
}

/**
 * OBJECTIVE:
 *
 *      Call write() with default codec (nominal case).
 *
 * INPUTS:
 *
 *      - Valid new file name.
 *      - String to write to the file.
 *      - Null codec (specifying to use default)
 *
 * EXPECTED RESULTS:
 *
 *      - Write is NOT in progress before call to write().
 *      - Write is in progress after call to write() before writeComplete()
 *        signal is received.
 *      - write() returns true.
 *      - Write is NOT in progress after writeComplete() signal is received.
 *      - writeComplete() signal is received.
 *      - writeError() signal is NOT received.
 *      - File is created and written to successfully.
 *      - File contents match input string.
 *      - QTextStream detects default codec when reading the file.
 */
void AsyncTextWriterTest::writeWithoutCodec()
{
    write("write.txt", nullptr, true);
}

/**
 * OBJECTIVE:
 *
 *      Call write() with writer having empty file name.
 *
 * INPUTS:
 *
 *      Empty file name string.
 *
 * EXPECTED RESULTS:
 *
 *      write() returns false.
 */
void AsyncTextWriterTest::emptyFileName()
{
    QString fileName = QString("");

    ghostwriter::AsyncTextWriter writer(fileName);

    // Verify return value.
    QCOMPARE(writer.write("empty"), false);
}

/**
 * OBJECTIVE:
 *
 *      Call write() with writer having null file name.
 *
 * INPUTS:
 *
 *      Null file name string.
 *
 * EXPECTED RESULTS:
 *
 *      write() returns false.
 */
void AsyncTextWriterTest::nullFileName()
{
    QString fileName = QString();

    ghostwriter::AsyncTextWriter writer(fileName);

    // Verify return value.
    QCOMPARE(writer.write("null"), false);
}

/**
 * OBJECTIVE:
 *
 *      Attempt to write to a file whose permissions are read-only.
 *      (robustness case).
 *
 * INPUTS:
 *
 *      A file name that already exists on disk as read-only.
 *
 * EXPECTED RESULTS:
 *
 *      A writeError() signal is received.
 */
void AsyncTextWriterTest::writeToReadOnlyFile()
{
    QString fileName = "readonly.txt";

    QFile file(fileName);
    bool readOnlyFileCreated =
        file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);

    // Verify file could be created.
    QVERIFY(readOnlyFileCreated);

    QTextStream stream(&file);

    stream << "This is a read-only file.";

    file.close();

    // Set file permissions to read-only.
    file.setPermissions(QFileDevice::ReadOwner |
        QFileDevice::ReadUser |
        QFileDevice::ReadGroup |
        QFileDevice::ReadOwner |
        QFileDevice::ReadOther);

    write(fileName, nullptr, false);

    // Cleanup.
    file.remove();
}

/**
 * OBJECTIVE:
 *
 *      Attempt to create a file in a read-only directory (robustness case).
 *
 * INPUTS:
 *
 *      A new file name whose path is in a read-only directory.
 *
 * EXPECTED RESULTS:
 *
 *      A writeError() signal is received.
 */
void AsyncTextWriterTest::writeToReadOnlyDirectory()
{
    bool readOnlyDirectoryCreated;
    QDir dir("");

    readOnlyDirectoryCreated = dir.mkdir("readonly");

    // Verify directory could be created.
    QVERIFY(readOnlyDirectoryCreated);

    dir.cd("readonly");

    QFile dirFile(dir.path());

    // Set directory permissions to read-only.
    dirFile.setPermissions(QFileDevice::ReadOwner |
        QFileDevice::ReadUser |
        QFileDevice::ReadGroup |
        QFileDevice::ReadOwner |
        QFileDevice::ReadOther);

    QString fileName = dir.path() + "/newfile.txt";

    write(fileName, nullptr, false);

    // Cleanup.
    dir.cdUp();
    dir.rmdir("readonly");
}

/**
 * OBJECTIVE:
 *
 *      Attempt to write when a write is already in progress.
 *
 * INPUTS:
 *
 *      First call to write() with initial text string.
 *      Second call to write() while first write is in progress
 *         with new text string.
 *
 * EXPECTED RESULTS:
 *
 *      - First call to write() returns true.
 *      - Second call to write() returns true.
 *      - File is created and written to successfully.
 *      - File contents match input string of final write() call.
 */
void AsyncTextWriterTest::writeAlreadyInProgress()
{
    bool writeCompleted = false;
    bool noErrors = true;
    QString fileName = "inprogress.txt";
    QString expectedContents = "12345\n6789\n0";
    bool firstCallStatus;
    bool secondCallStatus;

    ghostwriter::AsyncTextWriter writer(fileName);

    this->connect(
        &writer,
        &ghostwriter::AsyncTextWriter::writeComplete,
        [this, &writer, &writeCompleted]() {
            writeCompleted = true;
        }
    );

    this->connect(
        &writer,
        &ghostwriter::AsyncTextWriter::writeError,
        [this, &writer, &noErrors](const QString &err) {
            noErrors = false;
            qWarning() << QString("Error writing to file: ") + err;
        }
    );

    firstCallStatus = writer.write("Hello, world!\n");
    secondCallStatus = writer.write(expectedContents);

    // Give the thread time to complete the writes.
    QThread::sleep(1);

    // Force signal/event processing.
    qApp->processEvents();

    // Verify first call's return value.
    QCOMPARE(firstCallStatus, true);
    QCOMPARE(secondCallStatus, true);

    // Verify writeError() signal was NOT received.
    QVERIFY(noErrors);

    // Verify writeCompleted() signal was received.
    QVERIFY(writeCompleted);

    // Verify write is no longer in progress.
    QCOMPARE(writer.writeInProgress(), false);

    QFile file(writer.fileName());
    bool fileReadable = file.open(QIODevice::ReadOnly | QIODevice::Text);

    QVERIFY(fileReadable);

    if (fileReadable) {
        QString actualContents;
        QTextStream stream(&file);
        actualContents = stream.readAll();
        file.close();

        // Verify file contents read from disk.
        QCOMPARE(actualContents, expectedContents);

        // Cleanup.
        file.remove();
    }
}

QTEST_MAIN(AsyncTextWriterTest)
#include "asynctextwritertest.moc"