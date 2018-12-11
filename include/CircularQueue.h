#pragma once

#include <cassert>
#include <type_traits>
#include <vector>

template <typename T>
class CircularQueue;

template <typename T, typename Pointer, typename Reference>
class CircularQueueIterator
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = typename CircularQueue<value_type>::difference_type;
    using pointer = Pointer;
    using reference = Reference;

    explicit CircularQueueIterator(CircularQueue<value_type>* pCircularQueue, pointer pElement)
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

    // Pre-decrement operator
    CircularQueueIterator& operator--()
    {
        m_pElement = m_pCircularQueue->prev(m_pElement);
        return *this;
    }

    // Post-decrement operator
    CircularQueueIterator operator--(int)
    {
        CircularQueueIterator temp(*this);
        --(*this);
        return temp;
    }

    reference operator*() const { return *m_pElement; }
    pointer operator->() const { return m_pElement; }
    friend bool operator==(const CircularQueueIterator& lhs, const CircularQueueIterator& rhs) { return lhs.m_pElement == rhs.m_pElement; }
    friend bool operator!=(const CircularQueueIterator& lhs, const CircularQueueIterator& rhs) { return !(lhs == rhs); }

private:
    CircularQueue<value_type>* m_pCircularQueue = nullptr;
    pointer m_pElement = nullptr;
};

// Fixed size circular queue
template <typename T>
class CircularQueue final
{
public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using size_type = size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = CircularQueueIterator<value_type, pointer, reference>;
    using const_iterator = CircularQueueIterator<value_type, const_pointer, const_reference>;
    template <class, class, class>
    friend class CircularQueueIterator;

    CircularQueue() = default;

    ~CircularQueue()
    {
        deallocate();
        reset();  // TODO: is this needed?
    }

    // Construct a circular queue with the given element capacity
    explicit CircularQueue(size_type capacity)
        : m_pData(allocate(capacity + 1))
        , m_Head(0)
        , m_Tail(capacity)
        , m_Size(0)
        , m_Capacity(capacity + 1)
    {
    }

    CircularQueue(const CircularQueue& other)
    {
        pointer pNewData = allocate(other.capacity());
        construct_range(other.begin(), other.end(), pNewData);

        m_pData = pNewData;
        m_Head = 0;
        m_Tail = other.empty() ? other.capacity() - 1 : other.size() - 1;
        m_Size = other.size();
        m_Capacity = other.capacity();
    }

    CircularQueue& operator=(const CircularQueue& other)
    {
        // Potential optimization is to copy the elements from other if the current memory has the capacity
        // Would lose the strong exception guarantee since an element copy may throw, but eliminates one memory allocation and deallocation
        return *this = CircularQueue(other);
    }

    CircularQueue(CircularQueue&& other) noexcept
    {
        // Move resources from other object into new
        move_from(other);
    }

    CircularQueue& operator=(CircularQueue&& other) noexcept
    {
        // Release current resources, then move from other object
        deallocate();
        move_from(other);
        return *this;
    }

    // Returns a reference to the last element in the queue
    reference back() { return *tail(); }
    const_reference back() const { return *tail(); }

    // Returns an iterator pointing to the first element in the queue
    iterator begin() noexcept { return iterator(this, head()); }
    const_iterator begin() const noexcept { return const_iterator(const_cast<CircularQueue*>(this), head()); }
    const_iterator cbegin() const noexcept { return const_cast<const CircularQueue&>(*this).begin(); }

    // Returns whether the queue is empty
    bool empty() const noexcept { return size() == 0; }

    // Returns an iterator pointing to the past-the-end element in the queue
    iterator end() noexcept { return iterator(this, next(tail())); }
    const_iterator end() const noexcept { return const_iterator(const_cast<CircularQueue*>(this), next(tail())); }
    const_iterator cend() const noexcept { return const_cast<const CircularQueue&>(*this).end(); }

    // Returns a reference to the first element in the queue
    reference front() { return *head(); }
    const_reference front() const { return *head(); }

    // Returns whether the queue is at maximum capacity
    bool full() const noexcept { return size() == capacity() - 1; }

    size_type max_size() const noexcept { return static_cast<size_type>(0xffffffff) / sizeof(value_type); }

