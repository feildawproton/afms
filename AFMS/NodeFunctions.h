#ifndef NODEFUNCTIONS_H
#define NODEFUNCTIONS_H

#include <vector>
#include <string>
#include "armadillo"

#include "NodeDescriptions.h"

namespace Node_System
{
	//arg list not long and unreadable enough?
	level createLevel(
		const std::vector<arma::ucube>& dictionary, const std::vector<arma::ucube>& data, const std::vector<std::string>& catagories,
		const std::vector<std::vector<std::string>>& classifications, double alpha);
}

#endif