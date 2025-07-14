#include <cassert>
#include <unordered_map>
#include <sys/stat.h>
#include <unistd.h>
#include <Eigen/Dense>

#include "csv_parser.hpp"
#include "definitions.hpp"
#include "arena.hpp"
#include "math/johansen_test.hpp"

int main() {
    Arena arena(constants::ARENA_SIZE);
    struct stat Stat;
    stat("data/Bitfinex_ETHUSD_d.csv", &Stat);

    std::unordered_map<const char*, ColumnType, CStrHash, CStrEqual> fields = {
        {arena.allocate("unix"),  Type_Int64},
        {arena.allocate("open"),  Type_Float64},
        {arena.allocate("high"),  Type_Float64},
        {arena.allocate("low"),   Type_Float64},
        {arena.allocate("close"), Type_Float64},
    };

    CsvParser btcFile("data/BTC_USD.csv", arena, fields);
    auto btcData = btcFile.parsedContent();

    CsvParser ethFile("data/BTC_USD.csv", arena, fields);
    auto ethData = ethFile.parsedContent();

    std::array<Column, 2> rawData{ btcData["close"], ethData["close"] };
    auto dataCount = std::min(btcData["close"].size, ethData["close"].size);
    Eigen::MatrixXd data(rawData.size(), dataCount);

    printf("Loaded %lu data points \n", dataCount);

    for (size_t i = 0; i < rawData.size(); ++i)  {
        for(size_t j = 0; j < dataCount; ++j) {
            data(i, j) = rawData[i].as_f64()[dataCount - j - 1];
        }
    }

    JohansenTest test(data, 3);

    return 0;
}


