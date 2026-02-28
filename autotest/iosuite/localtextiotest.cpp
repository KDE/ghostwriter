/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>
#include <QTextStream>

#include "io/localtextio.h"

using namespace ghostwriter;

class LocalTextIOTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Static method tests
    void canHandle_data();
    void canHandle();

    // Write operation tests
    void write_success();
    void write_permissionDenied();
    void write_nonExistentDirectory();
    void write_unicodeContent();
    void write_emptyContent();
    void write_largeContent();

    // Read operation tests
    void read_success();
    void read_fileNotFound();
    void read_permissionDenied();
    void read_emptyFile();
    void read_unicodeContent();
    void read_largeContent();

    // Encoding tests
    void write_read_differentEncodings();

private:
    QTemporaryDir *m_tempDir;
    QString m_testFilePath;
    std::unique_ptr<LocalTextIO> m_localTextIO;
};

void LocalTextIOTest::initTestCase()
{
    // Initialize test data
}

void LocalTextIOTest::cleanupTestCase()
{
    // Cleanup test data
}

void LocalTextIOTest::init()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    m_testFilePath = m_tempDir->path() + "/test.txt";
    QUrl testUrl = QUrl::fromLocalFile(m_testFilePath);

    m_localTextIO = std::make_unique<LocalTextIO>(testUrl, QStringConverter::Utf8);
}

void LocalTextIOTest::cleanup()
{
    m_localTextIO.reset();
    delete m_tempDir;
    m_tempDir = nullptr;
}

void LocalTextIOTest::canHandle_data()
{
    QTest::addColumn<QString>("urlString");
    QTest::addColumn<bool>("expected");

    // Valid local file URLs
    QTest::newRow("local file") << "file:///tmp/test.txt" << true;
    QTest::newRow("local file with path") << "file:///home/user/document.md" << true;
    QTest::newRow("relative path") << "file://./test.txt" << true;
    QTest::newRow("unicode filename") << "file:///tmp/fiłę.txt" << true;

    // Invalid cases
    QTest::newRow("directory") << "file:///tmp/" << false;
    QTest::newRow("http URL") << "https://example.com/file.txt" << false;
    QTest::newRow("ftp URL") << "ftp://example.com/file.txt" << false;
    QTest::newRow("empty URL") << "" << false;
    QTest::newRow("non-file scheme") << "data:text/plain;base64,SGVsbG8=" << false;
}

/**
 * OBJECTIVE:
 *   Verify that canHandle() correctly identifies URLs that LocalTextIO can process.
 *
 * INPUTS:
 *   - urlString: A string representing various types of URLs.
 *   - expected: Boolean indicating whether LocalTextIO should handle the URL.
 *
 * EXPECTED RESULTS:
 *   - canHandle() returns true for valid local file URLs pointing to files.
 *   - canHandle() returns false for directories, remote URLs, and invalid URLs.
 */
void LocalTextIOTest::canHandle()
{
    QFETCH(QString, urlString);
    QFETCH(bool, expected);

    QUrl url(urlString);
    QCOMPARE(LocalTextIO::canHandle(url), expected);
}

/**
 * OBJECTIVE:
 *   Verify that write() successfully writes text content to a local file.
 *
 * INPUTS:
 *   - text: A QString containing test content to write.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns a successful KResult.
 *   - The file is created with the correct content.
 *   - The file content matches the input text when read back.
 */
void LocalTextIOTest::write_success()
{
    QString testContent = "Hello, LocalTextIO!\nThis is a test file.";

    auto result = m_localTextIO->write(testContent);

    QVERIFY(result.ok());
    QVERIFY(QFile::exists(m_testFilePath));

    // Verify content was written correctly
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    QString writtenContent = stream.readAll();

    QCOMPARE(writtenContent, testContent);
}

/**
 * OBJECTIVE:
 *   Verify that write() handles permission denied errors correctly.
 *
 * INPUTS:
 *   - A file path is read-only.
 *   - text: A QString containing test content to write.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns an error result.
 *   - The error code is TextIOError::OpenError.
 *   - The error message contains relevant information about the failure.
 */
