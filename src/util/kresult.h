/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KRESULT_H
#define KRESULT_H

#include <cstdlib>
#include <iostream>
#include <optional>
#include <variant>

#include "kerrorcode.h"

/**
 * @brief A Rust-like "try" macro for KResult, in lieu of the '?' operator.
 *
 * K_TRY(STMT) is used to execute a statement that returns a KResult.
 * If the statement returns an error, the function will return that error immediately.
 * If the statement is successful, the code will continue executing.
 *
 * @code
 * KResult<int> someFunctionThatReturnsKResult()
 * {
 *     K_TRY(someFunctionThatReturnsKResult());
 *     // If the above function returns an error, this line will not be reached.
 *     return KResult<int>(42); // Return a successful result.
 * }
 * @endcode
 */
#define K_TRY(STMT)                                                                                                                                            \
    do {                                                                                                                                                       \
        auto result = STMT;                                                                                                                                    \
                                                                                                                                                               \
        if (!result) {                                                                                                                                         \
            return result;                                                                                                                                     \
        }                                                                                                                                                      \
    } while (false);

/**
 * @brief A result class similar to Rust's Result or C++'s std::expected.
 *
 * The rationale for this class is that KDE and Qt are still on C++20, and std::expected
 * will not be available until these two projects both move to C++23.
 *
 * Also, KResult is designed to be ABI-friendly, so it can be used in dynamic libraries.
 * C++'s std::expected is not ABI-friendly, as it can still throw exceptions.
 *
 * KResult's error type is restricted to KErrorCode. This is (a) to force the usage
 * of useful error codes and descriptive messages and (b) to provide an alternative
 * to C++'s std::error_condition.
 *
 * Unfortunately, C++ std::error_condition has the fatal flaw of needing an address
 * for a std::error_category global object to differentiate std::error_codes
 * from different domains.  Obviously, this approach causes issues with dynamic libraries.
 *
 * A nice side effect of restricting the error type to KErrorCode is that we can access
 * the KErrorCode's error code and message values directly from KResult!
 *
 * @note This class is not intended to be used with void "OK" types.
 * @note Be sure if using enum types with KErrorCode that the enum type is defined as an int
 *       for ABI compatibility if the enum is defined in a dynamic library.
 * @note Be sure to use only constant static error strings in memory with KErrorCode.
 *       toString() will call QObject::tr() on the error message, so it must be a valid C-string.
 *       You may call QObject::tr() after fetching the message if you need to translate the message
 *       for display to the user.
 *
 * @tparam T The "OK" result type.
 * @tparam E The error code type, defaults to int.
 */
template<class T, typename E = int>
class KResult
{
public:
    // Delete the default constructor
    KResult() = delete;

    /**
     * @brief Construct a new KResult object from an "OK" value.
     *
     * @param ok The "OK" value.
     */
    KResult(const T &ok) noexcept
    {
        m_result.template emplace<0>(ok);
    }

    /**
     * @brief Construct a new KResult object from an "OK" value (R-value reference).
     *
     * @param ok The "OK" value.
     */
    KResult(T &&ok) noexcept
    {
        m_result.template emplace<0>(std::forward<T>(ok));
    }

    /**
     * @brief Construct a new KResult object from a KErrorCode error.
     *
     * @param error The KErrorCode error.
     */
    KResult(const KErrorCode<E> &error) noexcept
    {
        m_result.template emplace<1>(error);
    }

    /**
     * @brief Construct a new KResult object from a KErrorCode error (R-value reference).
     *
     * @param error The KErrorCode error.
     */
    KResult(KErrorCode<E> &&error) noexcept
    {
        m_result.template emplace<1>(std::forward<KErrorCode<E>>(error));
    }

    /**
     * @brief Construct a new KResult object from an error code and message.
     *
     * @param code The error code value.
     * @param message The error message.
     */
    KResult(const E code, const char *message) noexcept
    {
        m_result.template emplace<1>(KErrorCode<E>(code, message));
    }

    /**
     * @brief Default copy constructor for KResult.
     *
     * This constructor initializes the result the given KResult's value.
     */
    KResult(const KResult<T, E> &) noexcept = default;

    /**
     * @brief Default copy constructor for KResult (R-value reference).
     *
     * This constructor initializes the result the given KResult's value.
     */
    KResult(KResult<T, E> &&) noexcept = default;

    /**
     * @brief Default assignment operator for KResult.
     *
     * This operator assigns the result to the given KResult's value.
     *
     * @return KResult<T, E>& A reference to this KResult object.
     */
    KResult<T, E> &operator=(const KResult<T, E> &) noexcept = default;

    /**
     * @brief Default assignment operator for KResult (R-value reference).
     *
     * This operator assigns the result to the given KResult's value.
     *
     * @return KResult<T, E>& A reference to this KResult object.
     */
    KResult<T, E> &operator=(KResult<T, E> &&) noexcept = default;

