
#ifndef SYNTHESE_Currency_H__
#define SYNTHESE_Currency_H__

#include <string>

namespace synthese
{
	namespace accounts
	{
		/** Currency.
			@ingroup m57
		*/
		class Currency : public util::Registrable<uid, Currency>
		{
		private:
			std::string	_symbol;
			std::string _name;
		};
	}
}

#endif // SYNTHESE_Currency_H__
