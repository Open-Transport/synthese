
/** Language class header.
	@file Language.hpp

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

#ifndef SYNTHESE__Language_hpp__
#define SYNTHESE__Language_hpp__

#include "Exception.h"

#include <string>
#include <map>
#include <vector>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// Language class.
	/// See http://www.loc.gov/standards/iso639-2/php/French_list.php
	//////////////////////////////////////////////////////////////////////////
	/// @ingroup m0
	/// @author Hugues Romain
	/// @since 3.2.1
	/// @date 2011
	class Language
	{
	public:
		typedef std::map<const Language*, std::string> NamesMap;

		typedef std::map<std::string, boost::shared_ptr<Language> > LanguagesMap;

		typedef std::vector<std::pair<boost::optional<std::string>, std::string> > LanguagesVector;

		class LanguageNotFoundException:
			public Exception
		{
		public:
			LanguageNotFoundException(const std::string& _iso639_1Code);
		};

	private:
		boost::optional<std::string> _iso639_1Code;
		std::string _iso639_2Code;
		NamesMap _names;
		std::vector<std::string> _weekDayNames;
		std::string _dateFormatString;

		static LanguagesMap _iso639_1LanguagesMap;
		static LanguagesMap _iso639_2LanguagesMap;

		static boost::shared_ptr<Language> AddLanguage(
			const boost::optional<std::string>& iso639_1Code,
			const std::string& iso639_2Code,
			const std::string& name
		);


	public:
		//! @name Getters
		//@{
			const boost::optional<std::string>& getIso639_1Code() const { return _iso639_1Code; }
			const std::string& getIso639_2Code() const { return _iso639_2Code; }
			const NamesMap getNames() const { return _names; }
		//@}

		//! @name Modifiers
		//@{
			void addName(const Language& language, const std::string& name);
		//@}

		//! @name Services
		//@{
			const std::string& getName(const Language& nameLanguage) const;
			const std::string& getName() const;

			const std::string& getWeekDayName(boost::gregorian::date::day_of_week_type value) const;

			//////////////////////////////////////////////////////////////////////////
			/// @throws Language::LanguageNotFoundException
			static const Language& GetLanguageFromIso639_1Code(const std::string& iso639_1Code);

			static const Language& GetLanguageFromIso639_2Code(const std::string& iso639_2Code);

			static void Populate();

			static LanguagesVector GetIso639_1LanguagesVector(const Language& nameLanguage);
			static LanguagesVector GetIso639_2LanguagesVector(const Language& nameLanguage);
		//@}
	};
}

#endif // SYNTHESE__Language_hpp__
