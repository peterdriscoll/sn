#pragma once

#include "pgc_registry.h"
#include "exp_ctrl_pgc.h"
#include "pgc_base.h"
#include "pgc_promotion.h"
#include "reversible_guard.h"
#include "threadregistry.h"

#include <string>
#include <unordered_set>
#include <typeindex>
#include <mutex>

namespace PGC
{
    struct WithLock_t { explicit constexpr WithLock_t() = default; };
    struct NoLock_t { explicit constexpr NoLock_t() = default; };

    inline constexpr WithLock_t WithLock{};
    inline constexpr NoLock_t  NoLock{};

    // Base class for any session/user that needs automatic promotion cleanup and state checks
    class PGC_EXPORT PGC_User
    {
    public:
        // Default error handler implementation
        // This can be overridden by the user if needed
        static void DefaultErrorHandler(bool p_Err, const std::string& message);
        
        static PGC_User *GetCurrentPGC_UserPtr();
        static PGC_User &GetCurrentPGC_User();
        static void SetCurrentPGC_User(PGC_User *p_User);

        // Constructor and destructor
        PGC_User(const RegEntry p_ClassRegistry[] = nullptr, OnErrorHandler *p_ErrorHandler = DefaultErrorHandler);
        virtual ~PGC_User();

        // Optional explicit calls (if needed)
        void Initialize();
        void Cleanup();

        OnErrorHandler* GetErrorHandler() const
        {
            return m_ErrorHandler;
		}

        void ClearAllPromotions();
        void PromoteRequests();

        PGC_Promotion* Allocate();
        void FreePromotion(PGC_Promotion* p_Promotion);
        bool FindInFreeList(PGC_Promotion* p_Promotion);

        void AppendRequest(PGC_Promotion*);

        size_t TotalNetMemoryUsed() const;
        size_t TotalGrossMemoryUsed() const;
        size_t TotalProcessedRefAttachedMemory() const;

        void AddTotalNetMemorySize(long memory);
        void AddTotalGrossMemorySize(long memory);
        void AddProcessedRefAttachedMemory(long memory);

        void ResetNetMemoryUsed();
        void ResetGrossMemoryUsed();
        void ResetProcessedRefAttachedMemory();

        size_t PromotionFreeMemory();
        size_t PromotionUsedMemory();
        bool ShouldRaiseError();
        size_t TotalPromotionMemory();

        template<class T>
        bool IsRegistered() const
        {
            return m_registered.find(std::type_index(typeid(T))) != m_registered.end();
        }

        template<class T>
        void RequireRegistered(const char* where) const
        {
            ASSERTM(m_registered.size()==0 || IsRegistered<T>(),
				std::string("Class not registered: ") + typeid(T).name() + " at " + where);
        }

        template<class F> decltype(auto) with_lock(WithLock_t, F&& f)
        {
        #ifdef PGC_DEBUG
            std::string save_label = PGC::consume_debug_label();
            PGC::save_debug_label("with-lock-"+save_label);
        #endif
            auto g = m_mutex.lock_token(); // EnGuard
        #ifdef PGC_DEBUG
            PGC::save_debug_label(save_label);
        #endif
            return std::forward<F>(f)();
        }
        template<class F> decltype(auto) with_lock(NoLock_t, F&& f) 
        {
            return std::forward<F>(f)();
        }

        [[nodiscard]] PGC::EnGuard Acquire() noexcept
        {
            return m_mutex.lock_token(); 
        }
        // Temporarily *drop* the lock for this thread; restores on dtor.
        [[nodiscard]] UnGuard Unguard() noexcept
        {
            return m_mutex.unlock_token();
        }

        ThreadRegistry& GetThreadRegistry() noexcept;

    private:
		ThreadRegistry m_ThreadRegistry;
        ThreadMembership m_ThreadMembership;

        ReversibleMutex m_mutex;
        std::mutex m_PromoteRequestsMutex;

        std::unordered_set<std::type_index> m_registered;
        OnErrorHandler *m_ErrorHandler;
		bool m_ErrorRaised = false;
    
        std::mutex m_FreeListMutex;   // protects promote list only
        PGC_Promotion* m_FreeList;

        std::mutex m_PromoteListMutex;   // protects promote list only
        PGC_Promotion* m_PromoteList;
        PGC_Promotion** m_PromoteListLast;

        size_t m_TotalNetMemoryUsed;
        size_t m_TotalGrossMemoryUsed;
        size_t m_ProcessedRefAttachedMemory;
    };
}