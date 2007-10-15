
/** AdminInterfaceElement class header.
	@file AdminInterfaceElement.h

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

#ifndef SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H
#define SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H

#include <string>

#include "01_util/Factorable.h"

#include "11_interfaces/Types.h"

#include "30_server/FunctionRequest.h"


namespace synthese
{
	namespace security
	{
		class Right;
	}

	namespace admin
	{
	    class AdminRequest;

		/** Composant d'administration.
			@ingroup m14
			
			Un composant d'administration est un formulaire param�trable, destin� � effectuer une �criture en temps r�el dans la base SYNTHESE, dans un but de param�trage de l'application (il ne s'agit pas du seul moyen d'�criture).
			Une m�me donn�e peut �tre mise � jour par autant de composants que n�cessaire, d�fini avant tout sur la base d'une ergonomie de qualit�.
			Chaque composant est rattach� au module correspondant aux donn�es � modifier sous forme d'une sous-classe de AdminInterfaceElement. 
			Le comportement des composants d'administration est en g�n�ral d�fini d'apr�s les @ref defRight "habilitations" de l'utilisateur connect�.
			Par exemple :
				- un utilisateur anonyme n'a acc�s � aucun composant d'administration
				- un administrateur a acc�s � tous les composants d'administration.
			
		*/
		class AdminInterfaceElement : public util::Factorable<AdminInterfaceElement>
		{
		public:
			typedef enum { EVER_DISPLAYED, DISPLAYED_IF_CURRENT, NEVER_DISPLAYED } DisplayMode;

		public:
			AdminInterfaceElement();

//			std::string getHTMLLink(const server::FunctionRequest<admin::AdminRequest>* request) const;

			//! \name Virtual initialization method
			//@{
				/** Initialization of the parameters from a request.
					@param request The request to use for the initialization.
				*/
				virtual void setFromParametersMap(const server::ParametersMap& map) = 0;
			//@}

			//! \name Virtual output methods
			//@{
				/** Parameters map generator, used when building an url to the admin page.
					@return server::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007					
				*/
				virtual server::ParametersMap getParametersMap() const { return server::ParametersMap(); }

				/** Authorization control.
					@param request The current request
					@return bool True if the displayed page can be displayed
					@author Hugues Romain
					@date 2007					
				*/
				virtual bool isAuthorized(const server::FunctionRequest<AdminRequest>* request) const = 0;

				/** Display of the content of the admin element.
					@param stream Stream to write on.
					@param variables Execution variables
					@param request Current request
				*/
				virtual void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<AdminRequest>* request=NULL) const = 0;

				/** Title of the admin compound.
					@return The title of the admin compound, for display purposes.
				*/
				virtual std::string getTitle() const = 0;

				virtual std::string getIcon() const = 0;

				virtual std::string getSuperiorVirtual() const = 0;

				virtual DisplayMode getDisplayMode() const = 0;
			//@}
		};
	}
}

#endif

