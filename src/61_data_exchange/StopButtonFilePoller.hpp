
/** StopButtonFilepoller class header.
	@file StopButtonFilePoller.hpp

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

#ifndef SYNTHESE_pt_StopButtonFilePoller_hpp__
#define SYNTHESE_pt_StopButtonFilePoller_hpp__

#include <boost/shared_ptr.hpp>
#include <string>

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace data_exchange
	{
		/** StopButtonFilePoller class.
			@ingroup m61
		*/
		class StopButtonFilePoller
		{
		public:
			static const std::string MODULE_PARAM_STOP_MONITORING_FILE;
			static const std::string MODULE_PARAM_STOP_MONITORING_PERIOD_MS;

			enum Status
			{
				offline,	// We are not connected and don't have to
				online,		// We have opened the file and we pool it
				connect		// We have to go online, we may stay in this
							// state if we fail to open the file
			};

		private:
			static boost::shared_ptr<StopButtonFilePoller> _theConnection;

			std::string _file;
			std::string _periodMS;

			mutable Status _status;

		public:
			StopButtonFilePoller();

			/// @name Setters
			//@{
				void setFileToPoll(const std::string& value){ _file = value; }
				void setPeriodMS(const std::string& value){ _periodMS = value; }
				void setStatus(const Status& value){ _status = value; }
			//@}

			/// @name Getters
			//@{
				const std::string& getFileToPoll() const { return _file; }
				const std::string& getPeriodMS() const { return _periodMS; }
				const Status& getStatus() const { return _status; }
			//@}


			static void RunThread();

			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);
		};
}	}

#endif // SYNTHESE_pt_StopButtonFilePoller_hpp__

