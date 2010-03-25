
/** UpdateDisplayScreenAction class header.
	@file UpdateDisplayScreenAction.h

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

#ifndef SYNTHESE_UpdateDisplayScreenAction_H__
#define SYNTHESE_UpdateDisplayScreenAction_H__

#include <map>

#include "Action.h"
#include "DeparturesTableTypes.h"

namespace synthese
{
	namespace pt
	{
		class PhysicalStop;
	}

	namespace departurestable
	{
		class DisplayScreen;
		class DisplayType;
		class DisplayScreenCPU;
		
		/** Display screen technical properties update action class.
			@ingroup m54Actions refActions
		*/
		class UpdateDisplayScreenAction : public util::FactorableTemplate<server::Action, UpdateDisplayScreenAction>
		{
		public:
			static const std::string PARAMETER_DISPLAY_SCREEN;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_WIRING_CODE;
			static const std::string PARAMETER_TYPE;
			static const std::string PARAMETER_COM_PORT;
			static const std::string PARAMETER_CPU;
			static const std::string PARAMETER_MAC_ADDRESS;

		private:
			std::string									_name;
			boost::shared_ptr<DisplayScreen>			_screen;
			int											_wiringCode;
			boost::shared_ptr<const DisplayType>		_type;
			boost::shared_ptr<const DisplayScreenCPU>	_cpu;
			int											_comPort;
			std::string									_macAddress;

			
		protected:
			/** Conversion from attributes to generic parameter maps.
				@warning Not yet implemented;
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Map to read
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			void setScreenId(util::RegistryKeyType id);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_UpdateDisplayScreenAction_H__
