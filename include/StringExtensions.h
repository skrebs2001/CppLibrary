#pragma once
#include <assert.h>
#include <eastl/string.h>

template <typename T1, typename T2>
int MixedTypeCompare(const T1* ptr1, const T2* ptr2, size_t nSize)
{
    for (; nSize > 0; ++ptr1, ++ptr2, --nSize)
    {
        if (static_cast<unsigned char>(*ptr1) < static_cast<unsigned char>(*ptr2))
        {
            return -1;
        }

        if (static_cast<unsigned char>(*ptr1) > static_cast<unsigned char>(*ptr2))
        {
            return 1;
        }
    }

    return 0;
}

// generic comparison of string, -1 if lhs less than rhs, 0 if equal, 1 if greater
template <class LHS, class RHS>
int Compare(const LHS& lhs, const RHS& rhs)
{
    size_t nLHSize = lhs.size();
    size_t nRHSize = rhs.size();
    size_t min = eastl::min_alt(nLHSize, nRHSize);
    int result = MixedTypeCompare(lhs.data(), rhs.data(), min);

    return ((result == 0) ? ((nLHSize < nRHSize) ? -1 : (nLHSize > nRHSize) ? 1 : 0) : result);
}

// generic comparison of string equality
template <class LHS, class RHS>
bool IsEqual(const LHS& lhs, const RHS& rhs)
{
    return (lhs.size() == rhs.size()) && (MixedTypeCompare(lhs.data(), rhs.data(), lhs.size()) == 0);
}

// #1 std::string to std::string (library)

// #2 std::string == std::wstring
inline bool operator==(const std::string& lhs, const std::wstring& rhs)
{
    return IsEqual(lhs, rhs);
}

// #2 std::string != std::wstring
inline bool operator!=(const std::string& lhs, const std::wstring& rhs)
{
    return !(lhs == rhs);
}

