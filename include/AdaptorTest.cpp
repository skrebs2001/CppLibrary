#if defined(_WIN32)
#pragma warning(disable : 4100)  // unreferenced formal parameter
#pragma warning(disable : 4101)  // unreferenced local variable
#pragma warning(disable : 4189)  // local variable is initialized but not referenced
#pragma warning(disable : 4505)  // unreferenced local function has been removed
#else
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#include <Types.h>
#include <eastl/vector.h>
#include <deque>
#include <forward_list>
#include <list>
#include <string>
#include <vector>
#include "Filter.h"
#include "Generate.h"
#include "Reverse.h"
#include "Take.h"
#include "Transform.h"
#include "Utility.h"

// clang-format off
namespace AdaptorTest
{
// clang-format on

template <typename SequenceContainer, typename T>
static SequenceContainer make_container(std::initializer_list<T> il)
{
    return SequenceContainer(begin(il), end(il));
}

struct Times3
{
    GDK::uint32 operator()(GDK::uint32 n) const { return n * 3; }
};

static double TimesPi(GDK::uint32 n)
{
    return n * 3.1415;
}

static GDK::uint32 Square(GDK::uint32 n)
{
    return n * n;
}

struct MyStruct
{
    MyStruct(unsigned i, double d)
        : m_i(i)
        , m_d(d)
    {
    }
    unsigned m_i;
    double m_d;
};

static double GetDouble(const MyStruct& m)
{
    return m.m_d;
}

MyStruct MakeStruct(GDK::uint32 n)
{
    return MyStruct{ n, 3.14159 };
}

static bool IsEven(GDK::uint32 n)
{
    return n % 2 == 0;
}

struct IsMultiple3
{
    bool operator()(GDK::uint32 n) const { return n % 3 == 0; }
};

struct IsOdd
{
    bool operator()(GDK::uint32 n) const { return n % 2 != 0; }
};

struct IsAny
{
    bool operator()(GDK::uint32 n) const { return true; }
};

// clang-format off
struct MakeInt
{
    MakeInt() = default;
    explicit MakeInt(GDK::uint32 n) : m_start(n) {}
    GDK::uint32 operator()() { return m_start++; }

private:
    GDK::uint32 m_start = 0;
};

static unsigned rand_count = 0;
GDK::uint32 MakeRand()
{
    ++rand_count;
    return static_cast<GDK::uint32>(rand() % 1000);
}

struct MakeIntString
{
    MakeIntString() = default;
    explicit MakeIntString(GDK::uint32 n) : m_Int(n) {}
    std::string operator()() { return std::to_string(m_Int()); }

private:
    MakeInt m_Int;
};
// clang-format on

template <typename Rng>
static void DoRangeConstructibleTests(const Rng& r)
{
    // Copy constructible
    Rng r1 = r;

    // Move constructible
    Rng r2 = std::move(r1);

    // Default constructible
    Rng r3;

    // Copy assignable
    r2 = r;

    // Move assignable
    r3 = std::move(r2);
}

static const std::vector<GDK::uint32> Answer1 = { 1, 2, 3, 6, 9, 12, 13 };
static const std::vector<GDK::uint32> Answer3 = { 3, 6, 9, 18, 27, 36, 39 };
static const std::vector<GDK::uint32> Answer4 = { 1, 4, 9, 36, 81, 144, 169 };
static const std::vector<std::string> Answer5 = { "3", "6", "9", "18", "27", "36", "39" };
static const std::vector<std::string> Answer6 = { "39", "36", "27", "18", "9", "6", "3" };

template <typename Container1, typename Container2>
static bool CheckResult(const Container1& c, const Container2& answer)
{
    return std::equal(c.begin(), c.end(), answer.begin());
}

template <typename SequenceContainer, typename Tag>
static void DoRandomAccessTransformTests(const SequenceContainer&, Tag)
{
    // Catch all other iterator categories that do not support random access methods
}

template <typename SequenceContainer, typename Tag>
static void DoBidirectionalTransformTests(const SequenceContainer&, Tag)
{
    // Catch all other iterator categories that do not support bidirectional methods
}

template <typename SequenceContainer>
static void DoBidirectionalTransformTests(const SequenceContainer& Source, std::bidirectional_iterator_tag)
{
    using Range::transform;

    auto range1 = Source | transform(Utility::ToUInt32);
    auto end = range1.end();

    auto i = --end;
    assert(*i == 13);

    i = end--;
    assert(*i == 13);
    assert(*end == 12);

    auto val = *end--;
    assert(val == 12);
    assert(*end == 9);

    val = *--end;
    assert(val == 6);
}

template <typename SequenceContainer>
static void DoBidirectionalTransformTests(const SequenceContainer& Source, std::random_access_iterator_tag)
{
    DoBidirectionalTransformTests(Source, std::bidirectional_iterator_tag{});
}

template <typename SequenceContainer>
static void DoRandomAccessTransformTests(const SequenceContainer& Source, std::random_access_iterator_tag)
{
    using Range::transform;

    auto range1 = Source | transform(Utility::ToUInt32);
    auto start = range1.begin();
    auto end = range1.end();

    // Iterator relations
    assert(start < end);
    assert(end > start);
    assert(start <= end);
    assert(start <= start);
    assert(end >= start);
    assert(end >= end);

    // Arithmetic operations
    auto next = start;
    assert(*next == 1);
    next = 1 + start;
    assert(*next == 2);
    next = start + 2;
    assert(*next == 3);
    auto prev = end - 1;
    assert(*prev == 13);
    auto diff = end - start;
    assert(diff == 7);

    // Compound assignment operations
    start += 2;
    assert(*start == 3);
    end -= 2;
    assert(*end == 12);

    // Random access
    start = range1.begin();
    assert(start[3] == 6);
}

template <typename SequenceContainer>
static void DoGenericTransformTests()
{
    using namespace Utility;
    using Range::copy_range;
    using Range::filter;
    using Range::transform;

    auto Source = make_container<SequenceContainer>({ L"1", L"2", L"3", L"6", L"9", L"12", L"13" });
    auto SourceIL = { 1u, 2u, 3u, 6u, 9u, 12u, 13u };

    // Increment tests
    auto range1 = Source | transform(ToUInt32);
    auto start = range1.begin();
    auto end = range1.end();
    assert(start == range1.begin());
    assert(start != end);

    auto i = start++;
    assert(*i == 1);
    assert(*start == 2);

    i = ++start;
    assert(*i == 3);

    auto val = *start++;
    assert(val == 3);
    assert(*start == 6);

    val = *++start;
    assert(val == 9);

    DoRangeConstructibleTests(range1);

    eastl::vector<GDK::uint32> destVector;
    eastl::list<GDK::uint32> destList;

    // Add to empty vector
    copy_range(destVector, Source | transform(ToUInt32));
    assert(CheckResult(destVector, Answer1));
    destVector.clear();

    // Add to empty list
    copy_range(destList, Source | transform(ToUInt32));
    assert(CheckResult(destList, Answer1));
    destList.clear();

    // Chain transforms, with function object
    copy_range(destVector, Source | transform(ToUInt32) | transform(Times3()));
    assert(CheckResult(destVector, Answer3));
    destVector.clear();

    // Copy from chained transforms
    Range::copy(Source | transform(ToUInt32) | transform(Square), std::back_inserter(destVector));
    assert(CheckResult(destVector, Answer4));
    destVector.clear();

    // Sum over a chained transforms, with lambda
    auto result = Range::accumulate(Source | transform(ToUInt32) | transform([](GDK::uint32 n) { return n * n; }), 0u);
    assert(result == 444);

    // Chain transforms, change final range type
    eastl::vector<double> doubleVector;
    push_back(doubleVector, Source | transform(ToUInt32) | transform([](GDK::uint32 n) { return n * 3; }) | transform(TimesPi));

    // Apply transform to range of structures
    eastl::list<MyStruct> listMyStruct;
    listMyStruct.push_back(MyStruct{ 1, 3.14 });
    listMyStruct.push_back(MyStruct{ 2, 2.717 });
    listMyStruct.push_back(MyStruct{ 3, 42.0 });
    push_back(doubleVector, listMyStruct | transform(GetDouble));

    // strings as ranges
    {
        std::string srcString("hello, world!");
        const std::string answer("HELLOWORLD");
        std::string destString;
        copy_range(destString, srcString | filter(isalpha) | transform(toupper));
        assert(destString == answer);
    }

    {
        eastl::string srcString("hello, world!");
        const eastl::string answer("HELLOWORLD");
        eastl::string destString;
        copy_range(destString, srcString | filter(isalpha) | transform(toupper));
        assert(destString == answer);
    }

    auto to_string = [](GDK::uint32 n) { return std::to_string(n); };

    // std::initializer_list as range
    {
        std::vector<std::string> stdStringVector;
        copy_range(stdStringVector, SourceIL | transform(Times3()) | transform(to_string));
        assert(CheckResult(stdStringVector, Answer5));
    }

    // Combinations of EASTL and std containers as ranges
    {
        std::vector<std::string> stdStringVector;
        copy_range(stdStringVector, Source | transform(ToUInt32) | transform(Times3()) | transform(to_string));
        assert(CheckResult(stdStringVector, Answer5));
    }

    {
        std::deque<std::string> stdStringDeque;
        copy_range(stdStringDeque, Source | transform(ToUInt32) | transform(Times3()) | transform(to_string));
        assert(CheckResult(stdStringDeque, Answer5));
    }

    {
        eastl::vector<std::string> eastlStringVector;
        copy_range(eastlStringVector, Source | transform(ToUInt32) | transform(Times3()) | transform(to_string));
        assert(CheckResult(eastlStringVector, Answer5));
    }

    {
        std::list<std::string> stdList;
        copy_range(stdList, Source | transform(ToUInt32) | transform(Times3()) | transform(to_string));
        assert(CheckResult(stdList, Answer5));
    }

    {
        eastl::list<std::string> eastlList;
        copy_range(eastlList, Source | transform(ToUInt32) | transform(Times3()) | transform(to_string));
        assert(CheckResult(eastlList, Answer5));
    }

    {
        std::forward_list<std::string> stdForwardList;
        auto r = Source | transform(ToUInt32) | transform(Times3()) | transform(to_string);
        std::copy(r.begin(), r.end(), front_inserter(stdForwardList));
        assert(CheckResult(stdForwardList, Answer6));
    }

    {
        // Apply view to rvalue
        copy_range(destVector, make_container<SequenceContainer>({ L"1", L"2", L"3", L"6", L"9", L"12", L"13" }) | transform(ToUInt32));
        assert(CheckResult(destVector, Answer1));
        destVector.clear();
    }

    typename std::iterator_traits<typename SequenceContainer::iterator>::iterator_category category;
    DoBidirectionalTransformTests(Source, category);
    DoRandomAccessTransformTests(Source, category);
}

template <template <typename T, typename... Allocator> class SequenceContainer>
static void DoTransformTestsImpl()
{
    DoGenericTransformTests<SequenceContainer<eastl::wstring> >();
}

static void DoTransformTests()
{
    DoTransformTestsImpl<eastl::vector>();
    DoTransformTestsImpl<eastl::list>();
    DoTransformTestsImpl<std::vector>();
    DoTransformTestsImpl<std::list>();
    DoTransformTestsImpl<std::deque>();
    DoTransformTestsImpl<std::forward_list>();
}

static const std::vector<GDK::uint32> AllEven = { 2, 6, 12 };
static const std::vector<GDK::uint32> AllOdd = { 1, 3, 9, 13 };
static const std::vector<GDK::uint32> EvenMultipleOf3 = { 6, 12 };
static const std::vector<GDK::uint32> EvenSquared = { 4, 36, 144 };
static const std::vector<GDK::uint32> SquaredEven = { 4, 36, 144 };
static const std::vector<GDK::uint32> EvenSquaredMultipleOf3 = { 36, 144 };

template <typename SequenceContainer>
static void DoGenericFilterTests()
{
    using Range::filter;
    using Range::transform;

    {
        auto Source = make_container<SequenceContainer>({ 2, 4, 6, 8 });
        auto range = Source | filter(IsEven);
        auto start = range.begin();
        auto end = range.end();
        assert(*start == 2);
        assert(std::distance(start, end) == 4);
        end = start;
        DoRangeConstructibleTests(range);
    }

    {
        auto Source = make_container<SequenceContainer>({ 1, 3, 5, 7 });
        auto range = Source | filter(IsEven);
        auto start = range.begin();
        auto end = range.end();
        assert(start == end);
        assert(std::distance(start, end) == 0);
    }

    {
        auto Source = make_container<SequenceContainer>({ 2, 1, 3, 5, 7 });
        auto range = Source | filter(IsOdd());
        auto start = range.begin();
        auto end = range.end();
        assert(*start == 1);
        assert(std::distance(start, end) == 4);
        DoRangeConstructibleTests(range);
    }

    {
        auto Source = make_container<SequenceContainer>({ 1, 3, 5, 7, 8 });
        auto range = Source | filter(IsOdd());
        auto start = range.begin();
        auto end = range.end();
        assert(*start == 1);
        assert(std::distance(start, end) == 4);
    }

    const std::vector<GDK::uint32> v = { 1, 2, 3, 6, 9, 12, 13 };
    SequenceContainer Source{ v.begin(), v.end() };

    eastl::vector<GDK::uint32> destVector;

    {
        auto range = Source | filter(IsEven);
        copy_range(destVector, range);
        assert(CheckResult(destVector, AllEven));
        destVector.clear();
        DoRangeConstructibleTests(range);
    }

    {
        auto range = Source | filter(IsEven) | filter(IsMultiple3());
        copy_range(destVector, range);
        assert(CheckResult(destVector, EvenMultipleOf3));
        destVector.clear();
        DoRangeConstructibleTests(range);
    }

    // Range constructibility tests will fail here due to the lambda expression, as they are not copy assignable
    copy_range(destVector, Source | filter([](GDK::uint32 n) { return n % 2 == 0; }) | transform(Square));
    assert(CheckResult(destVector, EvenSquared));
    destVector.clear();

    {
        auto range = Source | transform(Square) | filter(IsEven);
        copy_range(destVector, range);
        assert(CheckResult(destVector, SquaredEven));
        destVector.clear();
        DoRangeConstructibleTests(range);
    }

    {
        auto range = Source | filter(IsEven) | transform(Square) | filter(IsMultiple3());
        copy_range(destVector, range);
        assert(CheckResult(destVector, EvenSquaredMultipleOf3));
        destVector.clear();
        DoRangeConstructibleTests(range);
    }

    for (auto i : Source | filter(IsEven))
    {
        destVector.push_back(i);
    }
    assert(CheckResult(destVector, AllEven));
    destVector.clear();

    for (auto i : Source | filter(IsEven) | filter(IsMultiple3()))
    {
        destVector.push_back(i);
    }
    assert(CheckResult(destVector, EvenMultipleOf3));
    destVector.clear();

    for (auto i : Source | filter(IsEven) | transform(Square))
    {
        destVector.push_back(i);
    }
    assert(CheckResult(destVector, EvenSquared));
    destVector.clear();

    for (const auto& i : Source | filter(IsEven))
    {
        destVector.push_back(i);
    }
    assert(CheckResult(destVector, AllEven));
    destVector.clear();

    for (auto&& i : Source | filter(IsEven))
    {
        destVector.push_back(i);
    }
    assert(CheckResult(destVector, AllEven));
    destVector.clear();

    for (auto i : Source | filter(IsOdd()))
    {
        destVector.push_back(i);
    }
    assert(CheckResult(destVector, AllOdd));
    destVector.clear();

    for (auto i : Source | filter([](GDK::uint32 n) { return n % 2 == 0; }))
    {
        destVector.push_back(i);
    }
    assert(CheckResult(destVector, AllEven));
    destVector.clear();

    {
        // Apply view to rvalue
        copy_range(destVector, SequenceContainer(v.begin(), v.end()) | filter(IsEven));
        assert(CheckResult(destVector, AllEven));
        destVector.clear();
    }
}

template <template <typename T, typename... Allocator> class SequenceContainer>
static void DoFilterTestsImpl()
{
    DoGenericFilterTests<SequenceContainer<GDK::uint32> >();
}

static void DoFilterTests()
{
    DoFilterTestsImpl<eastl::vector>();
    DoFilterTestsImpl<eastl::list>();
    DoFilterTestsImpl<std::vector>();
    DoFilterTestsImpl<std::list>();
    DoFilterTestsImpl<std::deque>();
    DoFilterTestsImpl<std::forward_list>();
}

static void DoFilterIteratorTests()
{
    using Range::filter;
    using Range::generate;

    {
        auto Source = eastl::vector<GDK::uint32>{};
        auto r = Source | filter(IsEven);
        auto itCat = std::iterator_traits<decltype(r)::iterator>::iterator_category{};
        static_assert(std::is_same<std::bidirectional_iterator_tag, decltype(itCat)>::value, "Expected bidirectional iterator");
    }

    {
        auto Source = std::vector<GDK::uint32>{};
        auto r = Source | filter(IsEven);
        auto itCat = std::iterator_traits<decltype(r)::iterator>::iterator_category{};
        static_assert(std::is_same<std::bidirectional_iterator_tag, decltype(itCat)>::value, "Expected bidirectional iterator");
    }

    {
        auto Source = std::deque<GDK::uint32>{};
        auto r = Source | filter(IsEven);
        auto itCat = std::iterator_traits<decltype(r)::iterator>::iterator_category{};
        static_assert(std::is_same<std::bidirectional_iterator_tag, decltype(itCat)>::value, "Expected bidirectional iterator");
    }

    {
        auto Source = eastl::list<GDK::uint32>{};
        auto r = Source | filter(IsEven);
        auto itCat = std::iterator_traits<decltype(r)::iterator>::iterator_category{};
        static_assert(std::is_same<std::bidirectional_iterator_tag, decltype(itCat)>::value, "Expected bidirectional iterator");
    }

    {
        auto Source = std::list<GDK::uint32>{};
        auto r = Source | filter(IsEven);
        auto itCat = std::iterator_traits<decltype(r)::iterator>::iterator_category{};
        static_assert(std::is_same<std::bidirectional_iterator_tag, decltype(itCat)>::value, "Expected bidirectional iterator");
    }

    {
        auto Source = std::forward_list<GDK::uint32>{};
        auto r = Source | filter(IsEven);
        auto itCat = std::iterator_traits<decltype(r)::iterator>::iterator_category{};
        static_assert(std::is_same<std::forward_iterator_tag, decltype(itCat)>::value, "Expected forward iterator");
    }

    {
        auto r = generate(MakeInt()) | filter(IsEven);
        auto itCat = std::iterator_traits<decltype(r)::iterator>::iterator_category{};
        static_assert(std::is_same<std::input_iterator_tag, decltype(itCat)>::value, "Expected input iterator");
    }
}

static void DoPreTests()
{
    using Range::filter;
    using Range::transform;

    auto IsMatch = [](const std::string& s) { return s.size() > 4; };
    std::vector<std::string> s1({ "hello", "my", "there", "world" });
    eastl::vector<std::string> s(s1.begin(), s1.end());

    {
        eastl::vector<std::string> destVector;
        std::vector<std::string> result({ "hello", "there", "world" });
        for (const auto& i : s | filter(IsMatch))
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, result));
    }

    std::vector<int> v1({ 2, 3, 4, 6 });
    std::vector<int> EvenResult({ 2, 4, 6 });

    {
        auto IsEven = [](int s) { return s % 2 == 0; };
        auto Times2 = [](int s) { return s * 2; };

        std::vector<int> result({ 4, 6, 8, 12 });
        std::vector<int> destVector;
        for (const auto& i : v1 | transform(Times2) | filter(IsEven))
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, result));
    }

    {
        std::list<int> l1({ 2, 3, 4, 6 });
        std::vector<int> destVector;
        for (const auto& i : l1 | filter(IsEven))
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, EvenResult));
    }

    {
        std::deque<int> d1({ 2, 3, 4, 6 });
        std::vector<int> destVector;
        for (const auto& i : d1 | filter(IsEven))
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, EvenResult));
    }

    {
        std::forward_list<int> f1({ 2, 3, 4, 6 });
        std::vector<int> destVector;
        for (const auto& i : f1 | filter(IsEven))
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, EvenResult));
    }

    {
        eastl::vector<int> intVec(v1.begin(), v1.end());
        std::vector<int> destVector;
        for (const auto& i : intVec | filter(IsEven))
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, EvenResult));
    }

    {
        eastl::list<int> intList(v1.begin(), v1.end());
        std::vector<int> destVector;
        for (const auto& i : intList | filter(IsEven))
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, EvenResult));
    }
}

