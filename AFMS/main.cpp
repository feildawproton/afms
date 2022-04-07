#include "FileProducers.h"
#include "TiffOperations.h"
#include "FormatFunctions.h"
#include "FormatProducers.h"
#include "DictionaryFunctions.h"
#include "DictionaryOperations.h"
#include "DictionaryProducers.h"
#include "NodeDescriptions.h"
#include "NodeFunctions.h"

namespace FileS = File_System;
namespace TiffS = Tiff_System;
namespace FormS = Format_System;
namespace DictS = Dictionary_System;
namespace NodeS = Node_System;

#include <iostream>

#include "armadillo"

#include "boost\thread.hpp"
#include "boost\progress.hpp"

//returns -1 if no
int doesCatagoryExist(const std::vector<std::string>& list, const std::string& catagory)
{
	bool exists = false;
	int position = 0; //this thing is signed for a reason
	for (unsigned j = 0; j < list.size(); j++)
	{
		if (catagory == list[j])
		{
			position = j;
			exists = true;
		}
	}
	if (exists)
	{
		return position;
	}
	else
	{
		return -1;
	}
}

std::vector<std::string> uniqueList(const std::vector<std::string>& list)
{
	std::vector<std::string> current;

	int notUnique = 0;

	for (unsigned i = 0; i < list.size(); i++)
	{
		bool exists = false;
		for (unsigned j = 0; j < current.size(); j++)
		{
			if (list[i] == current[j])
			{
				exists = true;
			}
		}
		if (!exists)
		{
			current.push_back(list[i]);
		}
	}
	return current;
}

int main()
{

	unsigned pRows = 10; //other option 10, 10, 5
	unsigned pCols = 10;
	unsigned pSlices = 5;

	//I like square
	unsigned iRows = pRows*pRows;
	unsigned iCols = pCols*pCols;
	unsigned iSlices = 25;

	unsigned numBases = 128;
	unsigned numRawPatches = 2048;  //small number for debug time spent
	unsigned numCulledPatches = 256;
	double alpha = 0.5;

	bool overSample = false;

	//FormS::createTrainTestCubes_fromRaw("C:\\Users\\Alexander\\Desktop\\AFMS\\records20160925", iRows, iCols, iSlices, 0.8, "AFMSformatted");

	DictS::createWriteDictionaries("C:\\Users\\Alexander\\Desktop\\AFMS\\AFMSformatted\\Training", "AFMSdictionaries",
		numRawPatches, numBases, pRows, pCols, pSlices,
		overSample, alpha);

	std::vector<arma::ucube> pushUp = DictS::loadTrain_singleType("C:\\Users\\Alexander\\Desktop\\AFMS\\AFMSformatted\\Training\\1_Squat");

	std::vector<std::vector<arma::ucube>> patchesez = DictS::evenlySample_SingleType(pushUp, pRows, pCols, pSlices);

	std::vector<arma::ucube > patches = DictS::concatPatches(patchesez);

	//small for debug
	std::vector<arma::ucube> initDict = DictS::dictInitUniform(patches, numRawPatches);

	//std::vector<arma::ucube> culledDict_Act = DictS::optDict_crossActRepeated(initDict, numCulledPatches);

	std::vector<arma::ucube> SCdict = DictS::optDict_SCcost(initDict, pushUp, numBases, false, alpha);

	bool gool = DictS::writeDictionary(SCdict, "C:\\Users\\Alexander\\Desktop\\AFMS\\TempDict");

	std::vector<arma::ucube> dictionary = DictS::loadDictionary_singleType("C:\\Users\\Alexander\\Desktop\\AFMS\\TempDict");

	//lets test this then
	//arma::mat actMat = DictS::activationSum_eaFeatData(SCdict, pushUp, alpha);



	//arma::mat normedActMat = arma::normalise(actMat, 0, 0); //normalise the columns for safety

	std::string catagory1 = "Complete";
	std::vector<std::string> catagory1_Classes = { "1F", "1M", "2M", "3M", "2F", "3M", "3M", "3F" };
	std::string catagory2 = "Lag";
	std::vector<std::string> catagory2_Classes = { "Lag", "Lag", "No Lag", "No Lag", "No Lag", "No Lag", "No Lag", "No Lag" };

	std::vector<std::string> catagories = {catagory1, catagory2};
	std::vector<std::vector<std::string>> classifications = { catagory1_Classes, catagory2_Classes };

	NodeS::level pushUpLevel = NodeS::createLevel(SCdict, pushUp, catagories, classifications, 0.5);

	//kind of weird but pushup only has 1 catagory

	////concat act mat for catagory codes
	//std::vector<std::string> catagory_Labels;
	//std::vector<unsigned> count;
	//arma::mat catActMat;
	//for (unsigned i = 0; i < actMat.n_cols; i++)
	//{
	//	std::string currentLabel = catagory_Complete_Lables[i];

	//	int position = doesCatagoryExist(catagory_Labels, currentLabel);

	//	if (position == -1)
	//	{
	//		catagory_Labels.push_back(currentLabel);
	//		catActMat.insert_cols(catActMat.n_cols, actMat.col(i));
	//		count.push_back(1);
	//	}
	//	else
	//	{
	//		

	//		arma::mat temp;
	//		temp.insert_cols(0, catActMat.col(position));
	//		temp.insert_cols(1, actMat.col(i));

	//		arma::vec sum = arma::sum(temp, 1); //sum the rows of temp

	//		catActMat.shed_col(position);
	//		catActMat.insert_cols(position, sum);

	//		count[position] = count[position] + 1;
	//	}
	//}
	////now we need to average
	//for (unsigned i = 0; i < catActMat.n_cols; i++)
	//{
	//	catActMat.col(i) = catActMat.col(i) / count[i];
	//}

	//arma::vec var = arma::var(catActMat, 0, 1); //variance acrros the columns
	//
	////std::cout << "\nNum features = " << actMat.n_rows << ", Num data = " << actMat.n_cols << std::endl;

	//std::cout << "\nThe catagory order is ";

	//for (unsigned i = 0; i < catagory_Labels.size(); i++)
	//{
	//	std::cout << catagory_Labels[i] << " ";
	//}

	//std::cout << std::endl;

	//for (unsigned r = 0; r < catActMat.n_rows; r++)
	//{
	//	std::cout << std::endl;
	//	for (unsigned c = 0; c < catActMat.n_cols; c++)
	//	{
	//		std::cout << " " << catActMat(r, c);
	//	}
	//}
	
	//arma::cube actiCube = DictS::activationCube_ofFeature(squat[0], uniDict[2], false, 0.0);

	//for (unsigned slice = 0; slice < actiCube.n_slices; slice++)
	//{
	//	std::cout << std::endl;
	//	for (unsigned row = 0; row < actiCube.n_rows; row++)
	//	{
	//		std::cout << std::endl;
	//		for (unsigned col = 0; col < actiCube.n_cols; col++)
	//		{
	//			std::cout << actiCube(row, col, slice) << " ";
	//		}

	//	}
	//}


	////lets see the squat
 //	for (unsigned i = 0; i < uniDict.size(); i++)
	//{
	//	for (unsigned slice = 0; slice < pSlices; slice++)
	//	{
	//		std::cout << std::endl;
	//		for (unsigned row = 0; row < pRows; row++)
	//		{
	//			std:: cout << std::endl;
	//			for (unsigned col = 0; col < pCols; col++)
	//			{
	//				arma::ucube cub = uniDict[i];
	//				std::cout << (unsigned)cub(row, col, slice) << " ";
	//			}

	//		}
	//	}
	//}

 	std::cin.get();

	return 0;
}