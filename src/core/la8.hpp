#pragma once

#include <cstdint>

struct LA8 {
    uint8_t l, a;

    bool operator==(const LA8&) const = default;
};
