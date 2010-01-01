#include "Angle.h"

#include <cmath>


using namespace std;

namespace synthese
{
namespace geometry
{



double
ToDegrees (double angle)
{
    return 180.0 * angle / M_PI;
}


double
ToRadians (double angle)
{
    return M_PI * angle / 180.0;
}





}
}









