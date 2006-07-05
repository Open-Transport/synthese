#include "TransportNetwork.h"


namespace synthese
{
namespace env
{


TransportNetwork::TransportNetwork (const uid& id, 
		  const std::string& name)
    : synthese::util::Registrable<uid,TransportNetwork> (id)
    , _name (name)
{
}


TransportNetwork::~TransportNetwork()
{
}





}
}
