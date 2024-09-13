#ifndef KRESULT_H
#define KRESULT_H

#include <optional>

template<class T>
class Ok
{
public:
    Ok(const T &value);
    Ok(T &&value);
    ~Ok();

    T value;
};

template<class E>
class Err
{
public:
    Err(const E &value);
    Err(E &&value);
    ~Err();

    E value;
};

template<>
class Err<void>
{
};

template<class T, class E>
class KResult
{
public:
    KResult(const Ok<T> &value)
    {
        m_ok = true;
        m_value.ok = value;
    }

    KResult(const Err<E> &value)
    {
        m_ok = false;
        m_value.err = value;
    }

    ~KResult()
    {
        ;
    }

    bool isOk() const
    {
        return m_ok;
    }

    bool isErr() const
    {
        return !m_ok;
    }

    std::optional<T> ok() const
    {
        if (m_ok) {
            return m_value.ok;
        }

        return {};
    }

    std::optional<E> err() const
    {
        if (!m_ok) {
            return m_value.err;
        }

        return {};
    }

    bool operator()() const
    {
        return isOk();
    }

    template<class Func, F>
        inline typename std::enable_if < !std::is_convertible<Func, KResult<T, F>::value, void>::type orElse(Func fn)
    {
        if (isErr()) {
            return fn();
        }
    }

    template<class Func, U>
        inline typename std::enable_if < !std::is_convertible<Func, KResult<U, E>::value, void>::type andThen(Func fn)
    {
        if (isOk()) {
            return fn();
        }
    }

    template<class Func, U, F>
    bool operator&&(const KResult<U, F> &other) const
    {
        return isOK() && other.isOk();
    }

    bool operator&&(bool other) const
    {
        return isOK() && other;
    }

    template<class Func, U, F>
    bool operator||(const KResult<U, F> &other) const
    {
        return isOK() || other.isOk();
    }

    bool operator||(bool other) const
    {
        return isOk() || other;
    }

private:
    bool m_ok;

    union {
        Ok<T> ok;
        Err<T> err;
    } m_value;
};

#define TRY_RESULT(ARGS...) {
if
}

#endif // KRESULT_H