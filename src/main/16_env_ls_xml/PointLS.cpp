#include "PointLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"

#include "15_env/Point.h"


namespace su = synthese::util;

namespace synthese
{
namespace envlsxml
{

const std::string PointLS::POINT_TAG ("point");
const std::string PointLS::POINT_X_ATTR ("x");
const std::string PointLS::POINT_Y_ATTR ("y");


synthese::env::Point
PointLS::Load (XMLNode& node)
{
    // assert (POINT_TAG == node.getName ());

    double x (su::Conversion::ToDouble (
		node.getAttribute (POINT_X_ATTR.c_str())));
    double y (su::Conversion::ToDouble (
		node.getAttribute (POINT_Y_ATTR.c_str())));

    return synthese::env::Point (x, y);
}




XMLNode* 
PointLS::Save (const synthese::env::Point* point)
{
    // ...
}






}
}

