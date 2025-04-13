#ifndef KERRORCODE_H
#define KERRORCODE_H

#include <concepts>
#include <type_traits>

#include <QString>

template<class T>
concept KErrorCodeClass = std::is_integral_v<T> || std::is_enum_v<T>;

template<KErrorCodeClass T>
class KErrorCode
{
public:
    KErrorCode(T code, const QString &message) noexcept
        : m_code(code)
        , m_message(message)
    {
        ;
    }

    KErrorCode(T code, QString &&message) noexcept
        : m_code(code)
        , m_message(std::move(message))
    {
        ;
    }

    KErrorCode(const KErrorCode<T> &) = default;
    KErrorCode(KErrorCode<T> &&) = default;
    KErrorCode &operator=(const KErrorCode<T> &) = default;
    KErrorCode &operator=(KErrorCode<T> &&) = default;
    ~KErrorCode() = default;

    [[nodiscard]]
    constexpr T code() const & noexcept
    {
        qDebug() << "KErrorCode::code lvalue called";
        return m_code;
    }

    [[nodiscard]]
    constexpr T &&code() && noexcept
    {
        qDebug() << "KErrorCode::code rvalue called";
        return std::move(m_code);
    }

    constexpr QString message() const & noexcept
    {
        return m_message;
    }

    constexpr QString &&message() && noexcept
    {
        return std::move(m_message);
    }

    constexpr QString toString() const noexcept
    {
        return QString("Error code %1: %2").arg(m_code).arg(m_message);
    }

    friend constexpr bool operator==(const KErrorCode<T> &lhs, const KErrorCode<T> &rhs) noexcept
    {
        return lhs.m_code == rhs.m_code;
    }

    friend constexpr bool operator==(const KErrorCode<T> &lhs, const T rhs) noexcept
    {
        return lhs.m_code == code;
    }

    friend constexpr QTextStream &operator<<(QTextStream &out, const KErrorCode &err)
    {
        return out << err.toString();
    }

private:
    T m_code;
    QString m_message;
};

#endif // KERRORCODE_H