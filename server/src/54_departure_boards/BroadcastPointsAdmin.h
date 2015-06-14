
/** BroadcastPointsAdmin class header.
	@file BroadcastPointsAdmin.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "AdminInterfaceElementTemplate.h"
#include "AdvancedSelectTableSync.h"
#include "DeparturesTableTypes.h"
#include "ResultHTMLTable.h"

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace departure_boards
	{
		/** Ecran de recherche de lieux pour administration des points de diffusion.
			@ingroup m54Admin refAdmin

			@image html cap_admin_places.png
			@image latex cap_admin_places.png "Maquette de l'écran de recherche de lieux" width=14cm

			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Emplacements

			<i>Barre de navigation</i> :
				- Lien vers synthese::admin::HomeAdmin
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
					-# Bouton <tt>Editer</tt> : Conduit vers la page synthese::pt::PTPlaceAdmin sur l'emplacement sélectionné
				-# Un maximum de 50 emplacements est affiché à l'écran. En cas de dépassement de ce nombre d'après les critères de recherche, un lien <tt>Emplacements suivants</tt> apparait et permet de visualiser les entrées suivantes. A partir de la seconde page, un lien <tt>Emplacements précédents</tt> apparait également.

			<i>Sécurité</i>
				- Une habilitation publique PlacesRight de niveau READ est nécessaire pour accéder à la page et pour utiliser toutes les fonctionnalités.
				- Le résultat des recherches dépend du périmètre des habilitations de niveau READ et supérieur

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne génère d'entrée dans un journal.

		*/
		class BroadcastPointsAdmin : public admin::AdminInterfaceElementTemplate<BroadcastPointsAdmin>
		{
			std::string									_cityName;
			std::string									_placeName;
			boost::optional<util::RegistryKeyType>		_lineUId;
			BroadcastPointsPresence						_searchDevicesNumber;
			html::ResultHTMLTable::RequestParameters	_requestParameters;


		public:
			static const std::string PARAMETER_CITY_NAME;
			static const std::string PARAMETER_PLACE_NAME;
			static const std::string PARAMETER_LINE_ID;
			static const std::string PARAMETER_DEVICES_NUMBER;

			BroadcastPointsAdmin();

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			/** Parameters map generator, used when building an url to the admin page.
					@return util::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007
				*/
			virtual util::ParametersMap getParametersMap() const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			/** isAuthorized.
				@return bool
				@author Hugues Romain
				@date 2007
			*/
			bool isAuthorized(
				const security::User& user
			) const;

			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@return PageLinks A link to the page if the parent is Home
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;



			/** Gets the opening position of the node in the tree view.
				@return Always visible
				@author Hugues Romain
				@date 2008
			*/
			virtual bool isPageVisibleInTree(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

		};
	}
}

#endif // SYNTHESE_BroadcastPointsAdmin_H__
