
/** FenchSentence class implementation.
	@file FenchSentence.cpp

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

#include "FrenchSentence.h"

#include <utility>
#include <map>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	namespace lexmatcher
	{
		FrenchSentence::FrenchSentence()
		{}
		
		
		
		FrenchSentence::FrenchSentence(
			const std::string& source
		):	_source(source)
		{
			typedef tokenizer<char_separator<char> > tokenizer;
			char_separator<char> sep(" :,;.'");
			tokenizer words(source, sep);
			BOOST_FOREACH(string word, words)
			{
				string aliasedWord(_ConvertAlias(word));
				if(aliasedWord.empty()) continue;
				
				_words.push_back(FrenchPhoneticString(aliasedWord));
			}
		}
		
		
		string	FrenchSentence::_ConvertAlias(const std::string& ssource)
		{
			string source(to_lower_copy(ssource));
			
			if(source == "st")
			{
				return "saint";
			}
			
			if(	source == "ste")
			{
				return "sainte";
			}
			
			if(source == "pl")
			{
				return "place";
			}
			
			if(	source == "chu" ||
				source == "chr" ||
				source == "clinique"
			){
				return "hôpital";
			}
			
			if(	source == "a" ||
				source == "au" ||
				source == "d" ||
				source == "de" ||
				source == "des" ||
				source == "du" ||
				source == "en" ||
				source == "et" ||
				source == "l" ||
				source == "la" ||
				source == "le" ||
				source == "les" ||
				source == "un"
			){
				return string();
			}
			
			if(	source == "faculté" ||
				source == "fac" ||
				source == "faculte"
			){
				return "université";
			}
				
			return source;
		}
		
		
		
		const std::string& FrenchSentence::getSource() const
		{
			return _source;
		}
		
		
		
		FrenchSentence::ComparisonScore FrenchSentence::compare(
			const FrenchSentence& s
		) const {
			typedef map<size_t, pair<size_t, ComparisonScore> > Relations;
			
			Relations othersToThis;
			
			for(size_t i(0); i<_words.size(); ++i)
			{
				ComparisonScore bestScore(0);
				
				for(size_t j(0); j<s._words.size(); ++j)
				{
					FrenchPhoneticString::LevenshteinDistance distance(
						_words[i].levenshtein(s._words[j])
					);
					ComparisonScore score(
						1 - distance / (s._words[j].getPhonetic().size() + _words[i].getPhonetic().size())
					);
					assert(score >= 0 && score <= 1);
					if(score > bestScore)
					{
						if(	othersToThis.find(j) == othersToThis.end() ||
							othersToThis[j].second < score
						)
							othersToThis[j] = make_pair(i, score);
					}
				}
			}
			
			ComparisonScore totalScores(0);
			for(size_t j(0); j<s._words.size(); ++j)
			{
				if(othersToThis.find(j) != othersToThis.end())
				{
					totalScores += othersToThis[j].second;
				}
			}
			return totalScores / s._words.size();
		}
		
		int FrenchSentence::operator<(const FrenchSentence& s) const
		{
			return _source < s._source;
		}
		
		bool FrenchSentence::operator==(const FrenchSentence& s) const
		{
			return _source == s._source;
		}

	}
}

