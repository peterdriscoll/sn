#if !defined(SN_SN_TAGGEDVALUE_H_INCLUDED_)
#define SN_SN_TAGGEDVALUE_H_INCLUDED_

#pragma once

#include "exp_ctrl_sn.h"
#include "sn.h"

class SN_TaggedValue  
{
public:
	SN_TaggedValue();
	SN_TaggedValue(const SN_Value &p_Value);
	virtual ~SN_TaggedValue();

private:
	SNI::SNI_TaggedValue m_TaggedValue;
	//SN_Expression m_Value;
	//SNI::SNI_World *  m_World;
};

#endif // !defined(SN_SN_TAGGEDVALUE_H_INCLUDED_)
