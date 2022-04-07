#ifndef DICTIONARYFUNCTIONS_H
#define DICTIONARYFUNCTIONS_H

#include "armadillo"
#include <vector>
#include <string>

namespace Dictionary_System
{
	//Im going to consider this a function because even though caller does not have the input in scope,
	//the data on disk acts the same as a constant, plus it return the input in scope
	std::vector<arma::ucube> loadTrain_singleType(
		const std::string& directory);

	std::vector<arma::ucube> loadDictionary_singleType(
		const std::string& directory);

	std::vector<std::vector<arma::ucube>> evenlySample_SingleType(
		const std::vector<arma::ucube>& trainData, unsigned sample_nRows, unsigned sample_nCols, unsigned sample_nSlices);
		
	std::vector<arma::ucube> concatPatches(
		const std::vector<std::vector<arma::ucube>>& complicated);

	//this and the random dict are just a friggin subset of the patches (except the random is more random...)
	//normalizes
	std::vector<arma::ucube> dictInitUniform(
		const std::vector<arma::ucube>& patches, unsigned dictSize);

	//numbases is the run cost
	std::vector<arma::ucube> optDict_SCcost(
		const std::vector<arma::ucube>& initDict, const std::vector<arma::ucube>& allData, unsigned numBases, bool overBool, double threshold);

	arma::mat activationSum_eaFeatData(
		const std::vector<arma::ucube>& dictionary, const std::vector<arma::ucube>& data, double threshold);
	
	//HEY YOU, WRITE AN INTERFACE FOR RETURNING THE ACTIVATION CUBES FOR ALL DATA AND A SINGLE FEATURE
	//IT EXISTS ALREADY IN THE CPP JUST RETURN IT AND ALSO
	//WRITE A FUNCTION AND INTERFACCE FOR RETURNING ACTIVATION CUBES FOR ALL FEATURES AND SINGLE DATA
	//SOMEHOW CLEVERLY COMBINE THESE TWO MAYBE?

	////some will zero with themselves because dot(0,0) = 0
	//arma::mat crossActivation_Matrix(const std::vector<arma::ucube>& dict);

	//arma::vec crossActivation_Sums(const std::vector<arma::ucube>& dict);

	arma::mat crossActivation_halfMatrix(const std::vector<arma::ucube>& dict);

	arma::vec crossActivation_SumsHalfMat(const std::vector<arma::ucube>& dict);

	std::vector<arma::ucube> optDict_crossAct(const std::vector<arma::ucube>& dict);

	std::vector<arma::ucube> optDict_crossActRepeated(const std::vector<arma::ucube>& dict, unsigned desired);

}

#endif