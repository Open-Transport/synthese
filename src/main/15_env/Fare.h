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
    
    typedef enum { FARE_TYPE_ZONING = 0, 
		   FARE_TYPE_SECTION = 1, 
		   FARE_TYPE_DISTANCE = 2 } FareType;

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
    void setName (const std::string& name);

    const FareType& getType () const;
    void setType (const FareType& fareType);
    //@}

};


}
}

#endif

