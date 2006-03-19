#include "TransportNetwork.h"


namespace synthese
{
namespace env
{


TransportNetwork::TransportNetwork (const int& id, 
		  const std::string& name)
    : Registrable<int,TransportNetwork> (id)
    , _name (name)
{
}


TransportNetwork::~TransportNetwork()
{
}





}
}
