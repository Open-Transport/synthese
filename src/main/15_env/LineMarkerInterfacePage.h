
#ifndef SYNTHESE_LineMarkerInterfacePage_H__
#define SYNTHESE_LineMarkerInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}
	namespace env
	{
		class Line;

		/** Line marker.
		@code line_marker @endcode
		*/
		class LineMarkerInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Display of line of schedule sheet.
				@param stream Stream to write on
				@param tableOpeningHTML HTML d'ouverture du tableau (RIEN = Pas d'ouverture du tableau)
				@param tableClosingHTML HTML de fermeture du tableau (RIEN = Pas de fermeture du tableau)
				@param pixelWidth Largeur de la case en pixels
				@param pixelHeight Hauteur de la case en pixels
				@param line Line to display
				@param site Displayed site
			*/
			void display( std::ostream& stream, const std::string& tableOpeningHTML
				, const std::string& tableClosingHTML
				, int pixelWidth
				, int pixelHeight
				, const synthese::env::Line* line
				, const server::Request* request = NULL ) const;


		};
	}
}
#endif // SYNTHESE_LineMarkerInterfacePage_H__
