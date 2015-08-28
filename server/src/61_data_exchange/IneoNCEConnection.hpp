
/** IneoNCEConnection class header.
	@file IneoNCEConnection.hpp

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

#ifndef SYNTHESE_pt_IneoNCEConnection_hpp__
#define SYNTHESE_pt_IneoNCEConnection_hpp__

#include "IConv.hpp"
#include "XmlParser.h"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

#include "AlarmObjectLink.h"
#include "CommercialLine.h"
#include "SentScenario.h"
#include "Alarm.h"

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace data_exchange
	{
		/** IneoNCEConnection class.
			@ingroup m35
		*/
		class IneoNCEConnection
		{
		public:
			static const std::string MODULE_PARAM_INEO_NCE_HOST;
			static const std::string MODULE_PARAM_INEO_NCE_PORT;
			static const std::string MODULE_PARAM_INEO_NCE_DATASOURCE_ID;
			static const std::string MODULE_PARAM_INEO_NCE_MESSAGE_RECIPIENTS;

			enum Status
			{
				offline,	// The client is not connected
				online,		// The client is connected
				connect		// The client must be connected
			};

		private:
			static boost::shared_ptr<IneoNCEConnection> _theConnection;

			std::string _nceAddress;
			std::string _ncePort;
			const impex::DataSource* _dataSource;

			typedef std::set<util::RegistryKeyType> MessageRecipients;
			MessageRecipients _messageRecipients;

			mutable Status _status;

			typedef std::pair<XMLResults, XMLNode> XMLParserResult;
			
			static XMLNode ParseInput(
				const std::string& xml
			);

			boost::asio::io_service _io_service;
			mutable boost::asio::deadline_timer _deadline;
			mutable boost::asio::ip::tcp::socket _socket;
			mutable boost::shared_ptr<boost::asio::streambuf> _buf;
			
			typedef std::map<std::string, std::string> StopMnaNameMap;
			mutable StopMnaNameMap _stopMnaNameMap;
			typedef std::map<std::string, std::string> StopOrdMnaMap;
			mutable StopOrdMnaMap _stopOrdMnaMap;
			mutable std::string _curOrd;
			util::IConv _iconv;

			void establishConnection();

			void read();

			void handleData(
			) const;

			void checkDeadline();

			// Message management
			boost::shared_ptr<messages::SentScenario> _getScenario(bool createIfNoExistingScenario) const;
			typedef std::map<std::string, std::string> MessagesByType;
			void _setMessages(const MessagesByType& value) const;
			
		public:
			IneoNCEConnection();

			/// @name Setters
			//@{
				void setNCEAddress(const std::string& value){ _nceAddress = value; }
				void setNCEPort(const std::string& value){ _ncePort = value; }
				void setStatus(const Status& value){ _status = value; }
			//@}

			/// @name Getters
			//@{
				const std::string& getNCEAddress() const { return _nceAddress; }
				const std::string& getNCEPort() const { return _ncePort; }
				const Status& getStatus() const { return _status; }
			//@}

//				static void test(const boost::system::error_code& error);
			static void RunThread();

			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);

			static boost::shared_ptr<IneoNCEConnection> GetTheConnection(){ return _theConnection; }
		};
}	}

#endif // SYNTHESE_pt_IneoNCEConnection_hpp__

