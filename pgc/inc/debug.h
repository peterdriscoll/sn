#pragma once

#include "config.h"

#ifdef PGC_DEBUG

#include "exp_ctrl_pgc.h"
#include <string>

namespace PGC
{
    // Set a label for the *next* pin; consumed by the next pin site
    PGC_EXPORT void save_debug_label(const std::string &label) noexcept;

    // Retrieve & clear the pending label at the pin site
    PGC_EXPORT std::string consume_debug_label() noexcept;
}

#endif

