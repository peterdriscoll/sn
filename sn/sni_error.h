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
	class SNI_Error : public SNI_Value
	{
		PGC_CLASS(SNI_Error);
	public:
		static bool ErrorIsHandled();
		static void PushHandler();
		static void PopHandler();

		SNI_Error();
		SNI_Error(bool p_Success, bool p_Delay = false, const string &p_Description = "");
		SNI_Error(const SNI_Error &p_Other);
		SNI_Error(const string &p_Description);
		virtual ~SNI_Error();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual void AddNote(SNI_CallRecord *p_CallRecord);

		virtual bool IsNull() const;
		virtual bool IsError() const;

		virtual bool GetBool();
		virtual bool GetDelay();
		virtual string GetDescription();

		string GetLogDescription();
		virtual void Log();

	private:
		bool m_Success;
		bool m_Delay;
		string m_Description;
		SNI_CallRecordList m_CallHistory;
	};

	typedef vector<SNI_Error> SNI_ErrorList;
}

#endif // !defined(SNI_ERROR_H_INCLUDED)
