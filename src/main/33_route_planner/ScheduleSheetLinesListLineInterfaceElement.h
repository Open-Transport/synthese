

#include "11_interfaces/LibraryInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ScheduleSheetLinesListLineInterfaceElement : public LibraryInterfaceElement
		{
		private:
			static const bool _registered;

		public:
			void display(std::ostream& stream, const ParametersVector& parameters, const void* object = NULL, const server::Request* request = NULL) const;
			void parse( const std::string& text);
		};

	}
}


