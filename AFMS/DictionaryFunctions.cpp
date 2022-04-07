#include "DictionaryFunctions.h"
#include "FileFunctions.h"

namespace FileS = File_System;

namespace Dictionary_System
{
	
	std::vector<arma::ucube> patches_singleType(const std::vector<arma::ucube>& trainData, unsigned numPatches, unsigned pRows, unsigned pCols, unsigned pSlices)
	{
		std::vector<arma::ucube> patches;

		std::srand(time(NULL)); //seed the thing with time
		for (unsigned i = 0; i < numPatches; i++)
		{
			unsigned range = trainData.size();
			unsigned patchIndex = rand() % range + 0;

			arma::ucube selectedCube = trainData[patchIndex]; //this is the cube we will pick our patch from

			arma::ucube patch(pRows, pCols, pSlices);
						
			unsigned rRow = rand() % (selectedCube.n_rows - pRows) + 0; //this is the row where our patch will start
			unsigned rCol = rand() % (selectedCube.n_cols - pCols) + 0; //this is the col where our patch will start
			unsigned rSlc = rand() % (selectedCube.n_slices - pSlices) + 0;  //this is the slice where our patch will start

			//there has got to be a better way than nested loops...
			for (unsigned R = 0; R < pRows; R++)
			{
				for (unsigned C = 0; C < pCols; C++)
				{
					for (unsigned S = 0; S < pSlices; S++)
					{
						unsigned arr = R + rRow;
						unsigned cee = C + rCol;
						unsigned ess = S + rSlc;

						patch(R, C, S) = selectedCube(arr, cee, ess);
					}
				}
			}
			patches.push_back(patch);
		}
		return patches;
	}

	std::vector<arma::ucube> loadTrain_singleType(const std::string& directory)
	{
		//std::string name = FileS::get_FolderName(directory);

		//std::cout << "\nLoading the type " << name << std::endl;

		std::vector<std::string> paths = FileS::get_FullPathNames_ofFiles(directory);

		std::vector<arma::ucube> movements;
		for (unsigned i = 0; i < paths.size(); i++)
		{
			arma::ucube individual(100, 100, 100);  //an individual movement.  guessing at size here.  should I not?
			individual.load(paths[i], arma::arma_ascii);
			movements.push_back(individual);
		}

		//std::cout << "\nFinished Loading Type " << name << std::endl;

		return movements;
	}
}