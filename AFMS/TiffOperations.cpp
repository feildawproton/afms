#include "TiffOperations.h"
#include "tiffio.h"

namespace Tiff_System
{
	void tiff_toUint8Vec(const std::string& fileName, std::vector<std::array<unsigned char, 4>>& RGBA, unsigned& w, unsigned& h)
	{
		const char* pathCharName = fileName.c_str();
		TIFF * tif = TIFFOpen(pathCharName, "r");

		if (tif)
		{
			//uint32 w, h;
			size_t npixels;
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
			npixels = w*h;

			//arma::umat imageMat(h, w);

			uint32* raster = (uint32*)_TIFFmalloc(npixels*sizeof(uint32));

			if (raster != NULL)
			{
				if (TIFFReadRGBAImage(tif, w, h, raster, 0))
				{
					for (unsigned i = 0; i < npixels; i++)
					{
						uint32 value = raster[i];

						std::array<unsigned char, 4> result;  //because the results are RGBA

						result[0] = value;
						result[1] = value >> 8;
						result[2] = value >> 16;
						result[3] = value >> 24;  //always 255 (alpha) on my computer...

						RGBA.push_back(result);
					}
				}
			}
			_TIFFfree(raster);  //FREEDOM!!!
		}
		TIFFClose(tif); //close it!
	}
}