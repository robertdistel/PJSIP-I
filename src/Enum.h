// This file is part of Better Enums, released under the BSD 2-clause license.
// See doc/LICENSE for details, or visit http://github.com/aantron/better-enums.

#pragma once

#ifndef ENUMS_ENUM_H
#define ENUMS_ENUM_H



#include <cstddef>
#include <cstring>
#include <iosfwd>
#include <stdexcept>



// Feature detection.

#ifdef __GNUC__
#   ifdef __clang__
#       if __has_feature(cxx_constexpr)
#           define ENUMS_HAVE_CONSTEXPR
#       endif
#       if !defined(__EXCEPTIONS) || !__has_feature(cxx_exceptions)
#           define ENUMS_NO_EXCEPTIONS
#       endif
#   else
#       if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#           if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))
#               define ENUMS_HAVE_CONSTEXPR
#           endif
#       endif
#       ifndef __EXCEPTIONS
#           define ENUMS_NO_EXCEPTIONS
#       endif
#   endif
#endif

#ifdef _MSC_VER
#   ifndef _CPPUNWIND
#       define ENUMS_NO_EXCEPTIONS
#   endif
#   if _MSC_VER < 1600
#       define ENUMS_VC2008_WORKAROUNDS
#   endif
#endif

#ifdef ENUMS_CONSTEXPR
#   define ENUMS_HAVE_CONSTEXPR
#endif

#ifdef ENUMS_NO_CONSTEXPR
#   ifdef ENUMS_HAVE_CONSTEXPR
#       undef ENUMS_HAVE_CONSTEXPR
#   endif
#endif

// GCC (and maybe clang) can be made to warn about using 0 or NULL when nullptr
// is available, so Better Enums tries to use nullptr. This passage uses
// availability of constexpr as a proxy for availability of nullptr, i.e. it
// assumes that nullptr is available when compiling on the right versions of gcc
// and clang with the right -std flag. This is actually slightly wrong, because
// nullptr is also available in Visual C++, but constexpr isn't. This
// imprecision doesn't matter, however, because VC++ doesn't have the warnings
// that make using nullptr necessary.
#ifdef ENUMS_HAVE_CONSTEXPR
#   define ENUMS_CONSTEXPR_     constexpr
#   define ENUMS_NULLPTR        nullptr
#else
#   define ENUMS_CONSTEXPR_
#   define ENUMS_NULLPTR        NULL
#endif

#ifndef ENUMS_NO_EXCEPTIONS
#   define ENUMS_IF_EXCEPTIONS(x) x
#else
#   define ENUMS_IF_EXCEPTIONS(x)
#endif

#ifdef __GNUC__
#   define ENUMS_UNUSED(x) x __attribute__((__unused__))
#else
#   define ENUMS_UNUSED(x) x
#endif



// Higher-order preprocessor macros.

#ifdef ENUMS_MACRO_FILE
#   include ENUMS_MACRO_FILE
#else

#define ENUMS_PP_MAP(macro, data, ...) \
    ENUMS_ID( \
        ENUMS_APPLY( \
            ENUMS_PP_MAP_VAR_COUNT, \
            ENUMS_PP_COUNT(__VA_ARGS__)) \
        (macro, data, __VA_ARGS__))

#define ENUMS_PP_MAP_VAR_COUNT(count) ENUMS_M ## count

#define ENUMS_APPLY(macro, ...) ENUMS_ID(macro(__VA_ARGS__))

#define ENUMS_ID(x) x

#define ENUMS_M1(m, d, x) m(d,0,x)
#define ENUMS_M2(m,d,x,...) m(d,1,x) \
    ENUMS_ID(ENUMS_M1(m,d,__VA_ARGS__))
#define ENUMS_M3(m,d,x,...) m(d,2,x) \
    ENUMS_ID(ENUMS_M2(m,d,__VA_ARGS__))
#define ENUMS_M4(m,d,x,...) m(d,3,x) \
    ENUMS_ID(ENUMS_M3(m,d,__VA_ARGS__))
#define ENUMS_M5(m,d,x,...) m(d,4,x) \
    ENUMS_ID(ENUMS_M4(m,d,__VA_ARGS__))
#define ENUMS_M6(m,d,x,...) m(d,5,x) \
    ENUMS_ID(ENUMS_M5(m,d,__VA_ARGS__))
#define ENUMS_M7(m,d,x,...) m(d,6,x) \
    ENUMS_ID(ENUMS_M6(m,d,__VA_ARGS__))
#define ENUMS_M8(m,d,x,...) m(d,7,x) \
    ENUMS_ID(ENUMS_M7(m,d,__VA_ARGS__))
#define ENUMS_M9(m,d,x,...) m(d,8,x) \
    ENUMS_ID(ENUMS_M8(m,d,__VA_ARGS__))
#define ENUMS_M10(m,d,x,...) m(d,9,x) \
    ENUMS_ID(ENUMS_M9(m,d,__VA_ARGS__))
#define ENUMS_M11(m,d,x,...) m(d,10,x) \
    ENUMS_ID(ENUMS_M10(m,d,__VA_ARGS__))
#define ENUMS_M12(m,d,x,...) m(d,11,x) \
    ENUMS_ID(ENUMS_M11(m,d,__VA_ARGS__))
#define ENUMS_M13(m,d,x,...) m(d,12,x) \
    ENUMS_ID(ENUMS_M12(m,d,__VA_ARGS__))
#define ENUMS_M14(m,d,x,...) m(d,13,x) \
    ENUMS_ID(ENUMS_M13(m,d,__VA_ARGS__))
#define ENUMS_M15(m,d,x,...) m(d,14,x) \
    ENUMS_ID(ENUMS_M14(m,d,__VA_ARGS__))
#define ENUMS_M16(m,d,x,...) m(d,15,x) \
    ENUMS_ID(ENUMS_M15(m,d,__VA_ARGS__))
#define ENUMS_M17(m,d,x,...) m(d,16,x) \
    ENUMS_ID(ENUMS_M16(m,d,__VA_ARGS__))
#define ENUMS_M18(m,d,x,...) m(d,17,x) \
    ENUMS_ID(ENUMS_M17(m,d,__VA_ARGS__))
