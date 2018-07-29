#pragma once

#include <algorithm>
#include <array>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

template <typename T>
const T& min_alt(const T& a, const T& b)
{
    return b < a ? b : a;
}

//
// Find the most common elements in a vector
//
// src       Vector of elements to search
// n         Number of common elements to find
// elements  Output vector of common elements
// counts    Output vector of element occurrences
//
// Elements in the input must have operator< defined
//
// O(nLogn) where n is the number of elements in the range
//
template <typename T>
void top_n(const std::vector<T>& src, size_t n, std::vector<T>& elements, std::vector<size_t>& counts)
{
    using std::begin;
    using std::end;
    using value_type = T;

    // map of counters keyed by each of the possible values
    std::map<value_type, size_t> elementCounter;

    // iterate over the given vector and count the number of occurrences of each element
    // - each map access is O(log n), so the total loop is O(nLogn)
    std::for_each(begin(src), end(src), [&](const value_type& v) { ++elementCounter[v]; });

    // iterate over the map of counters to find the n most common
    // - O(n) where n is the number of elements in the map since iterators are constant time
    using Entry = typename std::map<value_type, size_t>::value_type;
    auto comp = [](const Entry& lhs, const Entry& rhs) { return lhs.second < rhs.second; };

    size_t size = min_alt(n, elementCounter.size());
    for (size_t count = 0; count < size; ++count)
    {
        auto largest = std::max_element(begin(elementCounter), end(elementCounter), comp);
        elements[count] = largest->first;
        counts[count] = largest->second;
        elementCounter.erase(largest);
    }
}

//
// Find the most common elements in the range [first, last)
//
// first    InputIterator to the first element in search range
// last     InputIterator to the last element in search range
// n        Number of common elements to find
//
// Elements in the input range must have operator< defined
//
// O(nLogn) where n is the number of elements in the range
//
template <typename InputIterator, typename value_type = typename std::iterator_traits<InputIterator>::value_type>
auto top_n(InputIterator first, InputIterator last, size_t n) -> std::vector<std::pair<value_type, size_t> >
{
    using std::begin;
    using std::end;

    std::vector<std::pair<value_type, size_t> > elements(n);

    // map of counters keyed by each of the possible values
    std::map<value_type, size_t> elementCounter;

    // iterate over the given sequence and count the number of occurrences of each element
    // - each map access is O(log n), so the total loop is O(nLogn)
    std::for_each(first, last, [&](const value_type& v) { ++elementCounter[v]; });

    // iterate over the map of counters to find the n most common
    // - O(n) where n is the number of elements in the map since iterators are constant time
    using Entry = typename std::map<value_type, size_t>::value_type;
    auto comp = [](const Entry& lhs, const Entry& rhs) { return lhs.second < rhs.second; };

    size_t size = min_alt(n, elementCounter.size());
    for (size_t count = 0; count < size; ++count)
    {
        auto largest = std::max_element(begin(elementCounter), end(elementCounter), comp);
        elements[count] = *largest;
        elementCounter.erase(largest);
    }

    return elements;
}

//
// Find the most common elements in the range [first, last)
//
// first    InputIterator to the first element in search range
// last     InputIterator to the last element in search range
// n        Number of common elements to find
// result   OutputIterator to the initial position in the destination range of at least n elements
//          Range must have type std::iterator_traits<InputIterator>::value_type
//
// Elements in the input range must have operator< defined
//
// O(nLogn) where n is the number of elements in the range
//
template <typename InputIterator, typename OutputIterator>
void top_n(InputIterator first, InputIterator last, size_t n, OutputIterator result)
{
    using std::begin;
    using std::end;
    using value_type = typename std::iterator_traits<InputIterator>::value_type;

    // map of counters keyed by each of the possible values
    std::map<value_type, size_t> elementCounter;

    // iterate over the given sequence and count the number of occurrences of each element
    // - each map access is O(log n), so the total loop is O(nLogn)
    std::for_each(first, last, [&](const value_type& v) { ++elementCounter[v]; });

    // iterate over the map of counters to find the n most common
    // - O(n) where n is the number of elements in the map since iterators are constant time
    using Entry = typename std::map<value_type, size_t>::value_type;
    auto comp = [](const Entry& lhs, const Entry& rhs) { return lhs.second < rhs.second; };

    size_t size = min_alt(n, elementCounter.size());
    for (size_t count = 0; count < size; ++count, ++result)
    {
        auto largest = std::max_element(begin(elementCounter), end(elementCounter), comp);
        *result = largest->first;
        elementCounter.erase(largest);
    }
}

