
/** ResaCustomersAdmin class header.
	@file ResaCustomersAdmin.h
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

#ifndef SYNTHESE_ResaCustomersAdmin_H__
#define SYNTHESE_ResaCustomersAdmin_H__

#include "ActionResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace resa
	{
		/** ResaCustomersAdmin Class.
			@ingroup m31Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class ResaCustomersAdmin : public admin::AdminInterfaceElementTemplate<ResaCustomersAdmin>
		{
		public:
			static const std::string PARAM_SEARCH_NAME;
			static const std::string PARAM_SEARCH_SURNAME;
			static const std::string PARAM_SEARCH_LOGIN;

		private:
			std::string										_searchLogin;
			std::string										_searchName;
			std::string										_searchSurname;
			html::ActionResultHTMLTable::RequestParameters	_requestParameters;
			html::ResultHTMLTable::ResultParameters			_resultParameters;

		public:
			ResaCustomersAdmin();
			
			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(
				const server::ParametersMap& map,
				bool doDisplayPreparationActions = true
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
			void display(std::ostream& stream, interfaces::VariablesMap& variables
			) const;
			
			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(
			) const;
			
			/** Gets sub page of the designed parent page, which are from the current class.
				@param parentLink Link to the parent page
				@param currentPage Currently displayed page
				@return PageLinks each subpage of the parent page designed in parentLink
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfParent(
				const PageLink& parentLink
				, const AdminInterfaceElement& currentPage
			) const;
		};
	}
}

#endif // SYNTHESE_ResaCustomersAdmin_H__
