#include "../../src/util/kerrorcode.h"
#include <QObject>
#include <QtTest>

// Define an enum for testing enum error codes
enum class TestErrorType {
    Success = 0,
    Warning = 1,
    Error = 2,
    Critical = 3
};

/**
 * @brief Unit tests for the KErrorCode class.
 *
 * This class tests the functionality of the KErrorCode class,
 * including constructors, assignment operators, equality operators,
 * and various methods for both integer and enum error code types.
 */
class KErrorCodeTest : public QObject
{
    Q_OBJECT

private slots:
    // Tests for integer error codes
    void testIntConstructorWithCode();
    void testIntEquality();
    void testIntInequality();
    void testIntToString();
    void testIntAssignment();
    void testIntMoveConstructor();
    void testIntMoveAssignment();
    void testIntRValueOperations();

    // Tests for enum error codes
    void testEnumConstructorWithCode();
    void testEnumEquality();
    void testEnumInequality();
    void testEnumToString();
    void testEnumAssignment();
    void testEnumMoveConstructor();
    void testEnumMoveAssignment();
    void testEnumRValueOperations();

    // Tests for const char* vs QString behavior
    void testCStringConstructor();
    void testMessageConversion();
    void testNullMessageHandling();
};

/**
 * OBJECTIVE:
 *      Verify creation of a KErrorCode object with a specific integer error code and message.
 *
 * INPUTS:
 *      A KErrorCode<int> object initialized with an error code and const char* message.
 *
 * EXPECTED RESULTS:
 *      The KErrorCode object should be created successfully,
 *      and its code and message should match the provided values.
 */
void KErrorCodeTest::testIntConstructorWithCode()
{
    KErrorCode<int> error(1, "Test error");
    QCOMPARE(error.code(), 1);
    QCOMPARE(error.message(), QString("Test error"));
}

/**
 * OBJECTIVE:
 *      Verify the equality operator for KErrorCode<int> objects.
 *
 * INPUTS:
 *      Two KErrorCode<int> objects with the same code and message,
 *      and one with a different code or message.
 *
 * EXPECTED RESULTS:
 *      The equality operator should return true for objects with the same code and message,
 *      and false for those that differ.
 */
void KErrorCodeTest::testIntEquality()
{
    KErrorCode<int> error1(1, "Error 1");
    KErrorCode<int> error2(1, "Error 1");
    KErrorCode<int> error3(2, "Error 2");

    QVERIFY(error1 == error2);
    QVERIFY(!(error1 == error3));

    // Test equality with raw code
    QVERIFY(error1 == 1);
    QVERIFY(!(error1 == 2));
}

/**
 * OBJECTIVE:
 *      Verify the inequality operator for KErrorCode<int> objects.
 *
 * INPUTS:
 *      Two KErrorCode<int> objects with the same code and message,
 *      and one with a different code or message.
 *
 * EXPECTED RESULTS:
 *      The inequality operator should return true for objects that differ,
 *      and false for those that are equal.
 */
void KErrorCodeTest::testIntInequality()
{
    KErrorCode<int> error1(1, "Error 1");
    KErrorCode<int> error2(1, "Error 1");
    KErrorCode<int> error3(2, "Error 2");

    QVERIFY(!(error1 != error2));
    QVERIFY(error1 != error3);
}

/**
 * OBJECTIVE:
 *      Verify the toString method of KErrorCode<int>.
 *
 * INPUTS:
 *      A KErrorCode<int> object initialized with an error code and message.
 *
 * EXPECTED RESULTS:
 *      The toString method should return a non-empty string containing the error message.
 */
void KErrorCodeTest::testIntToString()
{
    KErrorCode<int> error(1, "Test error");
    QVERIFY(!error.toString().isEmpty());
    QVERIFY(error.toString().contains("Test error"));
    QVERIFY(error.toString().contains("1"));
}

/**
 * OBJECTIVE:
 *      Verify the assignment operator for KErrorCode<int> objects.
 *
 * INPUTS:
 *      A KErrorCode<int> object initialized with an error code and message,
 *      and another KErrorCode<int> object to assign to.
 *
 * EXPECTED RESULTS:
 *      The assigned object should have the same code and message as the original.
 */
