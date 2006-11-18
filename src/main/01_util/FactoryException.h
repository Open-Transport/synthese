
#ifndef SYNTHESE_FactoryException_H__
#define SYNTHESE_FactoryException_H__


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace util
	{


		/** Thread related exception class
		@ingroup m01
		*/

		template<class T>
		class FactoryException : public synthese::util::Exception
		{
		private:

		public:

			FactoryException ( const std::string& message) : synthese::util::Exception(message) {}
			~FactoryException () throw () {}

		private:


		};




	}

}
#endif // SYNTHESE_FactoryException_H__
