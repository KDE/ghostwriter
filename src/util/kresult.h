/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KRESULT_H
#define KRESULT_H

#include <cstdlib>
#include <expected>
#include <iostream>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

#include "kerrorcode.h"

template<class T, KErrorCodeClass E>
class KResult;

namespace _KResultPrivate
{
template<class T, class U, class Fn>
concept MapFunction = std::is_invocable_r<U, Fn, T>::value;
}

/**
 * A result class similar to Rust's Result or C++'s std::expected. The
 * rationale for this class is that KDE and Qt are still on C++20, and std::expected
 * will not be available until these two projects both move to C++23.
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
 * @tparam T The "OK" result type.
 * @tparam E The int or enum error code type.
 */
template<class T, KErrorCodeClass E>
class KResult
{
public:
    KResult(const T &ok) noexcept
    {
        m_result.template emplace<0>(ok);
    }

    KResult(T &&ok) noexcept
    {
        m_result.template emplace<0>(std::forward<T>(ok));
    }

    KResult(const KErrorCode<E> &error) noexcept
    {
        m_result.template emplace<1>(error);
    }

    KResult(KErrorCode<E> &&error) noexcept
    {
        m_result.template emplace<1>(std::forward<KErrorCode<E>>(error));
    }

    KResult(E code, const QString &message) noexcept
    {
        m_result.template emplace<1>(KErrorCode<E>(code, message));
    }

    KResult(E code, QString &&message) noexcept
    {
        m_result.template emplace<1>(KErrorCode<E>(code, std::forward<QString>(message)));
    }

    KResult(const KResult<T, E> &) = default;
    KResult(KResult<T, E> &&) = default;
    KResult<T, E> &operator=(const KResult<T, E> &) = default;
    KResult<T, E> &operator=(KResult<T, E> &&) = default;

    KResult<T, E> &operator=(const T &ok) noexcept
    {
        m_result.template emplace<0>(ok);
        return *this;
    };

    KResult<T, E> &operator=(T &&ok) noexcept
    {
        m_result.template emplace<0>(std::forward<T>(ok));
        return *this;
    }

    KResult<T, E> &operator=(const KErrorCode<E> &err) noexcept
    {
        m_result.template emplace<1>(err);
        return *this;
    };

    KResult<T, E> &operator=(KErrorCode<E> &&err) noexcept
    {
        m_result.template emplace<1>(std::forward<KErrorCode<E>>(err));
        return *this;
    }

    ~KResult() = default;

    constexpr const T *operator->() const noexcept
    {
        return std::get_if<0>(m_result);
    }

    constexpr const T &operator*() const & noexcept
    {
        return value();
    }

    constexpr T &&operator*() && noexcept
    {
        return std::move(*this).value();
    }

    constexpr const T &&operator*() const && noexcept
    {
        return value();
    }

    [[nodiscard]]
    constexpr explicit operator bool() const noexcept
    {
        return ok();
    }

    [[nodiscard]]
    constexpr bool ok() const noexcept
    {
        return (0 == m_result.index());
    }

    [[nodiscard]]
    constexpr bool hasError() const noexcept
    {
        return (1 == m_result.index());
    }

    constexpr const T &value() const & noexcept
    {
        qDebug() << "KResult::value lvalue called";
        if (ok()) [[likely]] {
            return valueUnchecked();
        }

        std::cerr << "KResult::ok: Illegal access to OK value!" << std::endl;
        std::terminate();
    }

    constexpr T &&value() &&
    {
        qDebug() << "KResult::value rvalue called";
        if (ok()) [[likely]] {
            return std::move(*this).valueUnchecked();
        }

        std::cerr << "KResult::ok: Illegal access to OK value!" << std::endl;
        std::terminate();
    }

    constexpr const KErrorCode<E> &error() const & noexcept
    {
        qDebug() << "KResult::error lvalue called";
        if (!ok()) [[likely]] {
            return errorUnchecked();
        }

        std::cerr << "KResult::error: Illegal access to error value!" << std::endl;
        std::terminate();
    }

    constexpr KErrorCode<E> &&error() && noexcept
    {
        qDebug() << "KResult::error rvalue called";
        if (!ok()) [[likely]] {
            return std::move(*this).errorUnchecked();
        }

        std::cerr << "KResult::error: Illegal access to error value!" << std::endl;
        std::terminate();
    }