void KErrorCodeTest::testIntAssignment()
{
    KErrorCode<int> error1(1, "Error 1");
    KErrorCode<int> error2(2, "Error 2");

    error2 = error1;

    QCOMPARE(error2.code(), 1);
    QCOMPARE(error2.message(), QString("Error 1"));
    QCOMPARE(error1, error2);
}

/**
 * OBJECTIVE:
 *      Verify the move constructor for KErrorCode<int> objects.
 *
 * INPUTS:
 *      A KErrorCode<int> object initialized with an error code and message,
 *      and another KErrorCode<int> object to move from.
 *
 * EXPECTED RESULTS:
 *      The moved-to object should have the original's values,
 *      and the original should be in a valid but unspecified state.
 */
void KErrorCodeTest::testIntMoveConstructor()
{
    KErrorCode<int> original(5, "Original error");
    KErrorCode<int> moved(std::move(original));

    // The moved-to object should have the original's values
    QCOMPARE(moved.code(), 5);
    QCOMPARE(moved.message(), QString("Original error"));
}

/**
 * OBJECTIVE:
 *      Verify the move assignment operator for KErrorCode<int> objects.
 *
 * INPUTS:
 *      A KErrorCode<int> object initialized with an error code and message,
 *      and another KErrorCode<int> object to move into.
 *
 * EXPECTED RESULTS:
 *      The target object should have the original's values,
 *      and the original should be in a valid but unspecified state.
 */
void KErrorCodeTest::testIntMoveAssignment()
{
    KErrorCode<int> original(5, "Original error");
    KErrorCode<int> target(10, "Target error");

    target = std::move(original);

    // The target should now have original's values
    QCOMPARE(target.code(), 5);
    QCOMPARE(target.message(), QString("Original error"));
}

/**
 * OBJECTIVE:
 *      Verify the behavior of KErrorCode<int> with rvalue references.
 *
 * INPUTS:
 *      Temporary KErrorCode<int> objects created with rvalue references.
 *
 * EXPECTED RESULTS:
 *      The temporary objects should be correctly constructed,
 *      and their values should match the expected code and message.
 */
void KErrorCodeTest::testIntRValueOperations()
{
    // Test construction from temporary objects
    KErrorCode<int> error1(KErrorCode<int>(7, "Temporary error"));
    QCOMPARE(error1.code(), 7);
    QCOMPARE(error1.message(), QString("Temporary error"));

    // Test equality with temporary
    KErrorCode<int> error2(7, "Temporary error");
    QVERIFY(error2 == KErrorCode<int>(7, "Temporary error"));

    // Test inequality with temporary
    QVERIFY(error2 != KErrorCode<int>(8, "Different error"));

    // Test assignment from temporary
    KErrorCode<int> error3(1, "Initial");
    error3 = KErrorCode<int>(9, "Another temp");
    QCOMPARE(error3.code(), 9);
    QCOMPARE(error3.message(), QString("Another temp"));
}

/**
 * OBJECTIVE:
 *      Verify creation of a KErrorCode object with a specific enum error code and message.
 *
 * INPUTS:
 *      A KErrorCode<TestErrorType> object initialized with an error code and const char* message.
 *
 * EXPECTED RESULTS:
 *      The KErrorCode object should be created successfully,
 *      and its code and message should match the provided values.
 */
void KErrorCodeTest::testEnumConstructorWithCode()
{
    KErrorCode<TestErrorType> error(TestErrorType::Error, "Test enum error");
    QCOMPARE(error.code(), TestErrorType::Error);
    QCOMPARE(error.message(), QString("Test enum error"));
}

/**
 * OBJECTIVE:
 *      Verify the equality operator for KErrorCode<TestErrorType> objects.
 *
 * INPUTS:
 *      Two KErrorCode<TestErrorType> objects with the same code and message,
 *      and one with a different code or message.
 *
 * EXPECTED RESULTS:
 *      The equality operator should return true for objects with the same code and message,
 *      and false for those that differ.
 */
