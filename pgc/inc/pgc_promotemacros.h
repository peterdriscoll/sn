#pragma once
// pgc_promotemacros.h
//
// Contract (per class, right before PGC_MEMBER_DEFINITIONS):
//   #define PGC_ACTION_OVER_VALUE_MEMBERS(ACTION)              /* ACTION(NAME, TYPE, DEFAULT_VALUE) lines or empty */
//   #define PGC_ACTION_OVER_MEMBER_REFS(ACTION)                /* ACTION(NAME, TYPE) lines or empty */
//   #define PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(ACTION)      /* ACTION(NAME, FULLTYPE, ELEM_TYPE) lines or empty */
//
//   PGC_MEMBER_DEFINITIONS(MyType, MyBase, Initialize())
//   // or
//   PGC_MEMBER_DEFINITIONS_NOINIT(MyType, MyBase)
//
//   // then immediately:
//   #undef PGC_ACTION_OVER_VALUE_MEMBERS
//   #undef PGC_ACTION_OVER_MEMBER_REFS
//   #undef PGC_ACTION_OVER_MEMBER_CONTAINER_REFS
//
// Notes:
// - VALUE_MEMBER = plain field (e.g., int, std::string)
// - MEMBER_REF = promotion-aware pointer (PGCX::MemberRef<T>)
// - MEMBER_CONTAINER_REF = container that holds refs (e.g., PGC::PGC_Vector<PGCX::MemberRef<T>>)
// - INIT_CALL runs at the end of copy/move ctors (pass Initialize() or leave empty via _NOINIT)


// ----------------------------------------------------------
// VALUE MEMBERS (plain fields)
// ----------------------------------------------------------

#define PGC_DEFINE_VALUE_MEMBER(NAME, TYPE, VALUE) \
    TYPE m_##NAME = VALUE;

#define PGC_COPY_VALUE_MEMBER_INITIALIZER(NAME, TYPE, VALUE) \
    , m_##NAME(other.m_##NAME)

#define PGC_MOVE_VALUE_MEMBER_INITIALIZER(NAME, TYPE, VALUE) \
    , m_##NAME(std::move(other.m_##NAME))


// ----------------------------------------------------------
// MEMBER REFS (promotion-aware pointers)
// ----------------------------------------------------------

#define PGC_DEFINE_MEMBER_REF(NAME, TYPE) \
    PGCX::MemberRef<TYPE> m_##NAME;

#define PGC_MEMBER_REF_ACCESSORS(NAME, TYPE) \
    TYPE* Get##NAME() { return m_##NAME.Get(); } \
    void Set##NAME(TYPE* p_Ptr) { m_##NAME.Set(p_Ptr, GetTransaction()); }

#define PGC_COPY_MEMBER_REF_INITIALIZER(NAME, TYPE) \
    , m_##NAME(other.m_##NAME)

#define PGC_MOVE_MEMBER_REF_INITIALIZER(NAME, TYPE) \
    , m_##NAME(std::move(other.m_##NAME))

#define PGC_PROMOTE_MEMBER_REF_NOW(NAME, TYPE) \
    m_##NAME.PromoteNow(GetTransaction());

#define PGC_REQUEST_PROMOTION_MEMBER_REF(NAME, TYPE) \
    m_##NAME.RequestPromotion(GetTransaction());

