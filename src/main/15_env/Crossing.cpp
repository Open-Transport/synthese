#include "Crossing.h"



namespace synthese
{
namespace env
{


Crossing::Crossing (const uid& id,
		    const std::string& name,
		    const City* city,
		    const ConnectionType& connectionType)
    : synthese::util::Registrable<uid, Crossing> (id)
    , ConnectionPlace (name, city, connectionType)
{
}



Crossing::~Crossing ()
{
}







}
}
