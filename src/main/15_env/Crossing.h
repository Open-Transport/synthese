#ifndef SYNTHESE_ENV_CROSSING_H
#define SYNTHESE_ENV_CROSSING_H

#include "15_env/ConnectionPlace.h"



namespace synthese
{
namespace env
{

/** Crossing class.

Special kind of ConnectionPlace between roads, with a zero transfer delay between them.

@ingroup m15
*/
class Crossing : public ConnectionPlace
{
 private:

 public:

    Crossing (const uid& key = UNKNOWN_VALUE,
	      const City* city = NULL);
	      
    ~Crossing ();

};





}
}
#endif