// #2 std::string < std::wstring
inline bool operator<(const std::string& lhs, const std::wstring& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #2 std::string > std::wstring
inline bool operator>(const std::string& lhs, const std::wstring& rhs)
{
    return (Compare(lhs, rhs) > 0);
}

// #2 std::string <= std::wstring
inline bool operator<=(const std::string& lhs, const std::wstring& rhs)
{
    bool operator<(const std::wstring&, const std::string&);
    return !(rhs < lhs);
}

// #2 std::string >= std::wstring
inline bool operator>=(const std::string& lhs, const std::wstring& rhs)
{
    return !(lhs < rhs);
}

// #3 std::string == eastl::string
inline bool operator==(const std::string& lhs, const eastl::string& rhs)
{
    return IsEqual(lhs, rhs);
}

// #3 std::string != eastl::string
inline bool operator!=(const std::string& lhs, const eastl::string& rhs)
{
    return !(lhs == rhs);
}

// #3 std::string < eastl::string
inline bool operator<(const std::string& lhs, const eastl::string& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #3 std::string > eastl::string
inline bool operator>(const std::string& lhs, const eastl::string& rhs)
{
    return (Compare(lhs, rhs) > 0);
}

// #3 std::string <= eastl::string
inline bool operator<=(const std::string& lhs, const eastl::string& rhs)
{
    bool operator<(const eastl::string&, const std::string&);
    return !(rhs < lhs);
}

// #3 std::string >= eastl::string
inline bool operator>=(const std::string& lhs, const eastl::string& rhs)
{
    return !(lhs < rhs);
}

// #4 std::string == eastl::wstring
inline bool operator==(const std::string& lhs, const eastl::wstring& rhs)
{
    return IsEqual(lhs, rhs);
}

// #4 std::string != eastl::wstring
inline bool operator!=(const std::string& lhs, const eastl::wstring& rhs)
{
    return !(lhs == rhs);
}

// #4 std::string < eastl::wstring
inline bool operator<(const std::string& lhs, const eastl::wstring& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #4 std::string > eastl::wstring
inline bool operator>(const std::string& lhs, const eastl::wstring& rhs)
{
    return (Compare(lhs, rhs) > 0);
}

// #4 std::string <= eastl::wstring
inline bool operator<=(const std::string& lhs, const eastl::wstring& rhs)
{
    bool operator<(const eastl::wstring&, const std::string&);
    return !(rhs < lhs);
}

// #4 std::string >= eastl::wstring
inline bool operator>=(const std::string& lhs, const eastl::wstring& rhs)
{
    return !(lhs < rhs);
}

// #5 std::wstring == std::string
inline bool operator==(const std::wstring& lhs, const std::string& rhs)
{
    return operator==(rhs, lhs);
}

// #5 std::wstring != std::string
inline bool operator!=(const std::wstring& lhs, const std::string& rhs)
{
    return !(lhs == rhs);
}

// #5 std::wstring < std::string
inline bool operator<(const std::wstring& lhs, const std::string& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #5 std::wstring > std::string
inline bool operator>(const std::wstring& lhs, const std::string& rhs)
{
    return (rhs < lhs);
}

// #5 std::wstring <= std::string
inline bool operator<=(const std::wstring& lhs, const std::string& rhs)
{
    return !(rhs < lhs);
}

// #5 std::wstring >= std::string
inline bool operator>=(const std::wstring& lhs, const std::string& rhs)
{
    return !(lhs < rhs);
}

// #6 std::wstring to std::wstring (library)

// #7 std::wstring == eastl::string
inline bool operator==(const std::wstring& lhs, const eastl::string& rhs)
{
    return IsEqual(lhs, rhs);
}

// #7 std::wstring != eastl::string
inline bool operator!=(const std::wstring& lhs, const eastl::string& rhs)
{
    return !(lhs == rhs);
}

// #7 std::wstring < eastl::string
inline bool operator<(const std::wstring& lhs, const eastl::string& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #7 std::wstring > eastl::string
inline bool operator>(const std::wstring& lhs, const eastl::string& rhs)
{
    return (Compare(lhs, rhs) > 0);
}

// #7 std::wstring <= eastl::string
inline bool operator<=(const std::wstring& lhs, const eastl::string& rhs)
{
    bool operator<(const eastl::string&, const std::wstring&);
    return !(rhs < lhs);
}

// #7 std::wstring >= eastl::string
inline bool operator>=(const std::wstring& lhs, const eastl::string& rhs)
{
    return !(lhs < rhs);
}

// #8 std::wstring == eastl::wstring
inline bool operator==(const std::wstring& lhs, const eastl::wstring& rhs)
{
    return IsEqual(lhs, rhs);
}

// #8 std::wstring != eastl::wstring
inline bool operator!=(const std::wstring& lhs, const eastl::wstring& rhs)
{
    return !(lhs == rhs);
}

// #8 std::wstring < eastl::wstring
inline bool operator<(const std::wstring& lhs, const eastl::wstring& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #8 std::wstring > eastl::wstring
inline bool operator>(const std::wstring& lhs, const eastl::wstring& rhs)
{
    return (Compare(lhs, rhs) > 0);
}

// #8 std::wstring <= eastl::wstring
inline bool operator<=(const std::wstring& lhs, const eastl::wstring& rhs)
{
    bool operator<(const eastl::wstring&, const std::wstring&);
    return !(rhs < lhs);
}

// #8 std::wstring >= eastl::wstring
inline bool operator>=(const std::wstring& lhs, const eastl::wstring& rhs)
{
    return !(lhs < rhs);
}

// #9 eastl::string == std::string
inline bool operator==(const eastl::string& lhs, const std::string& rhs)
{
    return operator==(rhs, lhs);
}

// #9 eastl::string != std::string
inline bool operator!=(const eastl::string& lhs, const std::string& rhs)
{
    return !(lhs == rhs);
}

// #9 eastl::string < std::string
inline bool operator<(const eastl::string& lhs, const std::string& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #9 eastl::string > std::string
inline bool operator>(const eastl::string& lhs, const std::string& rhs)
{
    return (rhs < lhs);
}

// #9 eastl::string <= std::string
inline bool operator<=(const eastl::string& lhs, const std::string& rhs)
{
    return !(rhs < lhs);
}

// #9 eastl::string >= std::string
inline bool operator>=(const eastl::string& lhs, const std::string& rhs)
{
    return !(lhs < rhs);
}

// #10 eastl::string == std::wstring
inline bool operator==(const eastl::string& lhs, const std::wstring& rhs)
{
    return operator==(rhs, lhs);
}

// #10 eastl::string != std::wstring
inline bool operator!=(const eastl::string& lhs, const std::wstring& rhs)
{
    return !(lhs == rhs);
}

// #10 eastl::string < std::wstring
inline bool operator<(const eastl::string& lhs, const std::wstring& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #10 eastl::string > std::wstring
inline bool operator>(const eastl::string& lhs, const std::wstring& rhs)
{
    return (rhs < lhs);
}

// #10 eastl::string <= std::wstring
inline bool operator<=(const eastl::string& lhs, const std::wstring& rhs)
{
    return !(rhs < lhs);
}

// #10 eastl::string >= std::wstring
inline bool operator>=(const eastl::string& lhs, const std::wstring& rhs)
{
    return !(lhs < rhs);
}

// #11 eastl::string to eastl::string (library)

// #12 eastl::string == eastl::wstring
inline bool operator==(const eastl::string& lhs, const eastl::wstring& rhs)
{
    return IsEqual(lhs, rhs);
}

// #12 eastl::string != eastl::wstring
inline bool operator!=(const eastl::string& lhs, const eastl::wstring& rhs)
{
    return !(lhs == rhs);
}

// #12 eastl::string < eastl::wstring
inline bool operator<(const eastl::string& lhs, const eastl::wstring& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #12 eastl::string > eastl::wstring
inline bool operator>(const eastl::string& lhs, const eastl::wstring& rhs)
{
    return (rhs < lhs);
}

// #12 eastl::string <= eastl::wstring
inline bool operator<=(const eastl::string& lhs, const eastl::wstring& rhs)
{
    return !(rhs < lhs);
}

// #12 eastl::string >= eastl::wstring
inline bool operator>=(const eastl::string& lhs, const eastl::wstring& rhs)
{
    return !(lhs < rhs);
}

// #13 eastl::wstring == std::string
inline bool operator==(const eastl::wstring& lhs, const std::string& rhs)
{
    return operator==(rhs, lhs);
}

// #13 eastl::wstring != std::string
inline bool operator!=(const eastl::wstring& lhs, const std::string& rhs)
{
    return !(lhs == rhs);
}

// #13 eastl::wstring < std::string
inline bool operator<(const eastl::wstring& lhs, const std::string& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #13 eastl::wstring > std::string
inline bool operator>(const eastl::wstring& lhs, const std::string& rhs)
{
    return (rhs < lhs);
}

// #13 eastl::wstring <= std::string
inline bool operator<=(const eastl::wstring& lhs, const std::string& rhs)
{
    return !(rhs < lhs);
}

// #13 eastl::wstring >= std::string
inline bool operator>=(const eastl::wstring& lhs, const std::string& rhs)
{
    return !(lhs < rhs);
}

// #14 eastl::wstring == std::wstring
inline bool operator==(const eastl::wstring& lhs, const std::wstring& rhs)
{
    return operator==(rhs, lhs);
}

// #14 eastl::wstring != std::wstring
inline bool operator!=(const eastl::wstring& lhs, const std::wstring& rhs)
{
    return !(lhs == rhs);
}

// #14 eastl::wstring < std::wstring
inline bool operator<(const eastl::wstring& lhs, const std::wstring& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #14 eastl::wstring > std::wstring
inline bool operator>(const eastl::wstring& lhs, const std::wstring& rhs)
{
    return (rhs < lhs);
}

// #14 eastl::wstring <= std::wstring
inline bool operator<=(const eastl::wstring& lhs, const std::wstring& rhs)
{
    return !(rhs < lhs);
}

// #14 eastl::wstring >= std::wstring
inline bool operator>=(const eastl::wstring& lhs, const std::wstring& rhs)
{
    return !(lhs < rhs);
}

// #15 eastl::wstring == eastl::string
inline bool operator==(const eastl::wstring& lhs, const eastl::string& rhs)
{
    return operator==(rhs, lhs);
}

// #15 eastl::wstring != eastl::string
inline bool operator!=(const eastl::wstring& lhs, const eastl::string& rhs)
{
    return !(lhs == rhs);
}

// #15 eastl::wstring < eastl::string
inline bool operator<(const eastl::wstring& lhs, const eastl::string& rhs)
{
    return (Compare(lhs, rhs) < 0);
}

// #15 eastl::wstring > eastl::string
inline bool operator>(const eastl::wstring& lhs, const eastl::string& rhs)
{
    return (rhs < lhs);
}

// #15 eastl::wstring <= eastl::string
inline bool operator<=(const eastl::wstring& lhs, const eastl::string& rhs)
{
    return !(rhs < lhs);
}

// #15 eastl::wstring >= eastl::string
inline bool operator>=(const eastl::wstring& lhs, const eastl::string& rhs)
{
    return !(lhs < rhs);
}

// #16 eastl::wstring to eastl::wstring (library)

class IUnitTest
{
public:
    virtual ~IUnitTest() {}
    virtual void RunTests() = 0;
};

class StringExtensionTests : public IUnitTest
{
public:
    void RunTests() override { StringCompareTests(); }

private:
    const std::string STD_sTestString1 = "sample1";
    const std::string STD_sTestString2 = "sample2";
    const eastl::string EASTL_sTestString1 = "sample1";
    const eastl::string EASTL_sTestString2 = "sample2";

    const std::wstring STD_wsTestString1 = L"sample1";
    const std::wstring STD_wsTestString2 = L"sample2";
    const eastl::wstring EASTL_wsTestString1 = L"sample1";
    const eastl::wstring EASTL_wsTestString2 = L"sample2";

    // sTest1 and sTest2 must have a strict weak ordering
    // sTest3 must represent the same literal as sTest1
    template <typename T1, typename T2, typename T3>
    void StringTestCase(const T1& sTest1, const T2& sTest2, const T3& sTest3) const
    {
        static_assert(!std::is_same<T1, T2>::value, "sTest1 and sTest2 must have different types");
        static_assert(std::is_same<T2, T3>::value, "sTest2 and sTest3 must have matching types");

        // test for equality true
        assert(sTest1 == sTest3);

        // test for equality false
        assert(!(sTest1 == sTest2));

        // test for inequality true
        assert(sTest1 != sTest2);

        // test for inequality false
        assert(!(sTest1 != sTest3));

        // test for less than true
        assert(sTest1 < sTest2);

        // test for less than false
        assert(!(sTest2 < sTest1));

        // test for greater than true
        assert(sTest2 > sTest1);

        // test for greater than false
        assert(!(sTest1 > sTest2));

        // test for less than or equal true
        assert(sTest1 <= sTest3);
        assert(sTest1 <= sTest2);

        // test for less than or equal false
        assert(!(sTest2 <= sTest1));

        // test for greater than or equal true
        assert(sTest1 >= sTest3);
        assert(sTest2 >= sTest1);

        // test for greater than or equal false
        assert(!(sTest1 >= sTest2));
    }

    void StringCompareTests() const
    {
        // #1 std::string to std::string (library)
        //StringTestCase<std::string, std::string>(STD_sTestString1, STD_sTestString2, STD_sTestString1);

        // #2 std::string to std::wstring
        StringTestCase<std::string, std::wstring, std::wstring>(STD_sTestString1, STD_wsTestString2, STD_wsTestString1);

        // #3 std::string to eastl::string
        StringTestCase<std::string, eastl::string, eastl::string>(STD_sTestString1, EASTL_sTestString2, EASTL_sTestString1);

        // #4 std::string to eastl::wstring
        StringTestCase<std::string, eastl::wstring, eastl::wstring>(STD_sTestString1, EASTL_wsTestString2, EASTL_wsTestString1);

        // #5 std::wstring to std::string
        StringTestCase<std::wstring, std::string, std::string>(STD_wsTestString1, STD_sTestString2, STD_sTestString1);

        // #6 std::wstring to std::wstring (library)
        //StringTestCase<std::wstring, std::wstring, std::wstring>(STD_wsTestString1, STD_wsTestString2, STD_wsTestString1);

        // #7 std::wstring to eastl::string
        StringTestCase<std::wstring, eastl::string, eastl::string>(STD_wsTestString1, EASTL_sTestString2, EASTL_sTestString1);

        // #8 std::wstring to eastl::wstring
        StringTestCase<std::wstring, eastl::wstring, eastl::wstring>(STD_wsTestString1, EASTL_wsTestString2, EASTL_wsTestString1);

        // #9 eastl::string to std::string
        StringTestCase<eastl::string, std::string, std::string>(EASTL_sTestString1, STD_sTestString2, STD_sTestString1);

        // #10 eastl::string to std::wstring
        StringTestCase<eastl::string, std::wstring, std::wstring>(EASTL_sTestString1, STD_wsTestString2, STD_wsTestString1);

        // #11 eastl::string to eastl::string (library)
        //StringTestCase<eastl::string, eastl::string, eastl::string>(EASTL_sTestString1, EASTL_sTestString2, EASTL_sTestString1);

        // #12 eastl::string to eastl::wstring
        StringTestCase<eastl::string, eastl::wstring, eastl::wstring>(EASTL_sTestString1, EASTL_wsTestString2, EASTL_wsTestString1);

        // #13 eastl::wstring to std::string
        StringTestCase<eastl::wstring, std::string, std::string>(EASTL_wsTestString1, STD_sTestString2, STD_sTestString1);

        // #14 eastl::wstring to std::wstring
        StringTestCase<eastl::wstring, std::wstring, std::wstring>(EASTL_wsTestString1, STD_wsTestString2, STD_wsTestString1);

        // #15 eastl::wstring to eastl::string
        StringTestCase<eastl::wstring, eastl::string, eastl::string>(EASTL_wsTestString1, EASTL_sTestString2, EASTL_sTestString1);

        // #16 eastl::wstring to eastl::wstring (library)
        //StringTestCase<eastl::wstring, eastl::wstring, eastl::wstring>(EASTL_wsTestString1, EASTL_wsTestString2, EASTL_wsTestString1);
    }
};
