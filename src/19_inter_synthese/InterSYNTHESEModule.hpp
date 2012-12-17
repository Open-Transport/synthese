
/** InterSYNTHESEModule class header.
	@file InterSYNTHESEModule.hpp
	@author Hugues Romain
	@date 2012

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

#ifndef SYNTHESE_InterSYNTHESEModule_H__
#define SYNTHESE_InterSYNTHESEModule_H__

#include "ModuleClassTemplate.hpp"

#include "UtilTypes.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace db
	{
		class DBTransaction;
	}

	//////////////////////////////////////////////////////////////////////////
	/// 19 Inter-SYNTHESE Module namespace.
	/// @author Hugues Romain
	/// @ingroup m19
	namespace inter_synthese
	{
		class InterSYNTHESEContent;

		/**	@defgroup m19Actions 19.15 Actions
			@ingroup m19

			@defgroup m19Functions 19.15 Functions
			@ingroup m19

			@defgroup m19Exceptions 19.01 Exceptions
			@ingroup m19

			@defgroup m19Alarm 19.17 Messages recipient
			@ingroup m19

			@defgroup m19LS 19.10 Table synchronizers
			@ingroup m19

			@defgroup m19Rights 19.12 Rights
			@ingroup m19

			@defgroup m19Logs 19.13 DB Logs
			@ingroup m19

			@defgroup m19 19 Inter SYNTHESE
			@ingroup m1

			@{
		*/

		//////////////////////////////////////////////////////////////////////////
		/// 19 Inter SYNTHESE Module class.
		///	@author Hugues Romain
		class InterSYNTHESEModule:
			public server::ModuleClassTemplate<InterSYNTHESEModule>
		{
		public:
			static const std::string MODULE_PARAM_INTER_SYNTHESE_MASTER_HOST;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_MASTER_PORT;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_WAITING_TIME;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_SLAVE_ACTIVE;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_SLAVE_ID;

		private:
			static boost::posix_time::time_duration _syncWaitingTime;
			static std::string _masterHost;
			static std::string _masterPort;
			static bool _slaveActive;
			static util::RegistryKeyType _slaveId;

		public:
			static void InterSYNTHESE();

			static void Enqueue(
				const InterSYNTHESEContent& content,
				boost::optional<db::DBTransaction&> transaction
			);

			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);
		};

		/** @} */
	}
}

#endif // SYNTHESE_InterSYNTHESEModule_H__
