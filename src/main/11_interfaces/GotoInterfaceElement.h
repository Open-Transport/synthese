
#ifndef SYNTHESE_GotoInterfaceElement_H__
#define SYNTHESE_GotoInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"
#include <string>

namespace synthese
{
	namespace interfaces
	{
		class GotoInterfaceElement : public LibraryInterfaceElement
		{
		private:
			static const std::string _factory_key;
			std::string _label;

		public:
			void display(std::ostream& stream, const ParametersVector& parameters, const void* object = NULL, const Site* site = NULL) const;
			void parse( const std::string& text);
			const std::string& getLabel() const;
		};

	}
}

#endif // SYNTHESE_GotoInterfaceElement_H__
