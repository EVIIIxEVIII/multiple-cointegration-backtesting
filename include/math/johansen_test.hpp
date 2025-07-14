#pragma once

#include <Eigen/Dense>
#include "csv_parser.hpp"


class JohansenTest {

public:
    JohansenTest(Eigen::MatrixXd& data, i32 p);

private:
    void buildRegressorMatrix();

    Eigen::MatrixXd data_;
    Eigen::MatrixXd Z_;
    int p_ = 0;

};
