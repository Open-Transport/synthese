
#ifndef SYNTHESE_RoutePlannerSheetLineInterfacePage_H__
#define SYNTHESE_RoutePlannerSheetLineInterfacePage_H__


#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
	}

	namespace interfaces
	{
		/** Line of schedule sheet.
			@code schedule_sheet_row @endcode
		*/
		class RoutePlannerSheetLineInterfacePage : public InterfacePage
		{
		private:
			static const bool _registered;

		public:
			/** Display of line of schedule sheet.
				@param stream Stream to write on
				@param text (0) Text containing cells
				@param alternateColor (1) indicates if the line rank is odd or even. Values :
					- false/0 = odd
					- true/1 = even
				@param place (Place* object) : Corresponding place of the line
				@param site Displayed site
			*/
			void display( std::ostream& stream, const std::string& text, bool alternateColor, const synthese::env::ConnectionPlace* place
				, const Site* site = NULL ) const;


		};
	}
}
#endif // SYNTHESE_RoutePlannerSheetLineInterfacePage_H__