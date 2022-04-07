#include "DictionaryOperations.h"
#include <algorithm> 

namespace Dictionary_System
{
#pragma region not declared in interface

	void normalize(arma::ucube& cube, unsigned Lower, unsigned Upper)
	{
		unsigned localMin = cube.min();
		unsigned localMax = cube.max();
		unsigned localRange = localMax - localMin;

		unsigned MinDiff = 0;
		if (Lower > localMin)
		{
			//YOU SHOULD NOT ALLOW THIS TO HAPPEN!!!
		}
		else
		{
			MinDiff = localMin - Lower;
		}

		double ratio = 1;
		if (localRange != 0)
		{
			ratio = (double)(Upper - Lower) / (double)localRange;
		}

		//a lambda
		//the & means it captures local scope and allows use to use min and rat
		cube.for_each([&](arma::ucube::elem_type& elem) { elem = (unsigned)round((((double)elem - (double)MinDiff)*ratio)); });
	}

#pragma endregion

#pragma region declared in interface
	void normalize(arma::ucube& cube)
	{
		unsigned min = cube.min();
		unsigned max = cube.max();
		unsigned range = max - min;
		double ratio = 1;
		if (range != 0)
		{
			ratio = (double)255 / (double)range;
		}

		//a lambda
		//the & means it captures local scope and allows use to use min and rat
		cube.for_each([&](arma::ucube::elem_type& elem) { elem = (unsigned)round((((double)elem - (double)min)*ratio)); });
	}

	void normalize_Individually(std::vector<arma::ucube>& cubes)
	{
		//range based for 
		//pass the reference to avoid copying and get it back (ie not const)
		for (auto &cube : cubes)
		{
			normalize(cube);
		}
	}

	void normalize_AccrossVec(std::vector<arma::ucube>& cubes)
	{
		//uh mo loops
		unsigned OveralMin = 255; //reversed? you'll see what I'm going for
		unsigned OverallMax = 0;

		//i know the type but use auto for reasons
		//const here for the safeness
		for(const auto &cube : cubes)
		{
			unsigned thisMin = cube.min();
			if (thisMin < OveralMin)
			{
				OveralMin = thisMin;
			}
			unsigned thisMax = cube.max();
			if (thisMax > OverallMax)
			{
				OverallMax = thisMax;
			}
		}

		//lets loop again for old times sake
		//not const this time
		for (auto &cube : cubes)
		{
			normalize(cube, OveralMin, OverallMax);
		}

	}

	void midEnhanceByteCube(arma::ucube& dat)
	{
		//lotta ifs
		dat.for_each( [](arma::ucube::elem_type& elem)
			{ 
				if (elem > 85)
				{
					if (elem > 127)
					{
						if (elem > 170)
						{
							elem = 0;
						}
						else
						{
							int temp = elem*(-255 / 43) + 1008;
							elem = unsigned(temp);
						}
					}
					else
					{
						int temp = elem*(255 / 42) - 516;
						elem = (unsigned)temp;
					}
				}
				else
				{
					elem = 0;
				}
					 
			}
		);
	}

	void midEnhance_CollectionByteCube(std::vector<arma::ucube>& data)
	{
		for (arma::ucube& dat : data)
		{
			midEnhanceByteCube(dat);
		}
	}

	void saveCubes(std::vector<arma::ucube>& cubes, const std::string& directory)
	{
		//iterate with int this time?
		for (unsigned i = 0; i < cubes.size(); i++)
		{
			std::string saveLocation = directory + "\\patch" + std::to_string(i);
			cubes[i].save(saveLocation, arma::arma_ascii);
		}
	}

#pragma endregion

}