static void DoGenerateTests()
{
    using Range::filter;
    using Range::generate;
    using Range::generate_n;
    using Range::take;
    using Range::transform;

    {
        auto r = generate_n(MakeInt(42), 10);
        auto start = r.begin();
        auto end = r.end();
        assert(*start == 42);
        auto s1 = start;
        assert(*s1 == 42);
        end = start;
        DoRangeConstructibleTests(r);
    }

    {
        auto r = generate_n(MakeRand, 0);
        auto start = r.begin();
        auto end = r.end();
        assert(std::distance(start, end) == 0);
        DoRangeConstructibleTests(r);
    }

    {
        auto r = generate_n(MakeInt(), 10) | filter(IsOdd());

        std::vector<GDK::uint32> destVector;
        std::vector<GDK::uint32> OddResult({ 1, 3, 5, 7, 9 });
        for (auto i : r)
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, OddResult));
        DoRangeConstructibleTests(r);
    }

    {
        std::vector<GDK::uint32> destVector;
        copy_range(destVector, generate_n(MakeRand, 10) | filter(IsOdd()));
    }

    {
        auto r = generate_n(MakeIntString(5), 42);
        auto start = r.begin();
        auto end = r.end();
        assert(std::distance(start, end) == 42);
        assert(*start == "5");
        ++start;
        assert(*start == "6");
        start++;
        assert(*start == "7");
        assert(*start == "7");

        // With this generating function, we get pseudo-forward iterator behavior
        auto iter = start;
        assert(iter == start);
        assert(*iter == *start);
        ++iter;
        ++start;
        assert(iter == start);
        assert(*iter == *start);
        DoRangeConstructibleTests(r);
    }

    {
        auto r = generate_n(MakeRand, 42);
        auto start = r.begin();
        auto end = r.end();
        assert(std::distance(start, end) == 42);
        auto result = *start;
        assert(*start == result);

        // As the generating function is random, the dereference test will likely fail
        auto iter = start;
        assert(iter == start);
        ++iter;
        ++start;
        assert(iter == start);
    }
}

