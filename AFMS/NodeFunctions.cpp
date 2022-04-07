#include "NodeFunctions.h"
#include "NodeDescriptions.h"

#include "DictionaryFunctions.h"

#include <iostream>  //I know this is called Node Functions and it shouldn't have side effects
					 //But I'm giving myself a pass for writing error messages to the console

namespace DictS = Dictionary_System;

namespace Node_System
{
	//returns -1 if no
	int exists(const std::vector<std::string>& list, const std::string& catagory)
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
	node createNode(
		const arma::mat& activationMatrix, const std::string& catagory, const std::vector<std::string>& classLabels)
	{
		arma::mat actMat = activationMatrix; //create a copy because we will manipulate

		node thisNode;
		thisNode.catagory = catagory; //JIC we neednames
		//concat act mat for catagory codes
		//std::vector<std::string> class_Labels;
		std::vector<unsigned> count;
		//arma::mat catActMat;
		for (unsigned i = 0; i < actMat.n_cols; i++)
		{
			std::string currentLabel = classLabels[i];

			int position = exists(thisNode.labels, currentLabel);

			if (position == -1)
			{
				thisNode.labels.push_back(currentLabel);
				thisNode.Activations.insert_cols(thisNode.Activations.n_cols, actMat.col(i));
				count.push_back(1);
			}
			else
			{
				arma::mat temp;
				temp.insert_cols(0, thisNode.Activations.col(position));
				temp.insert_cols(1, actMat.col(i));

				arma::vec sum = arma::sum(temp, 1); //sum the rows of temp

				thisNode.Activations.shed_col(position);
				thisNode.Activations.insert_cols(position, sum);

				count[position] = count[position] + 1;
			}
		}

		thisNode.variances = arma::var(thisNode.Activations, 0, 1);  //variance accross the rows tells us the between class variance

		return thisNode;
	}
	level createLevel(
		const std::vector<arma::ucube>& dictionary, const std::vector<arma::ucube>& data, const std::vector<std::string>& catagories,
		const std::vector<std::vector<std::string>>& classifications, double alpha)
	{
		if (catagories.size() != classifications.size())
		{
			std::cout << "Number of catagories and number of string vectors for classification must match.  Aborting level creation." << std::endl;
			level emptyLevel;
			return emptyLevel;
		}
		else
		{
			level thisLevel;
			thisLevel.dictionary = dictionary;  //creating a copy for good measure
			thisLevel.Activations = DictS::activationSum_eaFeatData(dictionary, data, alpha);
			for (unsigned i = 0; i < classifications.size(); i++)
			{
				node nodey = createNode(thisLevel.Activations, catagories[i], classifications[i]);
				thisLevel.nodes.push_back(nodey);
			}
			return thisLevel;
		}
	}

}