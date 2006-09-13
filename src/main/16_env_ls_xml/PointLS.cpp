#include "PointLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"

#include "15_env/Point.h"


using namespace synthese::util::XmlToolkit;


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

    double x (GetDoubleAttr (node, POINT_X_ATTR));
    double y (GetDoubleAttr (node, POINT_Y_ATTR));

    return synthese::env::Point (x, y);
}




XMLNode* 
PointLS::Save (const synthese::env::Point* point)
{
    // ...
    return 0;

}






}
}

