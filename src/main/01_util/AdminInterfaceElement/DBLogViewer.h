
#ifndef SYNTHESE_UTIL_DBLOG_VIEWER_H
#define SYNTHESE_UTIL_DBLOG_VIEWER_H

namespace synthese
{
	namespace interfaces
	{
		/** Journal d'�v�nements (abstraite).
			@ingroup m01
		
			Un journal est un compte-rendu d'activit� de SYNTHESE. 
			Plusieurs entr�es sont consign�es dans la base de donn�es sous formes d'entr�es de journal. 
			Le journal lui-m�me est le composant d'administration d�di� � leur consultation.

			Le stockage des entr�es de journal s'effectue dans la base de donn�es SQLite.

			Les �l�ments de journal contiennent les donn�es suivantes :
				- date de l'�v�nement
				- module concern� (cl� texte)
				- rubrique concern�e (cl� texte selon le module)
				- utilisateur � l'origine de l'�v�nement
				- texte de l'entr�e (formalisme selon module et rubrique)

		*/
		class DBLogViewer : public AdminInterfaceElement
		{
		};
	}
}

