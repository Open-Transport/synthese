
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
#include <sstream>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	namespace lexical_matcher
	{
		FrenchSentence::FrenchSentence()
		{}
		
		
		
		FrenchSentence::FrenchSentence(
			const std::string& source
		):	_source(source)
		{
			typedef tokenizer<char_separator<char> > tokenizer;
			char_separator<char> sep(" :,;.-_|/\�'");
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

			if(source == "10")
			{
				return "dix";
			}

			if(source == "11")
			{
				return "onze";
			}

			if(source == "12")
			{
				return "douze";
			}

			if(source == "13")
			{
				return "treize";
			}

			if(source == "14")
			{
				return "quatorze";
			}

			if(source == "15")
			{
				return "quinze";
			}

			if(source == "16")
			{
				return "seize";
			}

			if(source == "17")
			{
				return "disept";
			}

			if(source == "18")
			{
				return "disuit";
			}

			if(source == "19")
			{
				return "disneuf";
			}

			if(source == "20")
			{
				return "vin";
			}

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
				return "h�pital";
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
			
			if(	source == "facult�" ||
				source == "fac" ||
				source == "faculte"
			){
				return "universit�";
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
			typedef map<size_t, pair<size_t, double> > Relations;
			
			Relations othersToThis;
			Relations thisToOthers;
			
			// Storage of scores by words
			for(size_t i(0); i<_words.size(); ++i)
			{
				if(_words[i].getPhonetic().empty()) continue;

				double bestScore(0);
				
				for(size_t j(0); j<s._words.size(); ++j)
				{
					if(s._words[j].getPhonetic().empty()) continue;

					FrenchPhoneticString::LevenshteinDistance distance(
						_words[i].levenshtein(s._words[j])
					);
					double score(
						1 - static_cast<double>(distance) / static_cast<double>(distance > s._words[j].getPhonetic().size() ? distance : s._words[j].getPhonetic().size())
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


			// Average score
			double totalScores(0);
			for(size_t j(0); j<s._words.size(); ++j)
			{
				if(	othersToThis.find(j) != othersToThis.end()
				){
					totalScores += othersToThis[j].second;
				}
			}
			totalScores /= s._words.size();

			// Order
			size_t lastIndex(othersToThis.size());
			size_t penalties(0);
			BOOST_FOREACH(Relations::value_type s, othersToThis)
			{
				if(s.first < lastIndex) ++penalties;
				lastIndex = s.first;
			}
			if(penalties) totalScores /= penalties;

			ComparisonScore score;
			score.levenshtein = FrenchPhoneticString::Levenshtein(_source, s._source);
			score.phoneticScore = totalScores;
			return score;
		}

		bool FrenchSentence::startsWith(const FrenchSentence& s) const
		{
			if(s.size() > size()) return false;

			size_t iword(0);
			size_t jword(0);
			size_t ipos(0);
			size_t jpos(0);

			while(iword < _words.size() &&
				jword < s._words.size() &&
				ipos < _words[iword].getPhonetic().size() &&
				jpos < s._words[jword].getPhonetic().size()
			){
				if(_words[iword].getPhonetic().at(ipos) != s._words[jword].getPhonetic().at(jpos)) return false;

				++ipos;
				++jpos;
				if(ipos >= _words[iword].getPhonetic().size())
				{
					ipos = 0;
					++iword;
				}
				if(jpos >= s._words[jword].getPhonetic().size())
				{
					jpos = 0;
					++jword;
				}
			}
			return true;
		}



		size_t FrenchSentence::size() const
		{
			size_t size(0);
			BOOST_FOREACH(_Words::value_type word, _words)
			{
				size += word.getPhonetic().size();
			}
			return size;
		}

		bool FrenchSentence::operator<(const FrenchSentence& s) const
		{
			return _source < s._source;
		}
		
		bool FrenchSentence::operator==(const FrenchSentence& s) const
		{
			return _source == s._source;
		}



		std::string FrenchSentence::getPhoneticString() const
		{
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const FrenchPhoneticString& word, _words)
			{
				if(!first) s << " ";
				s << word.getPhoneticString();
				first = false;
			}
			return s.str();
		}

	}
}

