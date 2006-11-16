
#ifndef SYNTHESE_MESSAGES_LIBRARY_RIGHT_H
#define SYNTHESE_MESSAGES_LIBRARY_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la bibliothèque de messages et de scénarios de diffusion.
			@ingroup m15

			Les niveaux de droit public utilisés sont les suivants :
				- WRITE : Permet de créer, modifier des modèles de messages et des scénarios de diffusion
				- DELETE : Permet de supprimer des modèles de messages et des scénarios de diffusion
			
			Aucun niveau de droit privé n'est utilisé pour cette habilitation.

			Cette habilitation ne peut être paramétrée

		*/
		class MessagesLibraryRight : public Right
		{

		};
	}
}

#endif