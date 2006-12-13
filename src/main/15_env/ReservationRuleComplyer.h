#ifndef SYNTHESE_ENV_RESERVATIONRULECOMPLYER_H
#define SYNTHESE_ENV_RESERVATIONRULECOMPLYER_H


#include <vector>


namespace synthese
{


namespace env
{

    class ReservationRule;


/** 
 @ingroup m15
*/
class ReservationRuleComplyer
{

private:

    const ReservationRuleComplyer* _parent;

    ReservationRule* _reservationRule;

protected:

    ReservationRuleComplyer (const ReservationRuleComplyer* parent, 
			     ReservationRule* reservationRule = 0);

public:

    ~ReservationRuleComplyer ();


    //! @name Getters/Setters
    //@{
    const ReservationRule* getReservationRule () const;
    void setReservationRule (ReservationRule* reservationRule);

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

