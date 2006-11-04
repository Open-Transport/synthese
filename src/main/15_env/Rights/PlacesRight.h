
#ifndef SYNTHESE_PLACES_RIGHT_H
#define SYNTHESE_PLACES_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la gestion de lieux.
			@ingroup m15

			Les niveaux de droit public utilis�s sont les suivants :
				- READ : Permet de consulter la liste des lieux et d'en visualiser les d�tails
				- WRITE : Permet de cr�er ou d'�diter un lieu
				- DELETE : Permet de supprimer un lieu

			Aucun droit priv� n'est utilis� pour cette habilitation.

			D�finition du p�rim�tre :
				- TransportNetwork : Autorisation portant sur les lieux desservis par au moins une ligne du r�seau sp�cifi�
				- Line : Autorisation portant sur les lieux desservis par la ligne sp�cifi�e
				- City : Autorisation portant sur les lieux appartenant � la commune sp�cifi�e
				- Place : Autorisation portant sur le lieu sp�cifi�
		*/
		class PlacesRight : public Right
		{

		};
	}
}

#endif