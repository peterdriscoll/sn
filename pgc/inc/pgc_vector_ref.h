#pragma once

#include <vector>
#include "pgc_user.h"
#include "pgc_transaction.h"
#include "memberref.h"

// ---------------------------------------------
// PGC_Vector: A promotion-aware vector wrapper
// ---------------------------------------------

namespace PGC
{
    template<typename T>
    class vector_ref {
    public:
        using value_type = MemberRef<T>;
        using container_type = std::vector<value_type>;
        using iterator = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;
        using Transaction = PGC_Transaction;

    private:
        container_type m_Data;
        Transaction* m_Transaction = nullptr;

    public:
        vector_ref() = delete;
        explicit vector_ref(Transaction* transaction)
            : m_Transaction(transaction) {
        }
        vector_ref(const vector_ref& other, Transaction* transaction)
            : m_Transaction(transaction), m_Data(other.m_Data)
        {
        }

        Transaction* GetTransaction() const { return m_Transaction; }

        void push_back(T* value)
        {
            MemberRef<T> ref;
            ref.Set(value, m_Transaction);
            ref.RequestPromotion(m_Transaction);
            m_Data.push_back(ref);
        }

        void push_back_ref(const MemberRef<T>& ref)
        {
            MemberRef<T> promotedRef = ref;
            promotedRef.RequestPromotion(m_Transaction);
            m_Data.push_back(promotedRef);
        }

        void push_back_ref(MemberRef<T>&& ref)
        {
            ref.RequestPromotion(m_Transaction);
            m_Data.push_back(std::move(ref));
        }

        void PromoteAll()
        {
            for (auto& ref : m_Data) {
                ref.RequestPromotion(m_Transaction);
            }
        }

        iterator begin() { return m_Data.begin(); }
        iterator end() { return m_Data.end(); }
        const_iterator begin() const { return m_Data.begin(); }
        const_iterator end() const { return m_Data.end(); }

        auto erase(typename std::vector<MemberRef<T>>::iterator pos)
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
            m_Data[index].RequestPromotion(m_Transaction);
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
                ref.RequestPromotion(m_Transaction);
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

