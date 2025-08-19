#pragma once

#include "pgc.h"

#include "sni_value.h"
#include "sni_callrecord.h"

#include <string>

namespace SN
{
    class LogContext;
}

namespace SNI
{
    class SNI_Error;

    // Promotion-aware container aliases
    using SNI_ErrorList = PGC::vector_ref<SNI_Error>;
    using SNI_CallRecordList = PGC::vector_ref<SNI_CallRecord>;

    // Reset any prior macro sets (mirrors other PGC-ified headers)
#undef PGC_ACTION_OVER_VALUES
#undef PGC_ACTION_OVER_MEMBERS
#undef PGC_ACTION_OVER_CONTAINERS

// Values tracked as plain fields
#define PGC_ACTION_OVER_VALUES(ACTION) \
        ACTION(Success,      bool,        true) \
        ACTION(Delay,        bool,        false) \
        ACTION(RequestRerun, bool,        false) \
        ACTION(Description,  std::string, std::string())

    // No MemberRef<> singletons on this class (yet)
#define PGC_ACTION_OVER_MEMBERS(ACTION)

// Promotion-aware containers
#define PGC_ACTION_OVER_CONTAINERS(ACTION) \
        ACTION(CallHistory,   SNI_CallRecordList, SNI_CallRecord) \
        ACTION(ChildErrorList, SNI_ErrorList,      SNI_Error)

    class SNI_Error : public SNI_Value
    {
        PGC_CLASS(SNI_Error);

        using Base = SNI_Value;

        //PGC_MEMBER_DEFINITIONS(SNI_Error);
    private:
        // --- Values ---
        bool        m_Success = true;
        bool        m_Delay = false;
        bool        m_RequestRerun = false;
        std::string m_Description;

        // --- Containers (promotion-aware vectors) ---
        SNI_CallRecordList m_CallHistory;
        SNI_ErrorList      m_ChildErrorList;

    public:
        // --- Dtor ---
        virtual ~SNI_Error() {}

        // --- Copy ctor: rebind containers to this object's transaction and promote ---
        SNI_Error(const SNI_Error& other)
            : Base(other)
            , m_Success(other.m_Success)
            , m_Delay(other.m_Delay)
            , m_RequestRerun(other.m_RequestRerun)
            , m_Description(other.m_Description)
            , m_CallHistory(other.m_CallHistory, GetTransaction())
            , m_ChildErrorList(other.m_ChildErrorList, GetTransaction())
        {
            m_CallHistory.PromoteAll();
            m_ChildErrorList.PromoteAll();
        }

        // --- Move ctor: rebind containers to this object's transaction and promote ---
        SNI_Error(SNI_Error&& other) noexcept
            : Base(std::move(other))
            , m_Success(other.m_Success)
            , m_Delay(other.m_Delay)
            , m_RequestRerun(other.m_RequestRerun)
            , m_Description(std::move(other.m_Description))
            , m_CallHistory(other.m_CallHistory, GetTransaction())
            , m_ChildErrorList(other.m_ChildErrorList, GetTransaction())
        {
            m_CallHistory.PromoteAll();
            m_ChildErrorList.PromoteAll();
        }

        // --- Clone helpers for PGC ---
        virtual PGC::PGC_Base* CloneTo(void* memory) const override
        {
            return new (memory) SNI_Error(*this);
        }

        virtual PGC::PGC_Base* MoveTo(void* memory) override
        {
            return new (memory) SNI_Error(std::move(*this));
        }

        // --- PGC hook (no MemberRef<> singletons to promote here; containers promote themselves) ---
        void PromoteMembers() override
        {
            m_CallHistory.PromoteAll();
            m_ChildErrorList.PromoteAll();
        }
        //End PGC_MEMBER_DEFINITIONS(SNI_Error);

        // --- Primary ctor ---
        SNI_Error(bool p_Success, bool p_Delay = false, const std::string& p_Description = "");

        // --- Default ctor: bind containers to this object's transaction and default to "success" ---
        SNI_Error()
            : m_Success(true)
            , m_Delay(false)
            , m_RequestRerun(false)
            , m_Description()
            , m_CallHistory(GetTransaction())
            , m_ChildErrorList(GetTransaction())
        {
        }

    public:
        // Static error-handling depth helpers
        static bool ErrorIsHandled();
        static void PushHandler();
        static void PopHandler();

        // RTTI-ish class info
        static SNI_Class* m_Class;
        static SNI_Class* Class();
        virtual SNI_Class* VClass();

        // SN interface
        virtual SNI_Expression* Copy() const;

        virtual std::string GetTypeName() const;
        virtual std::string DisplayCpp() const;
        virtual std::string DisplaySN(long priority, SNI_DisplayOptions& p_DisplayOptions) const;
        virtual long GetPriority() const;

        virtual void AddNote(SNI_CallRecord* p_CallRecord);

        virtual bool IsNull() const;
        void AddError(const SN::SN_Error& p_Error);
        void CheckChildError();
        virtual bool IsError() const;
        bool IsErrorType() const;
        bool IsSignificantError() const;

        virtual bool GetBool();
        virtual bool GetDelay();
        virtual bool RequestRerun();
        void MakeRerunRequest();
        virtual std::string GetDescription();

        void WriteJSON(ostream& p_Stream, SNI::SNI_DisplayOptions& p_DisplayOptions);
        std::string GetLogDescription();
        virtual void Log();
    };
}