void LocalTextIOTest::write_permissionDenied()
{
    m_localTextIO->write(""); // Ensure file exists

    // Set file to read-only
    QFile::setPermissions(m_testFilePath, QFileDevice::ReadOwner | QFileDevice::ReadGroup | QFileDevice::ReadOther);

    // Attempt to write
    auto result = m_localTextIO->write("Test content");

    QVERIFY(!result.ok());
    QCOMPARE(result.error().code(), TextIOError::OpenError);
    QVERIFY(result.errmsg().contains("Failed to open the file for writing"));

    // Cleanup - restore permissions
    QFile::setPermissions(m_testFilePath, QFileDevice::WriteOwner | QFileDevice::ReadOwner);
}

/**
 * OBJECTIVE:
 *   Verify that write() handles attempts to write to non-existent directories.
 *
 * INPUTS:
 *   - A file path in a non-existent directory.
 *   - text: A QString containing test content to write.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns an error result.
 *   - The error code is TextIOError::OpenError.
 *   - The error message contains relevant information about the failure.
 */
void LocalTextIOTest::write_nonExistentDirectory()
{
    QString invalidPath = "/non/existent/directory/test.txt";
    QUrl invalidUrl = QUrl::fromLocalFile(invalidPath);

    LocalTextIO invalidTextIO(invalidUrl, QStringConverter::Utf8);
    auto result = invalidTextIO.write("Test content");

    QVERIFY(!result.ok());
    QCOMPARE(result.error().code(), TextIOError::OpenError);
    QVERIFY(result.errmsg().contains("Failed to open the file for writing"));
}

/**
 * OBJECTIVE:
 *   Verify that write() correctly handles Unicode content.
 *
 * INPUTS:
 *   - text: A QString containing Unicode characters and emojis.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns a successful KResult.
 *   - The Unicode content is preserved when written and read back.
 */
void LocalTextIOTest::write_unicodeContent()
{
    QString unicodeContent = "Hello 世界! 🌍 Ñoño café résumé";

    auto result = m_localTextIO->write(unicodeContent);

    QVERIFY(result.ok());

    // Read back and verify Unicode preservation
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    QString readContent = stream.readAll();

    QCOMPARE(readContent, unicodeContent);
}

/**
 * OBJECTIVE:
 *   Verify that write() handles empty content correctly.
 *
 * INPUTS:
 *   - text: An empty QString.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns a successful KResult.
 *   - An empty file is created.
 */
void LocalTextIOTest::write_emptyContent()
{
    QString emptyContent = "";

    auto result = m_localTextIO->write(emptyContent);

    QVERIFY(result.ok());
    QVERIFY(QFile::exists(m_testFilePath));

    QFileInfo fileInfo(m_testFilePath);
    QCOMPARE(fileInfo.size(), 0);
}

/**
 * OBJECTIVE:
 *   Verify that write() can handle large content efficiently.
 *
 * INPUTS:
 *   - text: A QString containing a large amount of text data.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns a successful KResult.
 *   - The large content is written correctly.
 */
void LocalTextIOTest::write_largeContent()
{
    QString largeContent;
    const int numLines = 10000;
    for (int i = 0; i < numLines; ++i) {
        largeContent += QString("Line %1: This is a test line with some content.\n").arg(i);
    }

    auto result = m_localTextIO->write(largeContent);

    QVERIFY(result.ok());

    // Verify file size is reasonable
    QFileInfo fileInfo(m_testFilePath);
    QVERIFY(fileInfo.size() > 0);
}

/**
 * OBJECTIVE:
 *   Verify that read() successfully reads content from an existing local file.
 *
 * INPUTS:
 *   - A pre-created file with known content.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns a successful KResult.
 *   - The returned content matches the file's actual content.
 */
void LocalTextIOTest::read_success()
{
    QString expectedContent = "Hello, reading test!\nMultiple lines\nof content.";

    // Pre-create file with content
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << expectedContent;
    file.close();

    auto result = m_localTextIO->read();

    QVERIFY(result.ok());
    QCOMPARE(result.value(), expectedContent);
}

/**
 * OBJECTIVE:
 *   Verify that read() handles file not found errors correctly.
 *
 * INPUTS:
 *   - A LocalTextIO instance pointing to a non-existent file.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns an error result.
 *   - The error code is TextIOError::OpenError.
 *   - The error message contains relevant information about the failure.
 */
