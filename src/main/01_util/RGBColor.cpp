#include "RGBColor.h"

#include "01_util/Conversion.h"

#include <boost/tokenizer.hpp>
#include <iostream>



namespace synthese
{
namespace util
{

RGBColor::RGBColor (const std::string& colorName)
{
	// If color name starts with '(' we expect
	// a triplet of colors (red, green, blue)
	if ((colorName.size() > 0) && (colorName[0] == '('))
	{
	    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(",()");
	    tokenizer tokens(colorName, sep);

		tokenizer::iterator tok_iter = tokens.begin();
		std::string rs = *(tok_iter); ++tok_iter;
		std::string gs = *(tok_iter); ++tok_iter;
		std::string bs = *(tok_iter);

		r = Conversion::ToInt (rs) / 255.0;
		g = Conversion::ToInt (gs) / 255.0;
		b = Conversion::ToInt (bs) / 255.0;

	} else if (colorName == "blue") {
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



}
}

bool operator == ( const synthese::util::RGBColor& op1, const synthese::util::RGBColor& op2 )
{
	return (op1.r == op2.r) && (op1.g == op2.g) && (op1.b == op2.b);
}


