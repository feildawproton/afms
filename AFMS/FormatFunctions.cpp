#include "FormatFunctions.h"

#include "FileFunctions.h"
#include "TiffOperations.h"

namespace FSys = File_System;
namespace TSys = Tiff_System;

namespace Format_System
{
#pragma region not declared in interface

	//arma::umat grayTiffToMat_Resized(const std::string& name, unsigned nRows, unsigned nCols)
	//{
	//	std::vector<std::array<unsigned char, 4>> raster;
	//	unsigned iCols; //width 
	//	unsigned iRows; //height

	//	TSys::tiff_toUint8Vec(name, raster, iCols, iRows);

	//	double rowRatio = ((double)iRows) / ((double)nRows);
	//	double colRatio = ((double)iCols) / ((double)nCols);

	//	arma::umat mat(nRows, nCols);

	//	for (int r = 0; r < nRows; r++)
	//	{
	//		for (int c = 0; c < nCols; c++)
	//		{
	//			unsigned rowP = (unsigned)round(r*rowRatio);
	//			unsigned colP = (unsigned)round(c*colRatio);
	//			
	//			unsigned rawPosition = rowP*iCols + colP; //this is fun

	//			//uh more conversions
	//			std::array<unsigned char, 4> datum = raster[rawPosition];
	//			mat(r, c) = datum[1]; //[1] and [2] should be both safe to use on a gray scale image (R=G=B) regardless of endianness of RGBA
	//		}
	//	}

	//	return mat;
	//}
	arma::umat grayTiffToMat_ResizedAVG(const std::string& name, unsigned nRows, unsigned nCols)
	{
		std::vector<std::array<unsigned char, 4>> raster;
		unsigned iCols; //width 
		unsigned iRows; //height

		TSys::tiff_toUint8Vec(name, raster, iCols, iRows);

		double rowRatio = ((double)iRows) / ((double)nRows);
		double colRatio = ((double)iCols) / ((double)nCols);

		arma::umat mat(nRows, nCols);

		//loops in loops in loops :-(
		for (int r = 0; r < nRows; r++)
		{
			for (int c = 0; c < nCols; c++)
			{
				unsigned rowP = (unsigned)round(r*rowRatio);
				unsigned colP = (unsigned)round(c*colRatio);

				unsigned rowPnext = (unsigned)round((r+1)*rowRatio);
				unsigned colPnext = (unsigned)round((c+1)*colRatio);
				
				//check to make sure not out of bounds
				if (rowPnext*colPnext > raster.size())
				{
					//kind of an inelegant edge added to the image?
					unsigned rawPosition1 = rowP*iCols + colP;
					std::array<unsigned char, 4> datum = raster[rawPosition1];
					mat(r, c) = datum[1]; //[1] and [2] should be both safe to use on a gray scale image (R=G=B) regardless of endianness of RGBA
				}
				
				else
				{
					unsigned datumSum = 0;  //USE AN INT NOT A CHAR FOR THE SUM!!!
					// less than takes care of overlap in sum loop
					for (unsigned R = rowP; R < rowPnext; R++)
					{
						for (unsigned C = colP; C < colPnext; C++)
						{
							unsigned rawPosition = R*iCols + C;
							std::array<unsigned char, 4> datum = raster[rawPosition];
							datumSum += datum[1];  //[1] and [2] should be both safe to use on a gray scale image (R=G=B) regardless of endianness of RGBA
						}
					}
					//it might be cool to use the sum but I don't know yet how the testing data will be read int
					unsigned count = (rowPnext - rowP)*(colPnext - colP);
					double average = ((double)datumSum) / ((double)count); //useless castes?
					unsigned avg = (unsigned)average;
					mat(r, c) = avg;
				}
			 
			}
		}

		return mat;
	}
	arma::ucube sliceReshape_viaSample(const arma::ucube& baseCube, unsigned nSlices)
	{
		double sliceRatio = ((double)baseCube.n_slices) / ((double)nSlices);

		arma::ucube newCube(baseCube.n_rows, baseCube.n_cols, nSlices);

		//nexted loops...
		for (unsigned k = 0; k < nSlices; k++)
		{
			unsigned kay = (unsigned)round(k*sliceRatio);
			newCube.slice(k) = baseCube.slice(kay);
		}
		return newCube;
	}
#pragma endregion

#pragma region declared in interface

	arma::ucube convert_tiffFolder_toCube(const std::string& folderPath, unsigned nRows, unsigned nCols, unsigned nSlices)
	{
		std::vector<std::string> tiffPaths = FSys::get_FullPathNames_ofFiles(folderPath);

		arma::ucube tiffCube(nRows, nCols, tiffPaths.size());

		for (unsigned i = 0; i < tiffPaths.size(); i++)
		{
			arma::umat dat = grayTiffToMat_ResizedAVG(tiffPaths[i], nRows, nCols);

			tiffCube.slice(i) = dat;
		}

		arma::ucube tiffCuberesized = sliceReshape_viaSample(tiffCube, nSlices);

		return tiffCuberesized;
	}

#pragma endregion
}