
/** BookableCommercialLineAdmin class header.
	@file BookableCommercialLineAdmin.h
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

#ifndef SYNTHESE_BookableCommercialLineAdmin_H__
#define SYNTHESE_BookableCommercialLineAdmin_H__

#include "32_admin/AdminInterfaceElementTemplate.h"

#include "04_time/DateTime.h"

namespace synthese
{
	namespace env
	{
		class CommercialLine;
	}

	namespace resa
	{
		/** BookableCommercialLineAdmin Class.
			@ingroup m31Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class BookableCommercialLineAdmin : public admin::AdminInterfaceElementTemplate<BookableCommercialLineAdmin>
		{
			bool											_displayCancelled;
			boost::shared_ptr<const env::CommercialLine>	_line;
			time::DateTime									_startDateTime;
			time::DateTime									_endDateTime;

		public:
			static const std::string PARAMETER_DISPLAY_CANCELLED;
			static const std::string PARAMETER_DATE;

			BookableCommercialLineAdmin();
			
			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(const server::ParametersMap& map);

			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@param request The current request
				@author Hugues Romain
				@date 2008
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request=NULL) const;
			
			/** Authorization control.
				@param request The current request
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;
			
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
				, const server::FunctionRequest<admin::AdminRequest>* request
			) const;
			
			/** Sub pages getter.
				@param currentPage Currently displayed page
				@param request User request
				@return PageLinks each subpage of the current page
				@author Hugues Romain
				@date 2008
				REMOVE IF YOU WANT TO USE THE STANDARD INHERITANCE METHOD DESIGNED IN ADMININTERFACEELEMENT
			*/
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const PageLink& parentLink
				, const AdminInterfaceElement& currentPage
				, const server::FunctionRequest<admin::AdminRequest>* request
				
			) const;
			
			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
				REMOVE IF YOU WANT ALWAYS USE THE DEFAULT_TITLE
			*/
			virtual std::string getTitle() const;
			
			/** Parameter name getter.
				@return The name of the parameter of the page
				@author Hugues Romain
				@date 2008
				REMOVE IF YOU DONT USE A PARAMETER
			*/
			virtual std::string getParameterName() const;
			
			/** Parameter value getter.
				@return The value of the parameter of the page
				@author Hugues Romain
				@date 2008
				REMOVE IF YOU DONT USE A PARAMETER
			*/
			virtual std::string getParameterValue() const;
		};
	}
}

#endif // SYNTHESE_BookableCommercialLineAdmin_H__
