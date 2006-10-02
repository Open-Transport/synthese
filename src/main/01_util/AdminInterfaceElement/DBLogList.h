
#ifndef SYNTHESE_UTIL_DBLOG_LIST_H
#define SYNTHESE_UTIL_DBLOG_LIST_H

namespace synthese
{
	namespace interfaces
	{
		/** Ecran de liste des journaux d'événements.
			@ingroup m01
		
			@image html cap_admin_logs.png
			@image latex cap_admin_logs.png "Maquette de l'écran de liste des journaux d'événements" width=14cm

			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Journaux

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminInterfaceElement
				- Texte <tt>Journaux</tt>

			<i>Zone de contenus</i> :
				-# <b>Tableau de liste des journaux</b> : Chaque journal est affichées dans les colonnes suivantes :
					-# <tt>Module</tt> : Module auquel appartient le journal
					-# <tt>Journal</tt> : Nom du journal
					-# Bouton <tt>Consulter</tt> : Accès au journal correspondant

			<i>Sécurité</i>
				- L'accès à la page nécessite une habilitation Util/DBLog de niveau Utilisation
				- Les habilitations nécessaires pour visualiser chaque journal sont définies individuellement par chaque journal (voir classe correspondante). Seuls les journaux autorisés pour l'utilisateur apparaissent dans la liste

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne génère d'entrée dans un journal.
		*/
		class DBLogViewer : public AdminInterfaceElement
		{
		};
	}
}

