#include "DictionaryFunctions.h"
#include "DictionaryOperations.h"
#include "FileFunctions.h"
#include <algorithm> //for the max() function

#include <iostream> //I know i said function collections don't have side effects but this is needed for debug

namespace FileS = File_System;

namespace Dictionary_System
{
#pragma region not declared in interface
	
	std::vector<arma::ucube> evenlySample(
		const arma::ucube& data, unsigned sample_nRows, unsigned sample_nCols, unsigned sample_nSlices)
	{
		std::vector<arma::ucube> samples;

		unsigned numR = data.n_rows / sample_nRows; //truncation is goo here
		unsigned numC = data.n_cols / sample_nCols;
		unsigned numS = data.n_slices / sample_nSlices;
		unsigned numSamples = numR*numC*numS;

		samples.reserve(numSamples);  //reserve instead of push back

									  //must I use a crude iterator here?
									  //and a push back?
									  //maybe something clever...  but cant think right now
									  //ok, so I want row major in the samples so rows should be in the inner loop
									  //it's super fast though
		for (unsigned S = 0; S < numS; S++)
		{
			for (unsigned C = 0; C < numC; C++)
			{
				for (unsigned R = 0; R < numR; R++)
				{
					unsigned rStart = sample_nRows * R;
					unsigned cStart = sample_nCols * C;
					unsigned sStart = sample_nSlices * S;
					unsigned rFin = sample_nRows * R + sample_nRows - 1;
					unsigned cFin = sample_nCols * C + sample_nCols - 1;
					unsigned sFin = sample_nSlices * S + sample_nSlices - 1;
					samples.push_back(data.subcube(rStart, cStart, sStart, rFin, cFin, sFin));
				}
			}
		}

		return samples;
	}

	std::vector<arma::ucube> overSample(
		const arma::ucube& data, unsigned sample_nRows, unsigned sample_nCols, unsigned sample_nSlices)
	{
		std::vector<arma::ucube> samples;

		unsigned rUpper = data.n_rows - sample_nRows;
		unsigned cUpper = data.n_cols - sample_nCols;
		unsigned sUpper = data.n_slices - sample_nSlices;
		unsigned numSamples = rUpper*cUpper*sUpper;

		samples.reserve(numSamples);

		//oh dear
		for (unsigned S = 0; S < sUpper; S++)
		{
			for (unsigned C = 0; C < cUpper; C++)
			{
				for (unsigned R = 0; R < rUpper; R++)
				{
					unsigned rStart = R;
					unsigned cStart = C;
					unsigned sStart = S;
					unsigned rFin = R + sample_nRows - 1;
					unsigned cFin = C + sample_nCols - 1;
					unsigned sFin = S + sample_nSlices - 1;
					samples.push_back(data.subcube(rStart, cStart, sStart, rFin, cFin, sFin));
				}
			}
		}

		return samples;
	}

	double calculataActivation_dataSample(
		const arma::ucube& dataSample, const  arma::ucube& feature, double threshold)
	{
		//hmm clever maybe??
		//dims must matck
		if (dataSample.n_rows != feature.n_rows || dataSample.n_cols != feature.n_cols || dataSample.n_slices != feature.n_slices)
		{
			return 0;
		}
		else
		{
			//in the paper I follow he does matrix multiplication for every sample of an image with all the features
			//in that case the features and samples where vector being dotted together
			//since I has a single sample and feature here it makes since to dot them as vectors
			//don't believe uint will be a problem
			arma::uvec dataVec = arma::vectorise(dataSample);
			arma::uvec featureVec = arma::vectorise(feature);

			arma::vec doubleData = arma::conv_to<arma::vec>::from(dataVec);
			arma::vec doubleFeature = arma::conv_to<arma::vec>::from(featureVec);

			double normDot = arma::norm_dot(doubleData, doubleFeature);

			//using soft thesholdings like coates et. al
			double result = std::max((normDot - threshold), 0.0);

			return result;
		}
	}

