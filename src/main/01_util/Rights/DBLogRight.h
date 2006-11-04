
#ifndef SYNTHESE_DB_LOG_RIGHT_H
#define SYNTHESE_DB_LOG_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la gestion des journaux.
			@ingroup m34

			Les niveaux de droit utilis�s sont les suivants :
				- READ : Permet de consulter un journal
				- DELETE : Permet de purger un journal

			D�finition du p�rim�tre :
				- NB : Le p�rim�tre n'est pas d�fini par l'habilitation DBLogRight qui porte sur le principe m�me d'acc�der aux journaux. Chaque journal fait � son tour l'objet d'un contr�le de droits le plus souvent bas� sur les habilitations du module auquel ils appartiennent. Voir la documentation de chaque journal.
		*/
		class DBLogRight : public Right
		{

		};
	}
}

#endif