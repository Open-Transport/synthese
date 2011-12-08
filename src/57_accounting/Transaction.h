
/** Transaction class header.
	@file Transaction.h

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

#ifndef SYNTHESE_Transaction_H__
#define SYNTHESE_Transaction_H__

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

#include "01_util/UId.h"
#include "01_util/Registrable.h"

#include "04_time/DateTime.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace accounts
	{
		class TransactionPart;
		class Account;

		/** Financial transaction.
			@ingroup m57
		*/
		class Transaction : public util::RegistrableTemplate< Transaction>
		{
		public:
			typedef std::vector<TransactionPart*> PartsVector;

		private:
			std::string			_name;
			uid					_documentId;
			time::DateTime		_startDateTime;
			time::DateTime		_endDateTime;
			uid					_leftUserId;	// Applicable if the used accounts are template
			uid					_placeId;
			std::string			_comment;
			PartsVector			_parts;

		public:
			Transaction(uid id = 0);

			const std::string&			getName()			const;
			uid							getDocumentId()		const;
			const time::DateTime&		getStartDateTime()	const;
			const time::DateTime&		getEndDateTime()	const;
			uid							getLeftUserId()		const;
			uid							getPlaceId()		const;
			const std::string&			getComment()		const;
			const PartsVector&			getParts()			const;

			void setName(const std::string& name);
			void setDocumentId(uid id);
			void setStartDateTime(const time::DateTime& dateTime);
			void setEndDateTime(const time::DateTime& dateTime);
			void setLeftUserId(uid id);
			void setPlaceid(uid id);
			void setComment(const std::string& comment);

			boost::shared_ptr<TransactionPart> getPart(boost::shared_ptr<const Account> account) const;
		};
	}
}

#endif // SYNTHESE_Transaction_H__
