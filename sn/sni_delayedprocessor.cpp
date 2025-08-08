//  SNI_DelayedProcessor class
//
//  Description:
//      Some function calls do not have sufficient information yet to calculate the unknown variables.
//      The processor manages a list of delayed calls and performs calls with the greatest determinancy first
//      

#include "sni_delayedprocessor.h"
#include "sni_delayedcall.h"
#include "sni_world.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_DelayedProcessor::SNI_DelayedProcessor()
		: m_Processing(false)
		, m_Manager(SNI_Thread::GetThread()->GetTopManager(), GetTransaction())
		, m_DelayedCallList(GetTransaction())
		, m_FailedList(GetTransaction())
		, m_PreventReread(GetTransaction())
	{
	}

	SNI_DelayedProcessor::SNI_DelayedProcessor(SNI_Manager *p_Manager)
		: m_Processing(false)
		, m_Manager(p_Manager, GetTransaction())
		, m_DelayedCallList(GetTransaction())
		, m_FailedList(GetTransaction())
		, m_PreventReread(GetTransaction())
	{
	}

	SNI_DelayedProcessor::~SNI_DelayedProcessor()
	{
	}

	void SNI_DelayedProcessor::AddVariables(long p_MetaLevel, SNI_VariablePointerMap& p_Map)
	{
		for (SNI_DelayedCall* call : m_DelayedCallList)
		{
			if (call)
			{
				call->AddVariables(p_MetaLevel, p_Map);
			}
		}
	}

	// Create a delayed call and link it in as the value of the variables..
	void SNI_DelayedProcessor::Delay(SN::SN_FunctionDef p_Function, size_t p_NumParams, SN::SN_Expression *p_ParamList, const SNI_Expression *p_Source, SNI_World *p_World)
	{
		SNI_DelayedCall *call = new SNI_DelayedCall(p_Function, p_NumParams, p_ParamList, p_Source, SNI_Frame::Top(), p_World);
		call->LinkToVariables();
		m_SearchLock.lock();
		m_DelayedCallList.push_back(call);
		// REQUESTPROMOTION(m_DelayedCallList.back());
		m_SearchLock.unlock();
		if (call->IsCallRequested())
		{
			Request(call);
		}
		SNI_Thread::GetThread()->Breakpoint(SN::DebugStop, SN::DelayId, "Delay Processor", "Delayed call", p_Source, SN::WarningPoint);
	}

	void SNI_DelayedProcessor::DelayCall(SNI_DelayedCall * p_Call, SNI_World * p_World)
	{
		if (p_Call)
		{
			p_Call->SetWorld(p_World);
			p_Call->LinkToVariables();
		}
	}

	//  Post the delayed call to the list for processing.
	void SNI_DelayedProcessor::Request(SNI_DelayedCall *p_Call)
	{
		if (!p_Call->IsScheduled())
		{
			if (p_Call->IsCallRequested())
			{
				p_Call->MarkScheduled();
				//m_DelayedCallList.push_front(p_Call);
				Display();
			}
		}
		if (!m_Processing)
		{
			Process();
		}
	}

	SN::SN_Error SNI_DelayedProcessor::DoAssert()
	{
		for (SNI_DelayedCall* call : m_FailedList)
		{
			SN::SN_Error e = call->GetError();
			if (e.IsError())
			{
				return e;
			}
		}
		return Process();
	}

	SN::SN_Expression SNI_DelayedProcessor::DoEvaluate(long p_MetaLevel) const
	{
		return Check();
	}

	void SNI_DelayedProcessor::Run()
	{
		Process();
	}

	bool SNI_DelayedProcessor::Finish()
	{
		return true;
		// Process();
		// return m_DelayedCallList.size() == 0;
	}

	void SNI_DelayedProcessor::Promote(PGC::PGC_Transaction * p_Transaction)
	{
	}

	PreventRereadList& SNI_DelayedProcessor::GetPreventReread()
	{
		return m_PreventReread;
	}

	void SNI_DelayedProcessor::Display()
	{
		LOG(WriteLine(SN::DebugLevel, "Delayed Call List"));
		for (SNI_DelayedCall *call : m_DelayedCallList)
		{
			call->Display();
		}
	}

	void SNI_DelayedProcessor::RemoveEmptyCalls()
	{
		for (DelayedCallList::iterator callIt = m_DelayedCallList.begin(); callIt != m_DelayedCallList.end(); )
		{
			SNI_DelayedCall* call = *callIt;
			if (call->EmptyWorld())
			{
				callIt = m_DelayedCallList.erase(callIt);
			}
			else
			{
				callIt++;
			}
		}
	}


	// Process those calls in the list that can be processed, starting with the least cardinality.
	// Here, cardinality refers to the number of values. Nothing to do with multi threading.
	SN::SN_Error SNI_DelayedProcessor::Process()
	{
		if (m_Processing)
		{
			return skynet::OK;
		}
		m_Processing = true;
		bool found = true;
		bool success = false;
		SN::SN_Error e = skynet::OK;
		do
		{
			SNI_DelayedCall *call = NULL;
			size_t card = CARDINALITY_MAX;
			DelayedCallList::iterator it = m_DelayedCallList.end();
			long id = 0;
			m_SearchLock.lock();
			RemoveEmptyCalls();
			for (DelayedCallList::iterator loopIt = m_DelayedCallList.begin(); loopIt != m_DelayedCallList.end(); loopIt++)
			{
				SNI_DelayedCall *loopCall = *loopIt;
				if (!loopCall->IsLocked())
				{
					loopCall->ExpandedBooleanResult();
					size_t loopCard = loopCall->CallCardinality();
					SNI_Thread *thread = SNI_Thread::GetThread();
					if (thread)
					{
						m_SearchLock.unlock();
						thread->Breakpoint(SN::DebugStop, (SN::BreakId)id++, "Delayed", "Search min cardinality " + to_string(loopCard) + "<" + to_string(card), NULL, SN::DelayedPoint);
						m_SearchLock.lock();
					}
					if (loopCard < card)
					{
						call = loopCall;
						card = loopCard;
						it = loopIt;
					}
				}
			}
			if (card < CARDINALITY_MAX)
			{
				call->Lock();
			}
			m_SearchLock.unlock();

			if (card < CARDINALITY_MAX)
			{
				if (it != m_DelayedCallList.end())
				{
					m_SearchLock.lock();
					// Erase before calling Run(), as call->Run() may modify m_DelayedCallList
					it = m_DelayedCallList.erase(it);
					m_SearchLock.unlock();
				}
				e = call->Run();
				if (e.IsError())
				{
					m_FailedList.push_back(call);
				}
				call->Unlock();
				success = true;
			}
			else
			{
				found = false;
			}
		} while (found);
		m_Processing = false;
		return e;
	}

	SN::SN_Error SNI_DelayedProcessor::Check() const
	{
		for (SNI_DelayedCall* call : m_FailedList)
		{
			return call->GetError();
		}

		for (SNI_DelayedCall* call: m_DelayedCallList)
		{
			if (!call->EmptyWorld())
			{
				return SN::SN_Error(false, false, "Incomplete delayed calls.");
			}
		}

		return skynet::OK;
	}

	void SNI_DelayedProcessor::WriteJSON(ostream &p_Stream, SNI::SNI_DisplayOptions &p_DisplayOptions)
	{
		p_Stream << "{\"records\":[\n";
		string delimeter = "\n";
		m_SearchLock.lock();
		long id = 0;
		for (SNI_DelayedCall *call : m_DelayedCallList)
		{
			p_Stream << delimeter << "\t{\n";
			p_Stream << "\t\t\"id\" : \"" << id++ << "\",\n";
			call->WriteJSON(p_Stream, p_DisplayOptions);
			p_Stream << "\t}";
			delimeter = ",\n";
		}
		m_SearchLock.unlock();
		p_Stream << "\n]}\n";
	}

	size_t SNI_DelayedProcessor::CountDelayedCalls()
	{
		return m_DelayedCallList.size();
	}

	// Create a delayed call and link it in as the value of the variables..
	SN::SN_Error SNI_DelayedProcessor::Run(SN::SN_FunctionDef p_Function, size_t p_NumParams, SN::SN_Expression* p_ParamList, const SNI_Expression* p_Source, SNI_World* p_World)
	{
		SN::SN_Error e = skynet::OK;
		SNI_DelayedCall* call = new SNI_DelayedCall(p_Function, p_NumParams, p_ParamList, p_Source, SNI_Frame::Top(), p_World);
		call->LinkToVariables();
		call->ExpandedBooleanResult();
		if (call->CallCardinality() < CARDINALITY_MAX)
		{
			e = call->Run();
			if (e.IsError())
			{
				m_FailedList.push_back(call);
			}
		}
		else
		{
			m_SearchLock.lock();
			m_DelayedCallList.push_back(call);
			m_SearchLock.unlock();
			if (call->IsCallRequested())
			{
				Request(call);
			}
		}
		SNI_Thread::GetThread()->Breakpoint(SN::DebugStop, SN::DelayId, "Delay Processor", "Delayed call", p_Source, SN::WarningPoint);
		return e;
	}
}
