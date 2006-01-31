
#ifndef SYNTHESE_CVELO_H
#define SYNTHESE_CVELO_H

// Modalit�s de prise en charge des v�los
class cVelo;

#include "cModaliteReservation.h"
#include "cTexte.h"
#include "Parametres.h"

/** Modalit�s de prise en charge des v�los
	@ingroup m05
 */
class cVelo
{
	tBool3					vTypeVelo;
	cModaliteReservation*	vResa;
	tContenance				vContenance;
	cTexte					vDoc;
	tPrix					vPrix;

public:
	//! \name Modificateurs
	//@{
	void setPrix(tPrix newVal);
	void setDoc(const cTexte& newVal);
	void setContenance(tContenance newVal);
	void setResa(cModaliteReservation* newVal);
	void setTypeVelo(tBool3 newVal);
	//@}
	
	//! \name Accesseurs
	//@{
	tContenance Contenance();
	tBool3 TypeVelo();
	//@}
};

inline void cVelo::setTypeVelo(tBool3 newVal)
{
	vTypeVelo = newVal;
}

inline tBool3 cVelo::TypeVelo()
{
	return(vTypeVelo);
}

inline void cVelo::setResa(cModaliteReservation* newVal)
{
	vResa = newVal;
}


inline void cVelo::setContenance(tContenance newVal)
{
	vContenance = newVal;
}

inline tContenance cVelo::Contenance()
{
	return(vContenance);
}


inline void cVelo::setDoc(const cTexte &newVal)
{
	vDoc.Vide();
	vDoc << newVal;
}


inline void cVelo::setPrix(tPrix newVal)
{
	vPrix = newVal;
}

#endif
