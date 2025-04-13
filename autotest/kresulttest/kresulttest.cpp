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

enum class MappedErrorCode {
    MappedErrorOne,
    MappedErrorTwo
};

enum class RefType {
    LValue,
    RValue
};

typedef KResult<QString, TestErrorCode> TestResult;
typedef KResult<float, TestErrorCode> MappedTestResult;
typedef KResult<QString, MappedErrorCode> MappedErrResult;

TestResult prependTest(const QString &value)
{
    return QString("TEST: %1").arg(value);
}

TestResult prependWarn(TestErrorCode code)
{
    return QString("WARN: %1").arg(int(code));
}

float strToFloat(const QString &value)
{
    return value.toFloat();
}

MappedErrorCode mapErrorCode(TestErrorCode code)
{
    if (TestErrorCode::BadError == code) {
        return MappedErrorCode::MappedErrorOne;
    }

    return MappedErrorCode::MappedErrorTwo;
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
};

void KResultTest::initTestCase()
{
    ;
}

void KResultTest::cleanupTestCase()
{
    ;
}

/**
 * OBJECTIVE:
 *      Verify creation of an OK result that is an lvalue, and the return values of
 *      its accessors. Also verify its transformation methods (valueOr, errorOr, orElse,
 *      andThen, map, mapError) work as expected.
 *
 * INPUTS:
 *      A KResult with an OK value.
 *
 * EXPECTED RESULTS:
 *      All data accessors return the expected values. All formation methods
 *      (valueOr, errorOr, orElse, andThen, map, mapError) work as expected.
 */
void KResultTest::okResult_lvalue()
{
    TestResult result("4.8");

    QVERIFY(result);
    QVERIFY(result.ok());
    QVERIFY(!result.hasError());
    QCOMPARE(result.value(), "4.8");
    QCOMPARE(*result, "4.8");
    QCOMPARE(result.valueOr(QString("ERROR")), "4.8");

    TestResult expectedErrResult = KErrorCode<TestErrorCode>(TestErrorCode::SomewhatBadError, "NOT_TOO_BAD");
    TestResult actualResult = result.errorOr(KErrorCode<int>(0, "NOT_TOO_BAD"));
    QCOMPARE(actualResult.errcode(), expectedErrResult.errcode());
    QCOMPARE(actualResult.errmsg(), expectedErrResult.errmsg());

    QCOMPARE(result.orElse(prependWarn), "4.8");
    QCOMPARE(result.andThen(prependTest), "TEST: 4.8");
    QCOMPARE(result.map(strToFloat), 4.8f);
    QCOMPARE(result.mapError(mapErrorCode), "4.8");
}

/**
 * OBJECTIVE:
 *      Verify creation of an OK result that is an rvalue, and the return values of
 *      its accessors. Also verify its transformation methods (valueOr, errorOr, orElse,
 *      andThen, map, mapError) work as expected.
 *
 * INPUTS:
 *      A KResult with an OK value.
 *
 * EXPECTED RESULTS:
 *      All data accessors return the expected values. All formation methods
 *      (valueOr, errorOr, orElse, andThen, map, mapError) work as expected.
 */
void KResultTest::okResult_rvalue()
{
    QVERIFY(TestResult("OK"));
    QVERIFY(TestResult("OK").ok());
    QVERIFY(!TestResult("OK").hasError());
    QCOMPARE(TestResult("OK").value(), "OK");
    QCOMPARE(*TestResult("OK"), "OK");
    QCOMPARE(TestResult("OK").valueOr(QString("ERROR")), "OK");

    TestResult expectedErrResult = KErrorCode<TestErrorCode>(TestErrorCode::SomewhatBadError, "NOT_TOO_BAD");
    TestResult actualResult = TestResult("OK").errorOr(KErrorCode<int>(0, "NOT_TOO_BAD"));
    QCOMPARE(actualResult.errcode(), expectedErrResult.errcode());
    QCOMPARE(actualResult.errmsg(), expectedErrResult.errmsg());

    QCOMPARE(TestResult("OK").orElse(prependWarn), "OK");
    QCOMPARE(TestResult("OK").andThen(prependTest), "TEST: OK");
    QCOMPARE(TestResult("4.8").map(strToFloat), 4.8f);
    QCOMPARE(TestResult("4.8").mapError(mapErrorCode), "4.8");
}

