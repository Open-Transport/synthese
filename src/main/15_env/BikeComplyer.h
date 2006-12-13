#ifndef SYNTHESE_ENV_BIKECOMPLYER_H
#define SYNTHESE_ENV_BIKECOMPLYER_H


#include <vector>


namespace synthese
{


namespace env
{

    class BikeCompliance;


/** 
    Base class for an entity providing a bike regulation.
    If no regulation is provided, the parent complyer is called.
    
 @ingroup m15
*/
class BikeComplyer
{

private:

    const BikeComplyer* _parent;

    BikeCompliance* _bikeCompliance;

protected:

    BikeComplyer (const BikeComplyer* parent, 
		  BikeCompliance* bikeCompliance = 0);

public:

    ~BikeComplyer ();


    //! @name Getters/Setters
    //@{
    const BikeCompliance* getBikeCompliance () const;
    void setBikeCompliance (BikeCompliance* bikeCompliance);
    //@}

    //! @name Query methods.
    //@{
    //@}
    
    //! @name Update methods.
    //@{
    //@}
    
    
};


}
}

#endif 	    