// clang-format off
namespace TopNTest
{
class MyClass
{
public:
    MyClass()
        : x(0)
    {
    }
    explicit MyClass(int val)
        : x(val)
    {
    }
    ~MyClass() {}
    bool operator<(const MyClass& rhs) const { return (x < rhs.x); }
    int value() const { return x; }

private:
    int x;
};

inline void FindTopTests()
{
    std::vector<unsigned> twoUniqueElements = { 1, 2, 1, 2, 1, 2, 2, 2 };
    std::vector<unsigned> intResults;
    top_n(twoUniqueElements.begin(), twoUniqueElements.end(), 3, std::back_inserter(intResults));

    std::vector<unsigned> testIntVector = { 3, 1, 4, 10, 1, 5, 9, 5, 1, 6, 11, 4, 4, 4, 0, 2, 3, 4 };
    std::vector<size_t> Counts(3);
    intResults.clear();
    top_n(testIntVector.begin(), testIntVector.end(), 3, std::back_inserter(intResults));
    top_n(testIntVector, 3, intResults, Counts);

    std::cout << "first is " << intResults[0] << "\n";
    std::cout << "second is " << intResults[1] << "\n";
    std::cout << "third is " << intResults[2] << "\n";

    std::deque<double> testDoubleVector = { 3.1, 1.1, 4.1, 10.1, 1.1, 5.1, 9.1, 5.1, 1.1, 6.1, 11.1, 4.1, 4.1, 4.1, 0.1, 2.1, 3.1, 4.1 };
    std::vector<double> doubleResults(3);
    top_n(testDoubleVector.begin(), testDoubleVector.end(), 3, doubleResults.begin());
    std::cout << "first is " << doubleResults[0] << "\n";
    std::cout << "second is " << doubleResults[1] << "\n";
    std::cout << "third is " << doubleResults[2] << "\n";

    std::list<std::string> testStringVector = { "three", "one",    "four", "ten",  "one",  "five", "nine", "five",  "one",
                                                "six",   "eleven", "five", "five", "four", "zero", "two",  "three", "four" };
    std::vector<std::string> stringResults(3);
    top_n(testStringVector.begin(), testStringVector.end(), 3, stringResults.begin());
    std::cout << "first is " << stringResults[0] << "\n";
    std::cout << "second is " << stringResults[1] << "\n";
    std::cout << "third is " << stringResults[2] << "\n";

    MyClass D1(1), D2(2), D3(3), D4(4), D5(5);
    std::forward_list<MyClass> testMyClassVector = { D5, D5, D5, D5, D4, D4, D4, D3, D3, D1, D2 };
    std::vector<MyClass> MyClassResults(3);
    top_n(testMyClassVector.begin(), testMyClassVector.end(), 3, MyClassResults.begin());
    std::cout << "first is " << MyClassResults[0].value() << "\n";
    std::cout << "second is " << MyClassResults[1].value() << "\n";
    std::cout << "third is " << MyClassResults[2].value() << "\n";

    std::array<std::string, 6> a = { "hello", "there", "world", "there", "you", "are" };
    auto p = top_n(a.begin(), a.end(), 3);

    std::vector<std::string> v = { "hello", "there", "world", "there", "you", "are" };
    p = top_n(v.begin(), v.end(), 3);
    top_n(v, 3, stringResults, Counts);

    std::list<std::string> l = { "one", "two", "three", "one", "four", "one" };
    p = top_n(l.begin(), l.end(), 3);

    std::forward_list<std::string> fl = { "one", "two", "three", "one", "four", "one" };
    p = top_n(fl.begin(), fl.end(), 3);

    std::deque<std::string> d = { "one", "two", "three", "one", "four", "one" };
    p = top_n(d.begin(), d.end(), 3);
}

}
