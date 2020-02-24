#include "utility.h"

#include "sn_parameter.h"

#include "sni_world.h"
#include "sni_worldset.h"
#include "sni_taggedvalue.h"
#include "sni_displayoptions.h"

#include "sn_pch.h"

#include <windows.h>

#include <boost/property_tree/json_parser/detail/write.hpp>
//#include <boost/property_tree/json_parser/detail/read.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>

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

	string EscapeStringToCPP(const std::string &p_Unescaped)
	{
		string out;
		for (std::string::const_iterator i = p_Unescaped.begin(), end = p_Unescaped.end(); i != end; ++i) {
			unsigned char c = *i;
			if (' ' <= c && c <= '~' && c != '\\' && c != '"') {
				out += c;
			}
			else {
				out += '\\';
				switch (c) {
				case '"':  out +=  '"'; break;
				case '\\': out +=  '\\'; break;
				case '\t': out +=  't'; break;
				case '\r': out +=  'r'; break;
				case '\n': out +=  'n'; break;
				default:
					char const* const hexdig = "0123456789ABCDEF";
					out += 'x';
					out += hexdig[c >> 4];
					out += hexdig[c & 0xF];
				}
			}
		}
		return out;
	}

	string UnescapeStringToCPP(const std::string &p_Escaped)
	{
		string out;
		for (std::string::const_iterator i = p_Escaped.begin(), end = p_Escaped.end(); i != end; ++i) {
			unsigned char c = *i;
			if (c == '\\')
			{
				c = *(++i);
				switch (c) {
				case '"':  out += '"'; break;
				case '\\': out += '\\'; break;
				case 't': out += '\t'; break;
				case 'r': out += '\r'; break;
				case 'n': out += '\n'; break;
				default:
					c = *(++i);
					if (c == 'x')
					{
						char hex_string[3];
						hex_string[0] = *(++i);;
						hex_string[1] = *(++i);;
						unsigned char hex_value = (unsigned char)std::strtoul(hex_string, 0, 16);
						out += hex_value;
					}
					else
					{
						out += c;
					}
				}
			}
			else
			{
				out += c;
			}
		}
		return out;
	}
	
	string EscapeStringToCPPUsingQuoted(const std::string &p_Unescaped)
	{
		std::stringstream ss;
		ss << std::quoted(p_Unescaped);
		return ss.str();
	}

	string UnescapeStringToCPPUsingQuoted(const std::string &p_Escaped)
	{
		std::stringstream ss('\"' + p_Escaped + '\"');
		std::string out;
		ss >> std::quoted(out);
		return out;
	}

	string EscapeStringToJSON(const string &p_Unescaped)
	{
		return boost::property_tree::json_parser::create_escapes<char>(p_Unescaped);
	}

	string UnescapeStringToJSON(const string &p_Escaped)
	{
		// How to do this?
		return p_Escaped;
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

	string DetailsFS(const string &p_Text, const string &p_TextHTML, size_t p_Width)
	{
		string abbreviation;
		string textHTML = EscapeStringToJSON(p_TextHTML);
		size_t textWidth = p_Text.size();
		if (p_Width < textWidth)
		{
			abbreviation = EscapeStringToJSON(p_Text.substr(0, p_Width - 3));
		}
		return "{\"abbreviation\" : \"" + abbreviation + "\",\"text\" : \"" + textHTML + "\"}";
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

	string DisplayPmTaggedValueList(const SNI_TaggedValueList &p_TaggedValueList, SNI_DisplayOptions & p_DisplayOptions)
	{
		string result;
		string separator;
		for (unsigned long j = 0; j < p_TaggedValueList.size(); j++)
		{
			result += separator + p_TaggedValueList[j].GetValue().DisplaySN(p_DisplayOptions);

			SNI_World *world = p_TaggedValueList[j].GetWorld();
			if (world)
			{
				result += "::" + p_TaggedValueList[j].GetWorld()->DisplaySN(p_DisplayOptions);
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
					result += "::" + p_TaggedValueList[j].GetWorld()->DisplaySN(p_DisplayOptions);
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
		SNI_DisplayOptions displayOptions(doTextOnly);
		return DisplayWorldsD(displayOptions, p_NumWorlds, p_World);
	}

	string DisplayWorldsD(SNI_DisplayOptions &p_DisplayOptions, long p_NumWorlds, SNI_World ** p_World)
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
				result += p_World[j]->DisplaySN(p_DisplayOptions);
			}
		}
		return "worlds " + result;
	}

	string DisplayValues(long p_NumWorlds, SN::SN_Expression * p_ParamList, SNI_World ** p_World)
	{
		SNI_DisplayOptions displayOptions(doTextOnly);
		return DisplayValuesD(displayOptions, p_NumWorlds, p_ParamList, p_World);
	}
	
	string DisplayValuesD(SNI_DisplayOptions & p_DisplayOptions, long p_NumWorlds, SN::SN_Expression * p_ParamList, SNI_World ** p_World)
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
					result += "::" + p_World[j]->DisplaySN(p_DisplayOptions);
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

	string MakeBreakPoint(const string &p_DebugId, long p_Index)
	{
		return p_DebugId + "_" + to_string(p_Index);
	}

	string MakeBreakPointJS(const string &p_DebugId, long p_Index)
	{
		return "[\"" + p_DebugId + "\"," + to_string(p_Index) + "]";
	}
}