#define ENUMS_M19(m,d,x,...) m(d,18,x) \
    ENUMS_ID(ENUMS_M18(m,d,__VA_ARGS__))
#define ENUMS_M20(m,d,x,...) m(d,19,x) \
    ENUMS_ID(ENUMS_M19(m,d,__VA_ARGS__))
#define ENUMS_M21(m,d,x,...) m(d,20,x) \
    ENUMS_ID(ENUMS_M20(m,d,__VA_ARGS__))
#define ENUMS_M22(m,d,x,...) m(d,21,x) \
    ENUMS_ID(ENUMS_M21(m,d,__VA_ARGS__))
#define ENUMS_M23(m,d,x,...) m(d,22,x) \
    ENUMS_ID(ENUMS_M22(m,d,__VA_ARGS__))
#define ENUMS_M24(m,d,x,...) m(d,23,x) \
    ENUMS_ID(ENUMS_M23(m,d,__VA_ARGS__))
#define ENUMS_M25(m,d,x,...) m(d,24,x) \
    ENUMS_ID(ENUMS_M24(m,d,__VA_ARGS__))
#define ENUMS_M26(m,d,x,...) m(d,25,x) \
    ENUMS_ID(ENUMS_M25(m,d,__VA_ARGS__))
#define ENUMS_M27(m,d,x,...) m(d,26,x) \
    ENUMS_ID(ENUMS_M26(m,d,__VA_ARGS__))
#define ENUMS_M28(m,d,x,...) m(d,27,x) \
    ENUMS_ID(ENUMS_M27(m,d,__VA_ARGS__))
#define ENUMS_M29(m,d,x,...) m(d,28,x) \
    ENUMS_ID(ENUMS_M28(m,d,__VA_ARGS__))
#define ENUMS_M30(m,d,x,...) m(d,29,x) \
    ENUMS_ID(ENUMS_M29(m,d,__VA_ARGS__))
#define ENUMS_M31(m,d,x,...) m(d,30,x) \
    ENUMS_ID(ENUMS_M30(m,d,__VA_ARGS__))
#define ENUMS_M32(m,d,x,...) m(d,31,x) \
    ENUMS_ID(ENUMS_M31(m,d,__VA_ARGS__))
#define ENUMS_M33(m,d,x,...) m(d,32,x) \
    ENUMS_ID(ENUMS_M32(m,d,__VA_ARGS__))
#define ENUMS_M34(m,d,x,...) m(d,33,x) \
    ENUMS_ID(ENUMS_M33(m,d,__VA_ARGS__))
#define ENUMS_M35(m,d,x,...) m(d,34,x) \
    ENUMS_ID(ENUMS_M34(m,d,__VA_ARGS__))
#define ENUMS_M36(m,d,x,...) m(d,35,x) \
    ENUMS_ID(ENUMS_M35(m,d,__VA_ARGS__))
#define ENUMS_M37(m,d,x,...) m(d,36,x) \
    ENUMS_ID(ENUMS_M36(m,d,__VA_ARGS__))
#define ENUMS_M38(m,d,x,...) m(d,37,x) \
    ENUMS_ID(ENUMS_M37(m,d,__VA_ARGS__))
#define ENUMS_M39(m,d,x,...) m(d,38,x) \
    ENUMS_ID(ENUMS_M38(m,d,__VA_ARGS__))
#define ENUMS_M40(m,d,x,...) m(d,39,x) \
    ENUMS_ID(ENUMS_M39(m,d,__VA_ARGS__))
#define ENUMS_M41(m,d,x,...) m(d,40,x) \
    ENUMS_ID(ENUMS_M40(m,d,__VA_ARGS__))
#define ENUMS_M42(m,d,x,...) m(d,41,x) \
    ENUMS_ID(ENUMS_M41(m,d,__VA_ARGS__))
#define ENUMS_M43(m,d,x,...) m(d,42,x) \
    ENUMS_ID(ENUMS_M42(m,d,__VA_ARGS__))
#define ENUMS_M44(m,d,x,...) m(d,43,x) \
    ENUMS_ID(ENUMS_M43(m,d,__VA_ARGS__))
#define ENUMS_M45(m,d,x,...) m(d,44,x) \
    ENUMS_ID(ENUMS_M44(m,d,__VA_ARGS__))
#define ENUMS_M46(m,d,x,...) m(d,45,x) \
    ENUMS_ID(ENUMS_M45(m,d,__VA_ARGS__))
#define ENUMS_M47(m,d,x,...) m(d,46,x) \
    ENUMS_ID(ENUMS_M46(m,d,__VA_ARGS__))
#define ENUMS_M48(m,d,x,...) m(d,47,x) \
    ENUMS_ID(ENUMS_M47(m,d,__VA_ARGS__))
#define ENUMS_M49(m,d,x,...) m(d,48,x) \
    ENUMS_ID(ENUMS_M48(m,d,__VA_ARGS__))
#define ENUMS_M50(m,d,x,...) m(d,49,x) \
    ENUMS_ID(ENUMS_M49(m,d,__VA_ARGS__))
#define ENUMS_M51(m,d,x,...) m(d,50,x) \
    ENUMS_ID(ENUMS_M50(m,d,__VA_ARGS__))
#define ENUMS_M52(m,d,x,...) m(d,51,x) \
    ENUMS_ID(ENUMS_M51(m,d,__VA_ARGS__))
#define ENUMS_M53(m,d,x,...) m(d,52,x) \
    ENUMS_ID(ENUMS_M52(m,d,__VA_ARGS__))
#define ENUMS_M54(m,d,x,...) m(d,53,x) \
    ENUMS_ID(ENUMS_M53(m,d,__VA_ARGS__))
#define ENUMS_M55(m,d,x,...) m(d,54,x) \
    ENUMS_ID(ENUMS_M54(m,d,__VA_ARGS__))
#define ENUMS_M56(m,d,x,...) m(d,55,x) \
    ENUMS_ID(ENUMS_M55(m,d,__VA_ARGS__))
#define ENUMS_M57(m,d,x,...) m(d,56,x) \
    ENUMS_ID(ENUMS_M56(m,d,__VA_ARGS__))
