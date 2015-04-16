
/** BookableCommercialLineAdmin class header.
	@file BookableCommercialLineAdmin.h
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

#ifndef SYNTHESE_BookableCommercialLineAdmin_H__
#define SYNTHESE_BookableCommercialLineAdmin_H__

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
	}

	namespace pt
	{
		class ScheduledService;
	}

	namespace resa
	{
		/** Admin page class presenting the list of actual reservations on services on a commercial line.

			Two way of display are available :
				- reservations of all the services : the reservations are grouped by services, which are sorted on departure time.
				- reservations of a specific service

			The reservations are sorted on their departure time

			@todo If several services have the same number, the reservations are aggregated into one unique group, corresponding to the customer view.

			@ingroup m51Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class BookableCommercialLineAdmin:
			public admin::AdminInterfaceElementTemplate<BookableCommercialLineAdmin>
		{
		public:
			static const std::string PARAMETER_DISPLAY_CANCELLED;
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_SERVICE;

			static const std::string TAB_RESAS;
			static const std::string TAB_FREE_DRT;

		private:
			bool											_displayCancelled;
			boost::shared_ptr<const pt::CommercialLine>	_line;
			boost::gregorian::date							_date;
			bool											_hideOldServices;
			boost::optional<std::string>					_serviceNumber;


		public:
			BookableCommercialLineAdmin();

			/// @name Setters
			//@{
				void setServiceNumber(const boost::optional<std::string>& value){ _serviceNumber = value; }
				void setCommercialLine(boost::shared_ptr<const pt::CommercialLine> value){ _line = value; }
			//@}

			/// @name Getters
			//@{
				boost::shared_ptr<const pt::CommercialLine> getCommercialLine() const { return _line; }
				const boost::optional<std::string>& getServiceNumber() const { return _serviceNumber; }
			//@}



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
				const security::User& profile
			) const;



			/** Sub pages getter.
				@param currentPage Currently displayed page
				@return PageLinks each subpage of the current page
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;



			virtual bool isPageVisibleInTree(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;



			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getTitle() const;


			/** Icon generator.
				@return The icon path of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getIcon() const;



			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;



			virtual void _buildTabs(
				const security::Profile& profile
			) const;
		};
	}
}

#endif // SYNTHESE_BookableCommercialLineAdmin_H__
