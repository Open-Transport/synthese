
/** Poller class header.
	@file Poller.hpp

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

#ifndef SYNTHESE_server_Poller_hpp__
#define SYNTHESE_server_Poller_hpp__

#include <ostream>
#include <fstream>
#include <boost/optional.hpp>

namespace synthese
{
	namespace db
	{
		class DBTransaction;
	}

	namespace util
	{
		class Env;
		class ParametersMap;
	}

	namespace server
	{
		class Request;
		class PermanentThread;

		/** Poller class.
			@ingroup m15
		*/
		class Poller
		{
		public:

			Poller(
				util::Env& env,
				const PermanentThread& permanentThread,
				util::ParametersMap& pm
			);

		protected:
			util::Env&				_env;
			const PermanentThread&			_permanentThread;
			util::ParametersMap& _pm;	//!< Parameters map where entries must be stored


			//virtual db::DBTransaction _save() const = 0;

		public:
			//! @name Getters
			//@{
				const PermanentThread& getPermanentThread() const { return _permanentThread; }
			//@}

			//! @name Setters
			//@{
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// @param map Parameters map to interpret
			/// @author Camille Hue
			/// @date 2013
			/// @since 3.9.0
			virtual void setFromParametersMap(
				const util::ParametersMap& map
			) = 0;

			virtual util::ParametersMap getParametersMap() const = 0;


			//////////////////////////////////////////////////////////////////////////
			/// Launches the poller
			/// @return true if it is allowed to save the data
			virtual bool launchPoller() const = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Interface for the save method.
			/// @return transaction to run
			//db::DBTransaction save() const;
		};
}	}

#endif // SYNTHESE_server_Poller_hpp__