/**
 * OBJECTIVE:
 *      Verify creation of an error result that is an lvalue, and the return values of
 *      its accessors. Also verify its transformation methods (valueOr, errorOr, orElse,
 *      andThen, map, mapError) work as expected.
 *
 * INPUTS:
 *      A KResult with an OK value.
 *
 * EXPECTED RESULTS:
 *      All data accessors return the expected values. All formation methods
 *      (valueOr, errorOr, orElse, andThen, map, mapError) work as expected.
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
    QCOMPARE(result.error().message(), "REALLY BAD ERROR");
    QCOMPARE(result.errmsg(), "REALLY BAD ERROR");
    QCOMPARE(result.valueOr(QString("DEFAULT")), "DEFAULT");
    QCOMPARE(result.errorOr(KErrorCode<int>(0, "NOT_TOO_BAD")), ec);
    QCOMPARE(result.orElse(prependWarn), "WARN: 2");
    QCOMPARE(result.andThen(prependTest), ec);
    QCOMPARE(result.map(strToFloat), ec);
    QCOMPARE(result.mapError(mapErrorCode), MappedErrorCode::MappedErrorTwo);
}

/**
 * OBJECTIVE:
 *      Verify creation of an error result that is an rvalue, and the return values of
 *      its accessors. Also verify its transformation methods (valueOr, errorOr, orElse,
 *      andThen, map, mapError) work as expected.
 *
 * INPUTS:
 *      A KResult with an OK value.
 *
 * EXPECTED RESULTS:
 *      All data accessors return the expected values. All formation methods
 *      (valueOr, errorOr, orElse, andThen, map, mapError) work as expected.
 */
void KResultTest::errorResult_rvalue()
{
    KErrorCode<TestErrorCode> ec(TestErrorCode::ReallyBadError, "REALLY BAD ERROR");

    QVERIFY(!TestResult(ec));
    QVERIFY(!TestResult(ec).ok());
    QVERIFY(TestResult(ec).hasError());
    QCOMPARE(TestResult(ec).error().code(), TestErrorCode::ReallyBadError);
    QCOMPARE(TestResult(ec).errcode(), TestErrorCode::ReallyBadError);
    QCOMPARE(TestResult(ec).error().message(), "REALLY BAD ERROR");
    QCOMPARE(TestResult(ec).errmsg(), "REALLY BAD ERROR");
    QCOMPARE(TestResult(ec).valueOr(QString("DEFAULT")), "DEFAULT");
    QCOMPARE(TestResult(ec).errorOr(KErrorCode<int>(0, "NOT_TOO_BAD")), ec);
    QCOMPARE(TestResult(ec).orElse(prependWarn), "WARN: 2");
    QCOMPARE(TestResult(ec).andThen(prependTest), ec);
    QCOMPARE(TestResult(ec).map(strToFloat), ec);
    QCOMPARE(TestResult(ec).mapError(mapErrorCode), MappedErrorCode::MappedErrorTwo);
}

void KResultTest::compareResults_data()
{
    QTest::addColumn<TestResult>("lhs");
    QTest::addColumn<TestResult>("rhs");
    QTest::addColumn<bool>("equal");
    QTest::addColumn<bool>("lhsIsTrue");
    QTest::addColumn<bool>("rhsIsTrue");

    TestResult result1("OK");
    TestResult result2 = result1;

    QTest::newRow("nominal: two OK results that are the same") << result1 << result2 << true << true << true;

    result1 = TestResult(TestErrorCode::BadError, "ERROR");
    result2 = result1;

    QTest::newRow("nominal: two error results that are the same") << result1 << result2 << true << false << false;

    result1 = TestResult("OK");
    result2 = TestResult(TestErrorCode::ReallyBadError, "ERROR");

    QTest::newRow("nominal: LHS is OK result, RHS is error result") << result1 << result2 << false << true << false;

    result1 = KErrorCode<TestErrorCode>(TestErrorCode::SomewhatBadError, "ERROR");
    result2 = "OK";

    QTest::newRow("nominal: LHS is error result, RHS is OK result") << result1 << result2 << false << false << true;
}

/**
 * OBJECTIVE:
 *      Verify comparison operations for two results. Operations include ==, !=,
 *      or_, and and_.
 *
 * INPUTS:
 *      Different combinations of KResults for the left and right and sides of the
 *      operators.
 *
 * EXPECTED RESULTS:
 *      The == and != operators return the expected value based on whether the two
 *      results are equal or not. The or_ and and_ operators return the correct
 *      operand based on the operands' truth values.
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

    // Verify || operator result matches expected.
    TestResult orResult = (lhs.or_(rhs));
    TestResult orExpected = lhsIsTrue ? lhs : rhs;
    QCOMPARE(bool(orResult), (lhsIsTrue || rhsIsTrue));
    QCOMPARE(orResult, orExpected);

    // Verify && operator result matches expected.
    TestResult andResult = (lhs.and_(rhs));
    TestResult andExpected = lhsIsTrue ? rhs : lhs;
    QCOMPARE(bool(andResult), (lhsIsTrue && rhsIsTrue));
    QCOMPARE(andResult, andExpected);
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
 *      2. The function using K_TRY will return an OK result (not from teh wrapped function call).
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
    QCOMPARE(result.error().message(), "K_TRY SIMULATED ERROR");
    QCOMPARE(result.errmsg(), "K_TRY SIMULATED ERROR");

    // Verify K_TRY does not return early and the testKTry function returns the
    // expected OK result.
    result = testKTry(true); // pass true in to trigger an OK result

    QVERIFY(result);
    QVERIFY(result.ok());
    QVERIFY(!result.hasError());
    QCOMPARE(*result, "A_OK");
    QCOMPARE(result.value(), "A_OK");
}

QTEST_MAIN(KResultTest)
#include "kresulttest.moc"
