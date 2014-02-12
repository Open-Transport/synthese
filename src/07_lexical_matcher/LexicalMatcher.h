#ifndef SYNTHESE_LEXMATCHER_LEXICALMATCHER_H
#define SYNTHESE_LEXMATCHER_LEXICALMATCHER_H

#include "FrenchPhoneticString.h"
#include "FrenchSentence.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/case_conv.hpp>

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
					double minScore = 0
				) const;

				MatchResult	match(
					const std::string& fuzzyKey,
					double minScore,
					size_t maxNbValues
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
			double minScore
		) const {
			if(_map.empty ())
			{
				return typename LexicalMatcher<T>::MatchResult();
			}

			MatchResult result = match(fuzzyKey, minScore, 0);

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
			size_t maxNbValues
		) const {
			MatchResult result;
			FrenchSentence ppkey(fuzzyKey);

			// Iterate over all candidates
			BOOST_FOREACH(const typename Map::value_type& value, _map)
			{
				MatchHit hit;
				hit.score = value.first.compare(ppkey);

				if (hit.score.phoneticScore >= minScore)
				{
					hit.key = value.first;
					hit.value = value.second;
					double score = hit.score.phoneticScore;
					
					//Give bonus on Toulouse
					size_t found = hit.key.getSource().find("TOULOUSE");
					if (found != std::string::npos)
					{
						score += (1 - score) * score;
					}
					
					hit.score.phoneticScore = score;
					result.push_back(hit);
				}

				if (maxNbValues > 0 && result.size() == maxNbValues) break;
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
