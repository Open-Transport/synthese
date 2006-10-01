
#ifndef SYNTHESE_UTIL_DBLOG_VIEWER_H
#define SYNTHESE_UTIL_DBLOG_VIEWER_H

namespace synthese
{
	namespace interfaces
	{
		/** Journal d'événements (abstraite).
			@ingroup m01
		
			Un journal est un compte-rendu d'activité de SYNTHESE. 
			Plusieurs entrées sont consignées dans la base de données sous formes d'entrées de journal. 
			Le journal lui-même est le composant d'administration dédié à leur consultation.

			Le stockage des entrées de journal s'effectue dans la base de données SQLite.

			Les éléments de journal contiennent les données suivantes :
				- date de l'événement
				- module concerné (clé texte)
				- rubrique concernée (clé texte selon le module)
				- utilisateur à l'origine de l'événement
				- texte de l'entrée (formalisme selon module et rubrique)

		*/
		class DBLogViewer : public AdminInterfaceElement
		{
		};
	}
}