    constexpr E errcode() const & noexcept
    {
        qDebug() << "KResult::errorcode lvalue called";

        if (!ok()) [[likely]] {
            return errorUnchecked().code();
        }

        std::cerr << "KResult::errcode: Illegal access to error code!" << std::endl;
        std::terminate();
    }

    constexpr E &&errcode() && noexcept
    {
        qDebug() << "KResult::errorcode rvalue called";

        if (!ok()) [[likely]] {
            return std::move(*this).errorUnchecked().code();
        }

        std::cerr << "KResult::errcode: Illegal access to error code!" << std::endl;
        std::terminate();
    }

    constexpr QString errmsg() const & noexcept
    {
        qDebug() << "KResult::errmsg lvalue called";

        if (!ok()) [[likely]] {
            return errorUnchecked().message();
        }

        std::cerr << "KResult::errmsg: Illegal access to error message!" << std::endl;
        std::terminate();
    }

    constexpr QString &&errmsg() && noexcept
    {
        qDebug() << "KResult::errmsg rvalue called";

        if (!ok()) [[likely]] {
            return std::move(*this).errorUnchecked().message();
        }

        std::cerr << "KResult::errmsg: Illegal access to error message!" << std::endl;
        std::terminate();
    }

    template<class U>
        requires std::convertible_to<U, T>
    constexpr T valueOr(U &&defaultValue) const &
    {
        if (ok()) {
            return valueUnchecked();
        }

        return static_cast<T>(std::forward<U>(defaultValue));
    }

    template<class U>
        requires std::convertible_to<U, T>
    constexpr T valueOr(U &&defaultValue) &&
    {
        if (ok()) {
            return std::move(*this).valueUnchecked();
        }

        return static_cast<T>(std::forward<U>(defaultValue));
    }

    template<KErrorCodeClass F>
    constexpr KErrorCode<E> errorOr(KErrorCode<F> &&defaultValue) const & noexcept
    {
        qDebug() << "KResult::errorOr lvalue called";
        if (!ok()) {
            return errorUnchecked();
        }

        return KErrorCode<E>(static_cast<E>(defaultValue.code()), defaultValue.message());
    }

    template<KErrorCodeClass F>
    constexpr KErrorCode<E> errorOr(KErrorCode<F> &&defaultValue) && noexcept
    {
        qDebug() << "KResult::errorOr rvalue called";
        if (!ok()) {
            return std::move(*this).errorUnchecked();
        }

        return KErrorCode<E>(static_cast<E>(defaultValue.code()), defaultValue.message());
    }

    template<class Fn>
        requires _KResultPrivate::MapFunction<E, KResult<T, E>, Fn>
    constexpr KResult<T, E> orElse(Fn &&fn) const & noexcept
    {
        qDebug() << "KResult::orElse lvalue called";
        if (!ok()) {
            return std::invoke(std::forward<Fn>(fn), errorUnchecked().code());
        }

        return valueUnchecked();
    }

    template<class Fn>
        requires _KResultPrivate::MapFunction<E, KResult<T, E>, Fn>
    constexpr KResult<T, E> orElse(Fn &&fn) && noexcept
    {
        qDebug() << "KResult::orElse rvalue called";
        if (!ok()) {
            return std::invoke(std::forward<Fn>(fn), std::move(*this).errorUnchecked().code());
        }

        return std::move(*this).valueUnchecked();
    }

    template<class Fn>
        requires _KResultPrivate::MapFunction<T, KResult<T, E>, Fn>
    constexpr KResult<T, E> andThen(Fn &&fn) const & noexcept
    {
        qDebug() << "KResult::andThen lvalue called";
        if (ok()) {
            return std::invoke(std::forward<Fn>(fn), valueUnchecked());
        }

        return errorUnchecked();
    }

    template<class Fn>
        requires _KResultPrivate::MapFunction<T, KResult<T, E>, Fn>
    constexpr KResult<T, E> andThen(Fn &&fn) && noexcept
    {
        qDebug() << "KResult::andThen rvalue called";
        if (ok()) {
            return std::invoke(std::forward<Fn>(fn), std::move(*this).valueUnchecked());
        }

        return std::move(*this).errorUnchecked();
    }

    template<class Fn, typename U = std::invoke_result_t<Fn, T>>
        requires _KResultPrivate::MapFunction<T, U, Fn>
    constexpr KResult<U, E> map(Fn &&fn) const & noexcept
    {
        qDebug() << "KResult::map lvalue called";
        if (ok()) {
            return std::invoke(std::forward<Fn>(fn), valueUnchecked());
        }

        return errorUnchecked();
    }

