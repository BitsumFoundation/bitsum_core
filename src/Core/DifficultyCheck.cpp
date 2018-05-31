#include "DifficultyCheck.hpp"

static const bytecoin::DifficultyCheck table[1] = {
    {UINT32_MAX, "", 0},
    };

const bytecoin::DifficultyCheck *bytecoin::difficulty_check = table;
const size_t bytecoin::difficulty_check_count               = sizeof(table) / sizeof(*table);
