#ifndef SYNTHESE_ENV_AXIS_H
#define SYNTHESE_ENV_AXIS_H

#include "Registrable.h"


#include <string>

namespace synthese
{
	namespace env
	{
		/** Axis.
			@ingroup m35

			Regroupement technique de lignes techniques, donnant lieu � l'application de propri�t�s aux lignes contenues, dont par d�faut :
				- la correspondance entre deux services de lignes incluses dans un m�me axe est interdite.

			Le regroupement en axes s'effectue, par convention, uniquement � l'int�rieur des \ref defLigneCommerciale "lignes commerciales". Ainsi deux services pouvant �tre group�s selon les crit�res ci-dessus, mais n'appartenant pas � la m�me ligne commerciale, ne seront pas group�s dans une ligne technique. De ce fait, les axes sont consid�r�s comme des subdivisions de lignes commerciales.

			Several special axis follow additional rules :
				- <b>Free axis</b> : Axe permettant la correspondance entre tous les services contenus, y compris entre deux services de la m�me ligne
				- <b>Forbidden axis</b> : Axe contenant des lignes ne prenant pas de voyageurs (ex : trajets haut le pied)

		*/
		class Axis
		:	public virtual util::Registrable
		{

			std::string _name;   
			bool _free;   //!< Whether or not this axis is a free axis.
			bool _allowed;  //!< Whether or not this axis is an allowed axis (opposite to forbidden axis).

			public:

			Axis(
				util::RegistryKeyType id = UNKNOWN_VALUE,
				std::string name = std::string(),
				bool free = true,
				bool allowed = false
			);
			~Axis ();

			//! @name Getters/Setters
			//@{
			const std::string& getName () const;
			void setName (const std::string& name);

			bool isFree () const;
			void setFree (bool isFree);

			bool isAllowed () const;
			void setAllowed (bool isAllowed);
			//@}
		};
	}
}

#endif
