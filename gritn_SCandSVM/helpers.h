#ifndef HELPERS_H
#define HELPERS_H

#include <armadillo>

namespace SCandSVMspace
{
	//the slowness
	__inline arma::mat myreshape_viaSample(const arma::mat& baseMat, int nRows, int nCols)
	{
		double rowRatio = ((double)baseMat.n_rows) / ((double)nRows);
		double colRatio = ((double)baseMat.n_cols) / ((double)nCols);

		arma::mat newMat(nRows, nCols);

		//nexted loops...
		for (int i = 0; i < nRows; i++)
		{
			for (int j = 0; j < nCols; j++)
			{
				int eye = (int)round(i*rowRatio);
				int jay = (int)round(j*colRatio);
				newMat(i, j) = baseMat(eye, jay);
			}
		}
		return newMat;
	}
}

#endif