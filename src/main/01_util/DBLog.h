
#ifndef SYNTHESE_UTIL_DBLOG_H
#define SYNTHESE_UTIL_DBLOG_H

#include <string>

namespace synthese
{
	namespace util
	{
		/** Journal d'�v�nements stock� en base de donn�es (abstraite).
			@ingroup m01
		
			Un journal est un compte-rendu d'activit� de SYNTHESE. 
			Plusieurs entr�es sont consign�es dans la base de donn�es sous formes d'entr�es de journal. 
			Le journal lui-m�me est le composant d'administration d�di� � leur consultation.

			Le stockage des entr�es de journal s'effectue dans la base de donn�es SQLite.

			Les �l�ments de journal contiennent les donn�es suivantes :
				- date de l'�v�nement
				- nom du journal (cl� texte identique au nom d'enregistrement de la classe)
				- utilisateur � l'origine de l'�v�nement
				- niveau de l'entr�e (INFO, WARNING, ERROR)
				- texte de l'entr�e (formalisme selon module et rubrique, sp�cifi� par les sous-classes)

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