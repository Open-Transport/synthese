#ifndef SYNTHESE_BIKE_COMPLIANCE_H
#define SYNTHESE_BIKE_COMPLIANCE_H


#include "Compliance.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include <boost/logic/tribool.hpp>
#include <string>

namespace synthese
{
namespace env
{




/** Bike compliance class.
 @ingroup m15
 */
class BikeCompliance : 
    public synthese::util::Registrable<uid,BikeCompliance>,
    public Compliance
{
private:

public:

    BikeCompliance (const uid& id, 
		    const boost::logic::tribool& compliant,
		    const int& capacity);

    ~BikeCompliance();


};


}
}


#endif
