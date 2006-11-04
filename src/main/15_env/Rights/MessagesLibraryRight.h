
#ifndef SYNTHESE_MESSAGES_LIBRARY_RIGHT_H
#define SYNTHESE_MESSAGES_LIBRARY_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la biblioth�que de messages et de sc�narios de diffusion.
			@ingroup m15

			Les niveaux de droit public utilis�s sont les suivants :
				- WRITE : Permet de cr�er, modifier des mod�les de messages et des sc�narios de diffusion
				- DELETE : Permet de supprimer des mod�les de messages et des sc�narios de diffusion
			
			Aucun niveau de droit priv� n'est utilis� pour cette habilitation.

			Cette habilitation ne peut �tre param�tr�e

		*/
		class MessagesLibraryRight : public Right
		{

		};
	}
}

#endif