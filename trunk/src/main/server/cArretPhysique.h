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

class cGareLigne;

/*! \brief Arr�ts physiques (quais, arr�ts de bus, etc.)
	@ingroup m05
*/
class cArretPhysique : public NetworkAccessPoint
{

private:
	//! @name Chainages
	//@{
    cGareLigne*  _firstDepartureLineStop; //!< Pointeur vers le premier départ de ligne (ordre aléatoire) \todo trier par orientation ?
    cGareLigne*  _firstArrivalLineStop; //!< Pointeur vers la première arrivée de ligne (ordre aléatoire) \todo trier par orientation ?
	//@}

public:

	//!	@name Accesseurs
	//@{
	    cGareLigne*    PremiereGareLigneArr()         const;
	    cGareLigne*    PremiereGareLigneDep()         const;
	//@}

	//!	@name Calculateurs
	//@{
		AddressList getAddresses(bool forDeparture) const;
	//@}


	//! @name Modificateurs
	//@{
	    void setPremiereGareLigneDep(cGareLigne*);
	    void setPremiereGareLigneArr(cGareLigne*);
	//@}


	//!	\name Constructeur
	//@{
	cArretPhysique(LogicalPlace*, size_t);
	~cArretPhysique();
	//@}
};



#endif
