
#ifndef SYNTHESE_DB_LOG_RIGHT_H
#define SYNTHESE_DB_LOG_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la gestion des journaux.
			@ingroup m34

			Les niveaux de droit utilisés sont les suivants :
				- READ : Permet de consulter un journal
				- DELETE : Permet de purger un journal

			Définition du périmètre :
				- NB : Le périmètre n'est pas défini par l'habilitation DBLogRight qui porte sur le principe même d'accéder aux journaux. Chaque journal fait à son tour l'objet d'un contrôle de droits le plus souvent basé sur les habilitations du module auquel ils appartiennent. Voir la documentation de chaque journal.
		*/
		class DBLogRight : public Right
		{

		};
	}
}

#endif