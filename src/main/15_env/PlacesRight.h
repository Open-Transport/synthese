
#ifndef SYNTHESE_PLACES_RIGHT_H
#define SYNTHESE_PLACES_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la gestion de lieux.
			@ingroup m15

			Les niveaux de droit public utilisés sont les suivants :
				- READ : Permet de consulter la liste des lieux et d'en visualiser les détails
				- WRITE : Permet de créer ou d'éditer un lieu
				- DELETE : Permet de supprimer un lieu

			Aucun droit privé n'est utilisé pour cette habilitation.

			Définition du périmètre :
				- TransportNetwork : Autorisation portant sur les lieux desservis par au moins une ligne du réseau spécifié
				- Line : Autorisation portant sur les lieux desservis par la ligne spécifiée
				- City : Autorisation portant sur les lieux appartenant à la commune spécifiée
				- Place : Autorisation portant sur le lieu spécifié
		*/
		class PlacesRight : public Right
		{

		};
	}
}

#endif