void KErrorCodeTest::testEnumEquality()
{
    KErrorCode<TestErrorType> error1(TestErrorType::Warning, "Warning message");
    KErrorCode<TestErrorType> error2(TestErrorType::Warning, "Warning message");
    KErrorCode<TestErrorType> error3(TestErrorType::Critical, "Critical message");

    QVERIFY(error1 == error2);
    QVERIFY(!(error1 == error3));

    // Test equality with raw enum value
    QVERIFY(error1 == TestErrorType::Warning);
    QVERIFY(!(error1 == TestErrorType::Error));
}

/**
 * OBJECTIVE:
 *      Verify the inequality operator for KErrorCode<TestErrorType> objects.
 *
 * INPUTS:
 *      Two KErrorCode<TestErrorType> objects with the same code and message,
 *      and one with a different code or message.
 *
 * EXPECTED RESULTS:
 *      The inequality operator should return true for objects that differ,
 *      and false for those that are equal.
 */
void KErrorCodeTest::testEnumInequality()
{
    KErrorCode<TestErrorType> error1(TestErrorType::Warning, "Warning message");
    KErrorCode<TestErrorType> error2(TestErrorType::Warning, "Warning message");
    KErrorCode<TestErrorType> error3(TestErrorType::Critical, "Critical message");

    QVERIFY(!(error1 != error2));
    QVERIFY(error1 != error3);
}

/**
 * OBJECTIVE:
 *      Verify the toString method of KErrorCode<TestErrorType>.
 *
 * INPUTS:
 *      A KErrorCode<TestErrorType> object initialized with an error code and message.
 *
 * EXPECTED RESULTS:
 *      The toString method should return a non-empty string containing the error message
 *      and the underlying integer value of the enum.
 */
void KErrorCodeTest::testEnumToString()
{
    KErrorCode<TestErrorType> error(TestErrorType::Error, "Test enum error");
    QString result = error.toString();

    QVERIFY(!result.isEmpty());
    QVERIFY(result.contains("Test enum error"));
    QVERIFY(result.contains(QString::number(static_cast<int>(TestErrorType::Error))));
}

/**
 * OBJECTIVE:
 *      Verify the assignment operator for KErrorCode<TestErrorType> objects.
 *
 * INPUTS:
 *      A KErrorCode<TestErrorType> object initialized with an error code and message,
 *      and another KErrorCode<TestErrorType> object to assign to.
 *
 * EXPECTED RESULTS:
 *      The assigned object should have the same code and message as the original.
 */
void KErrorCodeTest::testEnumAssignment()
{
    KErrorCode<TestErrorType> error1(TestErrorType::Error, "Error message");
    KErrorCode<TestErrorType> error2(TestErrorType::Success, "Success message");

    error2 = error1;

    QCOMPARE(error2.code(), TestErrorType::Error);
    QCOMPARE(error2.message(), QString("Error message"));
    QCOMPARE(error1, error2);
}

/**
 * OBJECTIVE:
 *      Verify the move constructor for KErrorCode<TestErrorType> objects.
 *
 * INPUTS:
 *      A KErrorCode<TestErrorType> object initialized with an error code and message,
 *      and another KErrorCode<TestErrorType> object to move from.
 *
 * EXPECTED RESULTS:
 *      The moved-to object should have the original's values,
 *      and the original should be in a valid but unspecified state.
 */
void KErrorCodeTest::testEnumMoveConstructor()
{
    KErrorCode<TestErrorType> original(TestErrorType::Critical, "Critical error");
    KErrorCode<TestErrorType> moved(std::move(original));

    // The moved-to object should have the original's values
    QCOMPARE(moved.code(), TestErrorType::Critical);
    QCOMPARE(moved.message(), QString("Critical error"));
}

/**
 * OBJECTIVE:
 *      Verify the move assignment operator for KErrorCode<TestErrorType> objects.
 *
 * INPUTS:
 *      A KErrorCode<TestErrorType> object initialized with an error code and message,
 *      and another KErrorCode<TestErrorType> object to move into.
 *
 * EXPECTED RESULTS:
 *      The target object should have the original's values,
 *      and the original should be in a valid but unspecified state.
 */