#define ENUMS_M58(m,d,x,...) m(d,57,x) \
    ENUMS_ID(ENUMS_M57(m,d,__VA_ARGS__))
#define ENUMS_M59(m,d,x,...) m(d,58,x) \
    ENUMS_ID(ENUMS_M58(m,d,__VA_ARGS__))
#define ENUMS_M60(m,d,x,...) m(d,59,x) \
    ENUMS_ID(ENUMS_M59(m,d,__VA_ARGS__))
#define ENUMS_M61(m,d,x,...) m(d,60,x) \
    ENUMS_ID(ENUMS_M60(m,d,__VA_ARGS__))
#define ENUMS_M62(m,d,x,...) m(d,61,x) \
    ENUMS_ID(ENUMS_M61(m,d,__VA_ARGS__))
#define ENUMS_M63(m,d,x,...) m(d,62,x) \
    ENUMS_ID(ENUMS_M62(m,d,__VA_ARGS__))
#define ENUMS_M64(m,d,x,...) m(d,63,x) \
    ENUMS_ID(ENUMS_M63(m,d,__VA_ARGS__))

#define ENUMS_PP_COUNT_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,    \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, \
    _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, \
    _56, _57, _58, _59, _60, _61, _62, _63, _64, count, ...) count

#define ENUMS_PP_COUNT(...) \
    ENUMS_ID(ENUMS_PP_COUNT_IMPL(__VA_ARGS__, 64, 63, 62, 61, 60,\
        59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42,\
        41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24,\
        23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, \
        4, 3, 2, 1))

#define ENUMS_ITERATE(X, f, l) X(f, l, 0) X(f, l, 1) X(f, l, 2)         \
    X(f, l, 3) X(f, l, 4) X(f, l, 5) X(f, l, 6) X(f, l, 7) X(f, l, 8)          \
    X(f, l, 9) X(f, l, 10) X(f, l, 11) X(f, l, 12) X(f, l, 13) X(f, l, 14)     \
    X(f, l, 15) X(f, l, 16) X(f, l, 17) X(f, l, 18) X(f, l, 19) X(f, l, 20)    \
    X(f, l, 21) X(f, l, 22) X(f, l, 23)

#endif // #ifdef ENUMS_MACRO_FILE else case



namespace Enums {


// Optional type.

template <typename T>
ENUMS_CONSTEXPR_ inline T _default()
{
    return static_cast<typename T::_enumerated>(0);
}

template <>
ENUMS_CONSTEXPR_ inline const char* _default<const char*>()
{
    return ENUMS_NULLPTR;
}

template <>
ENUMS_CONSTEXPR_ inline std::size_t _default<std::size_t>()
{
    return 0;
}

template <typename T>
struct optional {
    ENUMS_CONSTEXPR_ optional() :
        _valid(false), _value(_default<T>()) { }

    ENUMS_CONSTEXPR_ optional(T v) : _valid(true), _value(v) { }

    ENUMS_CONSTEXPR_ const T& operator *() const { return _value; }
    ENUMS_CONSTEXPR_ const T* operator ->() const { return &_value; }

    ENUMS_CONSTEXPR_ operator bool() const { return _valid; }

    ENUMS_CONSTEXPR_ const T& value() const { return _value; }

  private:
    bool    _valid;
    T       _value;
};

template <typename CastTo, typename Element>
ENUMS_CONSTEXPR_ static optional<CastTo>
_map_index(const Element *array, optional<std::size_t> index)
{
    return index ? static_cast<CastTo>(array[*index]) : optional<CastTo>();
}

#ifdef ENUMS_VC2008_WORKAROUNDS

#define ENUMS_OR_THROW                                                  \
    if (!maybe)                                                                \
        throw std::runtime_error(message);                                     \
                                                                               \
    return *maybe;

#else

#define ENUMS_OR_THROW                                                  \
    return maybe ? *maybe : throw std::runtime_error(message);

#endif

ENUMS_IF_EXCEPTIONS(
template <typename T>
ENUMS_CONSTEXPR_ static T _or_throw(optional<T> maybe,
                                           const char *message)
{
    ENUMS_OR_THROW
}
)

template <typename T>
ENUMS_CONSTEXPR_ static T* _or_null(optional<T*> maybe)
{
    return maybe ? *maybe : ENUMS_NULLPTR;
}



// Functional sequencing. This is essentially a comma operator wrapped in a
// constexpr function. g++ 4.7 doesn't "accept" integral constants in the second
// position for the comma operator, and emits an external symbol, which then
// causes a linking error.

template <typename T, typename U>
ENUMS_CONSTEXPR_ U
continue_with(T ENUMS_UNUSED(ignored), U value) { return value; }



// Values array declaration helper.

template <typename EnumType>
struct _eat_assign {
    explicit ENUMS_CONSTEXPR_ _eat_assign(EnumType value) : _value(value)
        { }

    template <typename Any>
    ENUMS_CONSTEXPR_ const _eat_assign&
    operator =(Any ENUMS_UNUSED(dummy)) const { return *this; }

    ENUMS_CONSTEXPR_ operator EnumType () const { return _value; }

  private:
    EnumType    _value;
};



// Iterables.

template <typename Element>
struct _Iterable {
    typedef const Element*  iterator;

    ENUMS_CONSTEXPR_ iterator begin() const { return iterator(_array); }
    ENUMS_CONSTEXPR_ iterator end() const
        { return iterator(_array + _size); }
    ENUMS_CONSTEXPR_ std::size_t size() const { return _size; }
    ENUMS_CONSTEXPR_ const Element& operator [](std::size_t index) const
        { return _array[index]; }

    ENUMS_CONSTEXPR_ _Iterable(const Element *array, std::size_t s) :
        _array(array), _size(s) { }

