#include <cassert>
#include <unordered_map>
#include <sys/stat.h>
#include <unistd.h>
#include <Eigen/Dense>
#include <iostream>

#include "csv_parser.hpp"
#include "definitions.hpp"
#include "arena.hpp"
#include "math/johansen_test.hpp"

int main() {
    Arena arena(constants::ARENA_SIZE);
    struct stat Stat;
    stat("data/Bitfinex_ETHUSD_d.csv", &Stat);

    //std::unordered_map<const char*, ColumnType, CStrHash, CStrEqual> fields = {
    //    {arena.allocate("unix"),  Type_Int64},
    //    {arena.allocate("open"),  Type_Float64},
    //    {arena.allocate("high"),  Type_Float64},
    //    {arena.allocate("low"),   Type_Float64},
    //    {arena.allocate("close"), Type_Float64},
    //};

    //CsvParser btcFile("data/BTC_USD.csv", arena, fields);
    //auto btcData = btcFile.parsedContent();

    //CsvParser ethFile("data/BTC_USD.csv", arena, fields);
    //auto ethData = ethFile.parsedContent();

    //std::array<Column, 2> rawData{ btcData["close"], ethData["close"] };
    //auto dataCount = std::min(btcData["close"].size, ethData["close"].size);
    //Eigen::MatrixXd data(rawData.size(), dataCount);

    //for (size_t i = 0; i < rawData.size(); ++i)  {
    //    for(size_t j = 0; j < dataCount; ++j) {
    //        data(i, j) = rawData[i].as_f64()[dataCount - j - 1];
    //    }
    //}

    //JohansenTest test(data, 3);

    std::unordered_map<const char*, ColumnType, CStrHash, CStrEqual> fields = {
        {arena.allocate("A"),  Type_Float64},
        {arena.allocate("B"),  Type_Float64},
        {arena.allocate("C"),  Type_Float64},
    };

    CsvParser testFile("data/test_data.csv", arena, fields);
    auto testData = testFile.parsedContent();

    std::array<Column, 3> rawData{ testData["A"], testData["B"], testData["C"] };

    size_t dataCount = testData["A"].size;

    printf("Data Count: %lu \n", dataCount);
    Eigen::MatrixXd data(rawData.size(), dataCount);

    for (size_t i = 0; i < rawData.size(); ++i)  {
        for(size_t j = 0; j < dataCount; ++j) {
            data(i, j) = rawData[i].as_f64()[dataCount - j - 1];
        }
    }

    for (size_t i = 0; i < rawData.size(); ++i)  {
        for(size_t j = 0; j < dataCount; ++j) {
            printf(" %f ", data(i, j));
        }

        printf("\n");
    }


    JohansenTest test(data, 2);

    Eigen::VectorXd eigenvalues = test.getEigenvalues();

    printf("Eigenvalues: [");
    for (int i = 0; i < 3; ++i) {
        printf(" %f ", eigenvalues[i]);
    }
    printf("]\n");



    return 0;
}


