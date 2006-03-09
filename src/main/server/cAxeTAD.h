
#ifndef SYNTHESE_CAXETAD_H
#define SYNTHESE_CAXETAD_H

class cAxeTAD;

#include "cAxe.h"
#include "cBaseReservations.h"
#include <string>
#include "cTableauDynamique.h"
#include "cReservation.h"

typedef int tNombrePlaces;

/** @ingroup m31
*/
class cAxeTAD: public cAxe
{
    protected:
        std::string _Code;   //!< Prefixe des lignes de l'axe

    public:
        cTableauDynamiqueObjets<cReservation> ListeReservations( const std::string& __NumeroService );
        tNombrePlaces NombrePlacesDisponibles( const std::string& __NumeroService );
        tNombrePlaces NombrePlacesReservees( const std::string& __NumeroService );
        cReservation RechercheReservation( const std::string& __NomPersonne, const std::string& __NumeroPersonne );
};

#endif