  private:
    const Element * const   _array;
    const std::size_t       _size;
};



// String routines.

ENUMS_CONSTEXPR_ static const char       *_name_enders = "= \t\n";

ENUMS_CONSTEXPR_ inline bool _ends_name(char c, std::size_t index = 0)
{
    return
        c == _name_enders[index] ? true  :
        _name_enders[index] == '\0' ? false :
        _ends_name(c, index + 1);
}

ENUMS_CONSTEXPR_ inline bool _has_initializer(const char *s,
                                                     std::size_t index = 0)
{
    return
        s[index] == '\0' ? false :
        s[index] == '=' ? true :
        _has_initializer(s, index + 1);
}

ENUMS_CONSTEXPR_ inline std::size_t
_constant_length(const char *s, std::size_t index = 0)
{
    return _ends_name(s[index]) ? index : _constant_length(s, index + 1);
}

ENUMS_CONSTEXPR_ inline char
_select(const char *from, std::size_t from_length, std::size_t index)
{
    return index >= from_length ? '\0' : from[index];
}

ENUMS_CONSTEXPR_ inline char _to_lower_ascii(char c)
{
    return c >= 0x41 && c <= 0x5A ? static_cast<char>(c + 0x20) : c;
}

ENUMS_CONSTEXPR_ inline bool _names_match(const char *stringizedName,
                                                 const char *referenceName,
                                                 std::size_t index = 0)
{
    return
        _ends_name(stringizedName[index]) ? referenceName[index] == '\0' :
        referenceName[index] == '\0' ? false :
        stringizedName[index] != referenceName[index] ? false :
        _names_match(stringizedName, referenceName, index + 1);
}

ENUMS_CONSTEXPR_ inline bool
_names_match_nocase(const char *stringizedName, const char *referenceName,
                    std::size_t index = 0)
{
    return
        _ends_name(stringizedName[index]) ? referenceName[index] == '\0' :
        referenceName[index] == '\0' ? false :
        _to_lower_ascii(stringizedName[index]) !=
            _to_lower_ascii(referenceName[index]) ? false :
        _names_match_nocase(stringizedName, referenceName, index + 1);
}

inline void _trim_names(const char * const *raw_names,
                        const char **trimmed_names,
                        char *storage, std::size_t count)
{
    std::size_t     offset = 0;

    for (std::size_t index = 0; index < count; ++index) {
        trimmed_names[index] = storage + offset;

        std::size_t trimmed_length =
            std::strcspn(raw_names[index], _name_enders);
        storage[offset + trimmed_length] = '\0';

        std::size_t raw_length = std::strlen(raw_names[index]);
        offset += raw_length + 1;
    }
}



// Eager initialization.
template <typename Enum>
struct _initialize_at_program_start {
    _initialize_at_program_start() { Enum::initialize(); }
};

} // namespace Enums



// Array generation macros.

#define ENUMS_EAT_ASSIGN_SINGLE(EnumType, index, expression)            \
    ((::Enums::_eat_assign<EnumType>)EnumType::expression),

#define ENUMS_EAT_ASSIGN(EnumType, ...)                                 \
    ENUMS_ID(                                                           \
        ENUMS_PP_MAP(                                                   \
            ENUMS_EAT_ASSIGN_SINGLE, EnumType, __VA_ARGS__))



#ifdef ENUMS_HAVE_CONSTEXPR



#define ENUMS_SELECT_SINGLE_CHARACTER(from, from_length, index)         \
    ::Enums::_select(from, from_length, index),

#define ENUMS_SELECT_CHARACTERS(from, from_length)                      \
    ENUMS_ITERATE(                                                      \
        ENUMS_SELECT_SINGLE_CHARACTER, from, from_length)



