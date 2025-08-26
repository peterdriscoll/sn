#pragma once

#include "promotioncapture.h"

#include <vector>
#include <utility>

#include "pgc_pch.h"


namespace PGC
{
    thread_local FuncVector* t_CaptureVector = nullptr;

    PromotionCaptureScope::PromotionCaptureScope()
        : m_PreviousCaptureVector(t_CaptureVector)
    { 
        t_CaptureVector = &m_CaptureVector;
        m_CaptureVector.clear();
    }

    PromotionCaptureScope::~PromotionCaptureScope()
    { 
        t_CaptureVector = m_PreviousCaptureVector;
    }

    FuncVector& PromotionCaptureScope::GetCaptures()
    { 
        return m_CaptureVector; 
    }

    /*static*/ void PromotionCaptureScope::Register(std::function<void()> p_Func)
    { 
        if (t_CaptureVector)
        {
            t_CaptureVector->push_back(p_Func);
        }
    }
}
