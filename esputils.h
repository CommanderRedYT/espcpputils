#pragma once

// system includes
#include <algorithm>
#include <cstdint>
#include <functional>
#include <vector>

/*
   Avoid "unused parameter" warnings
*/
#define ESP_UNUSED(x) (void)x;

/* These two macros make it possible to turn the builtin line expander into a
 * string literal. */
#define ESP_STRINGIFY2(x) #x
#define ESP_STRINGIFY(x) ESP_STRINGIFY2(x)

/*
   Some classes do not permit copies to be made of an object. These
   classes contains a private copy constructor and assignment
   operator to disable copying (the compiler gives an error message).
*/
#define ESP_DISABLE_COPY(Class) \
    Class(const Class &) = delete;\
    Class &operator=(const Class &) = delete;
#define ESP_DISABLE_MOVE(Class) \
    Class(Class &&) = delete; \
    Class &operator=(Class &&) = delete;
#define ESP_DISABLE_COPY_MOVE(Class) \
    ESP_DISABLE_COPY(Class) \
    ESP_DISABLE_MOVE(Class)


/* These two macros make it possible to define a typesafe enum with parse and
 * toString methods */
#define DECLARE_TYPESAFE_ENUM_HELPER1(name) name,
#define DECLARE_TYPESAFE_ENUM_HELPER2(name) case TheEnum::name: return #name;
#define DECLARE_TYPESAFE_ENUM_HELPER3(name) else if (str == ESP_STRINGIFY(name)) return TheEnum::name;

#define DECLARE_TYPESAFE_ENUM(Name, Derivation, Values) \
    enum class Name Derivation \
    { \
        Values(DECLARE_TYPESAFE_ENUM_HELPER1) \
    }; \
    namespace { \
    template<typename T> \
    T toString(Name value) \
    { \
        switch (value) \
        { \
        using TheEnum = Name; \
        Values(DECLARE_TYPESAFE_ENUM_HELPER2) \
        } \
        return T{"Unknown " #Name "("} + int(value) + ')'; \
    } \
    template<typename T> \
    std::optional<Name> parse##Name(const T &str) \
    { \
    using TheEnum = Name; \
    if (false) {} \
    Values(DECLARE_TYPESAFE_ENUM_HELPER3) \
    return std::nullopt; \
    } \
    } // namespace

namespace espcpputils {
template<class T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi )
{
    // assert( !(hi < lo) );
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

template<typename T>
T mapValue(T x, T in_min, T in_max, T out_min, T out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template<typename... T>
class Signal
{
public:
    using Slot = std::function<void(T...)>;

    Signal &operator+=(Slot &&slot)
    {
        m_slots.emplace_back(std::move(slot));
        return *this;
    }

    Signal &operator+=(const Slot &slot)
    {
        m_slots.emplace_back(slot);
        return *this;
    }

    template<typename ...Targs>
    void operator()(Targs && ...args) const
    {
        for (const auto &slot : m_slots)
            slot(std::forward<Targs>(args)...);
    }

private:
    std::vector<Slot> m_slots;
};
} // namespace espcpputils
