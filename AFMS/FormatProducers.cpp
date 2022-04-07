#include "FormatProducers.h"

#include "FileFunctions.h"
#include "FileProducers.h"
#include "FormatFunctions.h"

namespace FileS = File_System;
namespace FormS = Format_System;

#include "boost\thread.hpp"
#include "boost\progress.hpp"

#include "armadillo"

namespace Format_System
{
#pragma region no declared in interface

	void createSave_TrainTestCubeS_fromRaw(
		const std::string& raw_PenultimateDir, const std::string& trainDirectory, const std::string& testDirectory, unsigned nRows, unsigned nCols, unsigned nSlices, double trainingFraction)
	{
		//boost::progress_timer timer(std::clog);

		std::vector<std::string> rawDataFolder_PathNames = FileS::get_FullPathNames_ofSubDirectories(raw_PenultimateDir);
		std::vector<std::string> trainData_PathNames = FileS::FilePathNames_fromFolders(raw_PenultimateDir, trainDirectory, ".cb"); 
		std::vector<std::string> testData_PathNames = FileS::FilePathNames_fromFolders(raw_PenultimateDir, testDirectory, ".cb");  //duplication but so what

		//to cute by half
		unsigned numTraining = round(trainingFraction*rawDataFolder_PathNames.size());
		unsigned max = 0;
		for (unsigned i = 0; i < (rawDataFolder_PathNames.size()/2); i++)
		{
			unsigned j = (rawDataFolder_PathNames.size() / 2) + i;
			max = j;
			if (i < (numTraining / 2))
			{
				arma::ucube eye = FormS::convert_tiffFolder_toCube(rawDataFolder_PathNames[i], nRows, nCols, nSlices);
				eye.save(trainData_PathNames[i], arma::arma_ascii);

				arma::ucube jay = FormS::convert_tiffFolder_toCube(rawDataFolder_PathNames[j], nRows, nCols, nSlices);
				jay.save(trainData_PathNames[j], arma::arma_ascii);
			}
			else
			{
				arma::ucube eye = FormS::convert_tiffFolder_toCube(rawDataFolder_PathNames[i], nRows, nCols, nSlices);
				eye.save(testData_PathNames[i], arma::arma_ascii);

				arma::ucube jay = FormS::convert_tiffFolder_toCube(rawDataFolder_PathNames[j], nRows, nCols, nSlices);
				jay.save(testData_PathNames[j], arma::arma_ascii);
			}
		}
		//need another loop to complete 
		//add the left over to testing

		//std::cout << "\n length is " << (unsigned)rawDataFolder_PathNames.size() << " and max j is " << (unsigned)max << std::endl;

		if (max < (rawDataFolder_PathNames.size() - 1))
		{
			//std::cout << "\n length is " << (unsigned)rawDataFolder_PathNames.size() << " and max j is " << (unsigned)max << std::endl;
			for (unsigned i = max; i < rawDataFolder_PathNames.size(); i++)
			{
				arma::ucube eye = FormS::convert_tiffFolder_toCube(rawDataFolder_PathNames[i], nRows, nCols, nSlices);
				eye.save(testData_PathNames[i], arma::arma_ascii);  //ascii for the ease
			}
		}

		//std::cout << "\n THREAD COMPLETED IN: \n";
	}

#pragma endregion

#pragma region declared in interface

	void createTrainTestCubes_fromRaw(
		const std::string& rawRootDirectory, unsigned nRows, unsigned nCols, unsigned nSlices, double trainingFraction, const std::string& newDirName)
	{
		std::cout << "\nFormatting data.  This may take a few minutes" << std::endl;
		boost::progress_timer timer(std::clog);

		std::vector<std::string> raw_SubDirectory_PathNames = FileS::get_FullPathNames_ofSubDirectories(rawRootDirectory);

		std::string TrainTest_DirName = FileS::create_Directory(FileS::get_ParentPathName(rawRootDirectory) + "\\" + newDirName);
		std::string Training_DirName = TrainTest_DirName + "\\Training";
		std::string Testing_DirName = TrainTest_DirName + "\\Testing";

		std::vector<std::string> Training_SubDirectory_PathNames =
			FileS::copy_DirectoryStructure_toDirectory(rawRootDirectory, Training_DirName);

		std::vector<std::string> Testing_SubDirectory_PathNames =
			FileS::copy_DirectoryStructure_toDirectory(rawRootDirectory, Testing_DirName);

		std::vector<std::string> raw_SubDirectoy_Names =
			FileS::get_SubDirectoryNames(rawRootDirectory);

		//thread this fool thing
		boost::thread_group group;
		for (int i = 0; i < raw_SubDirectoy_Names.size(); i++)
		{
			//std::cout << "\nCreating Reshape thread: " << raw_SubDirectoy_Names[i] << std::endl;
			group.create_thread(boost::bind(
				createSave_TrainTestCubeS_fromRaw, raw_SubDirectory_PathNames[i], Training_SubDirectory_PathNames[i], Testing_SubDirectory_PathNames[i], nRows, nCols, nSlices, trainingFraction));
		}
		group.join_all();

		std::cout << "\nFormatting Complete " << std::endl;
	}

#pragma endregion
}