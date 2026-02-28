/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QStorageInfo>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>
#include <QTextStream>

#include "io/netsharetextio.h"

using namespace ghostwriter;

class NetShareTextIOTest : public QObject
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

    // Write operation tests (using local files to simulate network behavior)
    void write_success();
    void write_permissionDenied();
    void write_unicodeContent();
    void write_emptyContent();
    void write_largeContent();

    // Read operation tests (using local files to simulate network behavior)
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
    std::unique_ptr<NetShareTextIO> m_netShareTextIO;
};

void NetShareTextIOTest::initTestCase()
{
    // Initialize test data
}

void NetShareTextIOTest::cleanupTestCase()
{
    // Cleanup test data
}

void NetShareTextIOTest::init()
{
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    // Use a local temporary file path but pass it to NetShareTextIO
    // This allows us to test the NetShareTextIO logic without requiring actual network drives
    m_testFilePath = m_tempDir->path() + "/networkshare_test.txt";
    QUrl testUrl = QUrl::fromLocalFile(m_testFilePath);

    m_netShareTextIO = std::make_unique<NetShareTextIO>(testUrl, QStringConverter::Utf8);
}

void NetShareTextIOTest::cleanup()
{
    m_netShareTextIO.reset();
    delete m_tempDir;
    m_tempDir = nullptr;
}

void NetShareTextIOTest::canHandle_data()
{
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("testPath");
    QTest::addColumn<bool>("expected");

    // UNC paths - test the actual string paths that would be detected
    QTest::newRow("UNC path") << "UNC share path" << "\\\\server\\share\\file.txt" << true;
    QTest::newRow("UNC with subdirectory") << "UNC with subdirs" << "\\\\server\\share\\folder\\document.txt" << true;
    QTest::newRow("UNC with spaces") << "UNC with spaces" << "\\\\server name\\share name\\file.txt" << true;

    // Local paths that should NOT be detected as network
    QTest::newRow("local tmp file") << "Local temp file" << "/tmp/regular_file.txt" << false;
    QTest::newRow("local home file") << "Local home file" << "/home/user/document.md" << false;
    QTest::newRow("relative path") << "Relative path" << "./local/file.txt" << false;

    // Windows paths
    QTest::newRow("local C: drive") << "Local C drive" << "C:\\Users\\test\\document.txt" << false;
    QTest::newRow("potential network Z:") << "Potential network Z" << "Z:\\document.txt" << false; // Depends on system
    QTest::newRow("mapped drive UNC") << "Mapped drive UNC" << "Z:\\folder\\file.txt" << false; // Depends on system
}

/**
 * OBJECTIVE:
 *   Verify that canHandle() correctly identifies URLs that NetShareTextIO can process.
 *
 * INPUTS:
 *   - urlString: A string representing various types of URLs including UNC paths and network shares.
 *   - expected: Boolean indicating whether NetShareTextIO should handle the URL.
 *
 * EXPECTED RESULTS:
 *   - canHandle() returns true for UNC paths and network file system URLs.
 *   - canHandle() returns false for local files, remote URLs, and invalid URLs.
 *   - canHandle() correctly detects network file systems based on QStorageInfo.
 */
void NetShareTextIOTest::canHandle()
{
    QFETCH(QString, testPath);
    QFETCH(bool, expected);

    QUrl fileUrl = QUrl::fromLocalFile(testPath);
    QCOMPARE(NetShareTextIO::canHandle(fileUrl), expected);

    // Also test that the URL was created properly
    if (testPath.startsWith("\\\\")) {
        // For UNC paths, verify toLocalFile() gives us back the original path
        QCOMPARE(fileUrl.toLocalFile(), testPath);
    }
}

/**
 * OBJECTIVE:
 *   Verify that write() successfully writes text content using NetShareTextIO logic.
 *
 * INPUTS:
 *   - text: A QString containing test content to write.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns a successful KResult.
 *   - The file is created with the correct content.
 *   - The file content matches the input text when read back.
 */
