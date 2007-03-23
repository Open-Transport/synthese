
/** ValueInterfaceElement class header.
	@file ValueInterfaceElement.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_ValueInterfaceElement_H__
#define SYNTHESE_ValueInterfaceElement_H__

#include <string>

#include "01_util/Factorable.h"

#include "11_interfaces/Types.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace interfaces
	{
		class ValueElementList;
		class InterfacePage;

		/** Code defined interface element which produces a string value at runtime.
		*/
		class ValueInterfaceElement : public util::Factorable
		{
		protected:
			const InterfacePage*	_page;

		public:
			//! \name Setters and modifiers
			//@{
				void				setPage(const InterfacePage* page);
				virtual void		storeParameters(ValueElementList& vel) = 0;
			//@}

			//! \name Evaluation methods
			//@{
				virtual std::string	getValue(const ParametersVector&, interfaces::VariablesMap& variables, const void* object = NULL, const server::Request* request = NULL) const = 0;
				bool				isZero(const ParametersVector&, interfaces::VariablesMap& variables, const void* object = NULL, const server::Request* request = NULL) const;
			//@}
		};
	}
}
#endif // SYNTHESE_ValueInterfaceElement_H__

/** @todo implement and register this
case EI_ATTRIBUT_Ligne_LibelleSimple:
return ( ( const Line* ) __Objet ) ->getShortName ();

case EI_ATTRIBUT_Ligne_Image:
return ( ( const Line* ) __Objet ) ->getImage();

case EI_ATTRIBUT_Ligne_LibelleComplet:
return ( ( const Line* ) __Objet ) ->getLongName ();

case EI_ATTRIBUT_Ligne_Style:
return ( ( const Line* ) __Objet ) ->getStyle();

case EI_ATTRIBUT_Ligne_ArticleMateriel:
return ( ( const Line* ) __Objet ) ->getLongName ();
// MJ TODO return ( ( const Line* ) __Objet ) ->Materiel() ->getArticle();

case EI_ATTRIBUT_Ligne_LibelleMateriel:
return ( ( const Line* ) __Objet ) ->getLongName ();
// MJ TODO return ( ( const Line* ) __Objet ) ->Materiel() ->getLibelleSimple();
*/


