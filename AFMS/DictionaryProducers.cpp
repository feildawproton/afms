#include "DictionaryProducers.h"
#include "DictionaryFunctions.h"
#include "FileFunctions.h"
#include "FileProducers.h"
#include "FormatProducers.h"

#include <iostream>

#include "boost\thread.hpp"
#include "boost\progress.hpp"

namespace FileS = File_System;

namespace Dictionary_System
{
	bool writeDictionary(const std::vector<arma::ucube>& dictionary, const std::string& location)
	{
		for (unsigned i = 0; i < dictionary.size(); i++)
		{
			std::string path = location + "\\feat" + std::to_string(i);

			dictionary[i].save(path, arma::arma_ascii);
		}
		
		std::vector<std::string> ping = FileS::get_FullPathNames_ofFiles(location);

		if (ping.size() == dictionary.size())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void createWriteDictionary(
		const std::string& dataPath, const std::string& dictPath,
		unsigned numRawPatches, unsigned numBases, unsigned pRows, unsigned pCols, unsigned pSlices,
		bool overSample, double alpha)
	{
		std::vector<arma::ucube> pushUp = loadTrain_singleType(dataPath);

		std::vector<std::vector<arma::ucube>> patchesez = evenlySample_SingleType(pushUp, pRows, pCols, pSlices);

		std::vector<arma::ucube > patches = concatPatches(patchesez);

		//small for debug
		std::vector<arma::ucube> initDict = dictInitUniform(patches, numRawPatches);

		std::vector<arma::ucube> SCdict = optDict_SCcost(initDict, pushUp, numBases, overSample, alpha);

		bool fool = writeDictionary(SCdict, dictPath);
	}

	void createWriteDictionaries(const std::string& trainingPath, const std::string& newDirName,
		unsigned numRawPatches, unsigned numBases, unsigned pRows, unsigned pCols, unsigned pSlices,
		bool overSample, double alpha)
	{
		std::cout << "\nCreating Dictionaries.  This may take a few minutes" << std::endl;
		boost::progress_timer timer(std::clog);

		std::vector<std::string> data_PathNames = FileS::get_FullPathNames_ofSubDirectories(trainingPath);

		//go up twice
		std::string allDataDir = FileS::get_ParentPathName(trainingPath);
		std::string rootDir = FileS::get_ParentPathName(allDataDir);
		//create a new director
		std::string dictDir = FileS::create_Directory(rootDir + "\\" + newDirName);
		//copy folder structure
		std::vector<std::string> dict_SubDirectories = FileS::copy_DirectoryStructure_toDirectory(trainingPath, dictDir);

		boost::thread_group group;
		for (unsigned i = 0; i < data_PathNames.size(); i++)
		{
			group.create_thread(boost::bind(
				createWriteDictionary, data_PathNames[i], dict_SubDirectories[i],
				numRawPatches, numBases, pRows, pCols, pSlices,
				overSample, alpha));
		}
		group.join_all();
		std::cout << "\nDictionary Creation Complete" << std::endl;
	}
}