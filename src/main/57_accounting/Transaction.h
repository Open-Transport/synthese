
#ifndef SYNTHESE_Transaction_H__
#define SYNTHESE_Transaction_H__

#include <vector>
#include <string>

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

		/** Financial transaction.
			@ingroup m57
		*/
		class Transaction : public util::Registrable<uid, Transaction>
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
		};
	}
}

#endif // SYNTHESE_Transaction_H__

