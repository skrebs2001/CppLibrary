#pragma once

#include <cassert>
#include <iterator>
#include <vector>

template <typename Queue, typename Pointer, typename Reference>
class CircularQueueIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename Queue::value_type;
    using difference_type = typename Queue::difference_type;
    using pointer = Pointer;
    using reference = Reference;

    explicit CircularQueueIterator(Queue* pCircularQueue, pointer pElement)
        : m_pCircularQueue(pCircularQueue)
        , m_pElement(pElement)
    {
    }

    CircularQueueIterator() = default;
    CircularQueueIterator(const CircularQueueIterator&) = default;
    CircularQueueIterator& operator=(const CircularQueueIterator&) = default;
    ~CircularQueueIterator() = default;

    // Pre-increment operator
    CircularQueueIterator& operator++()
    {
        m_pElement = m_pCircularQueue->next(m_pElement);
        return *this;
    }

    // Post-increment operator
    CircularQueueIterator operator++(int)
    {
        CircularQueueIterator temp(*this);
        ++(*this);
        return temp;
    }

    reference operator*() const { return *m_pElement; }

    pointer operator->() const { return m_pElement; }

    friend bool operator==(const CircularQueueIterator& lhs, const CircularQueueIterator& rhs) { return lhs.m_pElement == rhs.m_pElement; }

    friend bool operator!=(const CircularQueueIterator& lhs, const CircularQueueIterator& rhs) { return !(lhs == rhs); }

private:
    Queue* m_pCircularQueue = nullptr;
    pointer m_pElement = nullptr;
};

// Fixed size circular queue
template <typename T>
class CircularQueue final
{
    using container_type = std::vector<T>;

public:
    using value_type = T;
    using difference_type = typename container_type::difference_type;
    using size_type = typename container_type::size_type;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using pointer = typename container_type::pointer;
    using const_pointer = typename container_type::const_pointer;
    using iterator = CircularQueueIterator<CircularQueue<T>, pointer, reference>;
    using const_iterator = CircularQueueIterator<CircularQueue<T>, const_pointer, const_reference>;
    template <class, class, class>
    friend class CircularQueueIterator;

    CircularQueue() = delete;
    ~CircularQueue() = default;
    CircularQueue(const CircularQueue&) = default;
    CircularQueue(CircularQueue&&) = default;
    CircularQueue& operator=(const CircularQueue&) = default;
    CircularQueue& operator=(CircularQueue&&) = default;

    // Construct a circular queue with the given element capacity
    explicit CircularQueue(size_type capacity)
        : m_vContainer(capacity + 1)
        , m_Tail(capacity)
    {
    }

    // Returns a reference to the last element in the queue
    reference back() { return *tail(); }
    const_reference back() const { return *tail(); }

    // Returns an iterator pointing to the first element in the queue
    iterator begin() noexcept { return iterator(this, head()); }
    const_iterator begin() const noexcept { return const_iterator(const_cast<CircularQueue*>(this), head()); }

    // Returns whether the queue is empty
    bool empty() const noexcept { return size() == 0; }

    // Returns an iterator pointing to the past-the-end element in the queue
    iterator end() noexcept { return empty() ? begin() : iterator(this, next(tail())); }
    const_iterator end() const noexcept { return empty() ? begin() : const_iterator(const_cast<CircularQueue*>(this), next(tail())); }

    // Returns a reference to the first element in the queue
    reference front() { return *head(); }
    const_reference front() const { return *head(); }

    // Returns whether the queue is at maximum capacity
    bool full() const noexcept { return size() == m_vContainer.size() - 1; }

    // Removes the first element, reduces queue size by one
    void pop() noexcept
    {
        assert(!empty());
        m_Head = increment(m_Head);
        --m_Size;
    }

    // Inserts a new element at the end of the queue
    void push(const value_type& val) { add(val); }
    void push(value_type&& val) { add(std::move(val)); }

    // Returns the number of elements in the queue
    size_type size() const noexcept { return m_Size; }

    // Exchanges the contents of the queue with those of other
    void swap(CircularQueue& other) noexcept
    {
        std::swap(m_vContainer, other.m_vContainer);
        std::swap(m_Head, other.m_Head);
        std::swap(m_Tail, other.m_Tail);
        std::swap(m_Size, other.m_Size);
    }

    friend bool operator==(const CircularQueue& lhs, const CircularQueue& rhs) { return lhs.compare(rhs); }
    friend bool operator!=(const CircularQueue& lhs, const CircularQueue& rhs) { return !(lhs == rhs); }

private:
    template <typename Val>
    void add(Val&& val)
    {
        assert(!full());
        m_Tail = increment(m_Tail);
        assert(m_Tail < m_vContainer.size());
        m_vContainer[m_Tail] = std::forward<Val>(val);
        ++m_Size;
    }

    bool compare(const CircularQueue& other) const
    {
        if (m_Size != other.m_Size)
        {
            return false;
        }

        // iterate over each container, compare elements
        auto thisIndex = m_Head;
        auto otherIndex = other.m_Head;

        for (; thisIndex != m_Tail; thisIndex = increment(thisIndex), otherIndex = increment(otherIndex))
        {
            if (m_vContainer[thisIndex] != other.m_vContainer[otherIndex])
            {
                return false;
            }
        }

        return true;
    }

    const_pointer head() const
    {
        assert(m_Head < m_vContainer.size());
        return &m_vContainer[m_Head];
    }

    pointer head()
    {
        assert(m_Head < m_vContainer.size());
        return &m_vContainer[m_Head];
    }

    const_pointer tail() const
    {
        assert(!empty());
        assert(m_Tail < m_vContainer.size());
        return &m_vContainer[m_Tail];
    }

    pointer tail()
    {
        assert(!empty());
        assert(m_Tail < m_vContainer.size());
        return &m_vContainer[m_Tail];
    }

    pointer prev(pointer p)
    {
        assert(!empty());
        size_type nextIndex = decrement(p - m_vContainer.data());
        assert(nextIndex < m_vContainer.size());
        return &m_vContainer[nextIndex];
    }

    pointer next(pointer p)
    {
        assert(!empty());
        size_type nextIndex = increment(p - m_vContainer.data());
        assert(nextIndex < m_vContainer.size());
        return &m_vContainer[nextIndex];
    }

    const_pointer next(const_pointer p) const
    {
        assert(!empty());
        size_type nextIndex = increment(p - m_vContainer.data());
        assert(nextIndex < m_vContainer.size());
        return &m_vContainer[nextIndex];
    }

    // Decrement the given index, wrapping around the container size
    size_type decrement(size_type index) const noexcept { return (index == 0) ? m_vContainer.size() - 1 : index - 1; }

    // Increment the given index, wrapping around the container size
    size_type increment(size_type index) const noexcept { return (index == m_vContainer.size() - 1) ? 0 : index + 1; }

    container_type m_vContainer;
    size_type m_Head = 0;
    size_type m_Tail = 0;
    size_type m_Size = 0;
};

// clang-format off
namespace CircularQueueTest
{
extern void RunCircularQueueTest();
}
// clang-format on
