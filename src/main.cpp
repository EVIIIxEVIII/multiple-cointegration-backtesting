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

enum Confidence {
    p90, p95, p99
};

int pickMaxEigenStatRank(Eigen::MatrixXd& cvm, Eigen::VectorXd& maxEigenStat, Confidence conf) {
    int res = -1;
    for (int r = 0; r < maxEigenStat.size() - 1; ++r) {
        if(maxEigenStat[r] > cvm(r, conf)) {
            continue;
        } else {
            res = r;
            break;
        }
    }

    if (res == -1) return maxEigenStat.size() - 1;
    return res;
}

int pickTraceStatRank(Eigen::MatrixXd& cvt, Eigen::VectorXd& traceStat, Confidence conf) {
    int res = -1;
    for (int r = 0; r < traceStat.size() - 1; ++r) {
        if(traceStat[r] > cvt(r, conf)) {
            continue;
        } else {
            res = r;
            break;
        }
    }

    if (res == -1) return traceStat.size() - 1;
    return res;
}

Eigen::MatrixXd createInputMatrix(std::vector<Column> columns, size_t size) {
    Eigen::MatrixXd data(size, columns.size());
    for (size_t i = 0; i < columns.size(); ++i)  {
        for(size_t j = 0; j < size; ++j) {
            data(j, i) = columns[i].as_f64()[j];
        }
    }
    return data;
}

int main() {
    Arena arena(constants::ARENA_SIZE);

    std::unordered_map<std::string, ColumnType> dataCol = {
        {"data",  Type_Float64},
    };

    std::unordered_map<std::string, ColumnType> priceCol = {
        {"price",  Type_Float64},
    };

    std::unordered_map<std::string, ColumnType> closeCol = {
        {"close",  Type_Float64},
    };

    CsvParser gld("data/gld.csv", arena, dataCol);
    CsvParser slv("data/slv.csv", arena, dataCol);

    auto gldData = gld.parsedContent();
    auto slvData = slv.parsedContent();

    CsvParser series1("data/series1.csv", arena, priceCol);
    CsvParser series2("data/series2.csv", arena, priceCol);

    auto series1Data = series1.parsedContent();
    auto series2Data = series2.parsedContent();

    CsvParser btc("data/BTC_USD.csv", arena, closeCol);
    CsvParser eth("data/ETH_USD.csv", arena, closeCol);

    auto btcData = btc.parsedContent();
    auto ethData = eth.parsedContent();

    std::vector<Column> rawData{ btcData["close"], ethData["close"] };
    size_t dataCount = btcData["close"].size;
    std::cout << btcData["close"].size << "\n";

    auto data = createInputMatrix(rawData, dataCount);

    const int lags = 2;
    const int detOrder = 1;

    JohansenTest test(data, lags, detOrder);
    Eigen::VectorXd eigenvalues          = test.eigenvalues();
    Eigen::VectorXd maxEigenStat         = test.maxEigenStat();
    Eigen::VectorXd traceStat            = test.traceStat();
    Eigen::MatrixXd cointegrationVectors = test.cointegrationVectors();
    Eigen::MatrixXd cvt                  = test.cvt();
    Eigen::MatrixXd cvm                  = test.cvm();

    int traceStatRank = pickTraceStatRank(cvt, traceStat, p99);
    int maxEigenStatRank = pickMaxEigenStatRank(cvm, traceStat, p99);
    int rank = std::min(traceStatRank, maxEigenStatRank);

    std::cout << "Trace Stat Rank: " << traceStatRank << "\n";
    std::cout << "Max Eigen Stat Rank: " << maxEigenStatRank << "\n";
    std::cout << "The rank: " << rank << "\n";

    printf("Eigenvalues: [");
    for (int i = 0; i < eigenvalues.size(); ++i) {
        printf(" %.15f ", eigenvalues[i]);
    }
    printf("]\n");


    printf("Trace stat: [");
    for (int i = 0; i < traceStat.size(); ++i) {
        printf(" %.15f ", traceStat[i]);
    }
    printf("]\n");

    printf("Max eigenvalue stat: [");
    for (int i = 0; i < maxEigenStat.size(); ++i) {
        printf(" %.15f ", maxEigenStat[i]);
    }
    printf("]\n");

    printf("Cointegration vectors: \n");
    for (int i = 0; i < cointegrationVectors.rows(); ++i) {
        printf("[ ");
        for (int j = 0; j < cointegrationVectors.cols(); ++j) {
            printf(" %f ", cointegrationVectors.row(i)[j]);
        }
        printf(" ]\n");
    }

    return 0;
}


