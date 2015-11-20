#ifndef SYNTHESE_LEXMATCHER_LEXICALMATCHER_H
#define SYNTHESE_LEXMATCHER_LEXICALMATCHER_H

#include "FrenchPhoneticString.h"
#include "FrenchSentence.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>

#include <map>
#include <vector>

namespace synthese
{
	/** @defgroup m07 07 Lexical matcher
		@ingroup m0

		@{
	*/

	/** 07 Lexical matcher namespace.
	*/
	namespace lexical_matcher
	{

		/**
		*/
		template<class T>
		class LexicalMatcher
		{
		public:
			typedef T Content;
			typedef std::map<FrenchSentence, T> Map;

			struct MatchHit
			{
				FrenchSentence::ComparisonScore score;
				FrenchSentence key;
				T value;
			};

			typedef typename std::vector<MatchHit> MatchResult;

			struct MatchHitSort
			{
				int operator () (
					const MatchHit& op1,
					const MatchHit& op2
				) const {
					return
						op1.score.phoneticScore > op2.score.phoneticScore ||
						(op1.score.phoneticScore == op2.score.phoneticScore && op1.score.levenshtein < op2.score.levenshtein);
				}
			};

			Map _map;

		 public:

			LexicalMatcher() {}
			~LexicalMatcher() {}

			//! @name Getters/Setters
			//@{
				const Map& entries () const;
			//@}


			//! @name Query methods
			//@{
				size_t size () const;

				MatchHit bestMatch(
					const std::string& fuzzyKey
				) const;

				MatchResult	bestMatches(
					const std::string& fuzzyKey,
					size_t nbMatches,
					double minScore = 0,
					bool fuzzy = true
				) const;

				MatchResult	match(
					const std::string& fuzzyKey,
					double minScore,
					size_t maxNbValues,
					bool fuzzy = true // If true, use phonetic search, else use a simple text based search
				) const;


			//! @name Update methods
			//@{
				/** Removes and destroy all the values in map.
				*/
				void clear ();

				void add (const std::string& key, T value);
				void remove (const std::string& key);
			//@}
		};


	/** @} */


		template<class T>
		size_t LexicalMatcher<T>::size () const
		{
			return _map.size ();
		}



		template<class T>
		typename LexicalMatcher<T>::MatchHit LexicalMatcher<T>::bestMatch(
			const std::string& fuzzyKey
		) const {
			return bestMatches (fuzzyKey, 1).front ();
		}



		template<class T>
		typename LexicalMatcher<T>::MatchResult LexicalMatcher<T>::bestMatches(
			const std::string& fuzzyKey,
			size_t nbMatches,
			double minScore,
			bool fuzzy
		) const {
			if(_map.empty ())
			{
				return typename LexicalMatcher<T>::MatchResult();
			}

			MatchResult result = match(fuzzyKey, minScore, 0, fuzzy);

			if(nbMatches && result.size () > nbMatches)
			{
				// Truncate the result
				typename MatchResult::iterator it = result.begin ();
				std::advance (it, nbMatches);
				result.erase (it, result.end ());
			}
			return result;
		}



		template<class T>
		typename LexicalMatcher<T>::MatchResult LexicalMatcher<T>::match(
			const std::string& fuzzyKey,
			double minScore,
			size_t maxNbValues,
			bool fuzzy
		) const {
			MatchResult result;

			// Use a french phonetic matching
			if (fuzzy)
			{

				FrenchSentence ppkey(fuzzyKey);

				// Iterate over all candidates
				BOOST_FOREACH(const typename Map::value_type& value, _map)
				{
					MatchHit hit;
					hit.score = value.first.compare(ppkey);

					if (hit.score.phoneticScore >= minScore)
					{
						if(value.first.startsWith(ppkey))
						{
							hit.score.phoneticScore += ((1 - hit.score.phoneticScore) * hit.score.phoneticScore ) / 2;
						}
						else
						{
							hit.score.phoneticScore *= 0.9;
						}
						hit.key = value.first;
						hit.value = value.second;
						result.push_back(hit);
					}

					if (maxNbValues > 0 && result.size() == maxNbValues) break;
				}

			}
			else // Simple text based search
			{
				// The score is made from the number of characters of the search key mutliplied by a multiplier
				// In case where the searched key is at the start of the string, the multiplier is higher
				// The levenstein value is always at 0
				
				// Search are case-insensitive
				std::string key = fuzzyKey;
				boost::algorithm::to_lower(key);

				// Iterate over all candidates
				BOOST_FOREACH(const typename Map::value_type& value, _map)
				{
					MatchHit hit;
					hit.score.levenshtein = 0;
					double multiplier = 0.0;

					// Search are case-insensitive
					std::string name = value.first.getSource();
					boost::algorithm::to_lower(name);

					// Idea : use a loop, each time decreasing the size of the key from each side
					
					// Search for each word first letters, if found, a higher score is given
					std::vector<std::string> words;
					boost::algorithm::split(words, name, boost::algorithm::is_any_of(" -'/"));
					BOOST_FOREACH(const std::string word, words)
					{std::string val = word.substr(0,key.size());
						if ( val== key)
						{
							multiplier += 1.5;
						}
					}
					
					// Do a simple string search
					if (multiplier == 0.0 && std::search(name.begin(), name.end(), key.begin(), key.end()) != name.end())
					{
						multiplier += 1.0;
					}
					
					hit.score.phoneticScore = key.size() * multiplier;

					// Add in the result if good enough
					if (hit.score.phoneticScore >= minScore)
					{
						hit.key = value.first;
						hit.value = value.second;
						result.push_back(hit);
					}

					if (maxNbValues > 0 && result.size() == maxNbValues) break;
				}
			}

			// Sort the result by descending score.
			MatchHitSort hitSort;
			std::sort (result.begin(), result.end(), hitSort);

			return result;
		}



		template<class T>
		void LexicalMatcher<T>::clear ()
		{
			_map.clear ();
		}



		template<class T>
		void LexicalMatcher<T>::add (const std::string& key, T ptr)
		{
			if (key.empty()) return;
			_map.insert(std::make_pair(FrenchSentence(key), ptr));
		}



		template<class T>
		void LexicalMatcher<T>::remove (const std::string& key)
		{
			_map.erase(FrenchSentence(key));
		}



		template<class T>
		const typename LexicalMatcher<T>::Map& LexicalMatcher<T>::entries(
		) const {
			return _map;
		}
	}
}

#endif