	arma::cube activationCube_ofFeature(
		const arma::ucube& fullData, const  arma::ucube& feature, bool overBool, double theshold)
	{
		//data must be bigger that feature
		arma::cube distanceCube;
		unsigned nRows = 0;
		unsigned nCols = 0;
		unsigned nSlcs = 0;
		std::vector<arma::ucube> dataSamples;

		if (overBool)
		{
			dataSamples = overSample(fullData, feature.n_rows, feature.n_cols, feature.n_slices);

			//i called the function overSample above so that I could normalize accross all the samples... meh probably no different that below
			//now we have to figure out where to put stuff in a backwards kind of way

			nRows = fullData.n_rows - feature.n_rows;
			nCols = fullData.n_cols - feature.n_cols;
			nSlcs = fullData.n_slices - feature.n_slices;

			unsigned expectedSize = (nRows*nCols*nSlcs);
			unsigned actualSize = dataSamples.size();

			if (actualSize != expectedSize)
			{
				std::cout << "\nOops in overBool" << std::endl;
			}
		}
		else
		{
			dataSamples = evenlySample(fullData, feature.n_rows, feature.n_cols, feature.n_slices);

			//hey!!! this needs to be different for evenly sampled stuff!!!										

			nRows = fullData.n_rows / feature.n_rows;
			nCols = fullData.n_cols / feature.n_cols;
			nSlcs = fullData.n_slices / feature.n_slices;

			unsigned expectedSize = (nRows*nCols*nSlcs);
			unsigned actualSize = dataSamples.size();

			if (actualSize != expectedSize)
			{
				std::cout << "\nOops in !overBool(even sample)" << std::endl;
			}
		}

		//don't forget to normalize
		normalize_AccrossVec(dataSamples); //important

		distanceCube.set_size(nRows, nCols, nSlcs);

		for (unsigned i = 0; i < dataSamples.size(); i++)
		{
			double activation = calculataActivation_dataSample(dataSamples[i], feature, theshold);

			//now where do we go?  I figured this magic out... correctly I hope
			unsigned sliceP = i / (nRows*nCols);  //truncation is the expected behavior please!
			unsigned mati = i - sliceP*nRows*nCols;
			unsigned rowP = mati / nCols;
			unsigned colP = mati - rowP*nCols;

			distanceCube(rowP, colP, sliceP) = activation;
		}

		return distanceCube;
	}

	std::vector<arma::cube> activationCubeS_forFeature_allData(
		const std::vector<arma::ucube>& allData, const arma::ucube& feature, bool overBool, double threshold)
	{
		std::vector<arma::cube> activationCubeS(allData.size());

		//don't forget the ref
		for (int i = 0; i < activationCubeS.size(); i++)
		{
			activationCubeS[i] = activationCube_ofFeature(allData[i], feature, overBool, threshold);
		}
		return activationCubeS;
	}

	arma::cube costCube(const arma::cube& activationCube)
	{
		arma::cube cost = activationCube; //need a copy

										  //another lamba with arma for_each, these seem so much faster than loops
		cost.for_each(
			[](arma::cube::elem_type& elem)
		{
			if (elem == 0.0)
			{
				elem = 1.0;
			}
		}
		);

		return cost;
	}

	std::vector<arma::cube> costCubeS(const std::vector<arma::cube>& activationCubes)
	{
		std::vector<arma::cube> costs(activationCubes.size());

		for (unsigned i = 0; i < activationCubes.size(); i++)
		{
			costs[i] = costCube(activationCubes[i]);
		}
		return costs;
	}

	std::vector<double> SCcost(
		const std::vector<arma::ucube>& initDict, const std::vector<arma::ucube>& allData, unsigned numBases, bool overBool, double threshold)
	{
		//JIC
		std::vector<arma::ucube> dict = initDict;  //do a copy bc this is a functcion
		normalize_AccrossVec(dict);

		std::vector<arma::ucube> dat = allData;
		normalize_AccrossVec(dat);

		std::vector<double> cost(dict.size());

		for (unsigned i = 0; i < cost.size(); i++)
		{
			std::vector<arma::cube> actCubes = activationCubeS_forFeature_allData(dat, dict[i], overBool, threshold);
			std::vector<arma::cube> costCubes = costCubeS(actCubes);

			double costSum = 0; //sum of sums
			for (unsigned j = 0; j < costCubes.size(); j++)
			{
				for (arma::cube& cub : costCubes)
				{
					costSum += arma::accu(cub); //arma accumulate not arma sum 
				}
			}
			cost[i] = costSum;
		}
		return cost;
	}

	double totalActivation_fromFeature(
		const arma::ucube& fullData, const  arma::ucube& feature, bool overBool, double threshold)
	{
		arma::cube activation = activationCube_ofFeature(fullData, feature, overBool, threshold);

		double sum = arma::accu(activation);

		return sum;
	}

#pragma endregion

#pragma region declared in interface
	std::vector<arma::ucube> loadTrain_singleType(
		const std::string& directory)
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

	std::vector<arma::ucube> loadDictionary_singleType(
		const std::string& directory)
	{
		std::vector<std::string> paths = FileS::get_FullPathNames_ofFiles(directory);

		std::vector<arma::ucube> dict;
		for (unsigned i = 0; i < paths.size(); i++)
		{
			arma::ucube individual(10, 10, 5);  //guessing small because these are patches
			individual.load(paths[i], arma::arma_ascii);
			dict.push_back(individual);
		}

		//std::cout << "\nFinished Loading Type " << name << std::endl;

		return dict;
	}

	std::vector<std::vector<arma::ucube>> evenlySample_SingleType(
		const std::vector<arma::ucube>& trainData, unsigned sample_nRows, unsigned sample_nCols, unsigned sample_nSlices)
	{
		std::vector<std::vector<arma::ucube>> dangit(trainData.size());

		//uh gotta count i guess
		for (unsigned i = 0; i < trainData.size(); i++)
		{
			dangit[i] = evenlySample(trainData[i], sample_nRows, sample_nCols, sample_nSlices);
		}
		return dangit;
	}

