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

    std::unordered_map<const char*, ColumnType, CStrHash, CStrEqual> fields = {
        {arena.allocate("A"),  Type_Float64},
        {arena.allocate("B"),  Type_Float64},
        {arena.allocate("C"),  Type_Float64},
    };

    CsvParser testFile("data/test_data.csv", arena, fields);
    auto testData = testFile.parsedContent();

    std::array<Column, 3> rawData{ testData["A"], testData["B"], testData["C"] };
    size_t dataCount = testData["A"].size;

    Eigen::MatrixXd data(dataCount, rawData.size());
    for (size_t i = 0; i < rawData.size(); ++i)  {
        for(size_t j = 0; j < dataCount; ++j) {
            data(j, i) = rawData[i].as_f64()[j];
        }
    }

    JohansenTest test(data, 2);

    Eigen::VectorXd eigenvalues = test.getEigenvalues();

    printf("Eigenvalues: [");
    for (int i = 0; i < eigenvalues.size(); ++i) {
        printf(" %f ", eigenvalues[i]);
    }
    printf("]\n");

    //printf("Max eigenvalues: [");
    //for (int i = 0; i < 3; ++i) {
    //    printf(" %f ", test.maxEigenStat(i));
    //}
    //printf("]\n");

    return 0;
}


