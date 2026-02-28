/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <KJob>
#include <QEventLoop>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>

#include "io/localtextio.h"
#include "io/netsharetextio.h"
#include "io/textio.h"

using namespace ghostwriter;

// Mock TextIO implementation for testing
class MockTextIO : public TextIO
{
public:
    MockTextIO(const QUrl &fileUrl, QStringConverter::Encoding encoding = QStringConverter::Utf8)
        : TextIO(fileUrl, encoding)
        , m_writeDelay(0)
        , m_readDelay(0)
        , m_writeError(false)
        , m_readError(false)
        , m_testContent("Test file content")
    {
    }

    WriteResult write(const QString &text) override
    {
        m_lastWrittenText = text;

        if (m_writeDelay > 0) {
            QEventLoop loop;
            QTimer::singleShot(m_writeDelay, &loop, &QEventLoop::quit);
            loop.exec();
        }

        if (m_writeError) {
            return {TextIOError::PermissionDenied, "Mock write error"};
        }

        return KSuccess{};
    }

    ReadResult read() override
    {
        if (m_readDelay > 0) {
            QEventLoop loop;
            QTimer::singleShot(m_readDelay, &loop, &QEventLoop::quit);
            loop.exec();
        }

        if (m_readError) {
            return {TextIOError::FileNotFound, "Mock read error"};
        }

        return m_testContent;
    }

    // Test helpers
    void setWriteDelay(int ms)
    {
        m_writeDelay = ms;
    }
    void setReadDelay(int ms)
    {
        m_readDelay = ms;
    }
    void setWriteError(bool error)
    {
        m_writeError = error;
    }
    void setReadError(bool error)
    {
        m_readError = error;
    }
    void setTestContent(const QString &content)
    {
        m_testContent = content;
    }
    QString lastWrittenText() const
    {
        return m_lastWrittenText;
    }

private:
    int m_writeDelay;
    int m_readDelay;
    bool m_writeError;
    bool m_readError;
    QString m_testContent;
    QString m_lastWrittenText;
};

class TextIOTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor and basic properties
    void constructor_data();
    void constructor();
    void fileUrl();
    void encoding();
    void setEncoding();

    // Async write tests
    void writeAsync_success();
    void writeAsync_error();
    void writeAsync_cancellation();

    // Async read tests
    void readAsync_success();
    void readAsync_error();

    void canHandleLocalTextIO();

private:
    std::unique_ptr<MockTextIO> m_textIO;
    QUrl m_testUrl;
};

void TextIOTest::initTestCase()
{
    // Initialize test data
    m_testUrl = QUrl::fromLocalFile("/tmp/test.txt");
}

void TextIOTest::cleanupTestCase()
{
    // Cleanup test data
}

void TextIOTest::init()
{
    m_textIO = std::make_unique<MockTextIO>(m_testUrl, QStringConverter::Utf8);
}

void TextIOTest::cleanup()
{
    m_textIO.reset();
}

void TextIOTest::constructor_data()
{
    QTest::addColumn<QString>("urlString");
    QTest::addColumn<int>("encoding");

    QTest::newRow("local file with UTF-8") << "file:///tmp/test.txt" << static_cast<int>(QStringConverter::Utf8);
    QTest::newRow("local file with UTF-16") << "file:///home/user/doc.md" << static_cast<int>(QStringConverter::Utf16);
    QTest::newRow("network URL with Latin1") << "https://example.com/file.txt" << static_cast<int>(QStringConverter::Latin1);
}

/**
 * OBJECTIVE:
 *   Verify that the TextIO constructor correctly initializes the object with given parameters.
 *
 * INPUTS:
 *   - urlString: A string representing the file URL.
 *   - encoding: An integer representing the text encoding (mapped to QStringConverter::Encoding).
 *
 * EXPECTED RESULTS:
 *   - The TextIO object's fileUrl() method returns a QUrl matching the input url
 *   - The TextIO object's encoding() method returns the correct QStringConverter::Encoding value.
 */
