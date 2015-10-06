/** Alert class header.
	@file Alert.hpp

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

#ifndef SYNTHESE_regulation_Alert_hpp__
#define SYNTHESE_regulation_Alert_hpp__

#include "Object.hpp"
//#include "DateField.hpp"
//#include "Registrable.h"
//#include "Registry.h"
#include "CommercialLine.h"
#include "ScheduledService.h"

//#include "FrameworkTypes.hpp"
#include "PointerField.hpp"
#include "EnumObjectField.hpp"

#include "AlertTypes.hpp"

#include "SchemaMacros.hpp"

#include <set>
#include <vector>

namespace synthese
{
	namespace regulation
	{
		class Alert;

        FIELD_ENUM(Kind, regulation::AlertType)
		FIELD_POINTER(Line, pt::CommercialLine)
		FIELD_POINTER(Service, pt::ScheduledService)
		FIELD_STRING(ExtraData)


		typedef boost::fusion::map<
			FIELD(Key),
            FIELD(Kind),
			FIELD(Line),
			FIELD(Service),
			FIELD(ExtraData)
		> AlertSchema;

		/** Alert class.
		@ingroup m62
		*/
		class Alert:
			public Object<Alert, AlertSchema>
		{
		public:
            
		private:


		public:
			Alert(util::RegistryKeyType id=0);
			~Alert();

			//! @name Updaters
			//@{
			//@}

			//! @name Services
			//@{
/*				virtual void addAdditionalParameters(
				util::ParametersMap& map,
				std::string prefix = std::string()
				) const;
*/
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_regulation_Alert_hpp__
