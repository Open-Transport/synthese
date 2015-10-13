
/** LanguageSelectorService class implementation.
	@file LanguageSelectorService.cpp
	@author Hugues Romain
	@date 2011

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

#include "LanguageSelectorService.hpp"

#include "HTMLForm.h"
#include "Language.hpp"
#include "Request.h"
#include "RequestException.h"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace html;


	template<> const string util::FactorableTemplate<Function,cms::LanguageSelectorService>::FACTORY_KEY("language_selector");

	namespace cms
	{
		const string LanguageSelectorService::PARAMETER_LANGUAGE("language");
		const string LanguageSelectorService::PARAMETER_NAME("name");
		const string LanguageSelectorService::PARAMETER_VALUE("value");



		LanguageSelectorService::LanguageSelectorService():
			_language(NULL),
			_value(NULL)
		{}




		ParametersMap LanguageSelectorService::_getParametersMap() const
		{
			ParametersMap map;
			if(_language)
			{
				map.insert(PARAMETER_LANGUAGE, _language->getIso639_2Code());
			}
			map.insert(PARAMETER_NAME, _name);
			if(_value)
			{
				map.insert(PARAMETER_VALUE, _value->getIso639_2Code());
			}
			return map;
		}



		void LanguageSelectorService::_setFromParametersMap(const ParametersMap& map)
		{
			// Language
			if(map.getOptional<string>(PARAMETER_LANGUAGE)) try
			{
				_language = &synthese::Language::GetLanguageFromIso639_2Code(map.get<string>(PARAMETER_LANGUAGE));
			}
			catch (synthese::Language::LanguageNotFoundException& e)
			{
				throw RequestException("Inconsistent display language "+ e.getMessage());
			}

			// Name
			_name = map.getDefault<string>(PARAMETER_NAME);

			// Value
			if(map.getOptional<string>(PARAMETER_VALUE)) try
			{
				_value = &synthese::Language::GetLanguageFromIso639_2Code(map.get<string>(PARAMETER_VALUE));
			}
			catch (synthese::Language::LanguageNotFoundException& e)
			{
				throw RequestException("Inconsistent default value "+ e.getMessage());
			}
		}



		util::ParametersMap LanguageSelectorService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Language
			const synthese::Language* language(_language);
			if(!language)
			{
				if(request.getUser() && request.getUser()->getLanguage())
				{
					language = request.getUser()->getLanguage();
				}
				else
				{
					language = &synthese::Language::GetLanguageFromIso639_1Code("en");
				}
			}

			// Fake form
			string emptyString;
			HTMLForm f(emptyString, emptyString);

			// Output
			stream <<
				f.getSelectInput(
					_name,
					synthese::Language::GetIso639_2LanguagesVector(*language),
					_value ? optional<string>(_value->getIso639_2Code()) : optional<string>()
				);

			return util::ParametersMap();
		}



		bool LanguageSelectorService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string LanguageSelectorService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
