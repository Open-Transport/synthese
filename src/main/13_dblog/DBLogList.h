
/** DBLogList class header.
	@file DBLogList.h

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

#ifndef SYNTHESE_DBLogList_H__
#define SYNTHESE_DBLogList_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace dblog
	{
		/** Ecran de liste des journaux d'événements.
			@ingroup m13
		
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
				- Une habilitation privée ou publique DBLogRight de niveau READ est nécessaire pour accéder à la page.
				- Les habilitations nécessaires pour visualiser chaque journal sont définies individuellement par chaque journal (voir classe correspondante). Seuls les journaux autorisés pour l'utilisateur apparaissent dans la liste.

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne génère d'entrée dans un journal.
		*/
		class DBLogList : public admin::AdminInterfaceElement
		{
		public:
			DBLogList();
			
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

			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;
		};
	}
}

#endif // SYNTHESE_DBLogList_H__
