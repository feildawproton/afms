#ifndef NODEDESCRIPTIONS_H
#define NODEDESCRIPTIONS_H

#include <vector>
#include <string>

#include "armadillo"

namespace Node_System
{
	struct node
	{
		std::string catagory;
		std::vector<std::string> labels;
		arma::mat Activations;
		arma::vec variances;
	};

	struct level
	{
		std::vector<arma::ucube> dictionary;
		arma::mat Activations;
		std::vector<node> nodes;
	};
}

#endif
