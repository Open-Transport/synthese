
/** RightTemplate class header.
	@file RightTemplate.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "Right.h"

#include "FactorableTemplate.h"

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
			static const std::string NAME;
			static const bool USE_PRIVATE_RIGHTS;
			ParameterLabelsVector			getParametersLabels()	const;
			static ParameterLabelsVector	getStaticParametersLabels();
			const std::string& getName() const;

			bool getUsePrivateRights() const
			{
				return USE_PRIVATE_RIGHTS;
			}
		};

		template<class T>
		const std::string& synthese::security::RightTemplate<T>::getName() const
		{
			return NAME;
		}


		template<class T>
		ParameterLabelsVector synthese::security::RightTemplate<T>::getParametersLabels() const
		{
			return getStaticParametersLabels();
		}
	}
}

#endif // SYNTHESE_security_RightTemplate_h__