template <typename SequenceContainer>
static void DoGenericTakeTests()
{
    using Range::filter;
    using Range::generate;
    using Range::generate_n;
    using Range::take;
    using Range::transform;

    auto Source = make_container<SequenceContainer>({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });

    std::vector<GDK::uint32> FirstFive({ 0, 1, 2, 3, 4 });

    // Test copy constructor, copy assignment, equality/inequality, dereference
    {
        auto r = Source | take(5);
        auto start = r.begin();
        auto end = r.end();
        assert(start != end);
        end = start;
        assert(start == end);
        assert(*start == 0);
        DoRangeConstructibleTests(r);
    }

    // Take fewer than the range contains
    {
        std::vector<GDK::uint32> destVector;
        auto r = Source | take(5);
        for (auto i : r)
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, FirstFive));
    }

    // Take exactly how many the range contains
    {
        std::vector<GDK::uint32> destVector;
        auto r = Source | take(10);
        for (auto i : r)
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, Source));
    }

    // Take more than the range contains
    {
        std::vector<GDK::uint32> destVector;
        auto r = Source | take(12);
        for (auto i : r)
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, Source));
    }

    // Take from infinite range
    {
        std::vector<GDK::uint32> destVector;
        auto r = generate(MakeInt()) | take(5);
        for (auto i : r)
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, FirstFive));
        DoRangeConstructibleTests(r);
    }

    // Generate an infinite integer range, filter for the odd numbers, take the first 5, and square them
    {
        std::vector<GDK::uint32> answer({ 1, 9, 25, 49, 81 });
        std::vector<GDK::uint32> destVector;
        auto r = generate(MakeInt()) | filter(IsOdd()) | take(5) | transform(Square);
        copy_range(destVector, r);
        assert(CheckResult(destVector, answer));
        DoRangeConstructibleTests(r);
    }

    {
        // Apply view to rvalue
        std::vector<GDK::uint32> destVector;
        copy_range(destVector, make_container<SequenceContainer>({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }) | take(5));
        assert(CheckResult(destVector, FirstFive));
    }
}

