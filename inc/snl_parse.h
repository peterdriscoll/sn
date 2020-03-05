#if !defined(SNL_PARSE_H_INCLUDED)
#define SNL_PARSE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn_lib.h"
#include "sn.h"
#include "snl_Validate.h"

namespace SNL
{
	class SN_LIB_EXPORT SNL_Parse: public skynet::Domain
	{
	public:
		SNL_Parse(const SNL_Validate &p_Validate);

		skynet::Variable Integer;
		skynet::Variable Scientific;
		skynet::Variable Number;

		skynet::Variable Name;
		skynet::Variable String;
		skynet::Variable Value;

		private:
			void Setup();
			void Link();
			void Init();

			const SNL_Validate &m_Validate;
	};
}

#endif // !defined(SNP_PARSER_H_INCLUDED)
