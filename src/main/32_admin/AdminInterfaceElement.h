
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

#include "32_admin/AdminRequest.h"

namespace synthese
{
	namespace security
	{
		class Right;
	}

	namespace admin
	{
		/** Composant d'administration.
			@ingroup m32
			
			Un composant d'administration est un formulaire paramétrable, destiné à effectuer une écriture en temps réel dans la base SYNTHESE, dans un but de paramétrage de l'application (il ne s'agit pas du seul moyen d'écriture).
			Une même donnée peut être mise à jour par autant de composants que nécessaire, défini avant tout sur la base d'une ergonomie de qualité.
			Chaque composant est rattaché au module correspondant aux données à modifier sous forme d'une sous-classe de AdminInterfaceElement. 
			Le comportement des composants d'administration est en général défini d'après les @ref defRight "habilitations" de l'utilisateur connecté.
			Par exemple :
				- un utilisateur anonyme n'a accès à aucun composant d'administration
				- un administrateur a accès à tous les composants d'administration.
			
		*/
		class AdminInterfaceElement : public util::Factorable
		{
		public:
			typedef enum { EVER_DISPLAYED, DISPLAYED_IF_CURRENT, NEVER_DISPLAYED } DisplayMode;

		private:
			std::string							_superior;
			const DisplayMode					_everDisplayed;
			
		protected:
			void								_setSuperior(const std::string& superior);
			
		public:
			/** Constructor.
				@param superior Key of the superior admin interface element class in the tree
				@param everDisplayed Indicates when the admin interface element is displayed in the tree
				@param needeRight Right object describing minimum right to have in order to be able to see the admin interface element
			*/
			AdminInterfaceElement(const std::string& superior, DisplayMode everDisplayed);

			//!	@name Getters
			//@{
				const std::string& getSuperior() const;
				const DisplayMode getDisplayMode() const;
			//@}

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
			//@}
		};
	}
}

#endif

