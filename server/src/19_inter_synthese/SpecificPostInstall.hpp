#ifndef SYNTHESE_SPECIFICPOSTINSTALL_HPP
#define SYNTHESE_SPECIFICPOSTINSTALL_HPP

//////////////////////////////////////////////////////////////////////////
/// SpecificPostInstall class header.
///	@file SpecificPostInstall.hpp
///	@author RCSmobility
///	@date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "Action.h"
#include "FactorableTemplate.h"
#include "ParametersMap.h"
#include "InterSYNTHESEConfig.hpp"

namespace synthese
{
	namespace inter_synthese
	{

		class SpecificPostInstall:
			public util::FactorableTemplate<server::Action, SpecificPostInstall>
		{
		public:
			static const std::string PARAMETER_POST_INSTALL_PASSIVE_IMPORT_ID;
			static const std::string PARAMETER_POST_INSTALL_SLAVE_ID;
			static const std::string PARAMETER_POST_INSTALL_SLAVE_TO_MASTER_IP;
			static const std::string PARAMETER_POST_INSTALL_TABLES;

		private:
			const boost::shared_ptr<InterSYNTHESEConfig> getMyConfig();
			const boost::shared_ptr<InterSYNTHESESlave> getMySlave();
			void addTable(InterSYNTHESEConfig &config,
				const std::string tableCode);

			util::RegistryKeyType _passiveImportId;
			util::RegistryKeyType _slaveId;
			std::string _slaveToMasterIp;
			std::string _tables;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;

		};
	}
}

#endif // SYNTHESE_SPECIFICPOSTINSTALL_HPP
