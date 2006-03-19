#ifndef SYNTHESE_ENV_PLACEALIAS_H
#define SYNTHESE_ENV_PLACEALIAS_H


#include <vector>

#include "IncludingPlace.h"


namespace synthese
{
namespace env
{


/** Place alias.

 @ingroup m15
*/
class PlaceAlias : public IncludingPlace
{

private:
        

public:

    PlaceAlias (const std::string& name,
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
