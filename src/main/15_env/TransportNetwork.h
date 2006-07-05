#ifndef SYNTHESE_ENV_TRANSPORTNETWORK_H
#define SYNTHESE_ENV_TRANSPORTNETWORK_H

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include <string>


namespace synthese
{
namespace env 
{



/** Transport network.
 @ingroup m15
*/
class TransportNetwork : public synthese::util::Registrable<uid,TransportNetwork>
{
    std::string _name;

 public:

    TransportNetwork ( const uid& id,
	      const std::string& name );
    ~TransportNetwork ();

};



}
}

#endif
