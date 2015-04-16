
#ifndef SYNTHESE_DefaultRight_H__
#define SYNTHESE_DefaultRight_H__

#include "12_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Default habilitation.
			@ingroup m12

			Les niveaux de droit utilisés sont les suivants :

			Habilitations privées :
			- USE :
			- READ :
			- WRITE :

			Habilitations publiques :
			- READ :
			- WRITE :
			- DELETE :

			Définition du périmètre :
				- Aucun périmètre ne peut être défini
		*/
		class DefaultRight : public Right
		{
		private:


		};
	}
}
#endif // SYNTHESE_DefaultRight_H__

