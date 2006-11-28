
#ifndef SYNTHESE_Transaction_H__
#define SYNTHESE_Transaction_H__

#include <vector>
#include <string>

#include "01_util/UId.h"

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

		/** Financial transaction.
			@ingroup m57
		*/
		class Transaction : public util::Registrable<uid, Transaction>
		{
		public:
			typedef std::vector<TransactionPart*> TransactionVector;

		private:
//			Document*			_document;
			std::string			_name;
			time::DateTime		_dateTime;
			uid					_leftUserId;	// Applicable if the used accounts are template
			uid					_id;
			uid					_placeId;
			std::string			_comment;
			TransactionVector	_parts;
		}
	}
}

#endif // SYNTHESE_Transaction_H__
