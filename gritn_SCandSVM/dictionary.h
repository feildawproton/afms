#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>

namespace SCandSVMspace
{
	//good value for mem stuff is 100x13
	std::string createFormattedData_from2DRaw(
		const std::string& rawDirectoryName, int nRows, int nCols, const std::string& dirExtension);

	std::string createTrainTest_from2DFormatted(
		const std::string& rootDirectory, const std::string& dirExtension, float trainingFraction);


	std::string created3dFormatted_fromRawTIFFS(
		const std::string& rawRootDirectory, int nRows, int nCols, const std::string& dirExtension);

	std::string createTrainTest_from3dFormatted(
		const std::string& rootDirectory, const std::string& newDirExtension, float trainingFraction);

	
	//struct of arrays
	struct TrainTest
	{
		std::vector<arma::mat> Training;
		std::vector<std::string> TrainingLabels;
		std::vector<arma::mat> Testing;
		std::vector<std::string> TestingLabels;
	};
}

#endif