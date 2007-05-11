
/** BroadcastPointsAdmin class header.
	@file BroadcastPointsAdmin.h

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

#ifndef SYNTHESE_BroadcastPointsAdmin_H__
#define SYNTHESE_BroadcastPointsAdmin_H__

#include <vector>

#include "05_html/ResultHTMLTable.h"

#include "32_admin/AdminInterfaceElementTemplate.h"

#include "34_departures_table/AdvancedSelectTableSync.h"
#include "34_departures_table/Types.h"

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
	}

	namespace departurestable
	{
		/** Ecran de recherche de lieux pour administration des points de diffusion.
			@ingroup m34

			@image html cap_admin_places.png
			@image latex cap_admin_places.png "Maquette de l'écran de recherche de lieux" width=14cm
			
			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Emplacements

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Texte <tt>Emplacements</tt>

			<i>Zone de contenu</i> :
				-# <b>Formulaire de recherche</b>
					-# <tt>Commune</tt> : Champ texte permettant de rechercher les emplacements dont le nom de la commune contient le texte saisi. Le champ vide désactive le filtre.
					-# <tt>Nom</tt> : Champ texte permettant de rechercher les emplacements dont le nom contient le texte saisi. Le champ vide désactive le filtre.
					-# <tt>Terminaux d'affichage</tt> : Liste déroulante permettant de sélectionner l'affichage des seuls emplacements disposant d'au moins un afficheur installé, l'affichage des seuls emplacement qui n'en dispose d'aucun, ou de désactiver le filtre
					-# <tt>Ligne</tt> : Liste déroulante permettant de sélectionner une ligne de transport : seuls les emplacements desservis par la ligne sont sélectionnés. Si aucune ligne n'est sélectionné le filtre est désactivé.
					-# bouton <tt>Rechercher</tt> : Lance la recherche
				-# <b>Tableau des résultats</b> : les emplacements trouvés sont décrits dans les colonnes suivantes :
					-# <tt>Commune</tt> : Nom de la commune auquel appartient l'emplacement
					-# <tt>Nom</tt> : Nom de l'emplacement
					-# Bouton <tt>Editer</tt> : Conduit vers la page synthese::interfaces::PlaceAdmin sur l'emplacement sélectionné
				-# Un maximum de 50 emplacements est affiché à l'écran. En cas de dépassement de ce nombre d'après les critères de recherche, un lien <tt>Emplacements suivants</tt> apparait et permet de visualiser les entrées suivantes. A partir de la seconde page, un lien <tt>Emplacements précédents</tt> apparait également.
			
			<i>Sécurité</i>
				- Une habilitation publique PlacesRight de niveau READ est nécessaire pour accéder à la page et pour utiliser toutes les fonctionnalités.
				- Le résultat des recherches dépend du périmètre des habilitations de niveau READ et supérieur

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne génère d'entrée dans un journal.

		*/
		class BroadcastPointsAdmin : public admin::AdminInterfaceElementTemplate<BroadcastPointsAdmin>
		{
			std::string				_cityName;
			std::string				_placeName;
			uid						_lineUId;
			BroadcastPointsPresence	_displayNumber;
			html::ResultHTMLTable::RequestParameters	_requestParameters;
			html::ResultHTMLTable::ResultParameters	_resultParameters;

			std::vector<boost::shared_ptr<ConnectionPlaceWithBroadcastPoint> > _searchResult;

		public:
			static const std::string PARAMETER_CITY_NAME;
			static const std::string PARAMETER_PLACE_NAME;
			static const std::string PARAMETER_LINE_ID;
			static const std::string PARAMETER_DISPLAY_NUMBER;

			BroadcastPointsAdmin();
			
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

			/** isAuthorized.
				@param request
				@return bool
				@author Hugues Romain
				@date 2007
				
			*/
			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;
		};
	}
}

#endif // SYNTHESE_BroadcastPointsAdmin_H__
