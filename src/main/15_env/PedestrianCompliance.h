#ifndef SYNTHESE_PEDESTRIAN_COMPLIANCE_H
#define SYNTHESE_PEDESTRIAN_COMPLIANCE_H


#include "Compliance.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"


#include <boost/logic/tribool.hpp>
#include <string>

namespace synthese
{
namespace env
{




/** Pedestrian compliance class.
 @ingroup m15
 */
class PedestrianCompliance : 
    public synthese::util::Registrable<uid,PedestrianCompliance>,
    public Compliance
{
private:

public:

    PedestrianCompliance (const uid& id, 
		    const boost::logic::tribool& compliant,
		    const int& capacity);

    ~PedestrianCompliance();


};


}
}


#endif
