/*------------------------------------*
|                                    |
|  APDOS / APISAPI - SYNTHESE v0.6   |
|  � Hugues Romain 2000/2001         |
|  cArretPhysique.h                  |
|  Header Classes ArretPhysique      |
|                                    |
*------------------------------------*/

#ifndef SYNTHESE_CQUAI_H
#define SYNTHESE_CQUAI_H

#include "NetworkAccessPoint.h"
#include <set>

class cGareLigne;

/** Arrêts physiques (quais, arrêts de bus, etc.).
 @ingroup m15
*/
class cArretPhysique : public NetworkAccessPoint
{
    public:
        /** Vector of Line-Stops */
        typedef std::set
            <cGareLigne*> LineStopVector;

    private:
        //! @name Chainages
        //@{
        LineStopVector _departureLineStopVector; //!< Vector of line-stops of the physical stop
        LineStopVector _arrivalLineStopVector; //!< Vector of line-stops of the physical stop
        //@}

    public:

        //! @name Accesseurs
        //@{
    const LineStopVector& departureLineStopVector() const { return _departureLineStopVector; }
        const LineStopVector& arrivalLineStopVector() const { return _arrivalLineStopVector; }
        //@}

        //! @name Calculateurs
        //@{
        AddressList getAddresses( bool forDeparture ) const;
        //@}


        //! @name Modificateurs
        //@{
        void addDepartureLineStop( cGareLigne* const );
        void addArrivalLineStop( cGareLigne* const );
        //@}


        //! \name Constructeur
        //@{
        cArretPhysique( LogicalPlace* const, size_t );
        ~cArretPhysique();
        //@}
};



#endif