    // Removes the head element, reduces queue size by one
    void pop()
    {
        assert(!empty());
        head()->~value_type();
        m_Head = increment(m_Head);
        --m_Size;
    }

    // Inserts a new element at the tail of the queue
    void push(const value_type& val) { add(val); }
    void push(value_type&& val) { add(std::move(val)); }

    // Returns the number of elements in the queue
    size_type size() const noexcept { return m_Size; }

    // Exchanges the contents of the queue with those of other
    void swap(CircularQueue& other) noexcept
    {
        std::swap(m_pData, other.m_pData);
        std::swap(m_Head, other.m_Head);
        std::swap(m_Tail, other.m_Tail);
        std::swap(m_Size, other.m_Size);
        std::swap(m_Capacity, other.m_Capacity);
    }

    void set_capacity(size_type new_capacity)
    {
        // If the new capacity is equal to the current capacity, just return
        //
        // Otherwise, if the new capacity is greater than the current:
        //   allocate a new buffer at the new capacity
        //   move the elements from the current buffer to the new buffer
        //   deallocate the current buffer
        //
        // Else, the new capacity is less than the current:
        //    elements from the head are removed until the new capacity is reached

        // Degenerate case of default constructed queue
        if (m_Capacity == 0 && new_capacity == 0)
        {
            return;
        }

        // Current capacity is non-zero, new capacity is 0, deallocate current buffer and reset all data members
        if (new_capacity == 0)
        {
            deallocate();
            reset();
            return;
        }

        // New capacity is non-zero, resize the buffer if not equal
        if (++new_capacity != m_Capacity)
        {
            pointer pNewData = reallocate(new_capacity);
            deallocate();

            m_pData = pNewData;
            m_Head = 0;
            m_Tail = empty() ? new_capacity - 1 : size() - 1;
            m_Capacity = new_capacity;
        }
    }

    //bool AddTest()
    //{
    //    for (m_Capacity = 1; m_Capacity < 1000; ++m_Capacity)
    //    {
    //        for (size_t i = 0; i < m_Capacity; ++i)
    //        {
    //            auto i1 = addOffset(0, i);
    //            auto i2 = addOffset2(0, i);

    //            if (i1 != i2)
    //            {
    //                return false;
    //            }
    //        }
    //    }

    //    return true;
    //}

    //bool IncrementTest()
    //{
    //    for (m_Capacity = 1; m_Capacity < 1000; ++m_Capacity)
    //    {
    //        for (size_t i = 0; i < m_Capacity; ++i)
    //        {
    //            auto i1 = increment(i);
    //            auto i2 = increment2(i);

    //            if (i1 != i2)
    //            {
    //                return false;
    //            }
    //        }
    //    }

    //    return true;
    //}

private:
    template <typename Val>
    struct TriviallyCopyable : std::is_trivially_copyable<typename std::remove_reference<Val>::type>
    {
    };

    template <typename Val>
    void add(Val&& val)
    {
        if (full()) pop();
        m_Tail = increment(m_Tail);
        assert(m_Tail < capacity());
        construct(std::forward<Val>(val), tail());
        ++m_Size;
    }

    // Construct elements from the range [first, last) into contiguous memory range pointed to by dest
    void construct_range(const_iterator first, const_iterator last, pointer dest)
    {
        for (; first != last; ++first, ++dest)
        {
            construct(*first, dest);
        }
    }

    // Construct elements from the range [first, last) into contiguous memory range pointed to by dest
    void construct_range(iterator first, iterator last, pointer dest)
    {
        for (; first != last; ++first, ++dest)
        {
            construct(*first, dest);
        }
    }

    // Construct a single element into memory pointed to by dest
    template <typename Val>
    typename std::enable_if<TriviallyCopyable<Val>::value>::type construct(Val&& val, pointer dest)
    {
        *dest = val;
    }

    // Construct a single element into memory pointed to by dest
    template <typename Val>
    typename std::enable_if<!TriviallyCopyable<Val>::value>::type construct(Val&& val, pointer dest)
    {
        ::new (dest) value_type(std::forward<Val>(val));
    }

    // Destroy the buffer elements in the range [first, last)
    template <typename Val = value_type>
    typename std::enable_if<std::is_trivially_destructible<Val>::value>::type destroy_range(pointer, pointer)
    {
    }

