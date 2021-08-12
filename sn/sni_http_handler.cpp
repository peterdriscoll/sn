#include "sni_http_handler.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_HTTP_Handler::SNI_HTTP_Handler()
	{
	}

	SNI_HTTP_Handler::~SNI_HTTP_Handler()
	{
	}

	bool SNI_HTTP_Handler::handle_response(const char *p_Path, const char *p_ParamString, const char *p_Extension)
	{

		SN::SN_Transaction::RegisterInWebServer();

		string path = p_Path;
		string paramString = p_ParamString;
		m_extension = p_Extension;

		string_umap umap;
		extract_parameters(paramString, umap);

		m_response_data = "";

		long threadNum = atol(umap["threadnum"].c_str());
		DisplayOptionType debugHTML = doDebugPointsHTML;
		DisplayOptionType debugJS = doDebugPointsJS;
		bool result = true;

		SNI::SNI_Thread *l_thread = SNI::SNI_Thread::GetThreadByNumber(threadNum);
		if (l_thread)
		{
			if (path == "/skynet")
			{
				m_response_data = l_thread->Skynet(debugHTML);
			}
			else if (path == "/run")
			{
				m_response_data = l_thread->RunWeb(debugHTML);
			}
			else if (path == "/runjs")
			{
				l_thread->Run();
			}
			else if (path == "/runtoend")
			{
				m_response_data = l_thread->RunToEndWeb(debugHTML);
			}
			else if (path == "/runtoendjs")
			{
				l_thread->RunToEnd();
			}
			else if (path == "/debug")
			{
				m_response_data = l_thread->DebugWeb(debugHTML);
			}
			else if (path == "/debugjs")
			{
				string breakpoints = umap["breakpoints"].c_str();
				l_thread->LoadBreakPoints(breakpoints);
				l_thread->Debug();
			}
			else if (path == "/codebreak")
			{
				m_response_data = l_thread->CodeBreakWeb(debugHTML);
			}
			else if (path == "/codebreakjs")
			{
				l_thread->CodeBreak();
			}
			else if (path == "/rerunjs")
			{
				l_thread->Rerun();
			}
			else if (path == "/stepover")
			{
				m_response_data = l_thread->StepOverWeb(debugHTML);
			}
			else if (path == "/stepoverjs")
			{
				l_thread->StepOver();
			}
			else if (path == "/stepinto")
			{
				m_response_data = l_thread->StepIntoWeb(debugHTML);
			}
			else if (path == "/stepintojs")
			{
				SN::DebuggingStop debugStop = (SN::DebuggingStop)atol(umap["debugstop"].c_str());
				l_thread->StepInto(debugStop);
			}
			else if (path == "/stepout")
			{
				m_response_data = l_thread->StepOutWeb(debugHTML);
			}
			else if (path == "/stepoutjs")
			{
				l_thread->StepOut();
			}
			else if (path == "/stepoutcalljs")
			{
				l_thread->StepOutCall();
			}
			else if (path == "/stepparam")
			{
				m_response_data = l_thread->StepParamWeb(debugHTML);
			}
			else if (path == "/stepparamjs")
			{
				l_thread->StepParam();
			}
			else if (path == "/gotostepcount")
			{
				long stepCount = atol(umap["stepcount"].c_str());
				if (0 < stepCount)
				{
					m_response_data = l_thread->GotoStepCountWeb(stepCount, debugHTML);
				}
			}
			else if (path == "/gotostepcountjs")
			{
				long stepCount = atol(umap["stepcount"].c_str());
				if (0 < stepCount)
				{
					l_thread->GotoStepCount(stepCount);
				}
			}
			else if (path == "/maxstackframes")
			{
				long maxStackFrames = atol(umap["maxstackframes"].c_str());
				m_response_data = l_thread->SetMaxStackFramesWeb(maxStackFrames, debugHTML);
			}
			else if (path == "/thread")
			{
				m_response_data = l_thread->Skynet(debugHTML);
			}
			else if (path == "/quit")
			{
				m_response_data = l_thread->QuitWeb(debugHTML);
			}
			else if (path == "/quitjs")
			{
				l_thread->Quit();
			}
			else if (path == "/skynet")
			{
				m_response_data = l_thread->Skynet(debugHTML);
			}
			else if (path == "/dashboard.json")
			{
				m_response_data = l_thread->DashboardJS(debugJS);
			}
			else if (path == "/stack.json")
			{
				long maxStackFrames = atol(umap["maxstackframes"].c_str());
				long startStackFrames = atol(umap["startstackframes"].c_str());
				long stepCount = atol(umap["stepcount"].c_str());
				m_response_data = l_thread->StackJS(maxStackFrames, startStackFrames, stepCount, debugJS);
			}
			else if (path == "/callstack.json")
			{
				long maxCallStackFrames = atol(umap["maxcallstackframes"].c_str());
				long startCallStackFrames = atol(umap["startcallstackframes"].c_str());
				long stepCount = atol(umap["stepcount"].c_str());
				m_response_data = l_thread->CallStackJS(maxCallStackFrames, startCallStackFrames, stepCount, debugJS);
			}
			else if (path == "/watchlist.json")
			{
				long stepCount = atol(umap["stepcount"].c_str());
				m_response_data = l_thread->WatchListJS(stepCount, debugJS);
			}
			else if (path == "/changehistory.json")
			{
				size_t fromstep = atol(umap["fromstep"].c_str());
				size_t tostep = atol(umap["tostep"].c_str());
				size_t columnwidth = atol(umap["columnwidth"].c_str());
				m_response_data = l_thread->ChangeHistoryJS(debugJS, columnwidth, fromstep, tostep);
			}
			else if (path == "/log.json")
			{
				long maxLogEntries = atol(umap["maxlogentries"].c_str());
				long startLog = atol(umap["startlog"].c_str());
				long stepCount = atol(umap["stepcount"].c_str());
				m_response_data = l_thread->LogJS(maxLogEntries, startLog, stepCount);
			}
			else if (path == "/derivation.json")
			{
				long maxLogEntries = atol(umap["maxlogentries"].c_str());
				m_response_data = l_thread->DerivationJS(maxLogEntries);
			}
			else if (path == "/code.json")
			{
				long maxCode = atol(umap["maxcode"].c_str());
				long startCode = atol(umap["startcode"].c_str());
				long stepCount = atol(umap["stepcount"].c_str());
				m_response_data = l_thread->CodeJS(maxCode, startCode, stepCount, debugJS);
			}
			else if (path == "/stepcount.json")
			{
				m_response_data = l_thread->StepCountJS();
			}
			else if (path == "/error.json")
			{
				m_response_data = l_thread->ErrorJS(debugJS);
			}
			else if (path == "/worldsets.json")
			{
				m_response_data = l_thread->WorldSetsJS(debugJS);
			}
			else if (path == "/delayed.json")
			{
				m_response_data = l_thread->DelayedJS(debugJS);
			}
			else
			{
				result = false;
			}
		}
		else
		{
			result = false;
		}
		return result;
	}

	const char * SNI_HTTP_Handler::extension()
	{
		return m_extension.data();
	}

	const char * SNI_HTTP_Handler::response_data()
	{
		return m_response_data.data();
	}

	void SNI_HTTP_Handler::extract_parameters(const string &p_ParamString, string_umap &p_Map)
	{
		string result;
		vector<string> assignmentList;
		if (!p_ParamString.empty())
		{
			Split(p_ParamString, "&", assignmentList);
			for (const string &element : assignmentList)
			{
				vector<string> tokenList;
				Split(element, "=", tokenList);
				if (tokenList.size() == 2)
				{
					string t0 = tokenList[0];
					string t1 = tokenList[1];
					p_Map[tokenList[0]] = tokenList[1];
					string r1 = p_Map[tokenList[0]];
				}
			}
		}
	}
}

