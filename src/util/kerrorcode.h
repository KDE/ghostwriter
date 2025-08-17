/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KERRORCODE_H
#define KERRORCODE_H

#include <QString>
#include <QTextStream>

/**
 * @brief Holds an ABI-friendly error code and message meant to work with KResult in lieu of exceptions.
 */
class KErrorCode
{
public:
    // Delete the default constructor
    KErrorCode() = delete;

    /**
     * @brief Construct a new KErrorCode object.
     *
     * @param code An error code. The value of this code is implementation-defined, but it should be unique to the error domain.
     *             It is recommended to use an enum or int type for this code.
     * @param message The error message. This should be a human-readable string that describes the error.
     *                It is recommended to use QString or a compatible string type.
     */
    KErrorCode(const int code, const QString &message) noexcept;

    /**
     * @brief Construct new KErrorCode object (R-value reference).
     *
     * @param code See L-value constructor description.
     * @param message See L-value constructor description.
     */
    KErrorCode(const int code, QString &&message) noexcept;

    /**
     * @brief Default constructor for KErrorCode.
     *
     * This constructor initializes the error code to the given value.
     */
    KErrorCode(const KErrorCode &) = default;

    /**
     * @brief Default constructor for KErrorCode (R-value reference).
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
     * @return int The error code.
     */
    [[nodiscard]]
    constexpr int code() const & noexcept
    {
        return m_code;
    }

    /**
     * @brief Returns the error code (R-value reference).
     *
     * @return int The error code.
     */
    [[nodiscard]]
    constexpr int &&code() && noexcept
    {
        return std::move(m_code);
    }

    /**
     * @brief Returns the error message.
     *
     * @return QString The error message.
     */
    [[nodiscard]]
    QString message() const & noexcept;

    /**
     * @brief Returns the error message (R-value reference).
     *
     * @return QString The error message.
     */
    [[nodiscard]]
    QString &&message() && noexcept;

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
     * @brief Compares a KErrorCode object with an integer code for equality.
     *
     * @param lhs The KErrorCode object.
     * @param code The integer code to compare against.
     * @return true if the KErrorCode's code matches the integer code, false otherwise.
     */
    friend inline bool operator==(const KErrorCode &lhs, const int code) noexcept
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
    // This should be a unique identifier for the error.
    // It is recommended to use an enum or int type for this code.
    int m_code;

    // The error message.
    // This should be a human-readable string that describes the error.
    // It is recommended to use QString or a compatible string type.
    QString m_message;
};

#endif // KERRORCODE_H
