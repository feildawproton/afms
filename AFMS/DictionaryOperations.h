#ifndef DICTIONARYOPERATIONS_H
#define DICTIONARYOPERATIONS_H

#include "armadillo"
#include <vector>
namespace Dictionary_System
{
	//normalizes the cube you pass to it
	//to the range 0 - 255
	void normalize(arma::ucube& cube);

	//normalize each patch with itself
	void normalize_Individually(std::vector<arma::ucube>& cubes);

	//normalize accross all of the patches
	//does this seem reasonable to do?
	void normalize_AccrossVec(std::vector<arma::ucube>& cubes);

	void midEnhanceByteCube(arma::ucube& dat);

	void midEnhance_CollectionByteCube(std::vector<arma::ucube>& data);

	//dont' use this for lots of patches. It takes too long
	void saveCubes(std::vector<arma::ucube>& cubes, const std::string& directory);

	//does nothing
	_inline void whiten(std::vector<arma::ucube>&)
	{
		//nada sorry for now
	}


}

#endif