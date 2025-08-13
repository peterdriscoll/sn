#if !defined(SN_HTTP_HANDLER_H)
#define SN_HTTP_HANDLER_H

#pragma once

#include "ihttp_handler.h"
#include <string>
#include <unordered_map>

using namespace std;

typedef std::unordered_map<std::string, std::string> string_umap;

namespace SNI
{
	class SNI_HTTP_Handler : public IHTTP_Handler
	{
	public:
		SNI_HTTP_Handler();
		virtual ~SNI_HTTP_Handler();

		virtual bool handle_response(const char *p_Path, const char *p_ParamString, const char *p_Extension);
		virtual const char *response_data();
		virtual const char *extension();

	private:
		void extract_parameters(const std::string & p_ParamString, string_umap & p_Map);

		std::string m_response_data;
		std::string m_extension;
	};
}

#endif // SN_HTTP_HANDLER_H