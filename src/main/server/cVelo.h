
#ifndef SYNTHESE_CVELO_H
#define SYNTHESE_CVELO_H


class cModaliteReservation;

#include <string>
#include "Parametres.h"

/** Modalit�s de prise en charge des v�los
	@ingroup m05
 */
class cVelo
{
private:
	const size_t _id;
	tBool3					vTypeVelo;
	cModaliteReservation*	vResa;
	tContenance				vContenance;
	std::string	vDoc;
	tPrix					vPrix;

public:
	//! \name Modificateurs
	//@{
	void setPrix(tPrix newVal);
	void setDoc(const std::string& newVal);
	void setContenance(tContenance newVal);
	void setResa(cModaliteReservation* newVal);
	void setTypeVelo(tBool3 newVal);
	//@}
	
	//! \name Accesseurs
	//@{
	tContenance Contenance() const;
	tBool3 TypeVelo() const;
	const size_t& getId() const;
	//@}

	cVelo(const size_t&);
	~cVelo();
};

#endif
