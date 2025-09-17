#pragma once

#include "memberref.h"
#include "pgc_base.h"
#include "pgc_promotemacros.h"
#include "pgc_ready.h"
#include "pgc_registry.h"
#include "pgc_stacktransaction.h"
#include "pgc_task.h"
#include "pgc_transaction.h"
#include "pgc_unordered_map_ref.h"
#include "pgc_user.h"
#include "pgc_vector_member_ref.h"
#include "promotioncapture.h"
#include "ref.h"
#include "sref.h"
#include "thread.h"
#include "vector_ref.h"

namespace PGCX
{
    typedef PGC::PGC_Base Base;
    typedef PGC::PGC_Transaction PGC_Transaction;
    typedef PGC::PGC_StackTransaction StackTransaction;
    typedef PGC::PGC_Task Task;
    typedef PGC::PGC_User PGC_User;
    typedef PGC::PGC_Promotion Promotion;
    typedef PGC::Thread Thread;
    typedef PGC::ThreadMembership ThreadMembership;

    // Smart pointer-style wrappers
    template <class T> using Ref = PGC::Ref<T>;

    template <class T> using Pin = PGC::Pin<T>;

    template <class T> using ConstPin = PGC::ConstPin<T>;

    template <class T> using SRef = PGC::SRef<T>;

    template <typename T> using MemberRef = PGC::MemberRef<T>;

    template <typename T> using vector_member_ref = PGC::vector_member_ref<T>;

    template <class Key, class T>
    using unordered_map_ref = PGC::unordered_map_ref<Key, T>;
} // namespace PGCX
