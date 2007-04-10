#ifndef SYNTHESE_ENV_PEDESTRIANCOMPLYER_H
#define SYNTHESE_ENV_PEDESTRIANCOMPLYER_H


#include <vector>


namespace synthese
{


namespace env
{

    class PedestrianCompliance;


/** 
    Base class for an entity providing a pedestrian regulation.
    If no regulation is provided, the parent complyer is called.
    
 @ingroup m15
*/
class PedestrianComplyer
{

private:

    const PedestrianComplyer* _parent;

    const PedestrianCompliance* _pedestrianCompliance;

protected:

    PedestrianComplyer (const PedestrianComplyer* parent, 
		  const PedestrianCompliance* pedestrianCompliance = 0);

public:

    ~PedestrianComplyer ();


    //! @name Getters/Setters
    //@{
    const PedestrianCompliance* getPedestrianCompliance () const;
    void setPedestrianCompliance (const PedestrianCompliance* pedestrianCompliance);
	void	setParent(PedestrianComplyer* parent);
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

