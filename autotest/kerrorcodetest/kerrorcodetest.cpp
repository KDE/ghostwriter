#include "../../src/util/kerrorcode.h"
#include <QObject>
#include <QtTest>

/**
 * @brief Unit tests for the KErrorCode class.
 *
 * This class tests the functionality of the KErrorCode class,
 * including constructors, assignment operators, equality operators,
 * and various methods.
 */
class KErrorCodeTest : public QObject
{
    Q_OBJECT

private slots:
    void testConstructorWithCode();
    void testEquality();
    void testInequality();
    void testToString();
    void testAssignment();
    void testMoveConstructor();
    void testMoveAssignment();
    void testRValueOperations();
};

/**
 * OBJECTIVE:
 *      Verify creation of a KErrorCode object with a specific error code and message.
 *
 * INPUTS:
 *      A KErrorCode object initialized with an error code and message.
 *
 * EXPECTED RESULTS:
 *      The KErrorCode object should be created successfully,
 *      and its code and message should match the provided values.
 */
void KErrorCodeTest::testConstructorWithCode()
{
    KErrorCode error(1, "Test error");
    QCOMPARE(error.code(), 1);
    QCOMPARE(error.message(), QString("Test error"));
}

/**
 * OBJECTIVE:
 *      Verify the equality operator for KErrorCode objects.
 *
 * INPUTS:
 *      Two KErrorCode objects with the same code and message,
 *      and one with a different code or message.
 *
 * EXPECTED RESULTS:
 *      The equality operator should return true for objects with the same code and message,
 *      and false for those that differ.
 */
void KErrorCodeTest::testEquality()
{
    KErrorCode error1(1, "Error 1");
    KErrorCode error2(1, "Error 1");
    KErrorCode error3(2, "Error 2");

    QVERIFY(error1 == error2);
    QVERIFY(!(error1 == error3));
}

/**
 * OBJECTIVE:
 *      Verify the inequality operator for KErrorCode objects.
 *
 * INPUTS:
 *      Two KErrorCode objects with the same code and message,
 *      and one with a different code or message.
 *
 * EXPECTED RESULTS:
 *      The inequality operator should return true for objects that differ,
 *      and false for those that are equal.
 */
void KErrorCodeTest::testInequality()
{
    KErrorCode error1(1, "Error 1");
    KErrorCode error2(1, "Error 1");
    KErrorCode error3(2, "Error 2");

    QVERIFY(!(error1 != error2));
    QVERIFY(error1 != error3);
}

/**
 * OBJECTIVE:
 *      Verify the isSuccess method of KErrorCode.
 *
 * INPUTS:
 *      A KErrorCode object initialized with an error code and message.
 *
 * EXPECTED RESULTS:
 *      The isSuccess method should return false for an error code,
 *      and true for a default-constructed KErrorCode (if applicable).
 */
void KErrorCodeTest::testToString()
{
    KErrorCode error(1, "Test error");
    QVERIFY(!error.toString().isEmpty());
    QVERIFY(error.toString().contains("Test error"));
}

/**
 * OBJECTIVE:
 *      Verify the assignment operator for KErrorCode objects.
 *
 * INPUTS:
 *      A KErrorCode object initialized with an error code and message,
 *      and another KErrorCode object to assign to.
 *
 * EXPECTED RESULTS:
 *      The assigned object should have the same code and message as the original.
 */
void KErrorCodeTest::testAssignment()
{
    KErrorCode error1(1, "Error 1");
    KErrorCode error2 = error1;

    QCOMPARE(error2.code(), 1);
    QCOMPARE(error2.message(), QString("Error 1"));
    QCOMPARE(error1, error2);
}

/**
 * OBJECTIVE:
 *      Verify the move constructor for KErrorCode objects.
 *
 * INPUTS:
 *      A KErrorCode object initialized with an error code and message,
 *      and another KErrorCode object to move from.
 *
 * EXPECTED RESULTS:
 *      The moved-to object should have the original's values,
 *      and the original should be in a valid but unspecified state.
 */
void KErrorCodeTest::testMoveConstructor()
{
    KErrorCode original(5, "Original error");
    KErrorCode moved(std::move(original));

    // The moved-to object should have the original's values
    QCOMPARE(moved.code(), 5);
    QCOMPARE(moved.message(), QString("Original error"));
}

/**
 * OBJECTIVE:
 *      Verify the move assignment operator for KErrorCode objects.
 *
 * INPUTS:
 *      A KErrorCode object initialized with an error code and message,
 *      and another KErrorCode object to move into.
 *
 * EXPECTED RESULTS:
 *      The target object should have the original's values,
 *      and the original should be in a valid but unspecified state.
 */
void KErrorCodeTest::testMoveAssignment()
{
    KErrorCode original(5, "Original error");
    KErrorCode target(10, "Target error");

    target = std::move(original);

    // The target should now have original's values
    QCOMPARE(target.code(), 5);
    QCOMPARE(target.message(), QString("Original error"));
}

/**
 * OBJECTIVE:
 *      Verify the behavior of KErrorCode with rvalue references.
 *
 * INPUTS:
 *      Temporary KErrorCode objects created with rvalue references.
 *
 * EXPECTED RESULTS:
 *      The temporary objects should be correctly constructed,
 *      and their values should match the expected code and message.
 */
void KErrorCodeTest::testRValueOperations()
{
    // Test construction from temporary objects
    KErrorCode error1(KErrorCode(7, "Temporary error"));
    QCOMPARE(error1.code(), 7);
    QCOMPARE(error1.message(), QString("Temporary error"));

    // Test equality with temporary
    KErrorCode error2(7, "Temporary error");
    QVERIFY(error2 == KErrorCode(7, "Temporary error"));

    // Test inequality with temporary
    QVERIFY(error2 != KErrorCode(8, "Different error"));

    // Test assignment from temporary
    KErrorCode error3 = KErrorCode(9, "Another temp");
    QCOMPARE(error3.code(), 9);
    QCOMPARE(error3.message(), QString("Another temp"));
}

QTEST_MAIN(KErrorCodeTest)
#include "kerrorcodetest.moc"
