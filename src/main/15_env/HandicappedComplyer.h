#ifndef SYNTHESE_ENV_HANDICAPPEDCOMPLYER_H
#define SYNTHESE_ENV_HANDICAPPEDCOMPLYER_H


#include <vector>


namespace synthese
{


namespace env
{

    class HandicappedCompliance;


/** 
    Base class for an entity providing a handicapped regulation.
    If no regulation is provided, the parent complyer is called.
    
 @ingroup m15
*/
class HandicappedComplyer
{

private:

    const HandicappedComplyer* _parent;

    HandicappedCompliance* _handicappedCompliance;

protected:

    HandicappedComplyer (const HandicappedComplyer* parent, 
		  HandicappedCompliance* handicappedCompliance = 0);

public:

    ~HandicappedComplyer ();


    //! @name Getters/Setters
    //@{
    const HandicappedCompliance* getHandicappedCompliance () const;
    void setHandicappedCompliance (HandicappedCompliance* handicappedCompliance);
	void	setParent(HandicappedComplyer*	parent);
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

