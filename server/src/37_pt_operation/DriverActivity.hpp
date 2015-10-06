
/** Driver activity class header.
	@file DriverActivity.hpp

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

#ifndef SYNTHESE_37_DriverActivity_hpp__
#define SYNTHESE_37_DriverActivity_hpp__

#include "Object.hpp"

#include "DataSourceLinksField.hpp"
#include "DriverService.hpp"
#include "ImportableTemplate.hpp"
#include "NumericField.hpp"
#include "StringField.hpp"

#include <vector>
#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace pt_operation
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(impex::DataSourceLinks)
		> DriverActivitySchema;

		/** DriverActivity class.
			@ingroup m37
		*/
		class DriverActivity:
			public Object<DriverActivity, DriverActivitySchema>,
			public impex::ImportableTemplate<DriverActivity>
		{
		public:
			typedef util::Registry<DriverActivity> Registry;

			DriverActivity(util::RegistryKeyType id = 0);

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_37_DriverActivity_hpp__

