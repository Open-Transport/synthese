#ifndef SYNTHESE_ENV_FARE_H
#define SYNTHESE_ENV_FARE_H


#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include <string>


namespace synthese
{
namespace env
{



/** Fare handling class
 @ingroup m15
*/
class Fare : public synthese::util::Registrable<uid,Fare>
{
 public:
    
    typedef enum { FARE_TYPE_ZONING, 
		   FARE_TYPE_SECTION, 
		   FARE_TYPE_DISTANCE } FareType;

 private:
    
    std::string _name; //!< Fare name
    FareType _type; //!< Fare type

 public:
    
    Fare (const uid& id, 
	  const std::string& name,
	  const FareType& type);
    ~Fare ();
    
    
    //! @name Getters/Setters
    //@{
    const std::string& getName () const;
    const FareType& getType () const;
    //@}

};


}
}

#endif
