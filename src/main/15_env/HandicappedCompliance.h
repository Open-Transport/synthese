#ifndef SYNTHESE_ENV_HANDICAPPEDCOMPLIANCE_H
#define SYNTHESE_ENV_HANDICAPPEDCOMPLIANCE_H


#include "Registrable.h"

#include <boost/logic/tribool.hpp>
#include <string>


namespace synthese
{
namespace env
{



/** Handicapped compliance class.
 @ingroup m15
 */
class HandicappedCompliance : public Registrable<int,HandicappedCompliance>
{
        boost::logic::tribool _compliant;
	int _capacity;

    public:

        HandicappedCompliance (const int& id, 
			       const boost::logic::tribool& compliant,
			       const int& capacity);

        ~HandicappedCompliance();

	//! @name Getters/Setters
	//@{
	int getCapacity () const;
        boost::logic::tribool isCompliant () const;
        //@}

};


}
}

#endif
