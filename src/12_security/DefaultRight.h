
#ifndef SYNTHESE_DefaultRight_H__
#define SYNTHESE_DefaultRight_H__

#include "12_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Default habilitation.
			@ingroup m12

			Les niveaux de droit utilis�s sont les suivants :

			Habilitations priv�es :
			- USE : 
			- READ : 
			- WRITE : 

			Habilitations publiques :
			- READ : 
			- WRITE : 
			- DELETE : 

			D�finition du p�rim�tre :
				- Aucun p�rim�tre ne peut �tre d�fini
		*/
		class DefaultRight : public Right
		{
		private:


		};
	}
}
#endif // SYNTHESE_DefaultRight_H__

