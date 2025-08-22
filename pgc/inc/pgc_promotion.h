#pragma once

#include "exp_ctrl_pgc.h"
#include "pgc_typecheck.h"
#include "pgc_promotionstrategy.h"

namespace PGC
{
	enum class PromotionResult
	{
		Keep,            // Still valid, not promoted
		Dropped,         // Invalid source/destination (free immediately)
		PromotedDone,    // Promoted and can be freed
		PromotedKeep     // Promoted but must remain alive (DoubleDipping)
	};

	class PGC_Base;
	class PGC_Transaction;
	class PGC_User;

	class PGC_EXPORT PGC_Promotion
		: public PGC_TypeCheck
	{
	public:
		static PGC_TypeCheck* CheckRequestPromotion(PGC_TypeCheck** p_Base, PGC_Transaction* p_Source, PGC_Transaction* p_Destination, PromotionStrategy p_CheckType);
		static PGC_TypeCheck* RequestPromotion(PGC_TypeCheck** p_Base, PGC_Transaction* p_Destination, PromotionStrategy p_Strategy);
		static PGC_TypeCheck* CopyMemory(PGC_TypeCheck* p_Base, PGC_Transaction* p_Destination);

		PGC_Promotion();
		virtual ~PGC_Promotion();

		PromotionResult PromoteOrReject();
		void Promote();

		bool IsPromotion() override;
		PGC_TypeCheck* GetFinalCopy() const;

		PGC_TypeCheck* GetBase();
		PGC_TypeCheck** GetBaseAddress();
		void SetBase(PGC_TypeCheck** p_Base);

		void Rebind(PGC_TypeCheck** p_Base, PGC_Transaction* p_DestinationTransaction);

		void Free();

		PGC_Transaction* GetSource();
		PGC_Transaction* GetDestination();
		PromotionStrategy GetStrategy();

		void Create(PGC_TypeCheck** p_Base, PGC_Transaction* p_Destination, PromotionStrategy p_Strategy);

		PGC_User* GetUser() const;

		bool IsPromoted() const;
		void MarkPromoted();

		void MarkNoLongerNeeded();

		virtual PGC_TypeCheck* GetLogicalPointer() override;
		virtual PGC_Transaction* GetLogicalOwnerTransaction() override;
		virtual PGC_Promotion* GetLogicalPromotion() override;

		PGC_Promotion *m_Next;

	private:
		PGC_TypeCheck **m_Base;
		PGC_Transaction *m_Destination;
		PGC_TypeCheck* m_FinalCopy;
		bool m_Promoted = false;
		bool m_StillNeeded = true;
		PromotionStrategy m_Strategy;
	};
}