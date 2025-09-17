#include "pgc_pch.h"

#include "debug.h"

#ifdef PGC_DEBUG
namespace PGC
{
	thread_local std::string t_label;

	PGC_EXPORT void save_debug_label(const std::string &label) noexcept
	{
		if (!label.empty())
		{
			t_label = label;
		}
	}
	PGC_EXPORT std::string consume_debug_label() noexcept
	{
		if (!t_label.empty())
		{
			std::string label = t_label;
			t_label.clear();
			return label;
		}
		return t_label;
	}
}
#endif

