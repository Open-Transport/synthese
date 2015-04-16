
/** JourneyPatternAdmin class header.
	@file JourneyPatternAdmin.hpp
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_LineAdmin_H__
#define SYNTHESE_LineAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "ResultHTMLTable.h"

namespace synthese
{
	namespace pt
	{
		class JourneyPattern;

		/** Journey pattern edition.
			@ingroup m35Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class JourneyPatternAdmin : public admin::AdminInterfaceElementTemplate<JourneyPatternAdmin>
		{
		public:
			static const std::string TAB_STOPS;
			static const std::string TAB_SCHEDULED_SERVICES;
			static const std::string TAB_CONTINUOUS_SERVICES;
			static const std::string TAB_PROPERTIES;
			static const std::string TAB_INDICES;

		private:
			boost::shared_ptr<const pt::JourneyPattern>	_line;
			html::ResultHTMLTable::RequestParameters	_requestParameters;

		public:
			JourneyPatternAdmin();

			virtual PageLinks _getCurrentTreeBranch() const;

			boost::shared_ptr<const pt::JourneyPattern> getLine() const;
			void setLine(boost::shared_ptr<const pt::JourneyPattern> value);

			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
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
				@author Hugues Romain
				@date 2008
			*/
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(
				const security::User& user
			) const;

			/** Sub pages getter.
				@return PageLinks Ordered vector of sub pages links
				@author Hugues Romain
				@date 2008

				The default implementation handles the auto registration of administrative components by getSuperiorVirtual() method.
				This method can be overloaded to create customized sub tree.
			*/
			virtual PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getTitle() const;

			virtual void _buildTabs(
				const security::Profile& profile
			) const;

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;

		};
	}
}

#endif // SYNTHESE_LineAdmin_H__
