#include "utility.h"

#include "sn_parameter.h"

#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_taggedvalue.h"

#include "sn_pch.h"

#ifdef _WIN32
#include <ShellApi.h>
#else
#include <stdlib.h>
#endif

namespace SNI
{
	string CurrentWorkingDirectory()
	{
		char* cwd = _getcwd(0, 0); // **** microsoft specific ****
		string working_directory(cwd);
		free(cwd);
		return working_directory;
	}

	string ExtractDirPath(const string& str)
	{
		size_t found = str.find_last_of("/\\");
		return str.substr(0, found);
	}

	void OpenURLInBrowser(const string& str)
	{
#ifdef _WIN32
		ShellExecuteA(NULL, "open", str.c_str(), NULL, NULL, SW_SHOW);
#else
		system((string("open")+ str).c_str());
#endif
	}

	// Returns the local date/time formatted as 2014-03-19 11:11:52
	string GetFormattedTime(void) {

		time_t rawtime;
		struct tm timeinfo;

		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);

		// Must be static, otherwise won't work
		static char _retval[20];
		strftime(_retval, sizeof(_retval), "%Y%m_%d_%H_%M_%S", &timeinfo);

		return _retval;
	}

	void Split(const string& a_buffer,
		const string& a_delimeter,
		vector<string>& a_result)
	{
		string tmp = a_buffer;
		string substring;

		a_result.clear();

		if (a_delimeter == "\n") {
			// Replace all \r with \n
			for (size_t i = 0; i < tmp.length(); i++)
			{
				if (tmp[i] == '\r')
				{
					tmp[i] = '\n';
				}
			}
		}
		bool isTrue = true;
		while (isTrue)
		{
			size_t index = tmp.find(a_delimeter, 1);
			if (index == string::npos)
			{
				a_result.push_back(tmp);
				return;
			}
			a_result.push_back(tmp.substr(0, index));
			tmp = tmp.substr(index + a_delimeter.length());
		};
	}

	string ReplaceAll(const string &source, const string &target, const string &replace)
	{
		string::size_type pos;
		string::size_type lastPos = 0;
		string result;
		while ((pos = source.find(target, lastPos)) != string::npos)
		{
			result += source.substr(lastPos, pos - lastPos) + replace;
			lastPos = pos + target.length();
		}
		result += source.substr(lastPos);
		return result;
	}

	string DisplayPmExpressionList(SN::SN_ExpressionList * p_ParameterList)
	{
		string result;
		string separator;
		for (unsigned long j = 0; j < p_ParameterList->size(); j++)
		{
			result += separator + (*p_ParameterList)[j].DisplayValueSN();
			separator = ", ";
		}
		return result;
	}

	string Pad(const string &p_Text, size_t p_Width)
	{
		size_t textWidth = p_Text.size();
		size_t spacerWidth = 0;
		string text = p_Text;
		if (p_Width > textWidth)
		{
			spacerWidth = p_Width - textWidth;
		}
		else if (p_Width < textWidth)
		{
			text = p_Text.substr(0, p_Width - 3) + "...";
		}
		return text + string(spacerWidth, ' ');
	}

	string Trim(const string &p_Text, size_t p_Width)
	{
		size_t textWidth = p_Text.size();
		string text = p_Text;
		if (p_Width < textWidth)
		{
			text = p_Text.substr(0, p_Width - 3) + "...";
		}
		return text;
	}

	string Details(const string &p_Text, size_t p_Width)
	{
		size_t textWidth = p_Text.size();
		string text = p_Text;
		if (p_Width < textWidth)
		{
			text =
				"<details>"
				"<summary>" + p_Text.substr(0, p_Width - 3) + "...</summary>"
				"<p>" + p_Text + "</p>"
				"</details>";
		}
		return text;
	}

	string DetailsFS(const string &p_Text, size_t p_Width)
	{
		size_t textWidth = p_Text.size();
		string abbreviation;
		string text = p_Text;
		ReplaceAll(text, "\"", "\\\"");
		if (p_Width < textWidth)
		{
			abbreviation =p_Text.substr(0, p_Width - 3);
			ReplaceAll(abbreviation, "\"", "\\\"");
		}
		return "{\"abbreviation\" : \"" + abbreviation + "\",\"text\" : \"" + text + "\"}";
	}

	string DisplayPmValueList(const SN::SN_ValueList &p_ParameterList)
	{
		string result;
		string separator;
		for (unsigned long j = 0; j < p_ParameterList.size(); j++)
		{
			result += separator + p_ParameterList[j].DisplayValueSN();
			separator = ", ";
		}
		return result;
	}

	string DisplayPmParameterList(SN::SN_ParameterList * p_ParameterList)
	{
		string result;
		string separator;
		for (unsigned long j = 0; j < p_ParameterList->size(); j++)
		{
			result += separator + (*p_ParameterList)[j].GetValue().DisplaySN();
			if (!(*p_ParameterList)[j].GetCondition().IsNull())
			{
				result += ": " + (*p_ParameterList)[j].GetCondition().DisplaySN();
			}
			separator = ", ";
		}
		return result;
	}

	string DisplayPmTaggedValueList(const SNI_TaggedValueList &p_TaggedValueList)
	{
		string result;
		string separator;
		for (unsigned long j = 0; j < p_TaggedValueList.size(); j++)
		{
			result += separator + p_TaggedValueList[j].GetValue().DisplaySN();

			SNI_World *world = p_TaggedValueList[j].GetWorld();
			if (world)
			{
				result += "::" + p_TaggedValueList[j].GetWorld()->DisplaySN();
			}
			separator = ", ";
		}
		return result;
	}

	string DisplayPmTaggedExpressionList(const SNI_TaggedValueList &p_TaggedValueList, SNI_DisplayOptions & p_DisplayOptions)
	{
		string result;
		string separator;
		for (unsigned long j = 0; j < p_TaggedValueList.size(); j++)
		{
			SNI_World *world = p_TaggedValueList[j].GetWorld();
			if (!world || !world->IsEmpty())
			{
				result += separator + p_TaggedValueList[j].GetValue().GetSNI_Expression()->DisplaySN(0, p_DisplayOptions);
				if (world)
				{
					result += "::" + p_TaggedValueList[j].GetWorld()->DisplayShort();
				}
				separator = ", ";
			}
		}
		return result;
	}

	string DisplayPmExpression(SNI_Expression *p_Expression)
	{
		if (p_Expression)
		{
			return SN::SN_Expression(p_Expression).DisplayValueSN();
		}
		return "Null";
	}

	string DisplayPmExpression(const SN::SN_Expression &p_Expression)
	{
		if (p_Expression.GetSNI_Expression())
		{
			return p_Expression.DisplayValueSN();
		}
		return "Null";
	}

	string DisplayWorlds(long p_NumWorlds, SNI_World ** p_World)
	{
		if (!p_World)
		{
			return "";
		}
		string result;
		for (long j = 0; j < p_NumWorlds; j++)
		{
			if (result.length())
			{
				result += ", ";
			}
			if (p_World[j])
			{
				result += p_World[j]->DisplaySN();
			}
		}
		return "worlds " + result;
	}

	string DisplayValues(long p_NumWorlds, SN::SN_Expression * p_ParamList, SNI_World ** p_World)
	{
		string result;
		for (long j = 0; j < p_NumWorlds; j++)
		{
			if (result.length())
			{
				result += ", ";
			}
			result += p_ParamList[j].DisplayValueSN();
			if (p_World)
			{
				if (p_World[j])
				{
					result += "::" + p_World[j]->DisplaySN();
				}
			}
		}
		return "values " + result;
	}

	string DisplayWorldSet(SNI_WorldSet *p_WorldSet)
	{
		if (p_WorldSet)
		{
			return p_WorldSet->DisplaySN();
		}
		return "";
	}
}