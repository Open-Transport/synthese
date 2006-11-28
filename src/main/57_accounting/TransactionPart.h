
#ifndef SYNTHESE_TransactionPart_H__
#define SYNTHESE_TransactionPart_H__

#include <string>

#include "01_util/UId.h"

namespace synthese
{
	namespace accounts
	{
		class Account;
		class Transaction;

		/** Part of financial transaction.
			@ingroup m57
		*/
		class TransactionPart : public util::Registrable<uid, TransactionPart>
		{
			Transaction*	_transaction;
			double			_leftCurrencyAmount;
			double			_rightCurrencyAmount;
			Account*		_account;
			uid				_rateId;
			std::string		_tradedObjectId;		//!< Use it to record precisely which piece was sold (example : put the serial number of the object into this field, or an uid converted with util::Conversion)
		};
	}
}
#endif // SYNTHESE_TransactionPart_H__