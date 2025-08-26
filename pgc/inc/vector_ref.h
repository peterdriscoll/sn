#pragma once

#include <vector>
#include "ref.h"

// ---------------------------------------------
// vector_ref: A promotion-aware vector wrapper
// ---------------------------------------------

namespace PGC
{
	class PGC_TypeCheck; // Forward declaration

    template <typename T>
    class vector_ref {
    public:
        using value_type = Ref<T>;
        using container_type = std::vector<value_type>; // Now `value_type` is a valid type
        using iterator = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;

    private:
        container_type m_Data;

    public:
        explicit vector_ref()
        {
        }
        vector_ref(const vector_ref& other)
            : m_Data(other.m_Data)
        {
        }

        void push_back(T* value)
        {
            // Direct in-place construction -> std::construct_at(ptr, value)
            m_Data.emplace_back(value);
        }

        void push_back_ref(const value_type& ref)
        {
            m_Data.emplace_back(ref.Get());
        }

        void push_back_ref(value_type& ref)
        {
            m_Data.emplace_back(ref.Get());
        }

        void PromoteAll()
        {
            for (auto& ref : m_Data) {
                ref.RequestPromotion();
            }
        }

        iterator begin() { return m_Data.begin(); }
        iterator end() { return m_Data.end(); }
        const_iterator begin() const { return m_Data.begin(); }
        const_iterator end() const { return m_Data.end(); }

        auto erase(typename container_type::iterator pos)
        {
            return m_Data.erase(pos);
        }

        size_t size() const { return m_Data.size(); }
        bool empty() const { return m_Data.empty(); }

        // Read-only access
        const value_type& operator[](size_t index) const
        {
            return m_Data[index];
        }

        // Mutable access — must promote
        value_type& operator[](size_t index)
        {
            m_Data[index].RequestPromotion();
            return m_Data[index];
        }

        void clear() { m_Data.clear(); }

        void reserve(size_t n) { m_Data.reserve(n); }
        void resize(size_t n) { m_Data.resize(n); }

        template<typename Func>
        void ForEach(Func&& func)
        {
            for (auto& ref : m_Data)
            {
                ref.RequestPromotion();
                func(ref);
            }
        }

        template<typename Func>
        void ForEach(Func&& func) const
        {
            for (const auto& ref : m_Data)
            {
                func(ref);
            }
        }

    protected:
        // Direct access to underlying container. Use with caution — no promotion triggered.
        container_type& Raw() { return m_Data; }
        const container_type& Raw() const { return m_Data; }
    };
}