    /**
     * @brief Assign an "OK" value to this KResult.
     *
     * @param ok The "OK" value.
     * @return KResult<T, E>& A reference to this KResult object.
     */
    KResult<T, E> &operator=(const T &ok) noexcept
    {
        m_result.template emplace<0>(ok);
        return *this;
    };

    /**
     * @brief Assign an "OK" value to this KResult (R-value reference).
     *
     * @param ok The "OK" value (R-value reference).
     * @return KResult<T, E>& A reference to this KResult object.
     */
    KResult<T, E> &operator=(T &&ok) noexcept
    {
        m_result.template emplace<0>(std::forward<T>(ok));
        return *this;
    }

    /**
     * @brief Assign a KErrorCode error to this KResult.
     *
     * @param err The KErrorCode error.
     * @return KResult<T, E>& A reference to this KResult object.
     */
    KResult<T, E> &operator=(const KErrorCode<E> &err) noexcept
    {
        m_result.template emplace<1>(err);
        return *this;
    };

    /**
     * @brief Assign a KErrorCode error to this KResult (R-value reference).
     *
     * @param err The KErrorCode error (R-value reference).
     * @return KResult<T, E>& A reference to this KResult object.
     */
    KResult<T, E> &operator=(KErrorCode<E> &&err) noexcept
    {
        m_result.template emplace<1>(std::forward<KErrorCode<E>>(err));
        return *this;
    }

    /**
     * @brief Destroys the KResult object.
     */
    ~KResult() noexcept = default;

    /**
     * @brief Returns a pointer to the "OK" value.
     *
     * @return const T* A pointer to the "OK" value.
     */
    [[nodiscard]]
    constexpr const T *operator->() const & noexcept
    {
        return std::get_if<0>(&m_result);
    }

    /**
     * @brief Returns a pointer to the "OK" value (R-value reference).
     *
     * @return T* A pointer to the "OK" value.
     */
    [[nodiscard]]
    constexpr T *operator->() && noexcept
    {
        return std::get_if<0>(&m_result);
    }

    /**
     * @brief Returns a reference to the "OK" value.
     *
     * @return const T& A reference to the "OK" value.
     */
    [[nodiscard]]
    constexpr const T &operator*() const & noexcept
    {
        return value();
    }

    /**
     * @brief Returns a constant reference to the "OK" value (R-value reference).
     *
     * @return T&& A reference to the "OK" value.
     */
    [[nodiscard]]
    constexpr T &&operator*() && noexcept
    {
        return std::move(*this).value();
    }

    /**
     * @brief Converts this KResult to a boolean value.
     *
     * @return true if the result is "OK", false if it has an error.
     */
    [[nodiscard]]
    constexpr explicit operator bool() const noexcept
    {
        return ok();
    }

    /**
     * @brief Checks if this KResult is "OK".
     *
     * @return true if the result is "OK", false if it has an error.
     */
    [[nodiscard]]
    constexpr bool ok() const noexcept
    {
        return (0 == m_result.index());
    }

    /**
     * @brief Checks if this KResult has an error.
     *
     * @return true if the result has an error, false if it is "OK".
     */
    [[nodiscard]]
    constexpr bool hasError() const noexcept
    {
        return (1 == m_result.index());
    }

    /**
     * @brief Returns the "OK" value if this KResult is "OK".
     *
     * If this KResult has an error, it will terminate the program with an error message.
     *
     * @return const T& The "OK" value.
     */
    [[nodiscard]]
    constexpr const T &value() const & noexcept
    {
        if (ok()) [[likely]] {
            return valueUnchecked();
        }

        std::cerr << "KResult::ok: Illegal access to OK value!" << std::endl;
        std::terminate();
    }

    /**
     * @brief Returns the "OK" value if this KResult is "OK" (R-value reference).
     *
     * If this KResult has an error, it will terminate the program with an error message.
     *
     * @return T&& The "OK" value (R-value reference).
     */
    [[nodiscard]]
    constexpr T &&value() &&
    {
        if (ok()) [[likely]] {
            return std::move(*this).valueUnchecked();
        }

        std::cerr << "KResult::ok: Illegal access to OK value!" << std::endl;
        std::terminate();
    }

    /**
     * @brief Returns the KErrorCode error if this KResult has an error.
     *
     * If this KResult is "OK", it will terminate the program with an error message.
     *
     * @return const KErrorCode<E>& The KErrorCode error.
     */
    [[nodiscard]]
    constexpr const KErrorCode<E> &error() const & noexcept
    {
        if (!ok()) [[likely]] {
            return errorUnchecked();
        }

        std::cerr << "KResult::error: Illegal access to error value!" << std::endl;
        std::terminate();
    }

    /**
     * @brief Returns the KErrorCode error if this KResult has an error (R-value reference).
     *
     * If this KResult is "OK", it will terminate the program with an error message.
     *
     * @return KErrorCode<E>&& The KErrorCode error (R-value reference).
     */
    [[nodiscard]]
    constexpr KErrorCode<E> &&error() && noexcept
    {
        if (!ok()) [[likely]] {
            return std::move(*this).errorUnchecked();
        }

        std::cerr << "KResult::error: Illegal access to error value!" << std::endl;
        std::terminate();
    }

