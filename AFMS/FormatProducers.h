#ifndef FORMATPRODUCERS_H
#define FORMATPRODUCERS_H

#include <string>

namespace Format_System
{
	void createTrainTestCubes_fromRaw(
		const std::string& rawRootDirectory, unsigned nRows, unsigned nCols, unsigned nSlices, double trainingFraction, const std::string& newDirName);
}

#endif
