#pragma once
#include <Eigen/Sparse>
#include <sofa/helper/random.h>
#include <sofa/config.h>

/// Sparsity is a ratio between 0 and 1
template <typename TReal = SReal>
static void generateRandomSparseMatrix(Eigen::SparseMatrix<TReal>& eigenMatrix, Eigen::Index nbRows, Eigen::Index nbCols, TReal sparsity)
{
    eigenMatrix.resize(nbRows, nbCols);
    sofa::type::vector<Eigen::Triplet<TReal> > triplets;

    const auto nbNonZero = static_cast<Eigen::Index>(sparsity * static_cast<TReal>(nbRows*nbCols));

    for (Eigen::Index i = 0; i < nbNonZero; ++i)
    {
        const auto value = static_cast<TReal>(sofa::helper::drand(1));
        const auto row = static_cast<Eigen::Index>(sofa::helper::drandpos(nbRows) - 1e-8);
        const auto col = static_cast<Eigen::Index>(sofa::helper::drandpos(nbCols) - 1e-8);
        triplets.emplace_back(row, col, value);
    }

    eigenMatrix.setFromTriplets(triplets.begin(), triplets.end());
}
