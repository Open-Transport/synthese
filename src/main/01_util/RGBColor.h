#ifndef SYNTHESE_UTIL_RGBCOLOR_H
#define SYNTHESE_UTIL_RGBCOLOR_H

#include <string>

namespace synthese
{
namespace util
{




struct RGBColor
{
	double r;
	double g;
	double b;
	
	RGBColor (double red, double green, double blue) 
		: r (red), g (green), b (blue) {}
	
	RGBColor (int red, int green, int blue) 
		: r (red/255.0), g (green/255.0), b (blue/255.0) {}

	RGBColor (const std::string& colorName) 
	{
	    if (colorName == "blue") {
		r = 0; g = 0; b = 255;
	    } else if (colorName == "red") {
		r = 255; g = 0; b = 0;
	    } else if (colorName == "green") {
		r = 0; g = 255; b = 0;
	    } else if (colorName == "yellow") {
		r = 255; g = 255; b = 0;
	    } else if (colorName == "magenta") {
		r = 255; g = 0; b = 255;
	    } else if (colorName == "cyan") {
		r = 0; g = 255; b = 255;
	    } else if (colorName == "white") {
		r = 255; g = 255; b = 255;
	    } else if (colorName == "black") {
		r = 0; g = 0; b = 0;
	    }
	}
	
	
};



}
}

#endif
