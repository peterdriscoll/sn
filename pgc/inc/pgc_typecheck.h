#pragma once

#include "exp_ctrl_pgc.h"

namespace PGC
{
	class PGC_Transaction;
	class PGC_Promotion;

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
		virtual const PGC_Transaction* GetTransaction() const
		{
			return nullptr;
		}
		virtual PGC_TypeCheck* GetPromotedCopy() const noexcept
		{
			return nullptr;
		}
		virtual void SetPromotedCopy(PGC_TypeCheck*)
		{
		}
		void RequestPromotion(PGC_TypeCheck** p_Base)
		{
		}
		virtual PGC_TypeCheck* GetLogicalPointer() = 0;
		virtual PGC_Transaction* GetLogicalOwnerTransaction() = 0;
		virtual PGC_Promotion* GetLogicalPromotion() = 0;
	};
}