#define PGC_REGISTER_MEMBER_REF(NAME, TYPE) \
    RegisterMember(reinterpret_cast<PGC::PGC_Base*>(m_##NAME.Get()));


// ----------------------------------------------------------
// MEMBER CONTAINER REFS (containers holding refs)
//   FULLTYPE: exact container type (e.g., PGC::PGC_Vector<PGCX::MemberRef<Foo>>)
//   ELEM_TYPE: element type (e.g., Foo)
// ----------------------------------------------------------

#define PGC_DEFINE_MEMBER_CONTAINER_REF(NAME, FULLTYPE, ELEM_TYPE) \
    FULLTYPE m_##NAME;

#define PGC_COPY_MEMBER_CONTAINER_REF_INITIALIZER(NAME, FULLTYPE, ELEM_TYPE) \
    , m_##NAME(other.m_##NAME)

#define PGC_MOVE_MEMBER_CONTAINER_REF_INITIALIZER(NAME, FULLTYPE, ELEM_TYPE) \
    , m_##NAME(std::move(other.m_##NAME))

// Promote all contained refs to this object's transaction
#define PGC_PROMOTE_MEMBER_CONTAINER_REF_ALL(NAME, FULLTYPE, ELEM_TYPE) \
    m_##NAME.PromoteAll();

// On copy, rebind into the new txn as well
#define PGC_REQUEST_PROMOTION_MEMBER_CONTAINER_REF_COPY(NAME, FULLTYPE, ELEM_TYPE) \
    m_##NAME.PromoteAll();


// ----------------------------------------------------------
// Promotion & Registration helpers
// ----------------------------------------------------------

#define PGC_PROMOTION_LOGIC() \
    void PromoteMembers() { \
        PGC_ACTION_OVER_MEMBER_REFS(PGC_PROMOTE_MEMBER_REF_NOW) \
        PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(PGC_PROMOTE_MEMBER_CONTAINER_REF_ALL) \
    }

#define PGC_REGISTRATION_LOGIC() \
    void RegisterMembers() { \
        PGC_ACTION_OVER_MEMBER_REFS(PGC_REGISTER_MEMBER_REF) \
    }


// ----------------------------------------------------------
// Constructors (copy/move) with INIT hook
// ----------------------------------------------------------

#define PGC_COPY_CONSTRUCTOR(T, BASE, INIT_CALL) \
    T(const T& other) \
        : BASE(other) \
          PGC_ACTION_OVER_VALUE_MEMBERS(PGC_COPY_VALUE_MEMBER_INITIALIZER) \
          PGC_ACTION_OVER_MEMBER_REFS(PGC_COPY_MEMBER_REF_INITIALIZER) \
          PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(PGC_COPY_MEMBER_CONTAINER_REF_INITIALIZER) \
    { \
        /* Rebind refs/containers into this object's transaction */ \
        PGC_ACTION_OVER_MEMBER_REFS(PGC_REQUEST_PROMOTION_MEMBER_REF) \
        PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(PGC_REQUEST_PROMOTION_MEMBER_CONTAINER_REF_COPY) \
        INIT_CALL; \
    }

#define PGC_MOVE_CONSTRUCTOR(T, BASE, INIT_CALL) \
    T(T&& other) noexcept \
        : BASE(std::move(other)) \
          PGC_ACTION_OVER_VALUE_MEMBERS(PGC_MOVE_VALUE_MEMBER_INITIALIZER) \
          PGC_ACTION_OVER_MEMBER_REFS(PGC_MOVE_MEMBER_REF_INITIALIZER) \
          PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(PGC_MOVE_MEMBER_CONTAINER_REF_INITIALIZER) \
    { \
        /* Rebind refs/containers into this object's transaction */ \
        PGC_ACTION_OVER_MEMBER_REFS(PGC_REQUEST_PROMOTION_MEMBER_REF) \
        PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(PGC_PROMOTE_MEMBER_CONTAINER_REF_ALL) \
        INIT_CALL; \
    }


// ----------------------------------------------------------
// Virtual clone/move (placement-new)
// ----------------------------------------------------------

#define PGC_CLONE_TO(T) \
    virtual PGC::PGC_Base* CloneTo(void* memory) const override { \
        return ::new (memory) T(*this); \
    }

#define PGC_MOVE_TO(T) \
    virtual PGC::PGC_Base* MoveTo(void* memory) override { \
        return ::new (memory) T(std::move(*this)); \
    }


// ----------------------------------------------------------
// Final glue: single entry macro
// ----------------------------------------------------------

#define PGC_MEMBER_DEFINITIONS(T, BASE, INIT_CALL) \
private: \
    /* Declarations */ \
    PGC_ACTION_OVER_VALUE_MEMBERS(PGC_DEFINE_VALUE_MEMBER) \
    PGC_ACTION_OVER_MEMBER_REFS(PGC_DEFINE_MEMBER_REF) \
    PGC_ACTION_OVER_MEMBER_CONTAINER_REFS(PGC_DEFINE_MEMBER_CONTAINER_REF) \
public: \
    /* Accessors for refs */ \
    PGC_ACTION_OVER_MEMBER_REFS(PGC_MEMBER_REF_ACCESSORS) \
    /* Ops */ \
    PGC_PROMOTION_LOGIC() \
    PGC_REGISTRATION_LOGIC() \
    /* Ctors */ \
    PGC_COPY_CONSTRUCTOR(T, BASE, INIT_CALL) \
    PGC_MOVE_CONSTRUCTOR(T, BASE, INIT_CALL) \
    /* Virtuals */ \
    PGC_CLONE_TO(T) \
    PGC_MOVE_TO(T)

#define PGC_MEMBER_DEFINITIONS_NOINIT(T, BASE) \
    PGC_MEMBER_DEFINITIONS(T, BASE, /* no init */)
