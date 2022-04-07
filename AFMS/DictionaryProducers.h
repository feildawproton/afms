#ifndef DICTIONARYPRODUCERS_H
#define DICTIONARYPRODUCERS_H

#include <vector>
#include "armadillo"
#include <string>

namespace Dictionary_System
{
	bool writeDictionary(const std::vector<arma::ucube>& dictionary, const std::string& location);

	//this slams the cpu btw
	void createWriteDictionaries(const std::string& trainingPath, const std::string& newDirName,
		unsigned numRawPatches, unsigned numBases, unsigned pRows, unsigned pCols, unsigned pSlices,
		bool overSample, double alpha);
}

#endif