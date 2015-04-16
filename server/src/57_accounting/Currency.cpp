
#include "57_accounting/Currency.h"

namespace synthese
{
	using namespace util;

	namespace accounts
	{
		Currency::Currency(uid id)
			: RegistrableTemplate< Currency>(id)
		{

		}

		const std::string& Currency::getSymbol() const
		{
			return _symbol;
		}

		const std::string& Currency::getName() const
		{
			return _name;
		}

		void Currency::setSymbol( const std::string& symbol )
		{
			_symbol = symbol;
		}

		void Currency::setName( const std::string& name )
		{
			_name = name;
		}
	}
}
