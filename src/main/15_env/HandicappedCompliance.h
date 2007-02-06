#ifndef SYNTHESE_ENV_HANDICAPPEDCOMPLIANCE_H
#define SYNTHESE_ENV_HANDICAPPEDCOMPLIANCE_H


#include "Compliance.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"


#include <boost/logic/tribool.hpp>
#include <string>


namespace synthese
{
namespace env
{



/** Handicapped compliance class.
 @ingroup m15
 */
class HandicappedCompliance : 
    public synthese::util::Registrable<uid,HandicappedCompliance>,
    public Compliance
{
private:

public:

    HandicappedCompliance (const uid& id, 
			   const boost::logic::tribool& compliant,
			   const int& capacity);
	HandicappedCompliance();

    ~HandicappedCompliance();

    //! @name Getters/Setters
    //@{
    //@}

};


}
}

#endif

