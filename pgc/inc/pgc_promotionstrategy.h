#pragma once

namespace PGC
{
	enum class PromotionStrategy
	{
		Backstabbing,    // Promotion modifies T* via back-reference (T**)
		DoubleDipping    // Promotion updates stored pointer inside Ref<T>
	};

	// Default strategy for new transactions
	constexpr PromotionStrategy g_DefaultPromotionStrategy = PromotionStrategy::DoubleDipping;
}