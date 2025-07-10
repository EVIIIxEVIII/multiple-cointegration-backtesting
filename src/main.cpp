#include <cassert>
#include <fmt/core.h>
#include <unordered_map>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <csv.h>
#include <vector>

#include "csv_parser.hpp"
#include "definitions.hpp"
#include "arena.hpp"
#include "profiling/profiler.hpp"
#include "profiling/timer.hpp"

int test_fast_csv(std::string filename)
{
    struct stat Stat;
    stat(filename.data(), &Stat);
    TimeThroughput("Fast CPP CSV", Stat.st_size);

    std::unordered_map<std::string, std::vector<double>> cols = {
        {"unix",  {}},
        {"open",  {}},
        {"high",  {}},
        {"low",   {}},
        {"close", {}}
    };

    io::CSVReader<5,
                  io::trim_chars<' ', '\t'>,
                  io::no_quote_escape<','>> in(filename);
    in.read_header(io::ignore_extra_column,
                   "unix", "open", "high", "low", "close");

    long long unix_ts;       // keep original type
    double     open, high, low, close;

    while (in.read_row(unix_ts, open, high, low, close)) {
        cols["unix"].push_back(static_cast<double>(unix_ts)); // convert for uniform map type
        cols["open"].push_back(open);
        cols["high"].push_back(high);
        cols["low"].push_back(low);
        cols["close"].push_back(close);
    }

    std::cout << "Loaded " << cols["unix"].size() << " rows from "
              << filename << '\n';
    return 0;
}

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

    u64 start_fast = readCPUTimer();
    test_fast_csv("data/Bitfinex_ETHUSD_d.csv");
    u64 end_fast = readCPUTimer();

    u64 start_mine = readCPUTimer();
    CsvParser btcFile("data/Bitfinex_ETHUSD_d.csv", arena, fields);
    u64 end_mine = readCPUTimer();

    auto data = btcFile.parsedContent();

    u64 gigabyte = 1024ull * 1024 * 1024;
    u64 size = Stat.st_size;

    f64 fast_ms  = ((f64)end_fast - (f64)start_fast) / (f64)cpuFreq * 1000.;
    f64 mine_ms  = ((f64)end_mine - (f64)start_mine) / (f64)cpuFreq * 1000.;

    f64 fast_sec = fast_ms / 1000.;
    f64 mine_sec = mine_ms / 1000.;

    f64 size_gb = (f64)size / gigabyte;

    f64 fast_gbps = size_gb / fast_sec;
    f64 mine_gbps = size_gb / mine_sec;

    printf("CPU freq: %lu\n", cpuFreq);

    printf("Fast cpp csv parser: %.3f ms (%.2f GB/s)\n", fast_ms, fast_gbps);
    printf("My csv parser:       %.3f ms (%.2f GB/s)\n", mine_ms, mine_gbps);
    return 0;
}


