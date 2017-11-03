#if !defined(SNI_RESULT_H_INCLUDED)
#define SNI_RESULT_H_INCLUDED

#pragma once

#define NULL 0

namespace SNI
{
	class SNI_DelayedCall;
	class SNI_Error;
	class SNI_World;

	class SNI_Result
	{
	public:
		SNI_Result();
		SNI_Result(bool  p_Result, bool p_Delay = false, SNI_DelayedCall *p_Call = NULL, SNI_Error *p_Error = NULL);
		~SNI_Result();

		bool             GetResult();
		bool             GetDelay();
		SNI_DelayedCall *GetCall();

		void ProcessWorld(SNI_World * p_World);
	private:
		bool             m_Result;
		bool             m_Delay;
		SNI_DelayedCall *m_Call;
		SNI_Error       *m_Error;
	};
}

#endif // !defined(SNI_RESULT_H_INCLUDED)
