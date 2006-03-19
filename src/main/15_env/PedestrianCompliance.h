#ifndef SYNTHESE_PEDESTRIAN_COMPLIANCE_H
#define SYNTHESE_PEDESTRIAN_COMPLIANCE_H


#include "Compliance.h"

#include <boost/logic/tribool.hpp>
#include <string>

namespace synthese
{
namespace env
{




/** Pedestrian compliance class.
 @ingroup m15
 */
class PedestrianCompliance : public Compliance
{
private:

public:

    PedestrianCompliance (const int& id, 
		    const boost::logic::tribool& compliant,
		    const int& capacity);

    ~PedestrianCompliance();


};


}
}


#endif
