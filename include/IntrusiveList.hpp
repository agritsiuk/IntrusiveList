#pragma once

#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <exception>
#include <new>
#include <utility>

template <class T, class Derived>
class IntrusiveList {
  private:
    struct Node {
        Node* prev;
        Node* next;
        T data;

        template <class... Args>
        Node(Node* prev, Node* next, Args&&... args) : prev(prev), next(next), data(std::forward<Args>(args)...) {}

        static constexpr std::size_t headerSize() { return offsetof(Node, data); }
    };

  public:
    template <class DerivedIter>
    class BaseIterator {
      public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

        BaseIterator() = default;
        constexpr BaseIterator(Node* node) : _node{node} {}

        T& operator*() { return _node->data; }

        const T& operator*() const { return _node->data; }

        T* operator->() { return &_node->data; }

        const T* operator->() const { return &_node->data; }

        constexpr bool operator==(const DerivedIter& other) const { return derived()._node == other._node; }

        constexpr bool operator!=(const DerivedIter& other) const { return !(*this == other); }

      private:
        const DerivedIter& derived() const { return *static_cast<const DerivedIter*>(this); }

      protected:
        friend class IntrusiveList;
        Node* _node{};
    };

    struct Iterator : public BaseIterator<Iterator> {
      public:
        using Base = BaseIterator<Iterator>;
        using Base::Base;

        Iterator& operator++() {
            Base::_node = Base::_node->next;
            return *this;
        }

        Iterator& operator--() {
            Base::_node = Base::_node->prev;
            return *this;
        }
    };

    class ReverseIterator : public BaseIterator<ReverseIterator> {
      public:
        using Base = BaseIterator<ReverseIterator>;
        using Base::Base;

        ReverseIterator& operator++() {
            Base::_node = Base::_node->prev;
            return *this;
        }

        ReverseIterator& operator--() {
            Base::_node = Base::_node->next;
            return *this;
        }
    };

    using iterator = Iterator;
    using reverse_iterator = ReverseIterator;

    IntrusiveList() = default;

    ~IntrusiveList() noexcept { clear(); }

    template <class... Args>
    static constexpr std::size_t dataSizeWithPayload(Args&&...) {
        return sizeof(T);
    }

    template <class... Args>
    T& emplace_back(Args&&... args) {
        const std::size_t fullDataSize = Derived::dataSizeWithPayload(std::forward<Args>(args)...);

        auto* ptr = std::malloc(Node::headerSize() + fullDataSize);
        if (!ptr) {
            throw std::bad_alloc();
        }

        Node* node = static_cast<Node*>(ptr);
        try {
            new (node) Node(_tail, nullptr, std::forward<Args>(args)...);
        } catch (...) {
            std::free(ptr);
            throw;
        }

        if (!_head) {
            _head = node;
        }

        if (_tail) {
            _tail->next = node;
        }

        ++_size;
        _tail = node;
        return node->data;
    }

    template <class Iterator>
    Iterator erase(const Iterator& iter) {
        Node* node = iter._node;
        Node* next = node->next;
        Node* prev = node->prev;

        if (prev) {
            prev->next = next;
        }

        if (next) {
            next->prev = prev;
        }

        if (_head == node) {
            _head = next;
        }

        if (_tail == node) {
            _tail = prev;
        }

        node->~Node();
        std::free(node);
        --_size;
        return Iterator{next};
    }

    void clear() {
        for (auto iter = begin(), endIter = end(); iter != endIter;) {
            iter = erase(iter);
        }
    }

    T& front() { return _head->data; }

    const T& front() const { return _head->data; }

    T& back() { return _tail->data; }

    const T& back() const { return _tail->data; }

    [[nodiscard]] bool empty() const { return !_size; }

    [[nodiscard]] std::size_t size() const { return _size; }

    Iterator begin() { return {_head}; }

    constexpr Iterator end() { return {nullptr}; }

    Iterator begin() const { return {_head}; }

    constexpr Iterator end() const { return {nullptr}; }

    ReverseIterator rbegin() { return {_tail}; }

    constexpr ReverseIterator rend() { return {nullptr}; }

    ReverseIterator rbegin() const { return {_tail}; }

    constexpr ReverseIterator rend() const { return {nullptr}; }

  private:
    Node* _head{};
    Node* _tail{};
    std::size_t _size{};
};