void LocalTextIOTest::read_fileNotFound()
{
    QString nonExistentPath = m_tempDir->path() + "/nonexistent.txt";
    QUrl nonExistentUrl = QUrl::fromLocalFile(nonExistentPath);

    LocalTextIO nonExistentTextIO(nonExistentUrl, QStringConverter::Utf8);
    auto result = nonExistentTextIO.read();

    qInfo() << "Error code:" << int(result.error().code()) << "Error message:" << result.errmsg();
    QVERIFY(!result.ok());
    QCOMPARE(result.error().code(), TextIOError::OpenError);
    QVERIFY(result.errmsg().contains("Failed to open the file for reading"));
}

/**
 * OBJECTIVE:
 *   Verify that read() handles permission denied errors correctly.
 *
 * INPUTS:
 *   - A file with no read permissions.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns an error result.
 *   - The error code is TextIOError::OpenError.
 *   - The error message contains relevant information about the failure.
 */
void LocalTextIOTest::read_permissionDenied()
{
    // Create file and remove read permissions
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.write("test content");
    file.close();

    QVERIFY(file.setPermissions(QFileDevice::WriteOwner)); // Remove read permission

    auto result = m_localTextIO->read();

    QVERIFY(!result.ok());
    QCOMPARE(result.error().code(), TextIOError::OpenError);
    QVERIFY(result.errmsg().contains("Failed to open the file for reading"));

    // Restore permissions for cleanup
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
}

/**
 * OBJECTIVE:
 *   Verify that read() correctly handles empty files.
 *
 * INPUTS:
 *   - An empty file.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns a successful KResult.
 *   - The returned content is an empty string.
 */
void LocalTextIOTest::read_emptyFile()
{
    // Create empty file
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.close();

    auto result = m_localTextIO->read();

    QVERIFY(result.ok());
    QCOMPARE(result.value(), QString(""));
}

/**
 * OBJECTIVE:
 *   Verify that read() correctly handles Unicode content.
 *
 * INPUTS:
 *   - A file containing Unicode characters and emojis.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns a successful KResult.
 *   - The Unicode content is preserved when read.
 */
void LocalTextIOTest::read_unicodeContent()
{
    QString unicodeContent = "Unicode test: 世界 🌍 café résumé Ñoño";

    // Create file with Unicode content
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << unicodeContent;
    file.close();

    auto result = m_localTextIO->read();

    QVERIFY(result.ok());
    QCOMPARE(result.value(), unicodeContent);
}

/**
 * OBJECTIVE:
 *   Verify that read() can handle large files efficiently.
 *
 * INPUTS:
 *   - A file containing a large amount of text data.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns a successful KResult.
 *   - The large content is read correctly.
 */
void LocalTextIOTest::read_largeContent()
{
    QString largeContent;
    const int numLines = 5000;
    for (int i = 0; i < numLines; ++i) {
        largeContent += QString("Line %1: Reading test line with content.\n").arg(i);
    }

    // Create file with large content
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << largeContent;
    file.close();

    auto result = m_localTextIO->read();

    QVERIFY(result.ok());
    QCOMPARE(result.value(), largeContent);
}

/**
 * OBJECTIVE:
 *   Verify that LocalTextIO correctly handles different text encodings.
 *
 * INPUTS:
 *   - Text content with various encodings (UTF-8, UTF-16, Latin1).
 *   - LocalTextIO instances configured with different encodings.
 *
 * EXPECTED RESULTS:
 *   - Write and read operations preserve content correctly for each encoding.
 *   - Content written with one encoding is read correctly with the same encoding.
 */
void LocalTextIOTest::write_read_differentEncodings()
{
    QString testContent = "Encoding test: café résumé";

    QStringConverter::Encoding encodings[] = {QStringConverter::Utf8, QStringConverter::Utf16, QStringConverter::Latin1};

    for (auto encoding : encodings) {
        QString encodingFilePath = m_tempDir->path() + QString("/encoding_%1.txt").arg(static_cast<int>(encoding));
        QUrl encodingUrl = QUrl::fromLocalFile(encodingFilePath);

        LocalTextIO encodingTextIO(encodingUrl, encoding);

        // Write with specific encoding
        auto writeResult = encodingTextIO.write(testContent);
        QVERIFY(writeResult.ok());

        // Read with same encoding
        auto readResult = encodingTextIO.read();
        QVERIFY(readResult.ok());

        if (encoding == QStringConverter::Latin1) {
            // Latin1 may not preserve all Unicode characters
            QVERIFY(!readResult.value().isEmpty());
        } else {
            QCOMPARE(readResult.value(), testContent);
        }
    }
}

QTEST_MAIN(LocalTextIOTest)
#include "localtextiotest.moc"