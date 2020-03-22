#if !defined(SNI_ERROR_H_INCLUDED)
#define SNI_ERROR_H_INCLUDED

#pragma once

#include <string>

#include "sni_value.h"
#include "sni_callrecord.h"

namespace SN
{
	class LogContext;
}

namespace SNI
{
	class SNI_Error;

	typedef vector<SNI_Error *> SNI_ErrorList;

	class SNI_Error : public SNI_Value
	{
		PGC_CLASS(SNI_Error);
	public:
		static bool ErrorIsHandled();
		static void PushHandler();
		static void PopHandler();

		static SNI_Class * m_Class;
		static SNI_Class * Class();
		virtual SNI_Class *VClass();

		SNI_Error();
		SNI_Error(bool p_Success, bool p_Delay, const string &p_Description = "");
		SNI_Error(const SNI_Error &p_Other);
		virtual ~SNI_Error();

		virtual SNI_Expression * Copy() const;

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual void AddNote(SNI_CallRecord *p_CallRecord);

		virtual bool IsNull() const;
		void AddError(SN::SN_Error & p_Error);
		void CheckChildError();
		virtual bool IsError() const;
		bool IsSignificantError() const;

		virtual bool GetBool();
		virtual bool GetDelay();
		virtual bool RequestRerun();
		void MakeRerunRequest();
		virtual string GetDescription();

		void WriteJS(ostream & p_Stream, SNI::SNI_DisplayOptions & p_DisplayOptions);
		string GetLogDescription();
		virtual void Log();

	private:
		bool m_Success;
		bool m_Delay;
		bool m_RequestRerun;
		string m_Description;
		SNI_CallRecordList m_CallHistory;
		SNI_ErrorList m_ChildErrorList;
	};
}

#endif // !defined(SNI_ERROR_H_INCLUDED)