    template<class Fn, typename U = std::invoke_result_t<Fn, T>>
        requires _KResultPrivate::MapFunction<T, U, Fn>
    constexpr KResult<U, E> map(Fn &&fn) && noexcept
    {
        qDebug() << "KResult::map rvalue called";
        if (ok()) {
            return std::invoke(std::forward<Fn>(fn), std::move(*this).valueUnchecked());
        }

        return std::move(*this).errorUnchecked();
    }

    template<class Fn, KErrorCodeClass F = typename std::invoke_result_t<Fn, E>>
        requires _KResultPrivate::MapFunction<E, F, Fn>
    constexpr KResult<T, F> mapError(Fn &&fn) const & noexcept
    {
        qDebug() << "KResult::mapError lvalue called";
        if (!ok()) {
            return KErrorCode(std::invoke(std::forward<Fn>(fn), errorUnchecked().code()), errorUnchecked().message());
        }

        return valueUnchecked();
    }

    template<class Fn, KErrorCodeClass F = typename std::invoke_result_t<Fn, E>>
        requires _KResultPrivate::MapFunction<E, F, Fn>
    constexpr KResult<T, F> mapError(Fn &&fn) && noexcept
    {
        qDebug() << "KResult::mapError rvalue called";
        if (!ok()) {
            return KErrorCode(std::invoke(std::forward<Fn>(fn), errorUnchecked().code()), errorUnchecked().message());
        }

        return std::move(*this).valueUnchecked();
    }

    [[nodiscard]]
    constexpr const KResult<T, E> &and_(const KResult<T, E> &other) const & noexcept
    {
        if (ok()) {
            return other;
        }

        return *this;
    }

    [[nodiscard]]
    constexpr KResult<T, E> &&and_(KResult<T, E> &&other) && noexcept
    {
        if (ok()) {
            return std::forward<KResult<T, E>>(other);
        }

        return std::move(*this);
    }

    [[nodiscard]]
    constexpr const KResult<T, E> &or_(const KResult<T, E> &other) const & noexcept
    {
        if (ok()) {
            return *this;
        }

        return other;
    }

    [[nodiscard]]
    constexpr KResult<T, E> &&or_(KResult<T, E> &&other) && noexcept
    {
        if (ok()) {
            return std::move(*this);
        }

        return std::forward<KResult<T, E>>(other);
    }

    [[nodiscard]]
    friend constexpr bool operator==(const KResult<T, E> &lhs, const KResult<T, E> &rhs) noexcept
    {
        return ((lhs.ok() == rhs.ok())
                && (((0 == lhs.m_result.index()) && (lhs.value() == rhs.value())) || ((1 == lhs.m_result.index()) && (lhs.error() == rhs.error()))));
    }

    [[nodiscard]]
    friend constexpr bool operator==(const KResult<T, E> &lhs, const T &rhs) noexcept
    {
        return (lhs.ok() && (lhs.value() == rhs));
    }

    [[nodiscard]]
    friend constexpr bool operator==(const KResult<T, E> &lhs, const KErrorCode<E> &rhs) noexcept
    {
        return (lhs.hasError() && (lhs.error() == rhs));
    }

    [[nodiscard]]
    friend constexpr bool operator==(const KResult<T, E> &lhs, E rhs) noexcept
    {
        return (lhs.hasError() && (lhs.errcode() == rhs));
    }

private:
    std::variant<T, KErrorCode<E>> m_result;

    constexpr const T &valueUnchecked() const & noexcept
    {
        return std::get<0>(m_result);
    }

    constexpr T &&valueUnchecked() && noexcept
    {
        return std::get<0>(std::move(m_result));
    }

    constexpr const KErrorCode<E> &errorUnchecked() const & noexcept
    {
        return std::get<1>(m_result);
    }

    constexpr KErrorCode<E> &&errorUnchecked() && noexcept
    {
        return std::get<1>(std::move(m_result));
    }
};

#define K_TRY(STMT)                                                                                                                                            \
    {                                                                                                                                                          \
        auto result = STMT;                                                                                                                                    \
                                                                                                                                                               \
        if (!result) {                                                                                                                                         \
            return result;                                                                                                                                     \
        }                                                                                                                                                      \
    }

#endif // KRESULT_H