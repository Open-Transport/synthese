
/** AlarmRecipient class header.
	@file AlarmRecipient.h

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

#ifndef SYNTHESE_AlarmRecipient_h__
#define SYNTHESE_AlarmRecipient_h__

#include <ostream>

#include "FactoryBase.h"
#include "SecurityTypes.hpp"
#include "MessagesTypes.h"
#include "StaticActionRequest.h"

namespace synthese
{
	namespace html
	{
		class HTMLForm;
	}

	namespace impex
	{
		class DataSource;
	}

	namespace security
	{
		class Right;
	}

	namespace messages
	{
		class Alarm;
		class AlarmObjectLink;
		class AlarmAddLinkAction;
		class AlarmRemoveLinkAction;

		/** Alarm recipient class.
			@ingroup m17

			All the available alarm recipients are listed in the @ref refAlarmrecipients page.
		*/
		class AlarmRecipient:
			public util::FactoryBase<AlarmRecipient>
		{
		public:
			typedef std::map<
				const util::Registrable*,
				std::set<const AlarmObjectLink*>
			>	ObjectLinks;
			static ObjectLinks::mapped_type _emptyAOLSet;

			//////////////////////////////////////////////////////////////////////////
			/// Name of the recipient.
			virtual const std::string& getTitle() const = 0;


			//////////////////////////////////////////////////////////////////////////
			/// Interface to administrate links to objects.
			virtual void displayBroadcastListEditor(
				std::ostream& stream
				, const messages::Alarm* alarm
				, const util::ParametersMap& request
				, server::Request& searchRequest
				, server::StaticActionRequest<AlarmAddLinkAction>& addRequest
				, server::StaticActionRequest<AlarmRemoveLinkAction>& removeRequest
			) = 0;

			virtual AlarmRecipientSearchFieldsMap getSearchFields(
				html::HTMLForm& form,
				const util::ParametersMap& parameters
			) const = 0;

			
			virtual void getParametersLabels(
				security::ParameterLabelsVector& m
			) const = 0;

			virtual util::RegistryKeyType getObjectIdBySource(
				const impex::DataSource& source,
				const std::string& key,
				util::Env& env
			) const = 0;

			virtual boost::shared_ptr<security::Right> getRight(const std::string& perimeter) const = 0;

			struct AvailableRecipients
			{
				util::RegistryKeyType id;
				std::string parameter;
				std::string name;
				typedef std::vector<boost::shared_ptr<AvailableRecipients> > Tree;
				Tree tree;
			};
		};
}	}

/** @defgroup refAlarmrecipients Alarm recipients.
	@ingroup ref
	@copydoc AlarmRecipient
*/

#endif // SYNTHESE_AlarmRecipient_h__
