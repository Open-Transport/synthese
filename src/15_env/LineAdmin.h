
/** LineAdmin class header.
	@file LineAdmin.h
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

#ifndef SYNTHESE_LineAdmin_H__
#define SYNTHESE_LineAdmin_H__

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace env
	{
		class Line;

		/** LineAdmin Class.
			@ingroup m35Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class LineAdmin : public admin::AdminInterfaceElementTemplate<LineAdmin>
		{
		public:
			static const std::string TAB_STOPS;
			static const std::string TAB_SCHEDULED_SERVICES;
			static const std::string TAB_CONTINUOUS_SERVICES;

		private:
			boost::shared_ptr<const Line>	_line;

		public:
			LineAdmin();
			
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
			void display(std::ostream& stream, interfaces::VariablesMap& variables) const;
			
			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized() const;
			
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

			boost::shared_ptr<const Line> getLine() const;

			virtual void _buildTabs() const;
		};
	}
}

#endif // SYNTHESE_LineAdmin_H__
