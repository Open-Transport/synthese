
/** DisplayTypesAdmin class header.
	@file DisplayTypesAdmin.h

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

#ifndef SYNTHESE_DISPLAY_TYPES_ADMIN_H
#define SYNTHESE_DISPLAY_TYPES_ADMIN_H

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace departurestable
	{

		/** Ecran d'administration des types d'affichage.
			@ingroup m34

			@image html cap_admin_display_types.png
			@image latex cap_admin_display_types.png "Ecran d'édition des types d'affichage" width=14cm

			<i>Titre de la fenêtre</i> : SYNTHESE Admin - Afficheurs - Types

			<i>Barre de navigation</i> :
 				- Lien vers @ref synthese::interface::HomeAdmin
				- Lien vers @ref synthese::interface::DisplaySearchAdmin

			<i>Zone de contenus</i> :
				-# <b>Tableau liste des types</b> : Les colonnes suivantes sont présentes, et permettent d'éditer directement les types d'afficheurs :
					- <tt>Nom</tt> : Texte devant être non vide
					- <tt>Interface</tt> : Liste de choix présentant les interfaces installées (exemple : Lumiplan, HTML charte 1, HTML charte 2, etc.)
					- <tt>Lignes</tt> : Nombre de départs affichés sous forme de liste de choix proposant les nombres de 1 à 50
					- <tt>Actions</tt> : Boutons : 
						- <tt>Modifier</tt> : enregistre les modifications effectuées sur la ligne du tableau correspondante. Si un champ ne respecte pas les règles énumérées, un message d'erreur empêche l'enregistrement des modifications :
						@code Le type ne peut être modifié car un ou plusieurs champ n'est pas correctement renseigné @endcode
						- <tt>Supprimer</tt> : supprime le type d'afficheur, seulement s'il n'est utilisé par aucun afficheur. Ce bouton n'apparaît que sur les types non utilisés. Un contrôle d'intégrité est réalisé à nouveau au moment de la suppression proprement dite. En cas d'existence d'afficheur au moment de la suppression, un message d'erreur apparaît :
						@code Ce type d'afficheur ne peut être supprimé car il est utilisé par au moins un afficheur. @endcode
					- La dernière ligne du tableau permet l'ajout de type d'afficheur : le bouton <tt>Ajouter</tt> permet d'ajouter un type d'afficheur directement. Un contrôle des donneés entrées est effectué : en cas de données manquante, l'afficheur n'est pas créé et un message d'erreur apparaît :
						@code L'afficheur ne peut être créé car un ou plusieurs champs n'a pas été correctement renseigné. @endcode
						
			<i>Sécurité</i> :
				- Une habilitation publique ArrivalDepartureTableRight de niveau READ sur le périmètre "tout" est nécessaire pour accéder à la page.
				- Une habilitation publique ArrivalDepartureTableRight de niveau WRITE sur le périmètre "tout" est nécessaire pour obtenir les boutons <tt>Modifier</tt> et <tt>Ajouter</tt>.
				
			<i>Journal</i> : Les actions suivantes génèrent une entrée dans le journal ArrivalDepartureTableLog :
				- INFO : Ajout de type d'afficheur
				- INFO : Modification de type d'afficheur
				- INFO : Suppression de type d'afficheur
		*/
		class DisplayTypesAdmin : public admin::AdminInterfaceElement
		{
		public:
			DisplayTypesAdmin();
			
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const server::ParametersMap& map);

			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request=NULL) const;

			/** Title of the admin compound.
				@return The title of the admin compound, for display purposes.
			*/
			std::string getTitle() const;
		};
	}
}

#endif // SYNTHESE_DisplayTypesAdmin_H__
