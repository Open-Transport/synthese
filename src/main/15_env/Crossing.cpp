#include "Crossing.h"



namespace synthese
{
namespace env
{


Crossing::Crossing (const int& id,
		    const std::string& name,
		    const City* city,
		    const ConnectionType& connectionType)
    : Registrable<int, Crossing> (id)
    , ConnectionPlace (name, city, connectionType)
{
}



Crossing::~Crossing ()
{
}







}
}
