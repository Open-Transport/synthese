#ifndef SYNTHESE_BIKE_COMPLIANCE_H
#define SYNTHESE_BIKE_COMPLIANCE_H


#include "Registrable.h"

#include <boost/logic/tribool.hpp>
#include <string>

namespace synthese
{
namespace env
{




/** Bike compliance class.
 @ingroup m15
 */
class BikeCompliance : public Registrable<int,BikeCompliance>
{
    private:
        boost::logic::tribool _compliant;
	int _capacity;

    public:

        BikeCompliance (const int& id, 
			const boost::logic::tribool& compliant,
			const int& capacity);

        ~BikeCompliance();


	//! @name Getters/Setters
	//@{
	int getCapacity () const;
        boost::logic::tribool isCompliant () const;
	//@}

};


}
}


#endif
