#pragma once
#if defined(_WIN32)
#pragma warning(disable : 4146)  // unary minus applied to unsigned type, result still unsigned
#pragma warning(disable : 4244)  //
#endif

#include <limits>
#include "Types.h"

#if defined(_WIN32) && defined(min)
#undef min
#endif

template <typename T>
struct is_numeric : std::integral_constant<bool, std::is_arithmetic<T>::value && !std::is_same<T, bool>::value>
{
};

template <typename T>
eastl::string ToString(T value)
{
    return eastl::string(std::to_string(value).c_str());
}

template <typename T>
eastl::wstring ToWString(T value)
{
    return eastl::wstring(std::to_wstring(value).c_str());
}

template <typename Derived>
class arithmetic_operators
{
    friend const Derived operator+(Derived lhs, const Derived& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    friend const Derived operator-(Derived lhs, const Derived& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    friend const Derived operator*(Derived lhs, const Derived& rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    friend const Derived operator/(Derived lhs, const Derived& rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    friend const Derived operator%(Derived lhs, const Derived& rhs)
    {
        lhs %= rhs;
        return lhs;
    }
};

template <typename Derived>
class bitwise_operators
{
    friend const Derived operator<<(Derived lhs, const Derived& rhs)
    {
        lhs <<= rhs;
        return lhs;
    }

    friend const Derived operator>>(Derived lhs, const Derived& rhs)
    {
        lhs >>= rhs;
        return lhs;
    }

    friend const Derived operator&(Derived lhs, const Derived& rhs)
    {
        lhs &= rhs;
        return lhs;
    }

    friend const Derived operator|(Derived lhs, const Derived& rhs)
    {
        lhs |= rhs;
        return lhs;
    }

    friend const Derived operator^(Derived lhs, const Derived& rhs)
    {
        lhs ^= rhs;
        return lhs;
    }
};

template <typename Derived>
class logical_operators
{
    friend bool operator!=(const Derived& lhs, const Derived& rhs) { return !(lhs == rhs); }

    friend bool operator>(const Derived& lhs, const Derived& rhs) { return rhs < lhs; }

    friend bool operator<=(const Derived& lhs, const Derived& rhs) { return !(lhs > rhs); }

    friend bool operator>=(const Derived& lhs, const Derived& rhs) { return !(lhs < rhs); }
};

template <typename Derived>
class increment_operators
{
    // Post-increment operator
    // note: this needs work, doesn't compile
    Derived operator++(int)
    {
        Derived temp(*this);
        ++(*this);
        return temp;
    }
};

template <typename T>
class ObjType : bitwise_operators<ObjType<T> >, logical_operators<ObjType<T> >, arithmetic_operators<ObjType<T> >
{
    static_assert(is_numeric<T>::value, "T must be integer or floating type");

public:
    using value_type = T;

    ObjType() = default;
    ~ObjType() = default;

    // Copy constructors
    ObjType(const ObjType&) = default;
    ObjType(T src) { m_val = src; }

    // Copy assignment
    ObjType& operator=(const ObjType&) = default;
    ObjType& operator=(T rhs)
    {
        m_val = rhs;
        return *this;
    }

    explicit operator bool() const { return m_val != 0; }

    // Pre-increment operator
    ObjType& operator++()
    {
        ++m_val;
        return *this;
    }

    // Post-increment operator
    ObjType operator++(int)
    {
        ObjType temp(*this);
        ++(*this);
        return temp;
    }

    // Pre-decrement operator
    ObjType& operator--()
    {
        --m_val;
        return *this;
    }

    // Post-decrement operator
    ObjType operator--(int)
    {
        ObjType temp(*this);
        --(*this);
        return temp;
    }

    // Unary minus, plus
    ObjType operator-() const { return -m_val; }
    ObjType operator+() const { return m_val; }

    // Binary arithmetic operators
    //	+= -= *= /= %= ^= &= |= <<= >>= << >>
    //
    // note: the following do not apply to float or double
    //  %=

    ObjType& operator+=(const ObjType& rhs)
    {
        m_val += rhs.m_val;
        return *this;
    }

    ObjType& operator-=(const ObjType& rhs)
    {
        m_val -= rhs.m_val;
        return *this;
    }

    ObjType& operator*=(const ObjType& rhs)
    {
        m_val *= rhs.m_val;
        return *this;
    }

    ObjType& operator/=(const ObjType& rhs)
    {
        m_val /= rhs.m_val;
        return *this;
    }

    ObjType& operator%=(const ObjType& rhs)
    {
        m_val %= rhs.m_val;
        return *this;
    }

    // Bitwise assignment operators

    ObjType& operator^=(const ObjType& rhs)
    {
        m_val ^= rhs.m_val;
        return *this;
    }

    ObjType& operator&=(const ObjType& rhs)
    {
        m_val &= rhs.m_val;
        return *this;
    }

    ObjType& operator|=(const ObjType& rhs)
    {
        m_val |= rhs.m_val;
        return *this;
    }

    ObjType& operator<<=(const ObjType& rhs)
    {
        m_val <<= rhs.m_val;
        return *this;
    }

    ObjType& operator>>=(const ObjType& rhs)
    {
        m_val >>= rhs.m_val;
        return *this;
    }

    const ObjType operator~() { return ~m_val; }

    // Logical operators
    //	== <

    friend bool operator==(const ObjType& lhs, const ObjType& rhs) { return lhs.m_val == rhs.m_val; }

    friend bool operator<(const ObjType& lhs, const ObjType& rhs) { return lhs.m_val < rhs.m_val; }

    std::string ToString() { return std::to_string(m_val); }
    std::wstring ToWString() { return std::to_wstring(m_val); }

    T Min() { return std::numeric_limits<T>::min(); }
    T Max() { return std::numeric_limits<T>::max(); }

private:
    T m_val;
};

using Int8 = ObjType<Aristocrat::GDK::int8>;
using UInt8 = ObjType<Aristocrat::GDK::uint8>;
using Int16 = ObjType<Aristocrat::GDK::int16>;
using UInt16 = ObjType<Aristocrat::GDK::uint16>;
using Int32 = ObjType<Aristocrat::GDK::int32>;
using UInt32 = ObjType<Aristocrat::GDK::uint32>;
using Int64 = ObjType<Aristocrat::GDK::uint64>;
using UInt64 = ObjType<Aristocrat::GDK::uint64>;
using Float32 = ObjType<Aristocrat::GDK::float32>;
using Float64 = ObjType<Aristocrat::GDK::float64>;

template <typename T>
void BaseTests()
{
    T val1;
    val1 = 1;
    T val2(12);
    T val3 = 3;

    ++val1;
    --val1;
    val2++;
    val2--;
    val3 = ++val2;
    val3 = --val2;
    val3 = val2++;
    val3 = val2--;

    val1 += val2;
    val1 += 5;
    val3 = val1 + val2;
    val3 = val1 + 5;
    val3 = 5 + val1;

    val1 -= val2;
    val1 -= 5;
    val3 = val1 - val2;
    val3 = val1 - 5;
    val3 = 5 - val1;

    val1 *= val2;
    val1 *= 5;
    val3 = val1 * val2;
    val3 = val1 * 5;
    val3 = 5 * val1;

    val1 /= val2;
    val1 /= 5;
    val3 = val1 / val2;
    val3 = val1 / 5;
    val3 = 5 / val1;

    if (val1 == val2)
    {
        printf("hello");
    }

    if (val1 != val2)
    {
        printf("hello");
    }

    if (val1 <= val2)
    {
        printf("hello");
    }

    if (val2 >= val1)
    {
        printf("hello");
    }

    if (val1 < 5)
    {
        printf("hello");
    }

    if (5 > val1)
    {
        printf("hello");
    }

    if (val1 < val2)
    {
        printf("hello");
    }

    if (val3 > val1)
    {
        printf("hello");
    }

    val1 = val2 / val3;
    val1 = val2 / 2;
    val1 = 60 / val2;

    std::string s = val1.ToString();

    val1 = val2.Min();
    val1 = val2.Max();

    val1 = -val2;
    val1 = +val2;

    typename T::value_type valX = 2;

    val1 = val2 + valX;
}

template <typename T>
void IntegerTests()
{
    T val1 = 1;
    T val2(12);
    T val3 = 3;
    T val4 = 4;
    T val5 = 5;

    val1 = val2 % val3;
    val1 = val2 % 2;
    val1 = 60 % val2;

    val1 %= val2;
    val1 %= 5;

    val2 &= val3 |= val1;

    val1 <<= 1;
    val2 <<= val3;
    val2 <<= val3 <<= val1;

    val1 >>= 2;
    val2 >>= val3;
    val2 >>= val3 >>= val1;

    val1 = val2 << val3;
    val1 = val2 >> val3;

    val2 = 2;
    val1 = val2 << 4;

    val2 = 16;
    val1 = val2 >> 2;

    val2 = 2;
    val1 = 2 << val2;

    val2 = 3;
    val1 = 80 >> val2;

    float f = 5.2f;
    val1 = val2 + f;

    val1 = val2 & val3;
    val1 = val2 | val3 | val4 | val5;
    val1 = val2 ^ val3;
    val1 = ~val2;

    if (val1 && val2)
    {
        printf("hi");
    }
}

inline void ObjTypeTests()
{
    int x1 = 1;
    int x2 = 2;
    int x3 = 0;
    unsigned x4 = 4;

    float f = 5.2f;
    x1 = x2 + f;

    x3 = -x1;
    x3 = -x4;

    x2 &= x3 |= x1;

    x2 <<= x3 >>= x1;

    x2 = 2;
    x1 = x2 << 4;

    x2 = 3;
    x1 = 80 >> x2;

    if (x1)
    {
        printf("hi");
    }

    x1 = ~x2;

    BaseTests<Int8>();
    BaseTests<UInt8>();
    BaseTests<Int16>();
    BaseTests<UInt16>();
    BaseTests<Int32>();
    BaseTests<UInt32>();
    BaseTests<Int64>();
    BaseTests<UInt64>();

    BaseTests<Float32>();
    BaseTests<Float64>();

    IntegerTests<Int8>();
    IntegerTests<UInt8>();
    IntegerTests<Int16>();
    IntegerTests<UInt16>();
    IntegerTests<Int32>();
    IntegerTests<UInt32>();
    IntegerTests<Int64>();
    IntegerTests<UInt64>();
}
