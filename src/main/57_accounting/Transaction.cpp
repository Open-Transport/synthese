
#include "57_accounting/Transaction.h"

namespace synthese
{
	using namespace util;

	namespace accounts
	{
		Transaction::Transaction( uid id /*= 0*/ )
			: Registrable<uid, Transaction>(id)
			, _documentId(0)
			, _leftUserId(0)
			, _placeId(0)
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

		const time::DateTime& Transaction::getDateTime() const
		{
			return _dateTime;
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

		void Transaction::setDateTime( const time::DateTime& dateTime )
		{
			_dateTime = dateTime;
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
	}
}