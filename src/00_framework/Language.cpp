
/** Language class implementation.
	@file Language.cpp

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

#include "Language.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;


namespace synthese
{
	Language::LanguagesMap Language::_iso639_1LanguagesMap;
	Language::LanguagesMap Language::_iso639_2LanguagesMap;

	const std::string& Language::getName( const Language& nameLanguage ) const
	{
		NamesMap::const_iterator it(_names.find(&nameLanguage));
		if(it == _names.end())
		{
			return _names.begin()->second;
		}
		else
		{
			return it->second;
		}
	}



	const std::string& Language::getName() const
	{
		return getName(*this);
	}



	const Language& Language::GetLanguageFromIso639_1Code( const std::string& iso639_1Code )
	{
		LanguagesMap::const_iterator it(_iso639_1LanguagesMap.find(iso639_1Code));
		if(it == _iso639_1LanguagesMap.end())
		{
			throw LanguageNotFoundException(iso639_1Code);
		}
		return *it->second;
	}



	const Language& Language::GetLanguageFromIso639_2Code( const std::string& iso639_2Code )
	{
		LanguagesMap::const_iterator it(_iso639_2LanguagesMap.find(iso639_2Code));
		if(it == _iso639_2LanguagesMap.end())
		{
			throw LanguageNotFoundException(iso639_2Code);
		}
		return *it->second;
	}



	Language::LanguageNotFoundException::LanguageNotFoundException( const std::string& _iso639_1Code ):
	Exception("Language "+ _iso639_1Code +" does not exist.")
	{

	}



	void Language::Populate()
	{
		shared_ptr<Language> english(AddLanguage(string("en"), "eng", "english"));
		shared_ptr<Language> french(AddLanguage(string("fr"), "fre", "français"));
		shared_ptr<Language> german(AddLanguage(string("de"), "ger", "deutsch"));

		english->addName(*french, "anglais");
		english->addName(*german, "englisch");
		
		french->addName(*english, "french");
		french->addName(*german, "französisch");

		german->addName(*english, "german");
		german->addName(*french, "allemand");
	}



	void Language::addName( const Language& language, const std::string& name )
	{
		_names.insert(make_pair(&language, name));
	}



	boost::shared_ptr<Language> Language::AddLanguage(
		const boost::optional<std::string>& iso639_1Code,
		const std::string& iso639_2Code,
		const std::string& name
	){
		shared_ptr<Language> lang(new Language);
		lang->_iso639_1Code = iso639_1Code;
		lang->_iso639_2Code = iso639_2Code;
		lang->_names.insert(make_pair(lang.get(), name));
		if(iso639_1Code)
		{
			_iso639_1LanguagesMap.insert(make_pair(*iso639_1Code, lang));
		}
		_iso639_2LanguagesMap.insert(make_pair(iso639_2Code, lang));
		return lang;
	}



	Language::LanguagesVector Language::GetIso639_1LanguagesVector( const Language& nameLanguage )
	{
		LanguagesVector result;
		BOOST_FOREACH(const LanguagesMap::value_type& lang, _iso639_1LanguagesMap)
		{
			result.push_back(make_pair(lang.first, lang.second->getName(nameLanguage)));
		}
		return result;
	}



	Language::LanguagesVector Language::GetIso639_2LanguagesVector( const Language& nameLanguage )
	{
		LanguagesVector result;
		BOOST_FOREACH(const LanguagesMap::value_type& lang, _iso639_2LanguagesMap)
		{
			result.push_back(make_pair(lang.first, lang.second->getName(nameLanguage)));
		}
		return result;
	}
}