	std::vector<arma::ucube> concatPatches(const std::vector<std::vector<arma::ucube>>& complicated)
	{
		std::vector<arma::ucube> patches;

		//using an iterator for reasons
		for (unsigned i = 0; i < complicated.size(); i++)
		{
			patches.insert(patches.end(), complicated[i].begin(), complicated[i].end());
		}
		return patches;
	}


	std::vector<arma::ucube> dictInitUniform(
		const std::vector<arma::ucube>& patches, unsigned dictSize)
	{
		std::vector<arma::ucube> selection;
		selection.resize(dictSize); //since i know the size...

		//hmmm uniform selection is essentially random since it was randomly selected before
		unsigned scaling = patches.size()/dictSize; //loss of precision here.  but I think this always rounds down so OK
		for (unsigned i = 0; i < dictSize; i++)
		{
			selection[i] = patches[(i*scaling)];
		}

		normalize_AccrossVec(selection);

		return selection;
	}



	std::vector<arma::ucube> optDict_SCcost(
		const std::vector<arma::ucube>& initDict, const std::vector<arma::ucube>& allData, unsigned numBases, bool overBool, double threshold)
	{

		//finding what to keep by looking for mins
		std::vector<double> costs = SCcost(initDict, allData, numBases, overBool, threshold);
		double maxValue = *std::max_element(costs.begin(), costs.end()); //value pointed to by the max iterator

		std::vector<bool> keep(costs.size(), false);
				
		//find what to keep
		unsigned currentBases = 0;
		while (currentBases < numBases)
		{
			auto minIter = std::min_element(costs.begin(), costs.end()); //min iterator
			unsigned minIndex = std::distance(costs.begin(), minIter);  //distance from begining to iterator of min value

			//we may be manipulating costs but it is a local variable so this is still a function
			costs[minIndex] = maxValue;  //set to max so that we ignore it on the next go round
			keep[minIndex] = true; //we want to keep the feature at the min cost index
			//increase current bases count
			currentBases++;
		}

		//so now lets fill in out new dictionary
		std::vector<arma::ucube> selected;  //lets rely on pushback here since we iterate through keep
		for (unsigned i = 0; i < keep.size(); i++)
		{
			if (keep[i])
			{
				selected.push_back(initDict[i]);
			}
		}
		return selected;
	}

	arma::mat activationSum_eaFeatData(
		const std::vector<arma::ucube>& dictionary, const std::vector<arma::ucube>& data, double threshold)
	{
		//for ea featurePatch
		//compute activations for ea data

		//imma layout like this
		//oh need to normalize!!
		std::vector<arma::ucube> dict = dictionary;
		normalize_AccrossVec(dict);

		std::vector<arma::ucube> dat = data;
		normalize_AccrossVec(dat);

		arma::mat actMatrix(dictionary.size(), data.size());
		for (unsigned r = 0; r < dictionary.size(); r++)
		{
			for (unsigned c = 0; c < data.size(); c++)
			{
				actMatrix( r, c) = totalActivation_fromFeature(dat[c], dict[r], false, threshold);
			}
		}

		return actMatrix;
	}

	arma::mat crossActivation_halfMatrix(const std::vector<arma::ucube>& dict)
	{
		std::vector<arma::ucube> normDict = dict;
		normalize_AccrossVec(normDict); //JIC

		arma::mat selfMat(dict.size(), dict.size());
		//don't do two calculations for 1 number since matrix is reflected
		for (unsigned R = 0; R < dict.size(); R++)
		{
			for (unsigned C = R; C < dict.size(); C++)
			{
				double activation = calculataActivation_dataSample(normDict[R], normDict[C], 0.0); //no thresh this time
				selfMat(R, C) = activation;
				selfMat(C, R) = 0.0;  //overwrite whe R=C
			}
		}
		return selfMat;
	}
	arma::vec crossActivation_SumsHalfMat(const std::vector<arma::ucube>& dict)
	{
		arma::mat selfMat = crossActivation_halfMatrix(dict);

		arma::vec sum = arma::sum(selfMat, 1);  //sum the rows into a single row

		return sum;
	}

	std::vector<arma::ucube> optDict_crossAct(const std::vector<arma::ucube>& dict)
	{
		arma::vec self_HalfMatSums = crossActivation_SumsHalfMat(dict);

		double meady = arma::median(self_HalfMatSums);

		std::vector<arma::ucube> culledDict; //will use push back and let std handle it this time
		
		//don't want something parallel so we keep order
		for (unsigned i = 0; i < self_HalfMatSums.n_rows; i++)
		{
			if (self_HalfMatSums[i] < meady && self_HalfMatSums[i] >= 1.0)
			{
				culledDict.push_back(dict[i]);
			}
		}
		return culledDict;
	}

	std::vector<arma::ucube> optDict_crossActRepeated(const std::vector<arma::ucube>& dict, unsigned desired)
	{
		std::vector<arma::ucube> copyDict = dict;

		while (copyDict.size() >= desired)
		{
			copyDict = optDict_crossAct(copyDict);
		}

		return copyDict;
	}

#pragma endregion
}