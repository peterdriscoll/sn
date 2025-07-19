#pragma once

#include "exp_ctrl_pgc.h"

namespace PGC
{
	class PGC_Transaction;

	class PGC_EXPORT PGC_TypeCheck
	{
	public:
		PGC_TypeCheck() = default;
		virtual ~PGC_TypeCheck() = default;
		virtual bool IsPromotion();
		virtual PGC_TypeCheck* GetPointer()
		{
			return nullptr;
		}
		virtual PGC_Transaction* GetTransaction()
		{
			return nullptr;
		}
		virtual PGC_TypeCheck* GetPromotedCopy()
		{
			return nullptr;
		}
		virtual void SetPromotedCopy(PGC_TypeCheck*)
		{
		}
		void RequestPromotion(PGC_TypeCheck** p_Base)
		{
		}
	};
}