
/** AlarmTestOnDisplayScreenFunction class header.
	@file AlarmTestOnDisplayScreenFunction.h
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

#ifndef SYNTHESE_AlarmTestOnDisplayScreenFunction_H__
#define SYNTHESE_AlarmTestOnDisplayScreenFunction_H__

#include "Function.h"
#include "FactorableTemplate.h"
#include "Registry.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;
	}

	namespace departure_boards
	{
		class DisplayType;

		/** AlarmTestOnDisplayScreenFunction Function class.

			Acts on a temporary environment.

			@author Hugues Romain
			@date 2008
			@ingroup m54Functions refFunctions
		*/
		class AlarmTestOnDisplayScreenFunction : public util::FactorableTemplate<server::Function, AlarmTestOnDisplayScreenFunction>
		{
		public:
			static const std::string PARAMETER_DISPLAY_TYPE_ID;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const DisplayType>		_type;
				boost::shared_ptr<const messages::Alarm>	_alarm;
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			AlarmTestOnDisplayScreenFunction();

			/** Action to run, defined by each subclass.
			*/
			util::ParametersMap run(std::ostream& stream, const server::Request& request) const;

			void setAlarmId(util::RegistryKeyType id);

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_AlarmTestOnDisplayScreenFunction_H__
