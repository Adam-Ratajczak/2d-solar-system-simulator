#pragma once

#include <optional>
#include <string>

namespace Util
{

template<class T, class Error>
class [[nodiscard]] ErrorOr
{
public:
    ErrorOr(T&& t)
    : m_value(std::move(t)) {}

    ErrorOr(Error&& t)
    : m_error(std::move(t)) {}

    template<class U>
    ErrorOr(U&& u)
    : m_value(std::move(u)) {}

    template<class U>
    ErrorOr(ErrorOr<U, Error>&& u)
    {
        if(u.has_error())
            m_error = u.release_error();
        else
            m_value = u.release_value();
    }

    bool has_value() const { return m_value.has_value(); }
    bool has_error() const { return m_error.has_value(); }
    T& value() { return m_value.value(); }
    T const& value() const { return m_value.value(); }
    T release_value() { return std::move(value()); }
    Error& error() { return m_error.value(); }
    Error const& error() const { return m_error.value(); }
    Error release_error() { return std::move(error()); }

private:
    std::optional<T> m_value;
    std::optional<Error> m_error;
};

}
