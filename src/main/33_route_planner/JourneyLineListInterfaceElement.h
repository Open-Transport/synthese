
#include "11_interfaces/LibraryInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueInterfaceElement;

		class JourneyLineListInterfaceElement : public LibraryInterfaceElement
		{
		private:
			ValueInterfaceElement* _displayPedestrianLines;
			ValueInterfaceElement* _rowStartHtml;
			ValueInterfaceElement* _rowEndHtml;
			ValueInterfaceElement* _pixelWidth;
			ValueInterfaceElement* _pixelHeight;
			~JourneyLineListInterfaceElement();

		public:
			/** Display.
				@param stream Stream to write on
				@param parameters Runtime parameters
				@param object (Journey*) Journey to display
				@param site Site to display
			*/
			void display(std::ostream& stream, const ParametersVector& parameters, const void* object = NULL, const server::Request* request = NULL) const;

			/** Parser.
			@param text Text to parse : standard list of parameters :
				-# Affichage des lignes à pied
				-# HTML en début de ligne (conseillé tr)
				-# HTML en fin de ligne (conseillé /tr)
				-# Largeur en pixels de la case de lignes
				-# Hauteur en pixels de la case de lignes
			*/
			void parse( const std::string& text);
		};

	}
}

