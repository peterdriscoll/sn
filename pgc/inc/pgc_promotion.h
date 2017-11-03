#if !defined(PGC_PROMOTION_H_INCLUDED)
#define PGC_PROMOTION_H_INCLUDED

#pragma once

#include "exp_ctrl_pgc.h"

namespace PGC
{
	class PGC_Base;
	class PGC_Transaction;

	class PGC_EXPORT PGC_Promotion
	{
	public:
		static void RequestPromotion(PGC_Base **p_Base, PGC_Transaction *p_Destination);
		static void PromoteRequests();

		PGC_Promotion();
		virtual ~PGC_Promotion();

		PGC_Base *GetBase();
		PGC_Promotion *GetNext();
		void SetNext(PGC_Promotion *p_Promotion);

		static size_t PromotionFreeMemory();
		static size_t PromotionUsedMemory();
	private:
		static PGC_Promotion *Allocate();
//		static PGC_Promotion *RetrieveRequest();

		void Free();
		bool FindInFreeList();
		void Create(PGC_Base **p_Base, PGC_Transaction *p_Destination);
		void AppendRequest();
		bool PromoteOrReject();
		void Promote();
		PGC_Base *CopyMemory();

		static PGC_Promotion *m_FreeList;
		static PGC_Promotion *m_PromoteList;
		static PGC_Promotion **m_PromoteListLast;

		PGC_Base **m_Base;
		PGC_Transaction *m_Destination;
		PGC_Promotion *m_Next;
	};
}

#endif // !defined(PGC_PROMOTION_H_INCLUDED)
