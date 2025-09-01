// PromotionCapture.h
#pragma once

#include "exp_ctrl_pgc.h"

#include <vector>
#include <utility>
#include <functional>

namespace PGC
{
    typedef  std::vector<std::function<void(PGC_Transaction*)>> FuncVector;

	class Promotable;

    // promotion_capture.h
    class PGC_EXPORT PromotionCaptureScope
    {
    private:
        FuncVector m_CaptureVector;
        FuncVector* m_PreviousCaptureVector = nullptr;
    //    static thread_local std::vector<Promotable*>* t_CaptureVector;

    public:
        PromotionCaptureScope();
        ~PromotionCaptureScope();

        FuncVector& GetCaptures();
        static void Register(std::function<void(PGC_Transaction*)> p_Func);
    };

    // promotable.h
    class Promotable
    {
    public:
        Promotable()
        { }
        Promotable(std::function<void(PGC_Transaction*)> p_Func)
        {
            PromotionCaptureScope::Register(p_Func);
        };
        ~Promotable() = default;
    };
}
