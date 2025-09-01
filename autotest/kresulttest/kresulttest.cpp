/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTest>

#include "../../src/util/kerrorcode.h"
#include "../../src/util/kresult.h"

enum class TestErrorCode {
    SomewhatBadError,
    BadError,
    ReallyBadError
};

typedef KResult<QString, TestErrorCode> TestResult;

TestResult prependTest(const QString &value)
{
    return QString("TEST: %1").arg(value);
}

TestResult prependWarn(TestErrorCode code)
{
    return QString("WARN: %1").arg(static_cast<int>(code));
}

float strToFloat(const QString &value)
{
    return value.toFloat();
}

TestResult testKTry(bool ok)
{
    auto getBadError = [](bool ok) {
        if (!ok) {
            return TestResult(TestErrorCode::BadError, "K_TRY SIMULATED ERROR");
        } else {
            return TestResult("OK");
        }
    };

    K_TRY(getBadError(ok));
    return TestResult("A_OK");
}

/**
 * Unit test for the KResult class.
 */
class KResultTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void okResult_lvalue();
    void okResult_rvalue();
    void errorResult_lvalue();
    void errorResult_rvalue();
    void compareResults_data();
    void compareResults();
    void k_try();
    void assignmentOperators();
    void pointerOperators();
    void conversionOperators();
    void errorCodeHandling();
};

void KResultTest::initTestCase()
{
    // Setup any test data if needed
}

void KResultTest::cleanupTestCase()
{
    // Cleanup any test data if needed
}

/**
 * OBJECTIVE:
 *      Verify creation of an OK result that is an lvalue, and the return values of
 *      its accessors.
 *
 * INPUTS:
 *      A KResult with an OK value.
 *
 * EXPECTED RESULTS:
 *      All data accessors return the expected values.
 */
void KResultTest::okResult_lvalue()
{
    TestResult result("4.8");

    QVERIFY(result);
    QVERIFY(result.ok());
    QVERIFY(!result.hasError());
    QCOMPARE(result.value(), "4.8");
    QCOMPARE(*result, "4.8");
}

/**
 * OBJECTIVE:
 *      Verify creation of an OK result that is an rvalue, and the return values of
 *      its accessors.
 *
 * INPUTS:
 *      A KResult with an OK value.
 *
 * EXPECTED RESULTS:
 *      All data accessors return the expected values.
 */
void KResultTest::okResult_rvalue()
{
    QVERIFY(TestResult("OK"));
    QVERIFY(TestResult("OK").ok());
    QVERIFY(!TestResult("OK").hasError());
    QCOMPARE(TestResult("OK").value(), "OK");
    QCOMPARE(*TestResult("OK"), "OK");
}

/**
 * OBJECTIVE:
 *      Verify creation of an error result that is an lvalue, and the return values of
 *      its accessors.
 *
 * INPUTS:
 *      A KResult with an error value.
 *
 * EXPECTED RESULTS:
 *      All data accessors return the expected values.
 */
void KResultTest::errorResult_lvalue()
{
    KErrorCode<TestErrorCode> ec(TestErrorCode::ReallyBadError, "REALLY BAD ERROR");
    TestResult result(ec);

    QVERIFY(!result);
    QVERIFY(!result.ok());
    QVERIFY(result.hasError());
    QCOMPARE(result.error().code(), TestErrorCode::ReallyBadError);
    QCOMPARE(result.errcode(), TestErrorCode::ReallyBadError);
    QCOMPARE(result.error().message(), QStringLiteral("REALLY BAD ERROR"));
    QCOMPARE(result.errmsg(), QStringLiteral("REALLY BAD ERROR"));
}

/**
 * OBJECTIVE:
 *      Verify creation of an error result that is an rvalue, and the return values of
 *      its accessors.
 *
 * INPUTS:
 *      A KResult with an error value.
 *
 * EXPECTED RESULTS:
 *      All data accessors return the expected values.
 */
