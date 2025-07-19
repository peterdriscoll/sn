#pragma once

#include "exp_ctrl_pgc.h"
#include "pgc_typecheck.h"
#include "pgc_promotionstrategy.h"

namespace PGC
{
	class PGC_Base;
	class PGC_Transaction;

	class PGC_EXPORT PGC_Promotion
		: public PGC_TypeCheck
	{
	public:
		static void PromoteRequests();

		PGC_Promotion();
		virtual ~PGC_Promotion();

		static PGC_TypeCheck* CheckRequestPromotion(PGC_TypeCheck** p_Base, PGC_Transaction* p_Source, PGC_Transaction* p_Destination, PromotionStrategy p_CheckType);
		static PGC_TypeCheck* RequestPromotion(PGC_TypeCheck** p_Base, PGC_Transaction* p_Destination, PromotionStrategy p_Strategy);
		static PGC_TypeCheck* CopyMemory(PGC_TypeCheck* p_Base, PGC_Transaction* p_Destination);

		bool PromoteOrReject();
		void Promote();

		bool IsPromotion() override;
		PGC_TypeCheck* GetFinalCopy() const;

		PGC_TypeCheck* GetBase();
		PGC_TypeCheck** GetBaseRef();
		PGC_Promotion *GetNext();
		void SetNext(PGC_Promotion *p_Promotion);
		void Free();

		PGC_Transaction* GetSource();
		PGC_Transaction* GetDestination();

		static size_t PromotionFreeMemory();
		static size_t PromotionUsedMemory();
		void Create(PGC_TypeCheck** p_Base, PGC_Transaction* p_Destination, PromotionStrategy p_Strategy);
		static PGC_Promotion* Allocate();
		void AppendRequest();
	private:

		bool FindInFreeList();

		static PGC_Promotion *m_FreeList;
		static PGC_Promotion *m_PromoteList;
		static PGC_Promotion **m_PromoteListLast;

		PGC_TypeCheck **m_Base;
		PGC_Transaction *m_Destination;
		PGC_Promotion *m_Next;
		PGC_TypeCheck* m_FinalCopy;
		PromotionStrategy m_Strategy;
	};
}