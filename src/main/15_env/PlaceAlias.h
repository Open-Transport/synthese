#ifndef SYNTHESE_ENV_PLACEALIAS_H
#define SYNTHESE_ENV_PLACEALIAS_H


#include <vector>

#include "IncludingPlace.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"


namespace synthese
{
namespace env
{


/** Place alias.

 @ingroup m15
*/
class PlaceAlias : 
    public synthese::util::Registrable<uid, PlaceAlias>,
    public IncludingPlace
{

private:
        

public:

    PlaceAlias (const uid& id,
		const std::string& name,
		const Place* aliasedPlace);

    virtual ~PlaceAlias ();


    //! @name Getters/Setters
    //@{

    /** Gets official name of this place.
     
        @return aliased place official name.
     */
    const std::string& getOfficialName () const;


    /** Gets aliased place.
     */
    const Place* getAliasedPlace () const;
    
    //@}



};


}
}

#endif 	    
