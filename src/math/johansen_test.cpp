#include "math/johansen_test.hpp"

#include <Eigen/Dense>
#include <cmath>

#include <Eigen/src/Core/IO.h>

Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
JohansenTest::JohansenTest(
    Eigen::MatrixXd& data,
    i32 p, i8 detOrder
) : data_(data), p_(p), detOrder_(detOrder)
{
    buildRegressionMatrices();
    regress();
    buildCovarianceMatrices();
    solveGenerEigenvalProb();
    return;
}

void JohansenTest::solveGenerEigenvalProb() {
    Eigen::LLT<Eigen::MatrixXd> llt(S_00);
    Eigen::MatrixXd A = S_10 * llt.solve(S_01);

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
Eigen::MatrixXd ZZt = Z_ * Z_.transpose();

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(ZZt, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::MatrixXd ZZt_pinv = svd.solve(Eigen::MatrixXd::Identity(ZZt.rows(), ZZt.cols()));

    Eigen::MatrixXd beta_gamma = ZZt_pinv * Z_ * deltaX_.transpose();
    Eigen::MatrixXd beta_phi = ZZt_pinv * Z_ * laggedX_.transpose();

    R_ = deltaX_  - beta_gamma.transpose() * Z_;
    S_ = laggedX_ - beta_phi  .transpose() * Z_;
}

void JohansenTest::buildRegressionMatrices() {
    int n = data_.cols();
    int T = data_.rows();

    if (detOrder_ == 0) data_.rowwise() -= data_.colwise().mean();
    if (detOrder_ == 1) {
        Eigen::VectorXd t = Eigen::VectorXd::LinSpaced(T, 1, T);
        for(int col = 0; col < n; ++col) {
            Eigen::VectorXd y = data_.col(col);
            double beta = (t.dot(y) - t.sum() * y.sum() / T) /
                          (t.dot(t) - t.sum() * t.sum() / T);
            double alpha = y.mean() - beta * t.mean();
            data_.col(col) = y - alpha * Eigen::VectorXd::Ones(T) - beta * t;
        }
    }

    Eigen::MatrixXd dx = data_.bottomRows(T - 1) - data_.topRows(T - 1);
    deltaX_  = dx.middleRows(p_ - 1, T - p_).transpose();
    laggedX_ = data_.middleRows(p_ - 1, T - p_).transpose();

    Z_.resize((p_ - 1) * n, T - p_);
    for(int k = 1; k < p_; ++k) {
        Z_.block((k - 1) * n, 0, n, T - p_) = dx.middleRows(p_ - 1 - k, T - p_).transpose();
    }

    if(detOrder_ >= 1) {
        Z_.conservativeResize(Z_.rows() + 1, Eigen::NoChange);
        Z_.row(Z_.rows() - 1) = Eigen::RowVectorXd::Ones(T - p_);
    }
}


