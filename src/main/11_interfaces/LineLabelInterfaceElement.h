
#ifndef SYNTHESE_LineLabelInterfaceElement_H__
#define SYNTHESE_LineLabelInterfaceElement_H__


#include "11_interfaces/LibraryInterfaceElement.h"
#include <string>

namespace synthese
{
	namespace interfaces
	{
		class LineLabelInterfaceElement : public LibraryInterfaceElement
		{
		private:
			std::string _label;

		public:
			void display(std::ostream& stream, const ParametersVector& parameters, const void* object = NULL, const server::Request* request = NULL) const;
			void parse( const std::string& text);
			const std::string& getLabel() const;
		};

	}
}

#endif // SYNTHESE_LineLabelInterfaceElement_H__
