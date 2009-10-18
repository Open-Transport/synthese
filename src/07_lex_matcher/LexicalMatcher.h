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
	namespace lexmatcher
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
					return op1.score > op2.score;
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
					size_t nbMatches
				) const;
				
				MatchResult	match(
					const std::string& fuzzyKey,
					double minScore,
					size_t maxNbValues
				) const;

				MatchResult findBeginsBy(
					const std::string& text,
					size_t nbMatches
				) const;
				
				MatchResult findCombined(
					const std::string& text,
					size_t nbMatches
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
			return bestMatches (fuzzyKey).front ();
		}



		template<class T>
		typename LexicalMatcher<T>::MatchResult LexicalMatcher<T>::bestMatches(
			const std::string& fuzzyKey,
			size_t nbMatches
		) const {
			if(_map.empty ())
			{
				return typename LexicalMatcher<T>::MatchResult();
			}
			
			MatchResult result = match(fuzzyKey, 0.0, 0);
			
			if (result.size () > nbMatches)
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
				hit.score = ppkey.compare(value.first);
				if (hit.score >= minScore)
				{
					hit.key = value.first;
					hit.value = value.second;
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
		typename LexicalMatcher<T>::MatchResult LexicalMatcher<T>::findBeginsBy(
			const std::string& text,
			size_t nbMatches
		) const {
			const std::string lowerCaseText(boost::algorithm::to_lower_copy(text));
			MatchResult r;
			BOOST_FOREACH(typename Map::value_type v, _map)
			{
				if(v.first.getSource() == lowerCaseText)
				{
					MatchHit h;
					h.value = v.second;
					h.key = v.first;
					r.push_back(h);
					if(r.size() == nbMatches) break;
				}
			}
			BOOST_FOREACH(typename Map::value_type v, _map)
			{
				if(	v.first.getSource() != lowerCaseText &&
					boost::algorithm::istarts_with(v.first.getSource(), lowerCaseText)
				){
					MatchHit h;
					h.value = v.second;
					h.key = v.first;
					r.push_back(h);
					if(r.size() == nbMatches) break;
				}
			}
			return r;
		}
		
		
		template<class T>
		typename LexicalMatcher<T>::MatchResult LexicalMatcher<T>::findCombined(
			const std::string& text,
			size_t nbMatches
		) const {
			MatchResult r1(findBeginsBy(text, nbMatches));
			if(r1.size() < nbMatches)
			{
				MatchResult r2(bestMatches(text, nbMatches));
				BOOST_FOREACH(const MatchHit& h2, r2)
				{
					bool ok(true);
					BOOST_FOREACH(const MatchHit& h1, r1)
					{
						if(h1.key == h2.key)
						{
							ok = false;
							break;
						}
					}
					if(ok)
					{
						r1.push_back(h2);
					}
					if(r1.size() == nbMatches) break;
				}
			}
			return r1;
		}



		template<class T>
		void 
		LexicalMatcher<T>::clear ()
		{
			_map.clear ();
		}



		template<class T>
		void 
		LexicalMatcher<T>::add (const std::string& key, T ptr)
		{
			if (key.empty()) return;
			_map.insert(std::make_pair(FrenchSentence(key), ptr));
		}



		template<class T>
		void 
		LexicalMatcher<T>::remove (const std::string& key)
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
