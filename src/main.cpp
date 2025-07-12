#include <cassert>
#include <fmt/core.h>
#include <unordered_map>
#include <sys/stat.h>
#include <unistd.h>
#include <csv.h>

#include "csv_parser.hpp"
#include "definitions.hpp"
#include "arena.hpp"
#include "profiling/profiler.hpp"
#include "profiling/timer.hpp"


int main() {
    Arena arena(constants::ARENA_SIZE);
    struct stat Stat;
    stat("data/Bitfinex_ETHUSD_d.csv", &Stat);

    u64 cpuFreq = estimateCPUFreq(1000);

    std::unordered_map<const char*, ColumnType, CStrHash, CStrEqual> fields = {
        {arena.allocate("unix"),  Type_Int64},
        {arena.allocate("open"),  Type_Float64},
        {arena.allocate("high"),  Type_Float64},
        {arena.allocate("low"),   Type_Float64},
        {arena.allocate("close"), Type_Float64},
    };

    CsvParser btcFile("data/Bitfinex_ETHUSD_d.csv", arena, fields);
    std::unordered_map<const char*, Column, CStrHash, CStrEqual> data = btcFile.parsedContent();

    Column openCol = data["open"];
    f64* open = openCol.as_f64();

    for (int i = 0; i < openCol.size; ++i) {
        printf("%f \n", open[i]);
    }

    return 0;
}


