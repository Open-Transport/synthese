
#ifndef SYNTHESE_UTIL_DBLOG_VIEWER_H
#define SYNTHESE_UTIL_DBLOG_VIEWER_H

namespace synthese
{
	namespace interfaces
	{
		/** Visualisation de journal d'événements.
			@ingroup m01
		
			@image html cap_admin_logviewer.png
			@image latex cap_admin_logviewer.png "Maquette de l'écran de visualisation de journal" width=14cm

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminInterfaceElement
				- Lien vers synthese::interfaces::DBLogList
				- Nom du journal affiché

			<i>Zone de contenus</i> :
				-# <b>Formulaire de recherche</b> :
					-# <tt>Date début</tt> : champ texte qui permet de filtrer les entrées postérieures à la date saisie. Si aucune date valide n'est entrée, alors le filtre est désactivé.
					-# <tt>Date fin</tt> : champ texte qui permet de filtrer les entrées antérieures à la date saisie. Si aucune date valide n'est entrée, alors le filtre est désactivé.
					-# <tt>Utilisateur</tt> : liste de choix contenant l'ensemble des utilisateurs ayant généré au moins une entrée sur le journal affiché. 
						- si un utilisateur est sélectionné, alors seules les entrées générées par l'utilisateurs sélectionné sont affichées
						- si aucun utilisateur n'est sélectionné, alors le filtre est désactivé.
					-# <tt>Type</tt> : liste de choix permettant de filtrer sur le type d'entrée :
						- <tt>Toutes</tt> : filtre désactive
						- <tt>Info</tt>
						- <tt>Warning</tt>
						- <tt>Warning + Error</tt>
						- <tt>Error</tt>
					-# <tt>Texte</tt> : champ texte permettant de chercher dans le contenu des autres colonnes (regroupée sous le nom de <tt>Entrée</tt>).
					-# Bouton <tt>Rechercher</tt> : lance la recherche
				-# <b>Tableau des résultats</b> : Les entrées du journal sont affichées dans les colonnes suivantes :
					-# <tt>Type</tt> : la couleur de l'entrée est liée au type pour une visualisation rapide
						- <tt>Info</tt> : l'entrée correspont à un fait qui est survenu dans le cadre d'une utilisation normale des fonctionnalités de SYNTHESE (Exemple : création d'utilisateur)
						- <tt>Warning</tt> : l'entrée correspond à un événement susceptible de constituer une anomalie sans conséquence majeure sur la continuité de service en dehors de l'anomalie constatée elle-même (Exemple : erreur de cohérence sur un tableau d'affichage : l'arrêt physique de sélection des départs n'existe plus)
						- <tt>Error</tt> : l'entrée correspond à un événement constituant une anomalie (Exemple : un terminal d'affichage ne fonctionne plus)
					-# <tt>Date</tt> : Date de l'entrée
					-# <tt>Utilisateur</tt> : Utilisateur ayant généré l'entrée. Attention, ne pas confondre cet utilisateur avec un éventuel utilisateur sur lequel porterait l'entrée (Exemple : utilisateur Paul créé par Jacques : l'utilisateur ayant généré l'entrée est Jacques). Un clic sur le nom de l'utilisateur conduit vers la page synthese::interfaces::UserAdmin si l'utilisateur dispose des habilitations nécessaires.
					-# <tt>Entrée</tt> : Ensemble de colonnes définissant le contenu de l'entrée. La liste des colonnes dépend du journal affiché (voir documentation individuelle des classes dérivées)
					-# Un clic sur les titres de colonne effectue un tri selon la colonne sélectionnée
				-# Un maximum de 50 entrées est affiché à l'écran. En cas de dépassement de ce nombre d'après les critères de recherche, un lien <tt>Entrées suivantes</tt> apparait et permet de visualiser les entrées suivantes. A partir de la seconde page, un lien <tt>Entrées précédentes</tt> apparait également.

			<i>Sécurité</i>
				- Les habilitations nécessaires pour visualiser le journal sont définies individuellement par chaque journal (voir classe correspondante)

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne génère d'entrée dans un journal.
		*/
		class DBLogViewer : public AdminInterfaceElement
		{
		};
	}
}

