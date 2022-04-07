//from examples at boost.org

#define	BOOST_FILESYSTEM_NO_DEPRECATED


#include <iostream>
#include <string>
#include <math.h>
#include <fstream>

#include <armadillo>


//#include "tiffio.h"
#include "tiffHandling.h"
#include "fileStuff.h"
#include "dictionary.h"

namespace ML = SCandSVMspace;


int main(int argc, char* argv[])
{
	//open data and resize
	//create
	std::cout << "enter path to dictionary root " << std::endl;

	std::string dictRoot;
	std::cin >> dictRoot;

	std::string path = ML::createFormattedData_from2DRaw(dictRoot, 100, 13, "Formatted100x13");

	std::string psad = ML::createTrainTest_from2DFormatted(path, "TT80", .8);



	arma::cube nube(100, 13, 1);

	nube.load("C:\\Users\\Alexander.Feild1\\Documents\\Machine\\SCandSVM\\SCandSVM\\memRecordingsCubes\\forward", arma::arma_binary);


    std::cout << "please enter training fraction " << std::endl;

	std::string fraction;
	std::cin >> fraction;
	
	double trainingFaction = std::stod(fraction);



	/*std::string meh;
	std::cin >> meh;

	std::cin.get();*/

	return 0;
}

