#include "Environment.h"


namespace synthese
{
namespace env
{


Environment::Environment ()
{
}



Environment::~Environment ()
{

}


City::Registry&
Environment::getCities ()
{
    return _cities;
}







}
}

