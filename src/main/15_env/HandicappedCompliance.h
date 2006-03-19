#ifndef SYNTHESE_ENV_HANDICAPPEDCOMPLIANCE_H
#define SYNTHESE_ENV_HANDICAPPEDCOMPLIANCE_H


#include "Compliance.h"

#include <boost/logic/tribool.hpp>
#include <string>


namespace synthese
{
namespace env
{



/** Handicapped compliance class.
 @ingroup m15
 */
class HandicappedCompliance : public Compliance
{
private:

public:

    HandicappedCompliance (const int& id, 
			   const boost::logic::tribool& compliant,
			   const int& capacity);

    ~HandicappedCompliance();

    //! @name Getters/Setters
    //@{
    //@}

};


}
}

#endif
