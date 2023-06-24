#pragma once
#include <vector>

#include <Eigen/Core>
#include <Eigen/Sparse>

template<class MatF, class MatI> 
auto Laplacian(const MatF &X, const MatI &T)//X为点坐标，T为面
{
    std::vector<Eigen::Triplet<double>> ijv;//通过三元数赋值laplace矩阵
    int i = 0;
    for (i = 0; i < T.rows(); i++) {
        ijv.push_back(Eigen::Triplet<double>(T(i, 0), T(i, 1), -1)); ijv.push_back(Eigen::Triplet<double>(T(i, 1), T(i, 0), -1));
        ijv.push_back(Eigen::Triplet<double>(T(i, 1), T(i, 2), -1)); ijv.push_back(Eigen::Triplet<double>(T(i, 2), T(i, 1), -1));
        ijv.push_back(Eigen::Triplet<double>(T(i, 2), T(i, 0), -1)); ijv.push_back(Eigen::Triplet<double>(T(i, 0), T(i, 2), -1));
    }

    //////////////////////////////////////////////////////////////////
    int nv = X.rows();
    Eigen::SparseMatrix<float, Eigen::ColMajor> M(nv, nv);

    M.setFromTriplets(ijv.cbegin(), ijv.cend());

    for (i = 0; i < nv; i++) {
        M.insert(i, i) = -M.row(i).sum();//对第i行求和取相反数
    }

    return M;
}

//求两点之间的距离的平方
template<class MatF>
double dis(const MatF& X, int i, int j) {
    double dis = 0;
    for (int k = 0; k < 3; k++) {
        dis += (X(i, k) - X(j, k)) * (X(i, k) - X(j, k));
    }
    return dis;
}


template<class MatF,class MatI>
auto Laplacian_cotweight(const MatF& X, const MatI& T) {
    std::vector<Eigen::Triplet<double>> ijv;//通过三元数赋值laplace矩阵
    int i = 0;
    double cot1, cot2, cot3; 
    double cos1, cos2, cos3;
    for (i = 0; i < T.rows(); i++) {
        //cos的余角公式
        cos1 = (dis(X, T(i, 1), T(i, 0)) + dis(X, T(i, 1), T(i, 2)) - dis(X, T(i, 0), T(i, 2))) / 2 / sqrt(dis(X, T(i, 1), T(i, 0)) * dis(X, T(i, 1), T(i, 2)));
        cos2 = (dis(X, T(i, 2), T(i, 0)) + dis(X, T(i, 1), T(i, 2)) - dis(X, T(i, 0), T(i, 1))) / 2 / sqrt(dis(X, T(i, 1), T(i, 2)) * dis(X, T(i, 0), T(i, 2)));
        cos3 = (dis(X, T(i, 1), T(i, 0)) + dis(X, T(i, 0), T(i, 2)) - dis(X, T(i, 1), T(i, 2))) / 2 / sqrt(dis(X, T(i, 1), T(i, 0)) * dis(X, T(i, 0), T(i, 2)));
        cot1 = cos1 / sqrt(1 - cos1 * cos1);
        cot2 = cos2 / sqrt(1 - cos2 * cos2);
        cot3 = cos3 / sqrt(1 - cos3 * cos3);
        ijv.push_back(Eigen::Triplet<double>(T(i, 0), T(i, 1), cot2));
        ijv.push_back(Eigen::Triplet<double>(T(i, 1), T(i, 0), cot2));
        ijv.push_back(Eigen::Triplet<double>(T(i, 1), T(i, 2), cot3));
        ijv.push_back(Eigen::Triplet<double>(T(i, 2), T(i, 1), cot3));
        ijv.push_back(Eigen::Triplet<double>(T(i, 2), T(i, 0), cot1));
        ijv.push_back(Eigen::Triplet<double>(T(i, 0), T(i, 2), cot1));
    }

    //////////////////////////////////////////////////////////////////
    int nv = X.rows();
    Eigen::SparseMatrix<float, Eigen::ColMajor> M(nv, nv);

    M.setFromTriplets(ijv.cbegin(), ijv.cend());

    for (i = 0; i < nv; i++) {
        M.insert(i, i) = -M.row(i).sum();//对第i行求和取相反数
    }

    return M;
}
