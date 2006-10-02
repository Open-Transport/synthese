
#ifndef SYNTHESE_UTIL_DBLOG_LIST_H
#define SYNTHESE_UTIL_DBLOG_LIST_H

namespace synthese
{
	namespace interfaces
	{
		/** Ecran de liste des journaux d'�v�nements.
			@ingroup m01
		
			@image html cap_admin_logs.png
			@image latex cap_admin_logs.png "Maquette de l'�cran de liste des journaux d'�v�nements" width=14cm

			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Journaux

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminInterfaceElement
				- Texte <tt>Journaux</tt>

			<i>Zone de contenus</i> :
				-# <b>Tableau de liste des journaux</b> : Chaque journal est affich�es dans les colonnes suivantes :
					-# <tt>Module</tt> : Module auquel appartient le journal
					-# <tt>Journal</tt> : Nom du journal
					-# Bouton <tt>Consulter</tt> : Acc�s au journal correspondant

			<i>S�curit�</i>
				- L'acc�s � la page n�cessite une habilitation Util/DBLog de niveau Utilisation
				- Les habilitations n�cessaires pour visualiser chaque journal sont d�finies individuellement par chaque journal (voir classe correspondante). Seuls les journaux autoris�s pour l'utilisateur apparaissent dans la liste

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne g�n�re d'entr�e dans un journal.
		*/
		class DBLogViewer : public AdminInterfaceElement
		{
		};
	}
}

