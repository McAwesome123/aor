#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>
#include <set>

#include "items.h"

const static std::int64_t ACTIVITY_TICK_RATE_MS = 500;

struct CharacterActivity {
    CharacterActivity() = default;
    CharacterActivity(ItemDomain action, std::int64_t ms);

    ItemDomain action;
    std::int64_t ms_left;
    std::int64_t ms_total;
};