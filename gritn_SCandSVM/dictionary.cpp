#include "boost\thread.hpp"
#include "boost\progress.hpp"

#include "armadillo"

#include "dictionary.h"
#include "fileStuff.h"
#include "helpers.h"

namespace SCandSVMspace
{
	void createSave_formattedData_from2DRaw(
		const std::string& raw_FinalDirectory, const std::string& formatted_FinalDirectory, int nRows, int nCols, const std::string& shortName)
	{
		boost::progress_timer timer(std::clog);
		std::vector<std::string> rawData_PathNames = get_FullPath_Names_ofFiles(raw_FinalDirectory);
		std::vector<std::string> formattedData_PathNames = create_File_PathNames(raw_FinalDirectory, formatted_FinalDirectory, ".mtrx");

		//imma do this in linear order (possible improvement here)
		for (int i = 0; i < rawData_PathNames.size(); i++)
		{
			arma::mat datMat(nRows, nCols);
			datMat.load(rawData_PathNames[i]); //don't assume type let this library do it

			arma::mat reMat = myreshape_viaSample(datMat, nRows, nCols);

			std::string newName = formattedData_PathNames[i];
			reMat.save(newName, arma::arma_binary); //for the size
		}
		std::cout << "\n" << shortName << " COMPLETED IN: \n";
	}

	std::string createFormattedData_from2DRaw(
		const std::string& rawDirectoryName, int nRows, int nCols, const std::string& dirExtension)
	{
		std::vector<std::string> raw_SubDirectory_PathNames = get_FullPath_Names_ofSubDirectories(rawDirectoryName);
		std::string formatted_DirName = rawDirectoryName + dirExtension;
		std::vector<std::string> formatted_SubDirectory_PathNames =
			copy_DirectoryStructure_toDirectory_FullNames(rawDirectoryName, formatted_DirName);
		std::vector<std::string> formatted_SubDirectoy_Names =
			get_SubDirectoryNames_inDirectory(rawDirectoryName);

		if (raw_SubDirectory_PathNames.size() != formatted_SubDirectory_PathNames.size())
		{
			std::cout << "number of sub directories does not match" << std::endl;
		}
		else
		{
			//thread this fool thing
			boost::thread_group group;
			for (int i = 0; i < formatted_SubDirectory_PathNames.size(); i++)
			{
				std::cout << "\nCreating Reshape thread: " << formatted_SubDirectoy_Names[i] << std::endl;
				group.create_thread(boost::bind(
					createSave_formattedData_from2DRaw, raw_SubDirectory_PathNames[i], formatted_SubDirectory_PathNames[i], nRows, nCols, formatted_SubDirectoy_Names[i]));
			}
			group.join_all();
		}
		return formatted_DirName;
	}

	void createSave_TrainTest_from2DFormatted(
		const std::string& formatted_FinalDirectory, const std::string& justDirName, const std::string& trainDirectory, const std::string& testDirectory, float trainingFraction)
	{
		boost::progress_timer timer(std::clog);
		std::vector<std::string> rawDataFile_PathNames = get_FullPath_Names_ofFiles(formatted_FinalDirectory);
		std::vector<std::string> trainData_PathNames = create_File_PathNames(formatted_FinalDirectory, trainDirectory, ".mtrx");
		std::vector<std::string> testData_PathNames = create_File_PathNames(formatted_FinalDirectory, testDirectory, ".mtrx");

		int numTraining = round(trainingFraction*rawDataFile_PathNames.size());
		//get better sampling of data by starting at the beginning and at the middle
		for (int i = 0; i < (rawDataFile_PathNames.size() / 2); i++)
		{
			int j = (rawDataFile_PathNames.size() / 2) + i;
			//hate having and if
			if (i < (numTraining / 2))
			{
				copyFile(rawDataFile_PathNames[i], trainData_PathNames[i]);
				copyFile(rawDataFile_PathNames[j], trainData_PathNames[j]); //and the otherone
			}
			else
			{
				copyFile(rawDataFile_PathNames[i], testData_PathNames[i]);
				copyFile(rawDataFile_PathNames[j], testData_PathNames[j]);
			}
		}
		std::cout << "\n" << justDirName << " COMPLETED IN: \n";
	}

	std::string createTrainTest_from2DFormatted(const std::string& rootDirectory, const std::string& dirExtension, float trainingFraction)
	{
		std::vector<std::string> formatted_SubDirectory_PathNames = get_FullPath_Names_ofSubDirectories(rootDirectory);
		std::vector<std::string> formatted_SubDirectory_FileNames = get_SubDirectoryNames_inDirectory(rootDirectory);

		std::string TT_DirName = rootDirectory + "TrainTest";
		std::string TT_DirPath = create_Directory(TT_DirName);
		std::string trainPath = create_Directory(TT_DirPath + "\\TrainX");
		std::string testPath = create_Directory(TT_DirPath + "\\TestX");

		std::vector<std::string> train_SubDirectory_PathNames =
			copy_DirectoryStructure_toDirectory_FullNames(rootDirectory, trainPath);
		std::vector<std::string> test_SubDirectory_PathNames =
			copy_DirectoryStructure_toDirectory_FullNames(rootDirectory, testPath);

		boost::thread_group group;
		for (int i = 0; i < formatted_SubDirectory_PathNames.size(); i++)
		{
			std::cout << "\nCreating Copy thread " << formatted_SubDirectory_FileNames[i] << std::endl;

			group.create_thread(boost::bind(
				createSave_TrainTest_from2DFormatted,
				formatted_SubDirectory_PathNames[i], formatted_SubDirectory_FileNames[i], train_SubDirectory_PathNames[i], test_SubDirectory_PathNames[i], trainingFraction));
		}
		group.join_all();

		return TT_DirPath;
	}
}