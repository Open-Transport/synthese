
#ifndef SYNTHESE_UTIL_DBLOG_H
#define SYNTHESE_UTIL_DBLOG_H

#include <string>

namespace synthese
{
	namespace util
	{
		/** Journal d'événements stocké en base de données (abstraite).
			@ingroup m01
		
			Un journal est un compte-rendu d'activité de SYNTHESE. 
			Plusieurs entrées sont consignées dans la base de données sous formes d'entrées de journal. 
			Le journal lui-même est le composant d'administration dédié à leur consultation.

			Le stockage des entrées de journal s'effectue dans la base de données SQLite.

			Les éléments de journal contiennent les données suivantes :
				- date de l'événement
				- nom du journal (clé texte identique au nom d'enregistrement de la classe)
				- utilisateur à l'origine de l'événement
				- niveau de l'entrée (INFO, WARNING, ERROR)
				- texte de l'entrée (formalisme selon module et rubrique, spécifié par les sous-classes)

		*/
		class DBLog
		{
		public:
			typedef vector<std::string> ColumnsNameVector;
				


		public:
			//static std::string getModuleName() = 0;
			//static std::string getName() = 0;
			//static ColumnsNameVector& getColumnNames() = 0;
		};
	}
}