    // Destroy the buffer elements in the range [first, last)
    template <typename Val = value_type>
    typename std::enable_if<!std::is_trivially_destructible<Val>::value>::type destroy_range(pointer first, pointer last)
    {
        for (; first != last; first = next(first))
        {
            first->~value_type();
        }
    }

    void move_from(CircularQueue& other) noexcept
    {
        m_pData = other.m_pData;
        m_Head = other.m_Head;
        m_Tail = other.m_Tail;
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;
        reset(other);
    }

    // Reset the queue to the default state
    void reset(CircularQueue& q) noexcept
    {
        q.m_pData = nullptr;
        q.m_Head = 0;
        q.m_Tail = 0;
        q.m_Size = 0;
        q.m_Capacity = 0;
    }

    void reset() noexcept { reset(*this); }

    // Allocate new buffer for n elements, copy current elements
    pointer reallocate(size_t n)
    {
        pointer p = allocate(n);
        if (!empty())
        {
            construct_range(begin(), end(), p);
        }
        return p;
    }

    // Allocate buffer for n elements
    pointer allocate(size_t n) { return static_cast<pointer>(::operator new(sizeof(value_type) * n)); }

    // Destroy all elements and release buffer memory
    void deallocate()
    {
        if (m_pData)
        {
            destroy_range(head(), next(tail()));
            ::operator delete(m_pData);
        }
    }

    // Returns pointer to head element
    pointer head() { return &m_pData[m_Head]; }
    const_pointer head() const { return &m_pData[m_Head]; }

    // Returns pointer to tail element
    pointer tail() { return &m_pData[m_Tail]; }
    const_pointer tail() const { return &m_pData[m_Tail]; }

    // Returns pointer to the next logical element
    pointer next(pointer p) { return const_cast<pointer>(next(const_cast<const_pointer>(p))); }
    const_pointer next(const_pointer p) const
    {
        if (!p) return nullptr;
        size_type index = increment(p - m_pData);
        assert(index < capacity());
        return &m_pData[index];
    }

    // Returns pointer to the previous logical element
    pointer prev(pointer p) { return const_cast<pointer>(prev(const_cast<const_pointer>(p))); }
    const_pointer prev(const_pointer p) const
    {
        size_type index = decrement(p - m_pData);
        assert(index < capacity());
        return &m_pData[index];
    }

    // Increment the given index, wrapping around the container size
    size_type increment(size_type index) const noexcept { return (index == capacity() - 1) ? 0 : index + 1; }

    size_type increment2(size_type index) const noexcept { return (index + 1) % capacity(); }

    size_type addOffset(size_type index, size_type n) const
    {
        size_type result = index;
        while (n-- > 0)
        {
            result = increment(result);
        }
        return result;
    }

    size_type addOffset2(size_type index, size_type n) const { return (index + n) % capacity(); }

    // Decrement the given index, wrapping around the container size
    size_type decrement(size_type index) const noexcept { return (index == 0) ? capacity() - 1 : index - 1; }

    size_type capacity() const noexcept { return m_Capacity; }

    pointer m_pData = nullptr;
    size_type m_Head = 0;
    size_type m_Tail = 0;
    size_type m_Size = 0;
    size_type m_Capacity = 0;
};

template <typename T>
bool operator==(const CircularQueue<T>& lhs, const CircularQueue<T>& rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator!=(const CircularQueue<T>& lhs, const CircularQueue<T>& rhs)
{
    return !(lhs == rhs);
}

template <typename T>
void swap(CircularQueue<T>& a, CircularQueue<T>& b) noexcept
{
    a.swap(b);
}

template <typename T>
auto begin(CircularQueue<T>& c) -> decltype(c.begin())
{
    return c.begin();
}

template <typename T>
auto begin(const CircularQueue<T>& c) -> decltype(c.begin())
{
    return c.begin();
}

template <typename T>
auto end(CircularQueue<T>& c) -> decltype(c.end())
{
    return c.end();
}

template <typename T>
auto end(const CircularQueue<T>& c) -> decltype(c.end())
{
    return c.end();
}

// clang-format off
namespace CircularQueueTest
{
extern void RunCircularQueueTest();
}
// clang-format on
