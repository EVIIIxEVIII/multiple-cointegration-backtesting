#include "math/johansen_test.hpp"

#include <Eigen/Dense>
#include <cmath>


JohansenTest::JohansenTest(Eigen::MatrixXd& data, i32 p) : data_(data), p_(p)
{
    buildRegressionMatrices();
    regress();
    buildCovarianceMatrices();
    solveGenerEigenvalProb();
}

void JohansenTest::solveGenerEigenvalProb() {
    Eigen::MatrixXd S_00_inv = S_00.inverse();
    Eigen::MatrixXd A = S_10 * S_00_inv * S_01;

    Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::MatrixXd> ges(A, S_11);
    Eigen::VectorXd eigenvalues = ges.eigenvalues();
    Eigen::MatrixXd eigenvectors = ges.eigenvectors();

    lambda_ = eigenvalues.reverse();
}

f64 JohansenTest::maxEigenStat(i32 rank) {
    if (rank >= lambda_.size()) return 0.0;
    return -(data_.rows() - p_) * std::log(1 - lambda_[rank]);
}

f64 JohansenTest::getTraceStat(i32 rank) {
    if (rank >= lambda_.size()) return 0.0;

    f64 traceStat = 0.0;
    for (int i = rank; i < lambda_.size(); ++i) {
        traceStat += std::log(1.0 - lambda_[i]);
    }
    traceStat *= -(data_.rows() - p_);

    return  traceStat;
}

void JohansenTest::buildCovarianceMatrices() {
    f64 normalization = 1./(data_.rows() - p_);
    S_00 = normalization * R_ * R_.transpose();
    S_11 = normalization * S_ * S_.transpose();
    S_01 = normalization * R_ * S_.transpose();
    S_10 = S_01.transpose();
}


Eigen::VectorXd JohansenTest::getEigenvalues() {
    return lambda_;
}


void JohansenTest::regress() {
    Eigen::MatrixXd ZZ_trans = Z_ * Z_.transpose();

    Eigen::MatrixXd gamma = deltaX_ * Z_.transpose() * (ZZ_trans).inverse();
    Eigen::MatrixXd phi = laggedX_ * Z_.transpose() * (ZZ_trans).inverse();

    R_ = deltaX_ - gamma*Z_;
    S_ = laggedX_ - phi * Z_;
}

void JohansenTest::buildRegressionMatrices() {
    int n = data_.cols();
    int T = data_.rows();

    Eigen::MatrixXd dx = data_.bottomRows(T - 1) - data_.topRows(T - 1);
    deltaX_  = dx.bottomRows(T - p_);
    laggedX_ = data_.middleRows(p_ - 1, T - p_);

    Z_.resize((p_ - 1) * n, T - p_);

    for (int k = 1; k < p_; ++k) {
        Z_.block(n * (k - 1), 0, n, T - p_) = dx.block((p_ - 1) - k, 0, T - p_, n).transpose();
    }
}



