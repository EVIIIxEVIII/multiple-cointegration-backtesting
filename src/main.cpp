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

    std::unordered_map<const char*, ColumnType, CStrHash, CStrEqual> gldFields = {
        {arena.allocate("data"),  Type_Float64},
    };

    std::unordered_map<const char*, ColumnType, CStrHash, CStrEqual> slvFields = {
        {arena.allocate("data"),  Type_Float64},
    };

    CsvParser gld("data/gld.csv", arena, gldFields);
    CsvParser slv("data/slv.csv", arena, slvFields);

    auto gldData = gld.parsedContent();
    auto slvData = slv.parsedContent();

    std::array<Column, 2> rawData{ gldData["data"], slvData["data"] };
    size_t dataCount = gldData["data"].size;

    Eigen::MatrixXd data(dataCount, rawData.size());
    for (size_t i = 0; i < rawData.size(); ++i)  {
        for(size_t j = 0; j < dataCount; ++j) {
            data(j, i) = rawData[i].as_f64()[j];
        }
    }

    int lags = 2;
    for (int i = -1; i < 2; ++i) {
        JohansenTest test(data, lags, i);
        Eigen::VectorXd eigenvalues          = test.eigenvalues();
        Eigen::VectorXd maxEigenStat         = test.maxEigenStat();
        Eigen::VectorXd traceStat            = test.traceStat();
        Eigen::MatrixXd cointegrationVectors = test.cointegrationVectors();

        printf("det_order=%d: ", i);
        printf("Eigenvalues: [");
        for (int i = 0; i < eigenvalues.size(); ++i) {
            printf(" %.15f ", eigenvalues[i]);
        }
        printf("]\n");

        printf("Max eigenvalue stat: [");
        for (int i = 0; i < maxEigenStat.size(); ++i) {
            printf(" %.15f ", maxEigenStat[i]);
        }
        printf("]\n");

        printf("Trace stat: [");
        for (int i = 0; i < traceStat.size(); ++i) {
            printf(" %.15f ", traceStat[i]);
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

        printf("----------------------------------------\n");
    }

    return 0;
}


