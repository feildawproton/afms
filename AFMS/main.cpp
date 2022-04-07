#include "FileProducers.h"
#include "TiffOperations.h"
#include "FormatFunctions.h"
#include "FormatProducers.h"
#include "DictionaryFunctions.h"

namespace FileS = File_System;
namespace TiffS = Tiff_System;
namespace FormS = Format_System;
namespace DictS = Dictionary_System;

#include <iostream>

#include "armadillo"

int main()
{

	//FormS::createTrainTestCubes_fromRaw("C:\\Users\\Alexander\\Desktop\\AFMS\\records20160925", 100, 100, 100, 0.8, "AFMSformatted");

	std::vector<arma::ucube> squat = DictS::loadTrain_singleType("C:\\Users\\Alexander\\Desktop\\AFMS\\AFMSformatted\\Training\\1_Squat");

	unsigned numRows = 10;
	unsigned numCols = 10;
	unsigned numSlices = 10;

	std::vector<arma::ucube> patches = DictS::patches_singleType(squat, 50000, numRows, numCols, numSlices);

	//lets see if it has patches
	for (unsigned i = 0; i < patches.size(); i++)
	{
		for (unsigned slice = 0; slice < numSlices; slice++)
		{
			std::cout << std::endl;
			for (unsigned row = 0; row < numRows; row++)
			{
				std:: cout << std::endl;
				for (unsigned col = 0; col < numCols; col++)
				{
					arma::ucube cub = patches[i];
					std::cout << (unsigned)cub(row, col, slice) << " ";
				}

			}
		}
	}

 	std::cin.get();

	return 0;
}