void KResultTest::errorResult_rvalue()
{
    KErrorCode<TestErrorCode> ec(TestErrorCode::ReallyBadError, "REALLY BAD ERROR");

    QVERIFY(!TestResult(ec));
    QVERIFY(!TestResult(ec).ok());
    QVERIFY(TestResult(ec).hasError());
    QCOMPARE(TestResult(ec).error().code(), TestErrorCode::ReallyBadError);
    QCOMPARE(TestResult(ec).errcode(), TestErrorCode::ReallyBadError);
    QCOMPARE(TestResult(ec).error().message(), QString("REALLY BAD ERROR"));
    QCOMPARE(TestResult(ec).errmsg(), QString("REALLY BAD ERROR"));
}

void KResultTest::compareResults_data()
{
    QTest::addColumn<TestResult>("lhs");
    QTest::addColumn<TestResult>("rhs");
    QTest::addColumn<bool>("equal");
    QTest::addColumn<bool>("lhsIsTrue");
    QTest::addColumn<bool>("rhsIsTrue");

    QTest::newRow("nominal: two OK results that are the same") << TestResult(QString("OK")) << TestResult(QString("OK")) << true << true << true;

    QTest::newRow("nominal: two error results that are the same")
        << TestResult(TestErrorCode::BadError, "ERROR") << TestResult(TestErrorCode::BadError, "ERROR") << true << false << false;

    QTest::newRow("nominal: LHS is OK result, RHS is error result")
        << TestResult(QString("OK")) << TestResult(TestErrorCode::ReallyBadError, "ERROR") << false << true << false;

    QTest::newRow("nominal: LHS is error result, RHS is OK result")
        << TestResult(TestErrorCode::SomewhatBadError, "ERROR") << TestResult(QString("OK")) << false << false << true;
}

/**
 * OBJECTIVE:
 *      Verify comparison operations for two results. Operations include == and !=.
 *
 * INPUTS:
 *      Different combinations of KResults for the left and right sides of the
 *      operators.
 *
 * EXPECTED RESULTS:
 *      The == and != operators return the expected value based on whether the two
 *      results are equal or not.
 */
void KResultTest::compareResults()
{
    QFETCH(TestResult, lhs);
    QFETCH(TestResult, rhs);
    QFETCH(bool, equal);
    QFETCH(bool, lhsIsTrue);
    QFETCH(bool, rhsIsTrue);

    // Verify == operator return value matches expected.
    QCOMPARE((lhs == rhs), equal);

    // Verify != operator return value matches expected.
    QCOMPARE((lhs != rhs), !equal);

    // Verify boolean conversion
    QCOMPARE(bool(lhs), lhsIsTrue);
    QCOMPARE(bool(rhs), rhsIsTrue);
}

/**
 * OBJECTIVE:
 *      Verify wrapping a function call in a K_TRY macro returns with an error result
 *      if the function returns KResult having an error, or else returns an expected OK
 *      result if the function returns an OK KResult.
 *
 * INPUTS:
 *      1. A call to a function wrapped in K_TRY that will return an error result.
 *      2. A call to a function wrapped in K_TRY that will return an OK result.
 *
 * EXPECTED RESULTS:
 *      1. The function using K_TRY will return the error result from the wrapped function call.
 *      2. The function using K_TRY will return an OK result (not from the wrapped function call).
 */
void KResultTest::k_try()
{
    // Verify K_TRY returns early from an error result.
    auto result = testKTry(false); // pass false in to trigger an error result

    QVERIFY(!result);
    QVERIFY(!result.ok());
    QVERIFY(result.hasError());
    QCOMPARE(result.error().code(), TestErrorCode::BadError);
    QCOMPARE(result.errcode(), TestErrorCode::BadError);
    QCOMPARE(result.error().message(), QString("K_TRY SIMULATED ERROR"));
    QCOMPARE(result.errmsg(), QString("K_TRY SIMULATED ERROR"));

    // Verify K_TRY does not return early and the testKTry function returns the
    // expected OK result.
    result = testKTry(true); // pass true in to trigger an OK result

    QVERIFY(result);
    QVERIFY(result.ok());
    QVERIFY(!result.hasError());
    QCOMPARE(*result, "A_OK");
    QCOMPARE(result.value(), "A_OK");
}

