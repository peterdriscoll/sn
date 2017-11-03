#if !defined(SN_SN_TAGGEDVALUE_H_INCLUDED_)
#define SN_SN_TAGGEDVALUE_H_INCLUDED_

#pragma once

#include "exp_ctrl_sn.h"
#include "SNI::SNI_taggedvalue.h"

#include <string>
#include <vector>
using namespace std;

class SNI::SNI_TaggedValue;

class SN_EXPORT SN_TaggedValue  
{
public:
	SN_TaggedValue();
	virtual ~SN_TaggedValue();
	SN_TaggedValue(const SN_Value &p_Value);

private:
	SNI::SNI_TaggedValue * m_TaggedValue;
	
};
typedef NovaList<SN_TaggedValue> SN_TaggedValueList;
#endif // !defined(SN_SN_TAGGEDVALUE_H_INCLUDED_)
