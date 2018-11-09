#include "CircularQueue.h"

namespace CircularQueueTest {

static void DoPODTests()
{
    CircularQueue<int> test(5);

    assert(test.empty());
    test.push(1);
    assert(!test.empty());
    test.push(2);
    test.push(3);
    assert(!test.full());
    test.push(4);
    test.push(5);
    assert(test.full());
    test.pop();
    assert(!test.full());
    assert(test.back() == 5);
    assert(test.front() == 2);

    const CircularQueue<int>& constRef = test;
    assert(constRef.back() == 5);
    assert(constRef.front() == 2);

    auto testCopy(test);
    assert(testCopy == test);
    assert(test == testCopy);
    testCopy.pop();
    assert(testCopy != test);
    int theValue = 42;
    testCopy.push(theValue);
    assert(testCopy != test);

    CircularQueue<int> test2;
    test2.set_capacity(10);
}

static void DoObjectTests()
{
    CircularQueue <std::string> test(5);

    assert(test.empty());
    test.push("Hello world");
    assert(!test.empty());
    std::string sJunk = "junk";
    test.push(sJunk);
}

void RunCircularQueueTest()
{
    DoPODTests();
    DoObjectTests();
}

}  // namespace CircularQueueTest
