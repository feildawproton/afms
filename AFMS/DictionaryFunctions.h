#ifndef DICTIONARYFUNCTIONS_H
#define DICTIONARYFUNCTIONS_H

#include "armadillo"
#include <vector>
#include <string>

namespace Dictionary_System
{
	//for classifying subtypes of movements (ie scores)
	std::vector<arma::ucube> patches_singleType(
		const std::vector<arma::ucube>& trainData, unsigned numPatches, unsigned pRows, unsigned pCols, unsigned pSlices);

	//Im going to consider this a function because even though caller does not have the input in scope,
	//the data on disk acts the same as a constant, plus it return the input in scope
	std::vector<arma::ucube> loadTrain_singleType(const std::string& directory);
}

#endif