/**
 * OBJECTIVE:
 *      Verify assignment operators work correctly for KResult.
 *
 * INPUTS:
 *      KResult objects assigned with OK values and error values.
 *
 * EXPECTED RESULTS:
 *      Assignment operators correctly update the KResult state.
 */
void KResultTest::assignmentOperators()
{
    TestResult result("Initial");
    QVERIFY(result.ok());
    QCOMPARE(*result, "Initial");

    // Assign OK value
    result = QString("New Value");
    QVERIFY(result.ok());
    QCOMPARE(*result, "New Value");

    // Assign error
    KErrorCode<TestErrorCode> error(TestErrorCode::BadError, "Assignment error");
    result = error;
    QVERIFY(!result.ok());
    QVERIFY(result.hasError());
    QCOMPARE(result.errcode(), TestErrorCode::BadError);
    QCOMPARE(result.errmsg(), QString("Assignment error"));

    // Assign back to OK
    result = QString("Back to OK");
    QVERIFY(result.ok());
    QCOMPARE(*result, "Back to OK");
}

/**
 * OBJECTIVE:
 *      Verify pointer operators work correctly for KResult.
 *
 * INPUTS:
 *      KResult objects with OK values.
 *
 * EXPECTED RESULTS:
 *      Pointer operators correctly access the OK value.
 */
void KResultTest::pointerOperators()
{
    TestResult result("Test String");

    // Test operator->
    QVERIFY(result->isEmpty() == false);
    QCOMPARE(result->length(), 11);

    // Test operator*
    QCOMPARE(*result, "Test String");

    // Test with error (operator-> should return nullptr)
    TestResult errorResult(TestErrorCode::BadError, "Error");
    QVERIFY(errorResult.operator->() == nullptr);
}

/**
 * OBJECTIVE:
 *      Verify conversion operators work correctly for KResult.
 *
 * INPUTS:
 *      KResult objects with OK and error values.
 *
 * EXPECTED RESULTS:
 *      Boolean conversion correctly reflects the KResult state.
 */
void KResultTest::conversionOperators()
{
    TestResult okResult("OK");
    TestResult errorResult(TestErrorCode::BadError, "Error");

    // Test boolean conversion
    QVERIFY(bool(okResult) == true);
    QVERIFY(bool(errorResult) == false);

    // Test explicit boolean conversion
    QVERIFY(static_cast<bool>(okResult) == true);
    QVERIFY(static_cast<bool>(errorResult) == false);

    // Test in conditional
    if (okResult) {
        QVERIFY(true); // Should reach here
    } else {
        QFAIL("OK result should be truthy");
    }

    if (errorResult) {
        QFAIL("Error result should be falsy");
    } else {
        QVERIFY(true); // Should reach here
    }
}

/**
 * OBJECTIVE:
 *      Verify error code handling works correctly across different enum types.
 *
 * INPUTS:
 *      KResult objects with different error code types.
 *
 * EXPECTED RESULTS:
 *      Error codes are properly stored and retrieved.
 */
void KResultTest::errorCodeHandling()
{
    // Test with TestErrorCode enum
    TestResult result1(TestErrorCode::ReallyBadError, "Really bad");
    QCOMPARE(result1.errcode(), TestErrorCode::ReallyBadError);
    QCOMPARE(result1.errmsg(), QString("Really bad"));

    // Test direct construction with error code and message
    TestResult result2(TestErrorCode::SomewhatBadError, "Somewhat bad");
    QVERIFY(!result2.ok());
    QVERIFY(result2.hasError());
    QCOMPARE(result2.errcode(), TestErrorCode::SomewhatBadError);

    // Test equality with KErrorCode
    KErrorCode<TestErrorCode> ec(TestErrorCode::SomewhatBadError, "Somewhat bad");
    QVERIFY(result2 == ec);

    // Test inequality
    KErrorCode<TestErrorCode> differentEc(TestErrorCode::BadError, "Different error");
    QVERIFY(!(result2 == differentEc));
}

QTEST_MAIN(KResultTest)
#include "kresulttest.moc"
