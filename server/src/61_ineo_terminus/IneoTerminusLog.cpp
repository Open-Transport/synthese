/** IneoTerminusLog implementation.
	@file IneoTerminusLog.cpp
	@author Camille Hue
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "IneoTerminusLog.hpp"

#include <DBLogEntry.h>
#include <DBTableSync.hpp>
#include <DBTableSyncTemplate.hpp>
#include <FactorableTemplate.h>
#include <Object.hpp>
#include <Registrable.h>

#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <sstream>
#include <vector>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace dblog;

	namespace util
	{
		template<> const std::string FactorableTemplate<DBLog, ineo_terminus::IneoTerminusLog>::FACTORY_KEY("ineoterminuserrormessages");
	}

	namespace ineo_terminus
	{
		string IneoTerminusLog::getName() const
		{
			return "Echange de messages Ineo / Terminus";
		}



		DBLog::ColumnsVector IneoTerminusLog::getColumnNames() const
		{
			DBLog::ColumnsVector v;
			v.push_back("Tag principal");
			v.push_back("ErrorType");
			v.push_back("ErrorMessage");
			v.push_back("ErrorID");
			v.push_back("RequestID");
			return v;
		}



		std::string IneoTerminusLog::getObjectName(
			RegistryKeyType id,
			const server::Request& searchRequest
		) const	{
			return DBLog::getObjectName(id,searchRequest);
		}



		void IneoTerminusLog::AddIneoTerminusErrorMessageEntry(
			XMLNode node
		) {
			Level level = DB_LOG_ERROR;
			DBLog::ColumnsVector content;
			string tagName(node.getName());
			content.push_back(tagName);

			if (node.nChildNode("ErrorType") == 1)
			{
				content.push_back(node.getChildNode("ErrorType", 0).getText());
			}
			else
			{
				content.push_back("");
			}
			if (node.nChildNode("ErrorMessage") == 1)
			{
				content.push_back(node.getChildNode("ErrorMessage", 0).getText());
			}
			else
			{
				content.push_back("");
			}
			if (node.nChildNode("ErrorID") == 1)
			{
				content.push_back(node.getChildNode("ErrorID", 0).getText());
			}
			else
			{
				content.push_back("");
			}
			if (node.nChildNode("RequestID") == 1)
			{
				content.push_back(node.getChildNode("RequestID", 0).getText());
			}
			else
			{
				content.push_back("");
			}

			_addEntry(FACTORY_KEY, level, content, NULL);
		}

		void IneoTerminusLog::AddIneoTerminusInfoMessageEntry(
			string requestName,
			string requestID,
			string shortMessage
		) {
			Level level = DB_LOG_INFO;
			DBLog::ColumnsVector content;
			content.push_back(requestName);
			content.push_back("Envoi d'un message au SAE Ineo depuis Terminus");
			content.push_back(shortMessage);
			content.push_back("");
			content.push_back(requestID);

			_addEntry(FACTORY_KEY, level, content, NULL);
		}

	} /* namespace ineo_terminus */
} /* namespace synthese */