    /**
     * @brief Returns the error code if this KResult has an error.
     *
     * If this KResult is "OK", it will terminate the program with an error message.
     *
     * @return E The error code.
     */
    [[nodiscard]]
    constexpr E errcode() const & noexcept
    {
        if (!ok()) [[likely]] {
            return errorUnchecked().code();
        }

        std::cerr << "KResult::errcode: Illegal access to error code!" << std::endl;
        std::terminate();
    }

    /**
     * @brief Returns the error code if this KResult has an error (R-value reference).
     *
     * If this KResult is "OK", it will terminate the program with an error message.
     *
     * @return E&& The error code (R-value reference).
     */
    [[nodiscard]]
    constexpr E &&errcode() && noexcept
    {
        if (!ok()) [[likely]] {
            return std::move(*this).errorUnchecked().code();
        }

        std::cerr << "KResult::errcode: Illegal access to error code!" << std::endl;
        std::terminate();
    }

    /**
     * @brief Returns the error message if this KResult has an error.
     *
     * If this KResult is "OK", it will terminate the program with an error message.
     *
     * @return char * The error message.
     */
    [[nodiscard]]
    constexpr const char *errmsg() const noexcept
    {
        if (!ok()) [[likely]] {
            return errorUnchecked().message();
        }

        std::cerr << "KResult::errmsg: Illegal access to error message!" << std::endl;
        std::terminate();
    }

    /**
     * @brief Compares two KResult objects for equality.
     *
     * @param lhs Left-hand side KResult object.
     * @param rhs Right-hand side KResult object.
     * @return true If both objects have the same "OK" value or error code.
     * @return false If both objects do not match.
     *
     * @note The OK value must implement operator== for this to work.
     */
    [[nodiscard]]
    friend constexpr bool operator==(const KResult<T, E> &lhs, const KResult<T, E> &rhs) noexcept
    {
        return ((lhs.ok() == rhs.ok())
                && (((0 == lhs.m_result.index()) && (lhs.value() == rhs.value())) || ((1 == lhs.m_result.index()) && (lhs.error() == rhs.error()))));
    }

    /**
     * @brief Compares a KResult object with an "OK" value for equality.
     *
     * @param lhs Left-hand side KResult object.
     * @param rhs Right-hand side "OK" value.
     * @return true If the KResult is "OK" and the value matches the right-hand-side "OK" value.
     * @return false If the KResult has an error or the values do not match.
     */
    [[nodiscard]]
    friend constexpr bool operator==(const KResult<T, E> &lhs, const T &rhs) noexcept
    {
        return (lhs.ok() && (lhs.value() == rhs));
    }

    /**
     * @brief Compares a KResult object with a KErrorCode for equality.
     *
     * @param lhs Left-hand side KResult object.
     * @param rhs Right-hand side KErrorCode object.
     * @return true If the KResult has an error and the error matches the right-hand-side KErrorCode.
     * @return false If the KResult is "OK" or the errors do not match.
     */
    [[nodiscard]]
    friend constexpr bool operator==(const KResult<T, E> &lhs, const KErrorCode<E> &rhs) noexcept
    {
        return (lhs.hasError() && (lhs.error() == rhs));
    }

private:
    // The result is stored as a variant of the "OK" value type and the KErrorCode error type.
    // This allows us to use the same KResult class for both successful results and errors.
    // The index 0 corresponds to the "OK" value, and index 1 corresponds to the KErrorCode error.
    // This is similar to Rust's Result<T, E> or C++'s std::expected<T, E>.
    //
    // The variant is used to ensure that the KResult can only hold either an "OK" value or an error, but not both.
    // This is a key feature of KResult, as it allows us to easily check if the result is "OK" or has an error.
    // The variant also allows us to access the "OK" value and error directly without needing to check the type first
    std::variant<T, KErrorCode<E>> m_result;

    // Returns the "OK" value without checking if this KResult is "OK".
    // This is used internally to avoid unnecessary checks when we know the result is "OK".
    constexpr const T &valueUnchecked() const & noexcept
    {
        return std::get<0>(m_result);
    }

    // Returns the "OK" value (R-value reference) without checking if this KResult is "OK".
    // This is used internally to avoid unnecessary checks when we know the result is "OK".
    constexpr T &&valueUnchecked() && noexcept
    {
        return std::get<0>(std::move(m_result));
    }

    // Returns the KErrorCode error without checking if this KResult has an error.
    // This is used internally to avoid unnecessary checks when we know the result has an error.
    constexpr const KErrorCode<E> &errorUnchecked() const & noexcept
    {
        return std::get<1>(m_result);
    }

    // Returns the KErrorCode error (R-value reference) without checking if this KResult has an error.
    // This is used internally to avoid unnecessary checks when we know the result has an error.
    constexpr KErrorCode<E> &&errorUnchecked() && noexcept
    {
        return std::get<1>(std::move(m_result));
    }
};

#endif // KRESULT_H
