/** 70_server module wide definitions
	@file module.h
*/

#ifndef SYNTHESE_SERVER_MODULE_H
#define SYNTHESE_SERVER_MODULE_H

namespace synthese
{

	/** @defgroup m70 70 SYNTHESE server
		
		Pour pouvoir ï¿½tre appelï¿½ par plusieurs applications, le serveur SYNTHESE est un serveur TCP autonome, tout comme MySQL par exemple. La couche TCP est basée sur le module @ref m00 .

		Pour l'interroger, des modules clients TCP sont dï¿½veloppï¿½s dans les applications clientes :
			- Les clients autonomes : @ref m71, @ref m71, @ref m73
			- Un client TCP intï¿½grï¿½ au rï¿½fï¿½rentiel rï¿½seau (voir projet Rï¿½fï¿½rentiel rï¿½seau), permettant l'affichage cartographiques de donnï¿½es inconnues de SYNTHESE

		@{
	*/

	/** 70_server namespace */
	namespace server
	{


	}

	/** @} */

}

#endif
