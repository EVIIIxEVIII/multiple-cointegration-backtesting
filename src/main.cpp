#include <cassert>
#include <fmt/core.h>
#include <unordered_map>

#include "csv_parser.hpp"
#include "definitions.hpp"
#include "arena.hpp"

int main() {

    std::unordered_map<const char*, size_t, CStrHash, CStrEqual> fields = {
        {"unix",  Type_uInt64},
        {"open",  Type_Float64},
        {"high",  Type_Float64},
        {"low",   Type_Float64},
        {"close", Type_Float64},
    };

    Arena arena(constants::ARENA_SIZE);


    return 0;
}