void TextIOTest::constructor()
{
    QFETCH(QString, urlString);
    QFETCH(int, encoding);

    QUrl url(urlString);
    auto encodingEnum = static_cast<QStringConverter::Encoding>(encoding);

    MockTextIO textIO(url, encodingEnum);

    QCOMPARE(textIO.fileUrl(), url);
    QCOMPARE(textIO.encoding(), encodingEnum);
}

/**
 * OBJECTIVE:
 *   Verify that the fileUrl() method returns the correct QUrl.
 *
 * INPUTS:
 *   None (uses m_testUrl set in initTestCase)
 *
 * EXPECTED RESULTS:
 *   The fileUrl() method returns a QUrl equal to m_testUrl.
 */
void TextIOTest::fileUrl()
{
    QCOMPARE(m_textIO->fileUrl(), m_testUrl);
}

/**
 * OBJECTIVE:
 *   Verify that the encoding() method returns the correct encoding.
 *
 * INPUTS:
 *   None (uses default encoding set in init)
 *
 * EXPECTED RESULTS:
 *   The encoding() method returns QStringConverter::Utf8.
 */
void TextIOTest::encoding()
{
    QCOMPARE(m_textIO->encoding(), QStringConverter::Utf8);
}

/**
 * OBJECTIVE:
 *   Verify that the setEncoding() method correctly updates the encoding.
 *
 * INPUTS:
 *   New encoding values to set (QStringConverter::Utf16, QStringConverter::Latin1)
 *
 * EXPECTED RESULTS:
 *   After setting a new encoding, the encoding() method returns the updated value.
 */
void TextIOTest::setEncoding()
{
    m_textIO->setEncoding(QStringConverter::Utf16);
    QCOMPARE(m_textIO->encoding(), QStringConverter::Utf16);

    m_textIO->setEncoding(QStringConverter::Latin1);
    QCOMPARE(m_textIO->encoding(), QStringConverter::Latin1);
}

/**
 * OBJECTIVE:
 *   Verify that writeAsync successfully writes text to the file.
 *
 * INPUTS:
 *   - text: A QString containing the text to write.
 *
 * EXPECTED RESULTS:
 *   - The returned QFuture completes successfully.
 *   - The lastWrittenText() method returns the text that was written.
 */
void TextIOTest::writeAsync_success()
{
    QString testText = "Hello, async world!";

    auto future = m_textIO->writeAsync(testText);

    // Wait for completion
    QFutureWatcher<TextIO::WriteResult> watcher;
    QEventLoop loop;
    connect(&watcher, &QFutureWatcher<TextIO::WriteResult>::finished, &loop, &QEventLoop::quit);
    watcher.setFuture(future);
    loop.exec();

    QVERIFY(future.isFinished());
    auto result = future.result();
    QVERIFY(result.ok());
    QCOMPARE(m_textIO->lastWrittenText(), testText);
}

/**
 * OBJECTIVE:
 *   Verify that writeAsync correctly handles write errors.
 *
 * INPUTS:
 *   - text: A QString containing the text to write.
 *   - Simulated error condition (setWriteError(true))
 *
 * EXPECTED RESULTS:
 *   - The returned QFuture completes with an error.
 *   - The error code is TextIOError::PermissionDenied.
 *   - The error message matches the simulated error message.
 */
void TextIOTest::writeAsync_error()
{
    m_textIO->setWriteError(true);
    QString testText = "This should fail";

    auto future = m_textIO->writeAsync(testText);

    QFutureWatcher<TextIO::WriteResult> watcher;
    QEventLoop loop;
    connect(&watcher, &QFutureWatcher<TextIO::WriteResult>::finished, &loop, &QEventLoop::quit);
    watcher.setFuture(future);
    loop.exec();

    QVERIFY(future.isFinished());
    auto result = future.result();
    QVERIFY(!result.ok());
    QCOMPARE(result.error().code(), TextIOError::PermissionDenied);
    QCOMPARE(result.errmsg(), QString("Mock write error"));
}

/**
 * OBJECTIVE:
 *   Verify that writeAsync can be cancelled.
 *
 * INPUTS:
 *   - text: A QString containing the text to write.
 *   - Simulated delay to allow cancellation (setWriteDelay(1000))
 *
 * EXPECTED RESULTS:
 *   - The returned QFuture is either cancelled or completes quickly after cancellation.
 */
