
/** DBLogViewer class header.
	@file DBLogViewer.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_DBLogViewer_H__
#define SYNTHESE_DBLogViewer_H__

#include <string>

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace dblog
	{
		class DBLog;

		/** Visualisation de journal d'événements.
			@ingroup m13
		
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
				- Une habilitation privée DBLogRight de niveau READ, ainsi qu'un contrôle positif des habilitations par le journal visualisé sont nécessaire pour accéder à la page, et y visualiser les entrées générées par l'utilisateur courant.
				- Une habilitation publique DBLogRight de niveau READ, ainsi qu'un contrôle positif des habilitations par le journal visualisé sont nécessaire pour accéder à la page, et y visualiser toutes les entrées.

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne génère d'entrée dans un journal.
		*/
		class DBLogViewer : public admin::AdminInterfaceElement
		{
			//! \name Stored parameters
			//@{
				std::string			_logKey;
				uid					_searchUser;
				int					_searchType;
			//@}

			//! \name Stored values
			//@{
			const DBLog*	_dbLog;
			//@}

		public:
			static const std::string PARAMETER_LOG_KEY;
			static const std::string PARAMETER_SEARCH_USER;
			static const std::string PARAMETER_SEARCH_TYPE;

			DBLogViewer();
			
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const server::Request::ParametersMap& map);

			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(std::ostream& stream, const server::Request* request=NULL) const;

			/** Title of the admin compound.
				@return The title of the admin compound, for display purposes.
			*/
			std::string getTitle() const;
		};
	}
}

#endif // SYNTHESE_DBLogViewer_H__
