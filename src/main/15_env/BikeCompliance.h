#ifndef SYNTHESE_BIKE_COMPLIANCE_H
#define SYNTHESE_BIKE_COMPLIANCE_H


#include "Compliance.h"

#include <boost/logic/tribool.hpp>
#include <string>

namespace synthese
{
namespace env
{




/** Bike compliance class.
 @ingroup m15
 */
class BikeCompliance : public Compliance
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
