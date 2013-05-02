////////////////////////////////////////////////////////////////////////////////
///	ImpExModule class implementation.
///	@file ImpExModule.cpp
///	@author Hugues Romain (RCS)
///	@date mer jun 24 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ImpExModule.h"
#include "Exception.h"
#include "FileFormat.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<ModuleClass,ImpExModule>::FACTORY_KEY("16_impex");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<ImpExModule>::NAME("Import Export");

		template<> void ModuleClassTemplate<ImpExModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<ImpExModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<ImpExModule>::End()
		{
		}

		template<> void ModuleClassTemplate<ImpExModule>::Start()
		{
		}



		template<> void ModuleClassTemplate<ImpExModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<ImpExModule>::CloseThread(
		){
		}
	}

	namespace impex
	{
		ImpExModule::FileFormatsList ImpExModule::GetFileFormatsList()
		{
			Factory<FileFormat>::Keys keys(Factory<FileFormat>::GetKeys());
			FileFormatsList vec;
			vec.push_back(make_pair(optional<string>(), "(saisie manuelle)"));
			BOOST_FOREACH(const Factory<FileFormat>::Keys::value_type& key, keys)
			{
				vec.push_back(make_pair(optional<string>(key), key));
			}
			return vec;
		}
	}
}