template <template <typename T, typename... Allocator> class SequenceContainer>
static void DoTakeTestsImpl()
{
    DoGenericTakeTests<SequenceContainer<GDK::uint32> >();
}

static void DoTakeTests()
{
    DoTakeTestsImpl<eastl::vector>();
    DoTakeTestsImpl<eastl::list>();
    DoTakeTestsImpl<std::vector>();
    DoTakeTestsImpl<std::list>();
    DoTakeTestsImpl<std::deque>();
    DoTakeTestsImpl<std::forward_list>();
}

template <typename SequenceContainer>
static void DoGenericReverseTests()
{
    using Range::filter;
    using Range::reverse;
    using Range::transform;

    using ValueType = typename SequenceContainer::value_type;
    auto Source = make_container<SequenceContainer>({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });
    std::vector<ValueType> destVector;

    // empty container
    {
        SequenceContainer v;
        auto r = v | reverse();
        auto start = r.begin();
        auto end = r.end();
        assert(start == end);
        assert(std::distance(start, end) == 0);
        DoRangeConstructibleTests(r);
    }

    // container of 1 element
    {
        SequenceContainer v = make_container<SequenceContainer>({ 42 });
        auto r = v | reverse();
        auto start = r.begin();
        auto end = r.end();
        assert(start != end);
        assert(std::distance(start, end) == 1);

        std::vector<ValueType> answer({ 42 });
        for (auto i : r)
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, answer));
        destVector.clear();
    }

    {
        auto r = Source | reverse();
        auto start = r.begin();
        auto end = r.end();
        assert(start != end);
        assert(*start == 9);
        ++start;
        assert(*start == 8);
        --end;
        assert(*end == 0);
        --end;
        assert(*end == 1);
        assert(std::distance(r.begin(), r.end()) == 10);

        std::vector<ValueType> answer({ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 });
        for (auto i : r)
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, answer));
        destVector.clear();
    }

    {
        auto r = Source | filter(IsOdd()) | reverse();
        auto start = r.begin();
        auto end = r.end();
        assert(start != end);
        assert(std::distance(r.begin(), r.end()) == 5);

        std::vector<ValueType> answer({ 9, 7, 5, 3, 1 });
        push_back(destVector, r);
        assert(CheckResult(destVector, answer));
        destVector.clear();
        DoRangeConstructibleTests(r);
    }

    {
        auto r = Source | reverse() | transform(Square);
        auto start = r.begin();
        auto end = r.end();
        assert(start != end);
        assert(*start == 81);
        start++;
        assert(*start == 64);
        --end;
        assert(*end == 0);
        --end;
        assert(*end == 1);
        assert(std::distance(r.begin(), r.end()) == 10);

        std::vector<ValueType> answer({ 81, 64, 49, 36, 25, 16, 9, 4, 1, 0 });
        push_back(destVector, r);
        assert(CheckResult(destVector, answer));
        destVector.clear();
        DoRangeConstructibleTests(r);
    }

    {
        auto r = Source | transform(Square) | reverse();
        auto start = r.begin();
        auto end = r.end();
        assert(start != end);
        assert(*start == 81);
        start++;
        assert(*start == 64);
        --end;
        assert(*end == 0);
        --end;
        assert(*end == 1);
        assert(std::distance(r.begin(), r.end()) == 10);

        std::vector<ValueType> answer({ 81, 64, 49, 36, 25, 16, 9, 4, 1, 0 });
        for (auto i : r)
        {
            destVector.push_back(i);
        }
        assert(CheckResult(destVector, answer));
        destVector.clear();
        DoRangeConstructibleTests(r);
    }

    {
        std::vector<ValueType> answer({ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 });
        copy_range(destVector, make_container<SequenceContainer>({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }) | reverse());
        assert(CheckResult(destVector, answer));
        destVector.clear();
    }
}

template <template <typename T, typename... Allocator> class SequenceContainer>
static void DoReverseTestsImpl()
{
    DoGenericReverseTests<SequenceContainer<GDK::uint32> >();
}

static void DoReverseTests()
{
    DoReverseTestsImpl<eastl::vector>();
    DoReverseTestsImpl<eastl::list>();
    DoReverseTestsImpl<std::vector>();
    DoReverseTestsImpl<std::list>();
    DoReverseTestsImpl<std::deque>();
}

void RunAdaptorTest()
{
    DoReverseTests();
    DoGenerateTests();
    DoTakeTests();
    DoPreTests();
    DoTransformTests();
    DoFilterTests();
    DoFilterIteratorTests();
}

// clang-format off
}
// clang-format on
