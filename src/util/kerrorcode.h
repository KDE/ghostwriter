/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KERRORCODE_H
#define KERRORCODE_H

#include <QObject>
#include <QString>
#include <QTextStream>
#include <type_traits>

/**
 * @brief Holds an ABI-friendly error code and message meant to work with KResult in lieu of exceptions.
 *
 * @tparam E The type of the error code. Must be an integer type or an enum type.
 */
template<typename E = int>
class KErrorCode
{
    static_assert(std::is_integral_v<E> || std::is_enum_v<E>, "E must be an integer type or an enum type");

public:
    // Delete the default constructor
    KErrorCode() = delete;

    /**
     * @brief Construct a new KErrorCode object.
     *
     * @param code An error code. The value of this code is implementation-defined, but it should be unique to the error domain.
     * @param message The error message. This should be a human-readable string that describes the error.
     */
    KErrorCode(const E code, const char *message) noexcept;

    /**
     * @brief Default copy constructor for KErrorCode.
     *
     * This constructor initializes the error code to the given value.
     */
    KErrorCode(const KErrorCode &) = default;

    /**
     * @brief Default move constructor for KErrorCode.
     *
     * This constructor initializes the error code to the given value.
     */
    KErrorCode(KErrorCode &&) = default;

    /**
     * @brief Default assignment operator for KErrorCode.
     *
     * @return KErrorCode& Assigns the error code to the given value.
     */
    KErrorCode &operator=(const KErrorCode &) = default;

    /**
     * @brief Default assignment operator for KErrorCode (R-value reference).
     *
     * @return KErrorCode& Assigns the error code to the given value.
     */
    KErrorCode &operator=(KErrorCode &&) = default;

    /**
     * @brief Destroys the KErrorCode object.
     */
    ~KErrorCode() = default;

    /**
     * @brief Returns the error code.
     *
     * @return E The error code.
     */
    [[nodiscard]]
    constexpr E code() const & noexcept
    {
        return m_code;
    }

    /**
     * @brief Returns the error code (R-value reference).
     *
     * @return E The error code.
     */
    [[nodiscard]]
    constexpr E &&code() && noexcept
    {
        return std::move(m_code);
    }

    /**
     * @brief Returns the error message.
     *
     * @return char* The error message.
     */
    [[nodiscard]]
    constexpr const char *message() const & noexcept
    {
        return m_message;
    }

    /**
     * @brief Returns the error message (R-value reference).
     *
     * @return char* The error message.
     */
    [[nodiscard]]
    constexpr const char *&&message() && noexcept
    {
        return std::move(m_message);
    }

    /**
     * @brief Returns a string representation of the error code and message.
     *
     * @return QString A string containing the error code and message.
     */
    [[nodiscard]]
    QString toString() const noexcept;

    /**
     * @brief Compares two KErrorCode objects for equality.
     *
     * @param lhs The left-hand side KErrorCode object.
     * @param rhs The right-hand side KErrorCode object.
     * @return true if both objects have the same code and message, false otherwise.
     */
    friend inline bool operator==(const KErrorCode &lhs, const KErrorCode &rhs) noexcept
    {
        return (lhs.m_code == rhs.m_code && lhs.m_message == rhs.m_message);
    }

    /**
     * @brief Compares a KErrorCode object with a code for equality.
     *
     * @param lhs The KErrorCode object.
     * @param code The code to compare against.
     * @return true if the KErrorCode's code matches the code, false otherwise.
     */
    friend inline bool operator==(const KErrorCode &lhs, const E code) noexcept
    {
        return (lhs.m_code == code);
    }

    /**
     * @brief Outputs the KErrorCode to a QTextStream.
     *
     * @param out The QTextStream to output to.
     * @param err The KErrorCode object to output.
     * @return QTextStream& A reference to the QTextStream after outputting the KErrorCode.
     */
    friend inline QTextStream &operator<<(QTextStream &out, const KErrorCode &err)
    {
        return out << err.toString();
    }

private:
    // The error code.
    // This can be an integer type or an enum type.
    E m_code;

    // The error message.
    // This should be a pointer to a static const string that describes the error.
    const char *m_message;
};

// Implementation of constructor
template<typename E>
KErrorCode<E>::KErrorCode(const E code, const char *message) noexcept
    : m_code(code)
    , m_message(message)
{
}

// Implementation of toString
template<typename E>
QString KErrorCode<E>::toString() const noexcept
{
    QString msgStr = m_message ? QObject::tr(m_message) : "";

    // If E is an enum, convert to underlying integral type for display
    if constexpr (std::is_enum_v<E>) {
        return QStringLiteral("Error %1 (%2): %3").arg(static_cast<typename std::underlying_type<E>::type>(m_code)).arg((int)m_code).arg(msgStr);
    } else {
        return QStringLiteral("Error %1: %2").arg(m_code).arg(msgStr);
    }
}

#endif // KERRORCODE_H
