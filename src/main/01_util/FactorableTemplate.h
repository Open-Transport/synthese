
/** FactorableTemplate class header.
	@file FactorableTemplate.h

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

#ifndef SYNTHESE_util_FactorableTemplate_h__
#define SYNTHESE_util_FactorableTemplate_h__

namespace synthese
{
	namespace util
	{
		/** FactorableTemplate class.
			@ingroup m01
		*/
		template<class F, class C>
		class FactorableTemplate : public F
		{
		public:
			FactorableTemplate()
				: F()
			{

			}

			static const std::string FACTORY_KEY;

			virtual const std::string& getFactoryKey() const;

			static void integrate ();
		};

		template<class F, class C>
		void synthese::util::FactorableTemplate<F, C>::integrate ()
		{
		    synthese::util::Factory<F>::template integrate<C> ();
		}

		template<class F, class C>
		const std::string& synthese::util::FactorableTemplate<F, C>::getFactoryKey() const
		{
			return FACTORY_KEY;
		}
	}
}

#endif // SYNTHESE_util_FactorableTemplate_h__
