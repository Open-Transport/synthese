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

	RGBColor (const std::string& colorName);
	
	
};



}
}

bool operator == ( const synthese::util::RGBColor& op1, const synthese::util::RGBColor& op2 );


#endif
