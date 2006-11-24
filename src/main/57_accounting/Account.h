
namespace synthese
{
	namespace accounting
	{
		/** Account.
			The account can be used in several ways :
				- standard account between two entities : leftUser and rightUser, with ou without numbers.
				- template account handled by an entity : all left variables are null or empty. leftUser will be specified in the transactions.
				- internal accounts of an entity : leftUser and rightUser points to the entity.
		*/
		class Account.h
		{
			User* leftUser;
			std::string leftNumber;
			std::string leftClassNumber;
//			Currency*	_leftCurrency;
			User* rightUser;
			std::string _rightNumber;
			std::string _rightClassNumber;
//			Currency*	_rightCurrency;

		};
	}
}