void TextIOTest::writeAsync_cancellation()
{
    // Test that we can cancel a long-running operation
    m_textIO->setWriteDelay(1000); // 1 second delay

    auto future = m_textIO->writeAsync("Test text");

    // Cancel immediately
    future.cancel();

    // Should not take long to finish
    QTest::qWait(100);
    QVERIFY(future.isCanceled() || future.isFinished());
}

/**
 * OBJECTIVE:
 *   Verify that readAsync successfully reads text from the file.
 *
 * INPUTS:
 *   None (uses default test content set in MockTextIO)
 *
 * EXPECTED RESULTS:
 *   - The returned QFuture completes successfully.
 *   - The result value matches the test content.
 */
void TextIOTest::readAsync_success()
{
    QString expectedContent = "Expected file content";
    m_textIO->setTestContent(expectedContent);

    auto future = m_textIO->readAsync();

    QFutureWatcher<TextIO::ReadResult> watcher;
    QEventLoop loop;
    connect(&watcher, &QFutureWatcher<TextIO::ReadResult>::finished, &loop, &QEventLoop::quit);
    watcher.setFuture(future);
    loop.exec();

    QVERIFY(future.isFinished());
    auto result = future.result();
    QVERIFY(result.ok());
    QCOMPARE(result.value(), expectedContent);
}

/**
 * OBJECTIVE:
 *   Verify that readAsync correctly handles read errors.
 *
 * INPUTS:
 *   - Simulated error condition (setReadError(true))
 *
 * EXPECTED RESULTS:
 *   - The returned QFuture completes with an error.
 *   - The error code is TextIOError::FileNotFound.
 *   - The error message matches the simulated error message.
 */
void TextIOTest::readAsync_error()
{
    m_textIO->setReadError(true);

    auto future = m_textIO->readAsync();

    QFutureWatcher<TextIO::ReadResult> watcher;
    QEventLoop loop;
    connect(&watcher, &QFutureWatcher<TextIO::ReadResult>::finished, &loop, &QEventLoop::quit);
    watcher.setFuture(future);
    loop.exec();

    QVERIFY(future.isFinished());
    auto result = future.result();
    QVERIFY(!result.ok());
    QCOMPARE(result.error().code(), TextIOError::FileNotFound);
    QCOMPARE(result.errmsg(), QString("Mock read error"));
}

/**
 * OBJECTIVE:
 *   Verify that canHandle() correctly identifies supported URLs for LocalTextIO.
 *
 * INPUTS:
 *   - A set of QUrl instances representing local file paths and non-local paths.
 *
 * EXPECTED RESULTS:
 *   - canHandle() returns true for local file URLs.
 *   - canHandle() returns false for non-local URLs, directories, and invalid or empty URLs.
 */
void TextIOTest::canHandleLocalTextIO()
{
    // Verify valid cases
    QVERIFY(LocalTextIO::canHandle(QUrl::fromLocalFile("/tmp/test.txt")));
    QVERIFY(LocalTextIO::canHandle(QUrl::fromLocalFile("relative/path/to/file.txt")));
    QVERIFY(LocalTextIO::canHandle(QUrl::fromLocalFile("./file.txt")));
    QVERIFY(LocalTextIO::canHandle(QUrl("file:///tmp/test.txt"))); // Explicit file scheme
    QVERIFY(LocalTextIO::canHandle(QUrl::fromLocalFile("/tmp/fiłę.txt"))); // Unicode filename

    // Verify invalid cases
    QVERIFY(!LocalTextIO::canHandle(QUrl::fromLocalFile("/tmp/"))); // Directory
    QVERIFY(!LocalTextIO::canHandle(QUrl("https://example.com/file.txt"))); // Non-local URL
    QVERIFY(!LocalTextIO::canHandle(QUrl("ftp://example.com/file.txt"))); // Non-local URL
    QVERIFY(!LocalTextIO::canHandle(QUrl())); // Empty URL
    QVERIFY(!LocalTextIO::canHandle(QUrl("::invalid::"))); // Invalid URL
}

QTEST_MAIN(TextIOTest)
#include "textiotest.moc"
