#if !defined(SNL_PARSER_H_INCLUDED)
#define SNL_PARSER_H_INCLUDED

#pragma once

#include "exp_ctrl_sn_lib.h"
#include "sn.h"
#include "snl_Validate.h"

namespace SNL
{
	class SN_LIB_EXPORT SNL_Parser: public skynet::Domain
	{
	public:
		SNL_Parser(const SNL_Validate &p_Validate);

		skynet::Variable ParseInteger;
		skynet::Variable ParseName;
		skynet::Variable ParseString;
		skynet::Variable ParseValue;

		private:
			void Setup();
			void Link();
			void Init();

			const SNL_Validate &m_Validate;
	};
}

#endif // !defined(SNP_PARSER_H_INCLUDED)
