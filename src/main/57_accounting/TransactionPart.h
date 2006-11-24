
namespace synthese
{
	namespace accounting
	{
		class TransactionPart
		{
			Transaction* _transaction;
			double		_leftCurrencyAmount;
			double		_rightCurrencyAmount;
			Account*	_account;
			Fare*		_fare;
			std::string	_tradedObjectId;		//!< Use it to record precisely wich piece was sold (eg : put the serial number of the object into this field, or an uid converted with util::Conversion)
		};
	}
}