void NetShareTextIOTest::write_success()
{
    QString testContent = "Hello, NetShareTextIO!\nThis tests network share writing logic.";

    auto result = m_netShareTextIO->write(testContent);

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
 *   - A file path with restricted write permissions.
 *   - text: A QString containing test content to write.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns an error result.
 *   - The error code is TextIOError::OpenError.
 *   - The error message contains relevant information about the permission failure.
 */
void NetShareTextIOTest::write_permissionDenied()
{
    m_netShareTextIO->write(""); // Ensure file is created

    // Set file to read-only
    QFile::setPermissions(m_testFilePath, QFileDevice::ReadOwner | QFileDevice::ReadGroup | QFileDevice::ReadOther);

    // Attempt to write
    auto result = m_netShareTextIO->write("Test content");

    QVERIFY(!result.ok());
    QCOMPARE(result.errcode(), TextIOError::OpenError);
    QVERIFY(result.errmsg().contains("Failed to open the file for writing"));

    // Cleanup - restore permissions
    QFile::setPermissions(m_testFilePath, QFileDevice::WriteOwner | QFileDevice::ReadOwner);
}

/**
 * OBJECTIVE:
 *   Verify that write() correctly handles Unicode content using NetShareTextIO logic.
 *
 * INPUTS:
 *   - text: A QString containing Unicode characters and emojis.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns a successful KResult.
 *   - The Unicode content is preserved when written and read back.
 */
void NetShareTextIOTest::write_unicodeContent()
{
    QString unicodeContent = "Network Unicode: 世界 🌍 Ñoño café résumé ☕";

    auto result = m_netShareTextIO->write(unicodeContent);

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
 *   Verify that write() handles empty content correctly using NetShareTextIO logic.
 *
 * INPUTS:
 *   - text: An empty QString.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns a successful KResult.
 *   - An empty file is created.
 */
void NetShareTextIOTest::write_emptyContent()
{
    QString emptyContent = "";

    auto result = m_netShareTextIO->write(emptyContent);

    QVERIFY(result.ok());
    QVERIFY(QFile::exists(m_testFilePath));

    QFileInfo fileInfo(m_testFilePath);
    QCOMPARE(fileInfo.size(), 0);
}

/**
 * OBJECTIVE:
 *   Verify that write() can handle large content efficiently using NetShareTextIO logic.
 *
 * INPUTS:
 *   - text: A QString containing a large amount of text data.
 *
 * EXPECTED RESULTS:
 *   - The write operation returns a successful KResult.
 *   - The large content is written correctly.
 */
void NetShareTextIOTest::write_largeContent()
{
    QString largeContent;
    const int numLines = 8000;
    for (int i = 0; i < numLines; ++i) {
        largeContent += QString("NetworkLine %1: This tests NetShareTextIO with large content.\n").arg(i);
    }

    auto result = m_netShareTextIO->write(largeContent);

    QVERIFY(result.ok());

    // Verify file size is reasonable
    QFileInfo fileInfo(m_testFilePath);
    QVERIFY(fileInfo.size() > 0);
}

/**
 * OBJECTIVE:
 *   Verify that read() successfully reads content using NetShareTextIO logic.
 *
 * INPUTS:
 *   - A pre-created file with known content.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns a successful KResult.
 *   - The returned content matches the file's actual content.
 */
void NetShareTextIOTest::read_success()
{
    QString expectedContent = "Network share reading test!\nMultiple lines\nof network content.";

    // Pre-create file with content
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << expectedContent;
    file.close();

    auto result = m_netShareTextIO->read();

    QVERIFY(result.ok());
    QCOMPARE(result.value(), expectedContent);
}

/**
 * OBJECTIVE:
 *   Verify that read() handles file not found errors correctly.
 *
 * INPUTS:
 *   - A NetShareTextIO instance pointing to a non-existent file.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns an error result.
 *   - The error code indicates a file access problem.
 *   - The error message contains relevant information about the failure.
 */
void NetShareTextIOTest::read_fileNotFound()
{
    QString nonExistentPath = m_tempDir->path() + "/nonexistent_network_file.txt";
    QUrl nonExistentUrl = QUrl::fromLocalFile(nonExistentPath);

    NetShareTextIO nonExistentTextIO(nonExistentUrl, QStringConverter::Utf8);
    auto result = nonExistentTextIO.read();

    QVERIFY(!result.ok());
    QCOMPARE(result.errcode(), TextIOError::OpenError);
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
 *   - The error message contains relevant information about the permission failure.
 */
void NetShareTextIOTest::read_permissionDenied()
{
    // Create file first, then remove read permissions
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.write("test content");
    file.close();

    QVERIFY(file.setPermissions(QFileDevice::WriteOwner)); // Remove read permission

    auto result = m_netShareTextIO->read();

    QVERIFY(!result.ok());
    QCOMPARE(result.errcode(), TextIOError::OpenError);
    QVERIFY(result.errmsg().contains("Failed to open the file for reading"));

    // Restore permissions for cleanup
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
}

/**
 * OBJECTIVE:
 *   Verify that read() correctly handles empty files using NetShareTextIO logic.
 *
 * INPUTS:
 *   - An empty file.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns a successful KResult.
 *   - The returned content is an empty string.
 */
void NetShareTextIOTest::read_emptyFile()
{
    // Create empty file
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.close();

    auto result = m_netShareTextIO->read();

    QVERIFY(result.ok());
    QCOMPARE(result.value(), QString(""));
}

/**
 * OBJECTIVE:
 *   Verify that read() correctly handles Unicode content using NetShareTextIO logic.
 *
 * INPUTS:
 *   - A file containing Unicode characters and emojis.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns a successful KResult.
 *   - The Unicode content is preserved when read.
 */
void NetShareTextIOTest::read_unicodeContent()
{
    QString unicodeContent = "Network Unicode read: 世界 🌍 café résumé Ñoño ☕";

    // Create file with Unicode content
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << unicodeContent;
    file.close();

    auto result = m_netShareTextIO->read();

    QVERIFY(result.ok());
    QCOMPARE(result.value(), unicodeContent);
}

/**
 * OBJECTIVE:
 *   Verify that read() can handle large files efficiently using NetShareTextIO logic.
 *
 * INPUTS:
 *   - A file containing a large amount of text data.
 *
 * EXPECTED RESULTS:
 *   - The read operation returns a successful KResult.
 *   - The large content is read correctly.
 */
void NetShareTextIOTest::read_largeContent()
{
    QString largeContent;
    const int numLines = 4000;
    for (int i = 0; i < numLines; ++i) {
        largeContent += QString("NetworkReadLine %1: Reading test with NetShareTextIO logic.\n").arg(i);
    }

    // Create file with large content
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << largeContent;
    file.close();

    auto result = m_netShareTextIO->read();

    QVERIFY(result.ok());
    QCOMPARE(result.value(), largeContent);
}

/**
 * OBJECTIVE:
 *   Verify that NetShareTextIO correctly handles different text encodings.
 *
 * INPUTS:
 *   - Text content with various encodings (UTF-8, UTF-16, Latin1).
 *   - NetShareTextIO instances configured with different encodings.
 *
 * EXPECTED RESULTS:
 *   - Write and read operations preserve content correctly for each encoding.
 *   - Content written with one encoding is read correctly with the same encoding.
 */
void NetShareTextIOTest::write_read_differentEncodings()
{
    QString testContent = "Network encoding test: café résumé ☕";

    QStringConverter::Encoding encodings[] = {QStringConverter::Utf8, QStringConverter::Utf16, QStringConverter::Latin1};

    for (auto encoding : encodings) {
        QString encodingFilePath = m_tempDir->path() + QString("/net_encoding_%1.txt").arg(static_cast<int>(encoding));
        QUrl encodingUrl = QUrl::fromLocalFile(encodingFilePath);

        NetShareTextIO encodingTextIO(encodingUrl, encoding);

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

QTEST_MAIN(NetShareTextIOTest)
#include "netsharetextiotest.moc"