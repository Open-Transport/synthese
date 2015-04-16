
#ifndef SYNTHESE_Currency_H__
#define SYNTHESE_Currency_H__

#include <string>

#include "01_util/UId.h"
#include "01_util/Registrable.h"

namespace synthese
{
	namespace accounts
	{
		/** Currency.
			@ingroup m57
		*/
		class Currency : public util::RegistrableTemplate< Currency>
		{
		private:
			std::string	_symbol;
			std::string _name;

		public:
			Currency(uid id=0);

			const std::string& getSymbol() const;
			const std::string& getName() const;

			void setSymbol(const std::string& symbol);
			void setName(const std::string& name);
		};
	}
}

#endif // SYNTHESE_Currency_H__

