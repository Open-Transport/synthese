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

    PlaceAlias (const uid id = UNKNOWN_VALUE,
		std::string name = std::string(),
		const Place* aliasedPlace = NULL,
		const City* city = NULL);

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

	void setAliasedPlace(const Place* place);
    //@}



};


}
}

#endif 	    

