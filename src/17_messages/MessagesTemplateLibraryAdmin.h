
/** MessagesTemplateLibraryAdmin class header.
	@file MessagesTemplateLibraryAdmin.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_MessagesTemplateLibraryAdmin_H__
#define SYNTHESE_MessagesTemplateLibraryAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "17_messages/Types.h"

namespace synthese
{
	namespace messages
	{
		class TextTemplate;

		/** MessagesTemplateLibraryAdmin Class.
			@ingroup m17Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class MessagesTemplateLibraryAdmin : public admin::AdminInterfaceElementTemplate<MessagesTemplateLibraryAdmin>
		{
		public:
			static const std::string PARAMETER_FOLDER_ID;

		private:
			boost::shared_ptr<const TextTemplate>	_folder;

		public:
			MessagesTemplateLibraryAdmin();
			
			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(
				const server::ParametersMap& map
			);
			
			
			
			/** Parameters map generator, used when building an url to the admin page.
					@return server::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007					
				*/
			virtual server::ParametersMap getParametersMap() const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@author Hugues Romain
				@date 2008
			*/
			void display(
				std::ostream& stream,
				const admin::AdminRequest& _request
			) const;


			
			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(
				const security::User& user
			) const;
			
			/** Gets sub page of the designed parent page, which are from the current class.
				@param parentLink Link to the parent page
				@param currentPage Currently displayed page
				@return PageLinks each subpage of the parent page designed in parentLink
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const std::string& moduleKey,
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;
			


			/** Gets the opening position of the node in the tree view.
				@return Always visible
				@author Hugues Romain
				@date 2008					
			*/
			virtual bool isPageVisibleInTree(
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;



			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getTitle() const;
		};
	}
}

#endif // SYNTHESE_MessagesTemplateLibraryAdmin_H__
