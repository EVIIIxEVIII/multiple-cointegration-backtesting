#include "math/johansen_test.hpp"

#include <Eigen/Dense>


JohansenTest::JohansenTest(Eigen::MatrixXd& data, i32 p) : data_(data), p_(p)
{
    buildRegressorMatrix();


}


void JohansenTest::buildRegressorMatrix() {
    int n = data_.cols();
    int T = data_.rows();

    Eigen::MatrixXd dx = data_.bottomRows(T - 1) - data_.topRows(T - 1);
    Z_.resize((p_ - 1) * n, T - p_);

    for (int k = 1; k < p_; ++k) {
        Z_.block(n * (k - 1), 0, n, T - p_) = dx.block((p_ - 1) - k, 0, T - p_, n).transpose();
    }
}



