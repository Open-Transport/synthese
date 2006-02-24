
#ifndef SYNTHESE_CAXETAD_H
#define SYNTHESE_CAXETAD_H

class cAxeTAD;

#include "cAxe.h"
#include "cBaseReservations.h"
#include "cTexte.h"
#include "cTableauDynamique.h"
#include "cReservation.h"

typedef int tNombrePlaces;

/** @ingroup m31
*/
class cAxeTAD: public cAxe
{
protected:
	cTexte									_Code;			//!< Prefixe des lignes de l'axe

public:
	cTableauDynamiqueObjets<cReservation>	ListeReservations(const cTexte& __NumeroService);
	tNombrePlaces							NombrePlacesDisponibles(const cTexte& __NumeroService);
	tNombrePlaces							NombrePlacesReservees(const cTexte& __NumeroService);
	cReservation							RechercheReservation(const cTexte& __NomPersonne, const cTexte& __NumeroPersonne);
};

#endif

