#pragma once

#include <unordered_map>
#include <utility>
#include "pgc_transaction.h"
#include "memberref.h"

// ---------------------------------------------
// unordered_map_ref: A promotion-aware unordered_map wrapper
// ---------------------------------------------
namespace PGC
{
    template<typename Key, typename T>
    class unordered_map_ref {
    public:
        using key_type = Key;
        using mapped_type = MemberRef<T>;
        using value_type = std::pair<const Key, mapped_type>;
        using container_type = std::unordered_map<Key, mapped_type>;
        using iterator = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;
        using Transaction = PGC_Transaction;

    private:
        container_type m_Data;
        Transaction* m_Transaction = nullptr;

    public:
        unordered_map_ref() = delete;
        explicit unordered_map_ref(Transaction* transaction)
            : m_Transaction(transaction) {
        }

        unordered_map_ref(const unordered_map_ref& other, Transaction* transaction)
            : m_Data(other.m_Data), m_Transaction(transaction) {
        }

        Transaction* GetTransaction() const { return m_Transaction; }

        void Insert(const Key& key, T* value)
        {
            MemberRef<T> ref;
            ref.Set(value, m_Transaction);
            m_Data[key] = ref;
        }

        void InsertRef(const Key& key, const mapped_type& ref)
        {
            m_Data[key] = ref;
			m_Data[key].RequestPromotion(m_Transaction);
        }

        void InsertRef(const Key& key, mapped_type&& ref)
        {
            ref.RequestPromotion(m_Transaction);
            m_Data[key] = std::move(ref);
        }

        mapped_type& operator[](const Key& key)
        {
            mapped_type& ref = m_Data[key];
            ref.RequestPromotion(m_Transaction);
            return ref;
        }

        const mapped_type& at(const Key& key) const
        {
            return m_Data.at(key);
        }

        bool Contains(const Key& key) const
        {
            return m_Data.find(key) != m_Data.end();
        }

        iterator begin() { return m_Data.begin(); }
        iterator end() { return m_Data.end(); }
        const_iterator begin() const { return m_Data.begin(); }
        const_iterator end() const { return m_Data.end(); }

        size_t size() const { return m_Data.size(); }
        bool empty() const { return m_Data.empty(); }

        void PromoteAll()
        {
            for (auto& kv : m_Data) {
                kv.second.RequestPromotion(m_Transaction);
            }
        }

        void clear() { m_Data.clear(); }

        template<typename Func>
        void ForEach(Func&& func)
        {
            for (auto& [key, ref] : m_Data) {
                func(key, ref);
            }
        }

        template<typename Func>
        void ForEach(Func&& func) const
        {
            for (const auto& [key, ref] : m_Data) {
                func(key, ref);
            }
        }

        iterator find(const Key& key) {
            return m_Data.find(key);
        }

        const_iterator find(const Key& key) const {
            return m_Data.find(key);
        }

    protected:
        container_type& Raw() { return m_Data; }
        const container_type& Raw() const { return m_Data; }
    };
}

