#ifndef FORMATFUNCTIONS_H
#define FORMATFUNCTIONS_H

#include <armadillo>
#include <string>

//for formating data
//armadillo is included here
//armadillo is not wrapped for this project because it is used everywhere
//btw it was the biggest pain to include into this project including openblas and lapack
namespace Format_System
{
	arma::ucube convert_tiffFolder_toCube(const std::string& folderPath, unsigned nRows, unsigned nCols, unsigned nSlices);
}

#endif