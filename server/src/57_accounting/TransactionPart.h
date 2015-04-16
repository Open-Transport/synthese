
#ifndef SYNTHESE_TransactionPart_H__
#define SYNTHESE_TransactionPart_H__

#include <string>

#include "01_util/UId.h"
#include "01_util/Registrable.h"

namespace synthese
{
	namespace accounts
	{
		/** Part of financial transaction.
			@ingroup m57
		*/
		class TransactionPart : public util::RegistrableTemplate< TransactionPart>
		{
		private:
			uid				_transactionId;
			double			_leftCurrencyAmount;
			double			_rightCurrencyAmount;
			uid				_accountId;
			uid				_rateId;
			std::string		_tradedObjectId;		//!< Use it to record precisely which piece was sold (example : put the serial number of the object into this field, or an uid converted with util::Conversion)
			std::string		_comment;
			uid				_stockId;

		public:
			TransactionPart(uid id=0);

			uid					getTransactionId()			const;
			double				getLeftCurrencyAmount()		const;
			double				getRightCurrencyAmount()	const;
			uid					getAccountId()				const;
			uid					getRateId()					const;
			const std::string&	getTradedObjectId()			const;
			const std::string&	getComment()				const;
			uid					getStockId()				const;

			void setTransactionId(uid id);
			void setLeftCurrencyAmount(double amount);
			void setRightCurrencyAmount(double amount);
			void setAmount(double amount);
			void setAccountId(uid id);
			void setRateId(uid id);
			void setTradedObjectId(const std::string& id);
			void setComment(const std::string& comment);
			void setStockId(uid id);
		};
	}
}
#endif // SYNTHESE_TransactionPart_H__
