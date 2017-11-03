#if !defined(PGC_BLOCK_H_INCLUDED)
#define PGC_BLOCK_H_INCLUDED

#pragma once

namespace PGC
{
	const size_t BlockSize = 2000;

	class PGC_Base;
	class PGC_Transaction;

	class PGC_Block
	{
	public:
		PGC_Block(PGC_Block *p_LastBlock);
		virtual ~PGC_Block();

		PGC_Block *GetNextBlock();
		void *Allocate(size_t p_size);

		void RegisterForDestruction(PGC_Base *p_Base);
		void DestroyUncopied(PGC_Transaction *p_Transaction);

		PGC_Base *FindOwner(void *p_member);

		size_t NetMemoryUsed();
	private:
		PGC_Block *m_NextBlock;
		char m_Data[BlockSize];
		char *m_current;
		char *m_end;

		PGC_Base *m_DestuctionList;
		PGC_Base *m_DestuctionListLast;
	};
}

#endif // !defined(PGC_BLOCK_H_INCLUDED)
