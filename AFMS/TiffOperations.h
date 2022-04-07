#ifndef TIFFFOPERATIONS_H
#define TIFFFOPERATIONS_H

#include <string>
#include <vector>
#include <array> //copy constructable and assignable fixed size arrays

//this wraps libTiff
//was going to convert to armadillo mats and cubes here but for simplicity removed
//this is a collection of functions I call a process
//it is the main unit of computation for TIFF OPERATIONS within this project
//it is stateless (there are no side effects (as fas as I can tell...))
namespace Tiff_System
{
	//inline because it doesn't look terrible yet
	//using a vector instead of uint* because I didn't want to allocate here and then force the caller to deallocate
	//it's harder to remember to release memory if you don't see yourself doing it
	//return results will depend on endianness (RGBA vs ABGR)
	//however if image is grayscale 3 of these values will be the same so use one of the middle two ([1] or [2] not [0] or [3])
	//to turn into an armadillo matrix (result is row major):
	//umat myMat(w,h)
	//for(i = 0:RGBA.Size)
	//	datum = RGBA[i].[1] or RGBA[i].[2]
	//	row = (flor)(i/w)
	//	col = i - row*w;
	//	myMat(row, col) = datum //use () not [] for armadillo
	//end
	void tiff_toUint8Vec(const std::string& fileName, std::vector<std::array<unsigned char, 4>>& RGBA, unsigned& w, unsigned& h);
}

#endif
