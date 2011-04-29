
#include "57_accounting/TransactionPart.h"

namespace synthese
{
	using namespace util;

	namespace accounts
	{
		TransactionPart::TransactionPart(uid id)
			: RegistrableTemplate< TransactionPart>(id)
			, _transactionId(0)
			, _accountId(0)
			, _rateId(0)
		{
		}

		uid TransactionPart::getTransactionId() const
		{
			return _transactionId;
		}

		double TransactionPart::getLeftCurrencyAmount() const
		{
			return _leftCurrencyAmount;
		}

		double TransactionPart::getRightCurrencyAmount() const
		{
			return _rightCurrencyAmount;
		}

		uid TransactionPart::getAccountId() const
		{
			return _accountId;
		}

		uid TransactionPart::getRateId() const
		{
			return _rateId;
		}

		const std::string& TransactionPart::getTradedObjectId() const
		{
			return _tradedObjectId;
		}

		const std::string& TransactionPart::getComment() const
		{
			return _comment;
		}

		void TransactionPart::setTransactionId( uid id )
		{
			_transactionId = id;
		}

		void TransactionPart::setLeftCurrencyAmount( double amount )
		{
			_leftCurrencyAmount = amount;
		}

		void TransactionPart::setRightCurrencyAmount( double amount )
		{
			_rightCurrencyAmount = amount;
		}

		void TransactionPart::setAccountId( uid id )
		{
			_accountId = id;
		}

		void TransactionPart::setRateId( uid id )
		{
			_rateId = id;
		}

		void TransactionPart::setTradedObjectId( const std::string& id )
		{
			_tradedObjectId = id;
		}

		void TransactionPart::setComment( const std::string& comment )
		{
			_comment = comment;
		}

		void TransactionPart::setAmount( double amount )
		{
			setLeftCurrencyAmount(amount);
			setRightCurrencyAmount(amount);
		}

		uid TransactionPart::getStockId() const
		{
			return _stockId;
		}

		void TransactionPart::setStockId( uid id )
		{
			_stockId = id;
		}
	}
}