#define ENUMS_TRIM_SINGLE_STRING(ignored, index, expression)            \
constexpr std::size_t   _length_ ## index =                                    \
    ::Enums::_constant_length(#expression);                             \
constexpr const char    _trimmed_ ## index [] =                                \
    { ENUMS_SELECT_CHARACTERS(#expression, _length_ ## index) };        \
constexpr const char    *_final_ ## index =                                    \
    ::Enums::_has_initializer(#expression) ?                            \
        _trimmed_ ## index : #expression;

#define ENUMS_TRIM_STRINGS(...)                                         \
    ENUMS_ID(                                                           \
        ENUMS_PP_MAP(                                                   \
            ENUMS_TRIM_SINGLE_STRING, ignored, __VA_ARGS__))



#define ENUMS_REFER_TO_SINGLE_STRING(ignored, index, expression)        \
    _final_ ## index,

#define ENUMS_REFER_TO_STRINGS(...)                                     \
    ENUMS_ID(                                                           \
        ENUMS_PP_MAP(                                                   \
            ENUMS_REFER_TO_SINGLE_STRING, ignored, __VA_ARGS__))



#endif // #ifdef ENUMS_HAVE_CONSTEXPR



#define ENUMS_STRINGIZE_SINGLE(ignored, index, expression)  #expression,

#define ENUMS_STRINGIZE(...)                                            \
    ENUMS_ID(                                                           \
        ENUMS_PP_MAP(                                                   \
            ENUMS_STRINGIZE_SINGLE, ignored, __VA_ARGS__))

#define ENUMS_RESERVE_STORAGE_SINGLE(ignored, index, expression)        \
    #expression ","

#define ENUMS_RESERVE_STORAGE(...)                                      \
    ENUMS_ID(                                                           \
        ENUMS_PP_MAP(                                                   \
            ENUMS_RESERVE_STORAGE_SINGLE, ignored, __VA_ARGS__))



// The enums proper.

#define ENUMS_NS(EnumType)  Enums::_data_ ## EnumType

#ifdef ENUMS_VC2008_WORKAROUNDS

#define ENUMS_COPY_CONSTRUCTOR(Enum)                                    \
        ENUMS_CONSTEXPR_ Enum(const Enum &other) :                      \
            _value(other._value) { }

#else

#define ENUMS_COPY_CONSTRUCTOR(Enum)

#endif

#define ENUMS_TYPE(SetUnderlyingType, SwitchType, GenerateSwitchType,   \
                          GenerateStrings, ToStringConstexpr,                  \
                          DeclareInitialize, DefineInitialize, CallInitialize, \
                          Enum, Underlying, ...)                               \
                                                                               \
namespace Enums {                                                       \
namespace _data_ ## Enum {                                                     \
                                                                               \
ENUMS_ID(GenerateSwitchType(Underlying, __VA_ARGS__))                   \
                                                                               \
}                                                                              \
}                                                                              \
                                                                               \
class Enum {                                                                   \
  private:                                                                     \
    typedef ::Enums::optional<Enum>                  _optional;         \
    typedef ::Enums::optional<std::size_t>           _optional_index;   \
                                                                               \
  public:                                                                      \
    typedef Underlying                                      _integral;         \
                                                                               \
    enum _enumerated SetUnderlyingType(Underlying) { __VA_ARGS__ };            \
                                                                               \
    ENUMS_CONSTEXPR_ Enum(_enumerated value) : _value(value) { }        \
                                                                               \
    ENUMS_COPY_CONSTRUCTOR(Enum)                                        \
                                                                               \
    ENUMS_CONSTEXPR_ operator SwitchType(Enum)() const                  \
    {                                                                          \
        return SwitchType(Enum)(_value);                                       \
    }                                                                          \
                                                                               \
    ENUMS_CONSTEXPR_ _integral _to_integral() const;                    \
    ENUMS_IF_EXCEPTIONS(                                                \
    ENUMS_CONSTEXPR_ static Enum _from_integral(_integral value);       \
    )                                                                          \
    ENUMS_CONSTEXPR_ static Enum                                        \
    _from_integral_unchecked(_integral value);                                 \
    ENUMS_CONSTEXPR_ static _optional                                   \
    _from_integral_nothrow(_integral value);                                   \
                                                                               \
    ToStringConstexpr const char* _to_string() const;                          \
    ENUMS_IF_EXCEPTIONS(                                                \
    ENUMS_CONSTEXPR_ static Enum _from_string(const char *name);        \
    )                                                                          \
    ENUMS_CONSTEXPR_ static _optional                                   \
    _from_string_nothrow(const char *name);                                    \
                                                                               \
    ENUMS_IF_EXCEPTIONS(                                                \
    ENUMS_CONSTEXPR_ static Enum _from_string_nocase(const char *name); \
    )                                                                          \
    ENUMS_CONSTEXPR_ static _optional                                   \
    _from_string_nocase_nothrow(const char *name);                             \
                                                                               \
    ENUMS_CONSTEXPR_ static bool _is_valid(_integral value);            \
    ENUMS_CONSTEXPR_ static bool _is_valid(const char *name);           \
    ENUMS_CONSTEXPR_ static bool _is_valid_nocase(const char *name);    \
                                                                               \
    typedef ::Enums::_Iterable<Enum>             _value_iterable;       \
    typedef ::Enums::_Iterable<const char*>      _name_iterable;        \
                                                                               \
    typedef _value_iterable::iterator                   _value_iterator;       \
    typedef _name_iterable::iterator                    _name_iterator;        \
                                                                               \
    ENUMS_CONSTEXPR_ static const std::size_t _size_constant =          \
        ENUMS_ID(ENUMS_PP_COUNT(__VA_ARGS__));                   \
    ENUMS_CONSTEXPR_ static std::size_t _size()                         \
        { return _size_constant; }                                             \
                                                                               \
    ENUMS_CONSTEXPR_ static const char* _name();                        \
    ENUMS_CONSTEXPR_ static _value_iterable _values();                  \
    ToStringConstexpr static _name_iterable _names();                          \
                                                                               \
    _integral      _value;                                                     \
                                                                               \
    ENUMS_DEFAULT_CONSTRUCTOR(Enum)                                     \
                                                                               \
  private:                                                                     \
    explicit ENUMS_CONSTEXPR_ Enum(const _integral &value) :            \
        _value(value) { }                                                      \
                                                                               \
    DeclareInitialize                                                          \
                                                                               \
    ENUMS_CONSTEXPR_ static _optional_index                             \
    _from_value_loop(_integral value, std::size_t index = 0);                  \
    ENUMS_CONSTEXPR_ static _optional_index                             \
    _from_string_loop(const char *name, std::size_t index = 0);                \
    ENUMS_CONSTEXPR_ static _optional_index                             \
    _from_string_nocase_loop(const char *name, std::size_t index = 0);         \
                                                                               \
    friend struct ::Enums::_initialize_at_program_start<Enum>;          \
};                                                                             \
                                                                               \
namespace Enums {                                                       \
namespace _data_ ## Enum {                                                     \
                                                                               \
static ::Enums::_initialize_at_program_start<Enum>                      \
                                                _force_initialization;         \
                                                                               \
enum _PutNamesInThisScopeAlso { __VA_ARGS__ };                                 \
                                                                               \
ENUMS_CONSTEXPR_ const Enum      _value_array[] =                       \
    { ENUMS_ID(ENUMS_EAT_ASSIGN(Enum, __VA_ARGS__)) };           \
                                                                               \
ENUMS_ID(GenerateStrings(Enum, __VA_ARGS__))                            \
                                                                               \
}                                                                              \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline const Enum                                      \
operator +(Enum::_enumerated enumerated)                                       \
{                                                                              \
    return static_cast<Enum>(enumerated);                                      \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline Enum::_optional_index                           \
Enum::_from_value_loop(Enum::_integral value, std::size_t index)               \
{                                                                              \
    return                                                                     \
        index == _size() ?                                                     \
            _optional_index() :                                                \
            ENUMS_NS(Enum)::_value_array[index]._value == value ?       \
                _optional_index(index) :                                       \
                _from_value_loop(value, index + 1);                            \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline Enum::_optional_index                           \
Enum::_from_string_loop(const char *name, std::size_t index)                   \
{                                                                              \
    return                                                                     \
        index == _size() ? _optional_index() :                                 \
        ::Enums::_names_match(                                          \
            ENUMS_NS(Enum)::_raw_names()[index], name) ?                \
            _optional_index(index) :                                           \
            _from_string_loop(name, index + 1);                                \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline Enum::_optional_index                           \
Enum::_from_string_nocase_loop(const char *name, std::size_t index)            \
{                                                                              \
    return                                                                     \
        index == _size() ? _optional_index() :                                 \
            ::Enums::_names_match_nocase(                               \
                ENUMS_NS(Enum)::_raw_names()[index], name) ?            \
                    _optional_index(index) :                                   \
                    _from_string_nocase_loop(name, index + 1);                 \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline Enum::_integral Enum::_to_integral() const      \
{                                                                              \
    return _integral(_value);                                                  \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline Enum                                            \
Enum::_from_integral_unchecked(_integral value)                                \
{                                                                              \
    return static_cast<_enumerated>(value);                                    \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline Enum::_optional                                 \
Enum::_from_integral_nothrow(_integral value)                                  \
{                                                                              \
    return                                                                     \
        ::Enums::_map_index<Enum>(ENUMS_NS(Enum)::_value_array,  \
                                         _from_value_loop(value));             \
}                                                                              \
                                                                               \
ENUMS_IF_EXCEPTIONS(                                                    \
ENUMS_CONSTEXPR_ inline Enum Enum::_from_integral(_integral value)      \
{                                                                              \
    return                                                                     \
        ::Enums::_or_throw(_from_integral_nothrow(value),               \
                                  #Enum "::_from_integral: invalid argument"); \
}                                                                              \
)                                                                              \
                                                                               \
ToStringConstexpr inline const char* Enum::_to_string() const                  \
{                                                                              \
    return                                                                     \
        ::Enums::_or_null(                                              \
            ::Enums::_map_index<const char*>(                           \
                ENUMS_NS(Enum)::_name_array(),                          \
                _from_value_loop(CallInitialize(_value))));                    \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline Enum::_optional                                 \
Enum::_from_string_nothrow(const char *name)                                   \
{                                                                              \
    return                                                                     \
        ::Enums::_map_index<Enum>(                                      \
            ENUMS_NS(Enum)::_value_array, _from_string_loop(name));     \
}                                                                              \
                                                                               \
ENUMS_IF_EXCEPTIONS(                                                    \
ENUMS_CONSTEXPR_ inline Enum Enum::_from_string(const char *name)       \
{                                                                              \
    return                                                                     \
        ::Enums::_or_throw(_from_string_nothrow(name),                  \
                                  #Enum "::_from_string: invalid argument");   \
}                                                                              \
)                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline Enum::_optional                                 \
Enum::_from_string_nocase_nothrow(const char *name)                            \
{                                                                              \
    return                                                                     \
        ::Enums::_map_index<Enum>(ENUMS_NS(Enum)::_value_array,  \
                                         _from_string_nocase_loop(name));      \
}                                                                              \
                                                                               \
ENUMS_IF_EXCEPTIONS(                                                    \
ENUMS_CONSTEXPR_ inline Enum Enum::_from_string_nocase(const char *name)\
{                                                                              \
    return                                                                     \
        ::Enums::_or_throw(                                             \
            _from_string_nocase_nothrow(name),                                 \
            #Enum "::_from_string_nocase: invalid argument");                  \
}                                                                              \
)                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline bool Enum::_is_valid(_integral value)           \
{                                                                              \
    return _from_value_loop(value);                                            \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline bool Enum::_is_valid(const char *name)          \
{                                                                              \
    return _from_string_loop(name);                                            \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline bool Enum::_is_valid_nocase(const char *name)   \
{                                                                              \
    return _from_string_nocase_loop(name);                                     \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline const char* Enum::_name()                       \
{                                                                              \
    return #Enum;                                                              \
}                                                                              \
                                                                               \
ENUMS_CONSTEXPR_ inline Enum::_value_iterable Enum::_values()           \
{                                                                              \
    return _value_iterable(ENUMS_NS(Enum)::_value_array, _size());      \
}                                                                              \
                                                                               \
ToStringConstexpr inline Enum::_name_iterable Enum::_names()                   \
{                                                                              \
    return                                                                     \
        _name_iterable(ENUMS_NS(Enum)::_name_array(),                   \
                       CallInitialize(_size()));                               \
}                                                                              \
                                                                               \
DefineInitialize(Enum)                                                         \
                                                                               \
ENUMS_CONSTEXPR_ inline bool operator ==(const Enum &a, const Enum &b)  \
    { return a._to_integral() == b._to_integral(); }                           \
ENUMS_CONSTEXPR_ inline bool operator !=(const Enum &a, const Enum &b)  \
    { return a._to_integral() != b._to_integral(); }                           \
ENUMS_CONSTEXPR_ inline bool operator <(const Enum &a, const Enum &b)   \
    { return a._to_integral() < b._to_integral(); }                            \
ENUMS_CONSTEXPR_ inline bool operator <=(const Enum &a, const Enum &b)  \
    { return a._to_integral() <= b._to_integral(); }                           \
ENUMS_CONSTEXPR_ inline bool operator >(const Enum &a, const Enum &b)   \
    { return a._to_integral() > b._to_integral(); }                            \
ENUMS_CONSTEXPR_ inline bool operator >=(const Enum &a, const Enum &b)  \
    { return a._to_integral() >= b._to_integral(); }



// Enum feature options.

// C++98, C++11
#define ENUMS_CXX98_UNDERLYING_TYPE(Underlying)

// C++11
#define ENUMS_CXX11_UNDERLYING_TYPE(Underlying)                         \
    : Underlying

// C++98, C++11
#define ENUMS_REGULAR_ENUM_SWITCH_TYPE(Type)                            \
    _enumerated

// C++11
#define ENUMS_ENUM_CLASS_SWITCH_TYPE(Type)                              \
    ENUMS_NS(Type)::_EnumClassForSwitchStatements

// C++98, C++11
#define ENUMS_REGULAR_ENUM_SWITCH_TYPE_GENERATE(Underlying, ...)

// C++11
#define ENUMS_ENUM_CLASS_SWITCH_TYPE_GENERATE(Underlying, ...)          \
    enum class _EnumClassForSwitchStatements : Underlying { __VA_ARGS__ };

// C++98
#define ENUMS_CXX98_TRIM_STRINGS_ARRAYS(Enum, ...)                      \
    inline const char** _raw_names()                                           \
    {                                                                          \
        static const char   *value[] =                                         \
            { ENUMS_ID(ENUMS_STRINGIZE(__VA_ARGS__)) };          \
        return value;                                                          \
    }                                                                          \
                                                                               \
    inline char* _name_storage()                                               \
    {                                                                          \
        static char         storage[] =                                        \
            ENUMS_ID(ENUMS_RESERVE_STORAGE(__VA_ARGS__));        \
        return storage;                                                        \
    }                                                                          \
                                                                               \
    inline const char** _name_array()                                          \
    {                                                                          \
        static const char   *value[Enum::_size_constant];                      \
        return value;                                                          \
    }                                                                          \
                                                                               \
    inline bool& _initialized()                                                \
    {                                                                          \
        static bool         value = false;                                     \
        return value;                                                          \
    }

// C++11 fast version
#define ENUMS_CXX11_PARTIAL_CONSTEXPR_TRIM_STRINGS_ARRAYS(Enum, ...)    \
    constexpr const char    *_the_raw_names[] =                                \
        { ENUMS_ID(ENUMS_STRINGIZE(__VA_ARGS__)) };              \
                                                                               \
    constexpr const char * const * _raw_names()                                \
    {                                                                          \
        return _the_raw_names;                                                 \
    }                                                                          \
                                                                               \
    inline char* _name_storage()                                               \
    {                                                                          \
        static char         storage[] =                                        \
            ENUMS_ID(ENUMS_RESERVE_STORAGE(__VA_ARGS__));        \
        return storage;                                                        \
    }                                                                          \
                                                                               \
    inline const char** _name_array()                                          \
    {                                                                          \
        static const char   *value[Enum::_size_constant];                      \
        return value;                                                          \
    }                                                                          \
                                                                               \
    inline bool& _initialized()                                                \
    {                                                                          \
        static bool         value = false;                                     \
        return value;                                                          \
    }

// C++11 slow all-constexpr version
#define ENUMS_CXX11_FULL_CONSTEXPR_TRIM_STRINGS_ARRAYS(Enum, ...)       \
    ENUMS_ID(ENUMS_TRIM_STRINGS(__VA_ARGS__))                    \
                                                                               \
    constexpr const char * const    _the_name_array[] =                        \
        { ENUMS_ID(ENUMS_REFER_TO_STRINGS(__VA_ARGS__)) };       \
                                                                               \
    constexpr const char * const * _name_array()                               \
    {                                                                          \
        return _the_name_array;                                                \
    }                                                                          \
                                                                               \
    constexpr const char * const * _raw_names()                                \
    {                                                                          \
        return _the_name_array;                                                \
    }

// C++98, C++11 fast version
#define ENUMS_NO_CONSTEXPR_TO_STRING_KEYWORD

// C++11 slow all-constexpr version
#define ENUMS_CONSTEXPR_TO_STRING_KEYWORD                               \
    constexpr

// C++98, C++11 fast version
#define ENUMS_DO_DECLARE_INITIALIZE                                     \
    static int initialize();

// C++11 slow all-constexpr version
#define ENUMS_DECLARE_EMPTY_INITIALIZE                                  \
    static int initialize() { return 0; }

// C++98, C++11 fast version
#define ENUMS_DO_DEFINE_INITIALIZE(Enum)                                \
    inline int Enum::initialize()                                              \
    {                                                                          \
        if (ENUMS_NS(Enum)::_initialized())                             \
            return 0;                                                          \
                                                                               \
        ::Enums::_trim_names(ENUMS_NS(Enum)::_raw_names(),       \
                                    ENUMS_NS(Enum)::_name_array(),      \
                                    ENUMS_NS(Enum)::_name_storage(),    \
                                    _size());                                  \
                                                                               \
        ENUMS_NS(Enum)::_initialized() = true;                          \
                                                                               \
        return 0;                                                              \
    }

// C++11 slow all-constexpr version
#define ENUMS_DO_NOT_DEFINE_INITIALIZE(Enum)

// C++98, C++11 fast version
#define ENUMS_DO_CALL_INITIALIZE(value)                                 \
    ::Enums::continue_with(initialize(), value)

// C++11 slow all-constexpr version
#define ENUMS_DO_NOT_CALL_INITIALIZE(value)                             \
    value



// User feature selection.

#ifdef ENUMS_STRICT_CONVERSION
#   define ENUMS_DEFAULT_SWITCH_TYPE                                    \
        ENUMS_ENUM_CLASS_SWITCH_TYPE
#   define ENUMS_DEFAULT_SWITCH_TYPE_GENERATE                           \
        ENUMS_ENUM_CLASS_SWITCH_TYPE_GENERATE
#else
#   define ENUMS_DEFAULT_SWITCH_TYPE                                    \
        ENUMS_REGULAR_ENUM_SWITCH_TYPE
#   define ENUMS_DEFAULT_SWITCH_TYPE_GENERATE                           \
        ENUMS_REGULAR_ENUM_SWITCH_TYPE_GENERATE
#endif



#ifndef ENUMS_DEFAULT_CONSTRUCTOR
#   define ENUMS_DEFAULT_CONSTRUCTOR(Enum)                              \
      private:                                                                 \
        Enum() : _value(0) { }
#endif



#ifdef ENUMS_HAVE_CONSTEXPR

#ifdef ENUMS_CONSTEXPR_TO_STRING
#   define ENUMS_DEFAULT_TRIM_STRINGS_ARRAYS                            \
        ENUMS_CXX11_FULL_CONSTEXPR_TRIM_STRINGS_ARRAYS
#   define ENUMS_DEFAULT_TO_STRING_KEYWORD                              \
        ENUMS_CONSTEXPR_TO_STRING_KEYWORD
#   define ENUMS_DEFAULT_DECLARE_INITIALIZE                             \
        ENUMS_DECLARE_EMPTY_INITIALIZE
#   define ENUMS_DEFAULT_DEFINE_INITIALIZE                              \
        ENUMS_DO_NOT_DEFINE_INITIALIZE
#   define ENUMS_DEFAULT_CALL_INITIALIZE                                \
        ENUMS_DO_NOT_CALL_INITIALIZE
#else
#   define ENUMS_DEFAULT_TRIM_STRINGS_ARRAYS                            \
        ENUMS_CXX11_PARTIAL_CONSTEXPR_TRIM_STRINGS_ARRAYS
#   define ENUMS_DEFAULT_TO_STRING_KEYWORD                              \
        ENUMS_NO_CONSTEXPR_TO_STRING_KEYWORD
#   define ENUMS_DEFAULT_DECLARE_INITIALIZE                             \
        ENUMS_DO_DECLARE_INITIALIZE
#   define ENUMS_DEFAULT_DEFINE_INITIALIZE                              \
        ENUMS_DO_DEFINE_INITIALIZE
#   define ENUMS_DEFAULT_CALL_INITIALIZE                                \
        ENUMS_DO_CALL_INITIALIZE
#endif



// Top-level macros.

#define ENUM(Enum, Underlying, ...)                                     \
    ENUMS_ID(ENUMS_TYPE(                                         \
        ENUMS_CXX11_UNDERLYING_TYPE,                                    \
        ENUMS_DEFAULT_SWITCH_TYPE,                                      \
        ENUMS_DEFAULT_SWITCH_TYPE_GENERATE,                             \
        ENUMS_DEFAULT_TRIM_STRINGS_ARRAYS,                              \
        ENUMS_DEFAULT_TO_STRING_KEYWORD,                                \
        ENUMS_DEFAULT_DECLARE_INITIALIZE,                               \
        ENUMS_DEFAULT_DEFINE_INITIALIZE,                                \
        ENUMS_DEFAULT_CALL_INITIALIZE,                                  \
        Enum, Underlying, __VA_ARGS__))

#define SLOW_ENUM(Enum, Underlying, ...)                                       \
    ENUMS_ID(ENUMS_TYPE(                                         \
        ENUMS_CXX11_UNDERLYING_TYPE,                                    \
        ENUMS_DEFAULT_SWITCH_TYPE,                                      \
        ENUMS_DEFAULT_SWITCH_TYPE_GENERATE,                             \
        ENUMS_CXX11_FULL_CONSTEXPR_TRIM_STRINGS_ARRAYS,                 \
        ENUMS_CONSTEXPR_TO_STRING_KEYWORD,                              \
        ENUMS_DECLARE_EMPTY_INITIALIZE,                                 \
        ENUMS_DO_NOT_DEFINE_INITIALIZE,                                 \
        ENUMS_DO_NOT_CALL_INITIALIZE,                                   \
        Enum, Underlying, __VA_ARGS__))

#else

#define ENUM(Enum, Underlying, ...)                                     \
    ENUMS_ID(ENUMS_TYPE(                                         \
        ENUMS_CXX98_UNDERLYING_TYPE,                                    \
        ENUMS_DEFAULT_SWITCH_TYPE,                                      \
        ENUMS_DEFAULT_SWITCH_TYPE_GENERATE,                             \
        ENUMS_CXX98_TRIM_STRINGS_ARRAYS,                                \
        ENUMS_NO_CONSTEXPR_TO_STRING_KEYWORD,                           \
        ENUMS_DO_DECLARE_INITIALIZE,                                    \
        ENUMS_DO_DEFINE_INITIALIZE,                                     \
        ENUMS_DO_CALL_INITIALIZE,                                       \
        Enum, Underlying, __VA_ARGS__))

#endif



namespace Enums {

// Maps.

template <typename T>
struct map_compare {
    ENUMS_CONSTEXPR_ static bool less(const T& a, const T& b)
        { return a < b; }
};

template <>
struct map_compare<const char*> {
    ENUMS_CONSTEXPR_ static bool less(const char *a, const char *b)
        { return less_loop(a, b); }

  private:
    ENUMS_CONSTEXPR_ static bool
    less_loop(const char *a, const char *b, size_t index = 0)
    {
        return
            a[index] != b[index] ? a[index] < b[index] :
            a[index] == '\0' ? false :
            less_loop(a, b, index + 1);
    }
};

template <typename Enum, typename T, typename Compare = map_compare<T> >
struct map {
    typedef T (*function)(Enum);

    ENUMS_CONSTEXPR_ explicit map(function f) : _f(f) { }

    ENUMS_CONSTEXPR_ T from_enum(Enum value) const { return _f(value); }
    ENUMS_CONSTEXPR_ T operator [](Enum value) const
        { return _f(value); }

    ENUMS_CONSTEXPR_ Enum to_enum(T value) const
    {
        return
            _or_throw(to_enum_nothrow(value), "map::to_enum: invalid argument");
    }

    ENUMS_CONSTEXPR_ optional<Enum>
    to_enum_nothrow(T value, size_t index = 0) const
    {
        return
            index >= Enum::_size() ? optional<Enum>() :
            Compare::less(_f(Enum::_values()[index]), value) ||
            Compare::less(value, _f(Enum::_values()[index])) ?
                to_enum_nothrow(value, index + 1) :
            Enum::_values()[index];
    }

  private:
    const function      _f;
};

template <typename Enum, typename T>
ENUMS_CONSTEXPR_ map<Enum, T> make_map(T (*f)(Enum))
{
    return map<Enum, T>(f);
}



// Stream I/O operators.

// This template is used as a sort of enable_if for SFINAE. It should be
// possible to use std::enable_if, however <type_traits> is not available in
// C++98. Non-char streams are currently not supported.
template <typename T, typename Enum>
struct only_if_enum { typedef T type; };

}

template <typename Char, typename Traits, typename Enum>
inline typename Enums::only_if_enum<std::basic_ostream<Char, Traits>,
                                           typename Enum::_enumerated>::type&
operator <<(std::basic_ostream<Char, Traits>& stream, const Enum &value)
{
    return stream << value._to_string();
}

template <typename Char, typename Traits, class Enum>
inline typename Enums::only_if_enum<std::basic_istream<Char, Traits>,
                                           typename Enum::_enumerated>::type&
operator >>(std::basic_istream<Char, Traits>& stream, Enum &value)
{
    std::basic_string<Char, Traits>     buffer;

    stream >> buffer;
    Enums::optional<Enum>        converted =
        Enum::_from_string_nothrow(buffer.c_str());

    if (converted)
        value = *converted;
    else
        stream.setstate(std::basic_istream<Char, Traits>::failbit);

    return stream;
}



#endif // #ifndef ENUMS_ENUM_H
