
#include "Factorable.h"

namespace synthese
{
	namespace util
	{
		void Factorable::setFactoryKey( const std::string& key )
		{
			__factory_key = key;
		}

		const std::string& Factorable::getFactoryKey() const
		{
			return __factory_key;
		}

		Factorable::Factorable()
		{

		}
	}
}
