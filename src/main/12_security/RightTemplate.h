
/** RightTemplate class header.
	@file RightTemplate.h

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

#ifndef SYNTHESE_security_RightTemplate_h__
#define SYNTHESE_security_RightTemplate_h__

#include "12_security/Right.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace security
	{
		/** RightTemplate class.
			@ingroup m12
		*/
		template<class T>
		class RightTemplate : public util::FactorableTemplate<Right, T>
		{
		public:
			Right::ParameterLabelsVector			getParametersLabels()	const;
			static Right::ParameterLabelsVector	getStaticParametersLabels();
		};

		template<class T>
		Right::ParameterLabelsVector synthese::security::RightTemplate<T>::getParametersLabels() const
		{
			return getStaticParametersLabels();
		}
	}
}

#endif // SYNTHESE_security_RightTemplate_h__