void KErrorCodeTest::testEnumMoveAssignment()
{
    KErrorCode<TestErrorType> original(TestErrorType::Critical, "Critical error");
    KErrorCode<TestErrorType> target(TestErrorType::Success, "Success message");

    target = std::move(original);

    // The target should now have original's values
    QCOMPARE(target.code(), TestErrorType::Critical);
    QCOMPARE(target.message(), QString("Critical error"));
}

/**
 * OBJECTIVE:
 *      Verify the behavior of KErrorCode<TestErrorType> with rvalue references.
 *
 * INPUTS:
 *      Temporary KErrorCode<TestErrorType> objects created with rvalue references.
 *
 * EXPECTED RESULTS:
 *      The temporary objects should be correctly constructed,
 *      and their values should match the expected code and message.
 */
void KErrorCodeTest::testEnumRValueOperations()
{
    // Test construction from temporary objects
    KErrorCode<TestErrorType> error1(KErrorCode<TestErrorType>(TestErrorType::Warning, "Temporary warning"));
    QCOMPARE(error1.code(), TestErrorType::Warning);
    QCOMPARE(error1.message(), QString("Temporary warning"));

    // Test equality with temporary
    KErrorCode<TestErrorType> error2(TestErrorType::Warning, "Temporary warning");
    QVERIFY(error2 == KErrorCode<TestErrorType>(TestErrorType::Warning, "Temporary warning"));

    // Test inequality with temporary
    QVERIFY(error2 != KErrorCode<TestErrorType>(TestErrorType::Error, "Different error"));

    // Test assignment from temporary
    KErrorCode<TestErrorType> error3(TestErrorType::Success, "Initial");
    error3 = KErrorCode<TestErrorType>(TestErrorType::Critical, "Critical temp");
    QCOMPARE(error3.code(), TestErrorType::Critical);
    QCOMPARE(error3.message(), QString("Critical temp"));
}

/**
 * OBJECTIVE:
 *      Verify that KErrorCode works correctly with const char* constructor parameter.
 *
 * INPUTS:
 *      Various const char* strings including string literals and pointers.
 *
 * EXPECTED RESULTS:
 *      The KErrorCode should properly store and convert the C-string to QString.
 */
void KErrorCodeTest::testCStringConstructor()
{
    // Test with string literal
    KErrorCode<int> error1(100, "String literal");
    QCOMPARE(error1.message(), QString("String literal"));

    // Test with const char* variable
    const char *msg = "Variable message";
    KErrorCode<int> error2(200, msg);
    QCOMPARE(error2.message(), QString("Variable message"));

    // Test with empty string
    KErrorCode<int> error3(300, "");
    QCOMPARE(error3.message(), QString(""));
}

/**
 * OBJECTIVE:
 *      Verify that message() method correctly converts C-string to QString.
 *
 * INPUTS:
 *      KErrorCode objects with various message strings.
 *
 * EXPECTED RESULTS:
 *      The message() method should return proper QString objects.
 */
void KErrorCodeTest::testMessageConversion()
{
    KErrorCode<int> error(404, "UTF-8 message: åäö");
    QString result = error.message();

    QVERIFY(result.contains("UTF-8"));
    QVERIFY(result.contains("åäö"));
    QCOMPARE(result, QString("UTF-8 message: åäö"));
}

/**
 * OBJECTIVE:
 *      Verify that KErrorCode handles null message pointers correctly.
 *
 * INPUTS:
 *      KErrorCode objects with null message pointers.
 *
 * EXPECTED RESULTS:
 *      The KErrorCode should handle null pointers gracefully without crashing.
 */
void KErrorCodeTest::testNullMessageHandling()
{
    KErrorCode<int> error(500, nullptr);

    // Should not crash and should return empty string
    QString result = error.message();
    QCOMPARE(result, QString(""));

    // toString should also handle null gracefully
    QString toStringResult = error.toString();
    QVERIFY(!toStringResult.isEmpty()); // Should contain the error code even if message is empty
    QVERIFY(toStringResult.contains("500"));
}

QTEST_MAIN(KErrorCodeTest)
#include "kerrorcodetest.moc"
