/** MGScreenConnection class header.
	@file MGScreenConnection.hpp

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

#ifndef SYNTHESE_pt_MGScreenConnection_hpp__
#define SYNTHESE_pt_MGScreenConnection_hpp__

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreenCPU;
	}

	namespace data_exchange
	{
		/** MGScreenConnection class.
			@ingroup m61
		*/
		class MGScreenConnection
		{
		public:
			static const std::string MODULE_PARAM_MG_SCREEN_HOST;
			static const std::string MODULE_PARAM_MG_SCREEN_PORT;
			static const std::string MODULE_PARAM_MG_SCREEN_SPEED;
			static const std::string MODULE_PARAM_MG_SCREEN_VALUE;
			static const std::string MODULE_PARAM_MG_SCREEN_MIN;
			static const std::string MODULE_PARAM_MG_SCREEN_MAX;
			static const std::string MODULE_PARAM_MG_CPU_NAME;
			static const std::string MODULE_PARAM_MG_ARCHIVE_MONITORING;

			enum Status
			{
				offline,	// The client is not connected
				online,		// The client is connected
				connect		// The client must be connected
			};

		private:
			static boost::shared_ptr<MGScreenConnection> _theConnection;

			mutable bool _initialized;
			std::string _mgScreenAddress;
			std::string _mgScreenPort;
			int _mgScreenSpeed;
			int _mgScreenValue;
			int _mgScreenMin;
			int _mgScreenMax;
			std::string _mgCPUName;
			departure_boards::DisplayScreenCPU* _cpu;
			bool _mgArchiveMonitoring;

			mutable Status _status;


			boost::asio::io_service _io_service;
			mutable boost::asio::deadline_timer _deadline;
			mutable boost::asio::ip::tcp::socket _socket;
			mutable boost::shared_ptr<boost::asio::streambuf> _buf;


			void establishConnection();

			void read();

			void registerData(
			) const;

			void handleData(
			) const;

			void checkDeadline();

			void displaySetBacklightParams(int min, int max, int speed) const;
			void displaySetBacklightValue(int value) const;

		public:
			MGScreenConnection();

			/// @name Setters
			//@{
				void setMGScreenAddress(const std::string& value){ _mgScreenAddress = value; }
				void setMGScreenPort(const std::string& value){ _mgScreenPort = value; }
				void setStatus(const Status& value){ _status = value; }
			//@}

			/// @name Getters
			//@{
				const std::string& getMGScreenAddress() const { return _mgScreenAddress; }
				const std::string& getMGScreenPort() const { return _mgScreenPort; }
				const Status& getStatus() const { return _status; }
			//@}

			static void RunThread();

			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);

		};
}	}

#endif // SYNTHESE_pt_MGScreenConnection_hpp__
