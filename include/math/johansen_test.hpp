#pragma once

#include <Eigen/Dense>
#include "definitions.hpp"

class JohansenTest {

public:
    JohansenTest(Eigen::MatrixXd& data, i32 p, i8 detOrder);
    f64 getTraceStat(i32 rank);
    f64 maxEigenStat(i32 rank);
    Eigen::VectorXd getEigenvalues();

private:
    void buildRegressionMatrices();
    void regress();
    void buildCovarianceMatrices();
    void solveGenerEigenvalProb();

    Eigen::MatrixXd data_;
    Eigen::MatrixXd Z_;
    Eigen::MatrixXd deltaX_;
    Eigen::MatrixXd laggedX_;

    // residual matrices
    Eigen::MatrixXd R_;
    Eigen::MatrixXd S_;

    // covariance matrices
    Eigen::MatrixXd S_00;
    Eigen::MatrixXd S_11;
    Eigen::MatrixXd S_01;
    Eigen::MatrixXd S_10;

    // eigenvalues
    Eigen::VectorXd lambda_;

    i32 p_ = 0;
    i8  detOrder_ = 0;
};
