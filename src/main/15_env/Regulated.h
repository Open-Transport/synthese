#ifndef SYNTHESE_ENV_REGULATED_H
#define SYNTHESE_ENV_REGULATED_H


#include <vector>


namespace synthese
{


namespace env
{

    class ReservationRule;
    class BikeCompliance;
    class HandicappedCompliance;
    class PedestrianCompliance;


/** A regulated object holds usability rules.
If a regulated object does not specify any rule for a certain usability condition,
it delegates regulation to its regulated parent object (if any).

The regulation hierarchy is the following :
Network => Axis => Path => Service.

 @ingroup m15
*/
class Regulated
{

private:

    const Regulated* _parent;

    ReservationRule* _reservationRule;
    PedestrianCompliance* _pedestrianCompliance;
    HandicappedCompliance* _handicappedCompliance;
    BikeCompliance* _bikeCompliance;

protected:

    Regulated (const Regulated* parent, 
	       ReservationRule* reservationRule = 0,
	       PedestrianCompliance* pedestrianCompliance = 0,
	       HandicappedCompliance* handicappedCompliance = 0,
	       BikeCompliance* bikeCompliance = 0);

public:

    ~Regulated ();


    //! @name Getters/Setters
    //@{
    const ReservationRule* getReservationRule () const;
    void setReservationRule (ReservationRule* reservationRule);

    const PedestrianCompliance* getPedestrianCompliance () const;
    void setPedestrianCompliance (PedestrianCompliance* pedestrianCompliance);

    const HandicappedCompliance* getHandicappedCompliance () const;
    void setHandicappedCompliance (HandicappedCompliance* handicappedCompliance);

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
