
/** Transaction class implementation.
	@file Transaction.cpp

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

#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace util;

	namespace accounts
	{
		Transaction::Transaction( uid id /*= 0*/ )
			: RegistrableTemplate< Transaction>(id)
			, _documentId(0)
			, _leftUserId(0)
			, _placeId(0)
			, _startDateTime(not_a_date_time)
			, _endDateTime(not_a_date_time)
		{

		}


		const std::string& Transaction::getName() const
		{
			return _name;
		}

		uid Transaction::getDocumentId() const
		{
			return _documentId;
		}

		const time::DateTime& Transaction::getStartDateTime() const
		{
			return _startDateTime;
		}

		uid Transaction::getLeftUserId() const
		{
			return _leftUserId;
		}

		uid Transaction::getPlaceId() const
		{
			return _placeId;
		}

		const std::string& Transaction::getComment() const
		{
			return _comment;
		}

		const Transaction::PartsVector& Transaction::getParts() const
		{
			return _parts;
		}

		void Transaction::setName( const std::string& name )
		{
			_name = name;
		}

		void Transaction::setDocumentId( uid id )
		{
			_documentId = id;
		}

		void Transaction::setStartDateTime( const time::DateTime& dateTime )
		{
			_startDateTime = dateTime;
		}

		void Transaction::setLeftUserId( uid id )
		{
			_leftUserId = id;
		}

		void Transaction::setPlaceid( uid id )
		{
			_placeId = id;
		}

		void Transaction::setComment( const std::string& comment )
		{
			_comment = comment;
		}

		void Transaction::setEndDateTime( const time::DateTime& dateTime )
		{
			_endDateTime = dateTime;
		}

		const time::DateTime& Transaction::getEndDateTime() const
		{
			return _endDateTime;
		}
	}
}
