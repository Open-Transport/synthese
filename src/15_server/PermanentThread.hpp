
/** PermanentThread class header.
	@file Import.hpp

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

#ifndef SYNTHESE_server_PermanentThread_hpp__
#define SYNTHESE_server_PermanentThread_hpp__

#include "Object.hpp"

#include "CoordinatesSystem.hpp"
#include "CMSScriptField.hpp"
#include "EnumObjectField.hpp"
#include "ParametersMapField.hpp"
#include "PointerField.hpp"
#include "Poller.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "SchemaMacros.hpp"
#include "SecondsField.hpp"
#include "StringField.hpp"

namespace synthese
{
	namespace util
	{
		class Env;
	}

	FIELD_STRING(DeviceKey)
	FIELD_CMS_SCRIPT(Documentation)
	
	namespace server
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(DeviceKey),
			FIELD(Parameters),
			FIELD(Active),
			FIELD(Documentation)
		> PermanentThreadRecord;



		//////////////////////////////////////////////////////////////////////////
		/// Import class.
		///	@ingroup m19
		/// @author Camille Hue
		/// @since 3.9.0
		class PermanentThread:
			public Object<PermanentThread, PermanentThreadRecord>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<PermanentThread>	Registry;

		private:
			mutable boost::shared_ptr<Poller> _poller;
			mutable boost::shared_ptr<util::Env> _pollerEnv;
		
		public:
			PermanentThread(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				boost::shared_ptr<Poller> getPoller(
					util::Env& env,
					util::ParametersMap& pm
				) const;

				void launch() const;
				virtual void addAdditionalParameters(util::ParametersMap& map, std::string prefix) const;
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif // SYNTHESE_server_PermanentThread_hpp__
