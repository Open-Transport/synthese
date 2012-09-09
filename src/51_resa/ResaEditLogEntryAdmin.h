
/** ResaEditLogEntryAdmin class header.
	@file ResaEditLogEntryAdmin.h
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

#ifndef SYNTHESE_ResaEditLogEntryAdmin_H__
#define SYNTHESE_ResaEditLogEntryAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "DBLogHTMLView.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace dblog
	{
		class DBLogEntry;
	}

	namespace resa
	{
		/** ResaEditLogEntryAdmin Class.
			@ingroup m51Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class ResaEditLogEntryAdmin : public admin::AdminInterfaceElementTemplate<ResaEditLogEntryAdmin>
		{
			boost::shared_ptr<const dblog::DBLogEntry>	_entry;
			dblog::DBLogHTMLView						_log;

		public:
			ResaEditLogEntryAdmin();

			void setEntry(boost::shared_ptr<const dblog::DBLogEntry> value);
			boost::shared_ptr<const dblog::DBLogEntry> getEntry() const;

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


			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getTitle() const;


			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;

		};
	}
}

#endif // SYNTHESE_ResaEditLogEntryAdmin_H__
