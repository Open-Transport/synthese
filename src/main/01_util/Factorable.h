
#ifndef SYNTHESE_Factorable_H__
#define SYNTHESE_Factorable_H__

#include "Factory.h"
#include <string>

namespace synthese
{
	namespace util
	{
		class Factorable
		{
		private:
			std::string __factory_key;

		public:
			Factorable();
			void setFactoryKey(const std::string& key);
			const std::string& getFactoryKey() const;
		};
	}
}

#endif // SYNTHESE_Factorable_H__

