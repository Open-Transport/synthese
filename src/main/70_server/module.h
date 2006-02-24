/** 70_server module wide definitions
	@file module.h
*/

#ifndef SYNTHESE_SERVER_MODULE_H
#define SYNTHESE_SERVER_MODULE_H

namespace synthese
{

	/** @defgroup m70 70 SYNTHESE server
		
		Pour pouvoir �tre appel� par plusieurs applications, le serveur SYNTHESE est un serveur TCP autonome, tout comme MySQL par exemple. La couche TCP est bas�e sur le module @ref m00 .

		Pour l'interroger, des modules clients TCP sont d�velopp�s dans les applications clientes :
			- Les clients autonomes : @ref m71, @ref m71, @ref m73
			- Un client TCP int�gr� au r�f�rentiel r�seau (voir projet R�f�rentiel r�seau), permettant l'affichage cartographiques de donn�es inconnues de SYNTHESE

		@{
	*/

	/** 70_server namespace */
	namespace server
	{


	}

	/** @} */

}

#endif
