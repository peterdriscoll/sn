#pragma once

// ------------------------------
// Member and container action macros
// ------------------------------

#define PGC_DEFINE_MEMBER(MEMBER, TYPE) \
    MemberRef<TYPE> m_##MEMBER;
#define PGC_DEFINE_CONTAINER_MEMBER(CONTAINER, LISTTYPE, TYPE) \
    LISTTYPE<MemberRef<TYPE>> m_##CONTAINER;

#define PGC_GETSET_MEMBER(MEMBER, TYPE) \
    TYPE *Get##MEMBER() \
    { \
        return m_##MEMBER.Get(); \
    } \
    void Set##MEMBER(TYPE *p_Pointer) \
    { \
        m_##MEMBER.Set(p_Pointer, GetTransaction()); \
    }

#define PGC_REQUEST_PROMOTION_CONTAINER_MEMBER(CONTAINER, LISTTYPE, TYPE) \
    void RequestPromotion##CONTAINER() \
    { \
        for (auto& _ref : m_##CONTAINER) \
            _ref.RequestPromotion(GetTransaction()); \
    }


// ------------------------------
// Promotion macros
// ------------------------------

#define PGC_PROMOTE_MEMBER(MEMBER, TYPE) \
    m_##MEMBER.RequestPromotion(GetTransaction());

#define PGC_PROMOTE_CONTAINER(CONTAINER, LISTTYPE, TYPE) \
    for (auto& _ref : m_##CONTAINER) \
        _ref.RequestPromotion(GetTransaction());

// ------------------------------
// registration
// ------------------------------

#define PGC_REGISTER_MEMBER(MEMBER, TYPE) \
    RegisterMember((PGC::PGC_Base *)m_##MEMBER.Get());

// Example setup
#define PGC_ACTION_OVER_MEMBERS(ACTION) \
    ACTION(Condition, SNI_Expression) \
    ACTION(Body, SNI_Expression)

#define PGC_ACTION_OVER_CONTAINERS(ACTION) \
    ACTION(Collection, std::vector, SNI_Expression)

#define PGC_COPY_CONSTRUCTOR(T) \
    T(const T& other) \
        : Base(other) \
          PGC_ACTION_OVER_MEMBERS(PGC_COPY_MEMBER_INITIALIZER) \
          PGC_ACTION_OVER_CONTAINERS(PGC_COPY_CONTAINER_INITIALIZER) \
    { \
        PGC_ACTION_OVER_MEMBERS(PGC_REQUEST_PROMOTION_MEMBER) \
        PGC_ACTION_OVER_CONTAINERS(PGC_REQUEST_PROMOTION_CONTAINER_COPY) \
    }

#define PGC_COPY_MEMBER_INITIALIZER(MEMBER, TYPE) \
    , m_##MEMBER(other.m_##MEMBER)

#define PGC_COPY_CONTAINER_INITIALIZER(CONTAINER, LISTTYPE, TYPE) \
    , m_##CONTAINER(other.m_##CONTAINER)

#define PGC_REQUEST_PROMOTION_MEMBER(MEMBER, TYPE) \
    m_##MEMBER.RequestPromotion(GetTransaction());

#define PGC_REQUEST_PROMOTION_CONTAINER_COPY(CONTAINER, LISTTYPE, TYPE) \
    for (auto& _ref : m_##CONTAINER) \
        _ref.RequestPromotion(GetTransaction());

#define PGC_CLONE_TO(T) \
	virtual PGC_Base* CloneTo(void* memory) const override { \
		return new (memory) T(*this); \
	}

#define PGC_DEFINE_MEMBERS(T) \
    PGC_ACTION_OVER_MEMBERS(PGC_DEFINE_MEMBER) \
    PGC_ACTION_OVER_CONTAINERS(PGC_DEFINE_CONTAINER_MEMBER)

#define PGC_MEMBER_ACCESSORS(T) \
    PGC_ACTION_OVER_MEMBERS(PGC_GETSET_MEMBER) \
    PGC_ACTION_OVER_CONTAINERS(PGC_REQUEST_PROMOTION_CONTAINER_MEMBER)

#define PGC_PROMOTION_LOGIC(T) \
    void PromoteMembers() \
    { \
        PGC_ACTION_OVER_MEMBERS(PGC_PROMOTE_MEMBER) \
        PGC_ACTION_OVER_CONTAINERS(PGC_PROMOTE_CONTAINER) \
    }

#define PGC_REGISTRATION_LOGIC(T) \
    void RegisterMembers() \
    { \
        PGC_ACTION_OVER_MEMBERS(PGC_REGISTER_MEMBER) \
    }

#define PGC_MEMBER_DEFINITIONS(T) \
private: \
    PGC_DEFINE_MEMBERS(T) \
public: \
    PGC_MEMBER_ACCESSORS(T) \
    PGC_PROMOTION_LOGIC(T) \
    PGC_REGISTRATION_LOGIC(T) \
    PGC_COPY_CONSTRUCTOR(T) \
    PGC_CLONE_TO(T)
