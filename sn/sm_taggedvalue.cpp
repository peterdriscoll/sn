#include "sn_taggedvalue.h"

SN_TaggedValue::SN_TaggedValue()
{

}

SN_TaggedValue::SN_TaggedValue(const SN_Value &p_Value)
:m_TaggedValue(dynamic_cast<SNI::SNI_TaggedValue>(p_Value.GetSNI::SNI_Value()))
{

}

SN_TaggedValue::~SN_TaggedValue()
{

}