#ifndef SYNTHESE_LEXMATCHER_LEXICALMATCHER_H
#define SYNTHESE_LEXMATCHER_LEXICALMATCHER_H


#include <iostream>


#include <map>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <limits>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>

#include "01_util/Exception.h"

#undef max
#undef min

namespace synthese
{
	/** @defgroup m07 07 Lexical matcher
		@ingroup m0

		@todo Move this module into the 01 util as sub-module

		@{
	*/

	/** 07 Lexical matcher namespace.
	*/
	namespace lexmatcher
	{

		typedef struct 
		{
			std::map<std::string, std::string> globalTranslations; 
			std::map<std::string, std::string> wordTranslations; 
		} TranslationMap;


		TranslationMap FrenchTranslationMap ();



		/** Generic map container on string keys which can return a set of pairs <value, score>.
			The score is equal to 1 if and only if it exists an identical key in the container.
		*/
		template<class T>
		class LexicalMatcher
		{
		 public:

			typedef 
			struct 
			{
				double score;
				std::string key;
				T value;
			} MatchHit;
		    
			typedef typename std::vector<MatchHit> MatchResult;

		 private:

			static const double EXTRA_INPUT_WORD_PENALTY_FACTOR;
			static const double EXTRA_MATCH_WORD_PENALTY_FACTOR;

			static const double LWD_SCORE_WEIGHT; // Word distance score weight
			static const double WWM_SCORE_WEIGHT; // Whole word matched score weight

			static const int WWM_LWD_THRESHOLD[30]; // Max LWD in a word to consider it is a WWM
			static const int WWM_BONUS[30]; // WWM bonus according to number of letters

			static const double WWM_SCORE_THRESHOLD; // score threshold to consider that the whole word is matched


			typedef 
			struct
			{
				int operator () ( const MatchHit& op1, 
						  const MatchHit& op2 ) const
				{
					return op1.score > op2.score;
				}
			} MatchHitSort;
		    
			typedef 
			struct 
			{
				std::vector<std::string> tokens;
				size_t size;
				std::string oneWord;
				int maxWwmBonus;
			} PreprocessedKey;



			std::map<std::string, T> _map;
			std::map<std::string, PreprocessedKey > _ppKeys;

			bool _ignoreCase;
			bool _ignorePunctuation;
			bool _ignoreWordOrder;
			bool _ignoreWordSpacing;

			TranslationMap _translations;
			std::string _separatorCharacters;

		 public:

			LexicalMatcher (bool ignoreCase=true,
					bool ignoreWordOrder=true,
					bool ignoreWordSpacing = true,
					const TranslationMap& translations = FrenchTranslationMap (),
					const std::string& separatorCharacters = "-,;.' &()");

			~LexicalMatcher ();

			//! @name Getters/Setters
			//@{
			const std::map<std::string, T >& entries () const;
			//@}


			//! @name Query methods
			//@{
			size_t size () const;

			MatchHit bestMatch (const std::string& fuzzyKey) const;
			MatchResult	bestMatches (const std::string& fuzzyKey, int nbMatches = 10) const;
			MatchResult	match (const std::string& fuzzyKey, double minScore = 0.0, int maxNbValues = -1) const;


		    
			//! @name Update methods
			//@{

			/** Removes and destroy all the values in map.
			 */
			void clear ();

			void add (const std::string& key, T value);
			void remove (const std::string& key);

			//@}
		    

		 private:
		    
			PreprocessedKey preprocessKey (const std::string& key) const;
			int getWwmLwdThreshold (int nbLetters) const;
			int getWwmBonus (int wordLwd, int nbLetters) const;
			int computeLWD (const std::string& s, const std::string& t) const;
			double computeScore (const PreprocessedKey& key, const PreprocessedKey& candidate) const;


		};

		 
		 
		 
	/** @} */		 


		template<class T>
		const double LexicalMatcher<T>::EXTRA_INPUT_WORD_PENALTY_FACTOR (1.0);

		template<class T>
		const double LexicalMatcher<T>::EXTRA_MATCH_WORD_PENALTY_FACTOR (0.4);


		template<class T>
		const double LexicalMatcher<T>::LWD_SCORE_WEIGHT (0.4);

		template<class T>
		const double LexicalMatcher<T>::WWM_SCORE_WEIGHT (0.6);


		template<class T>
		const int LexicalMatcher<T>::WWM_LWD_THRESHOLD[30] = {0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7 };

		template<class T>
		const int LexicalMatcher<T>::WWM_BONUS[30] = {0, 2, 4, 6, 6, 10, 10, 15, 15, 22, 30, 40, 40, 52, 64, 78, 92, 108, 124, 142, 160, 180, 200, 222, 244, 268, 292, 318, 344, 380 };



		template<class T>
		LexicalMatcher<T>::LexicalMatcher (bool ignoreCase,
						   bool ignoreWordOrder,
						   bool ignoreWordSpacing,
						   const TranslationMap& translations,
						   const std::string& separatorCharacters)
			: _ignoreCase (ignoreCase)
			, _ignoreWordOrder (ignoreWordOrder)
			, _ignoreWordSpacing (ignoreWordSpacing)
			, _translations ()
			, _separatorCharacters (separatorCharacters)
		{

		    for (std::map<std::string, std::string>::iterator it = _translations.globalTranslations.begin ();
			 it != _translations.globalTranslations.end (); ++it)
		    {
			std::string search (it->first);
			std::string replace (it->second);

			boost::algorithm::trim (search);
			boost::algorithm::trim (replace);
			if (_ignoreCase)
			{
			    boost::algorithm::to_lower (search);
			    boost::algorithm::to_lower (replace);
			}
			_translations.globalTranslations.insert (std::make_pair(search, replace));
		    }
		    
		    for (std::map<std::string, std::string>::iterator it = _translations.wordTranslations.begin ();
			 it != _translations.wordTranslations.end (); ++it)
		    {
			std::string search (it->first);
			std::string replace (it->second);

			boost::algorithm::trim (search);
			boost::algorithm::trim (replace);
			if (_ignoreCase)
			{
			    boost::algorithm::to_lower (search);
			    boost::algorithm::to_lower (replace);
			}
			_translations.wordTranslations.insert (std::make_pair(search, replace));
		    }
		    
		    
		    
		}



		template<class T>
		LexicalMatcher<T>::~LexicalMatcher ()
		{
		}






		template<class T>
		size_t 
		LexicalMatcher<T>::size () const
		{
			return _map.size ();
		}



		template<class T>
		typename LexicalMatcher<T>::MatchHit 
		LexicalMatcher<T>::bestMatch (const std::string& fuzzyKey) const
		{
			return bestMatches (fuzzyKey).front ();
		}


		template<class T>
		typename LexicalMatcher<T>::MatchResult
		LexicalMatcher<T>::bestMatches (const std::string& fuzzyKey, int nbMatches) const
		{
			if (_map.empty ()) throw synthese::util::Exception ("No match possible (lexical matcher has no entry).");
			MatchResult result = match (fuzzyKey, 0.0);
		    
			if (((int) result.size ()) > nbMatches)
			{
			// Truncate the result
			typename MatchResult::iterator it = result.begin ();
			std::advance (it, nbMatches);
			result.erase (it, result.end ());
			}
			return result;
		}




		template<class T>
		typename LexicalMatcher<T>::MatchResult
		LexicalMatcher<T>::match (const std::string& fuzzyKey, double minScore, int maxNbValues) const
		{
			MatchResult result;
			PreprocessedKey ppkey = preprocessKey (fuzzyKey);
			// Iterate over all candidates
			for (typename std::map<std::string, PreprocessedKey >::const_iterator it = _ppKeys.begin ();
			 it != _ppKeys.end (); ++it)
			{
			MatchHit hit;
			hit.score = computeScore (ppkey, it->second);
			if (hit.score >= minScore) 
			{
				hit.key = it->first;
				hit.value = _map.find (hit.key)->second;
				result.push_back (hit);
			}

			if ((maxNbValues >= 0) && (result.size () == maxNbValues)) break;
			}

			// Sort the result by descending score.
			MatchHitSort hitSort;
			std::sort (result.begin (), result.end (), hitSort);

			return result;

		}





		template<class T>
		typename LexicalMatcher<T>::PreprocessedKey
		LexicalMatcher<T>::preprocessKey (const std::string& key) const
		{
			std::string tmpkey (key);
			PreprocessedKey ppkey;
			ppkey.size = 0;
			ppkey.maxWwmBonus = 0;

			// Do some filtering...
			if (_ignoreCase)
			{
			boost::algorithm::to_lower (tmpkey); 
			}

			// Apply global translations
			for (std::map<std::string, std::string>::const_iterator it = _translations.globalTranslations.begin ();
			 it != _translations.globalTranslations.end (); ++it)
			{
			    boost::algorithm::replace_all (tmpkey, it->first, it->second);
			}

			// Trim the string
			boost::algorithm::trim (tmpkey);

			// Tokenize the string
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep (_separatorCharacters.c_str ());
			tokenizer keyTokens (tmpkey, sep);
		    
			for (tokenizer::iterator tok_iter = keyTokens.begin(); 
			 tok_iter != keyTokens.end (); 
			 ++tok_iter) 
			{
			    std::string tok (*tok_iter);
			    
			    // Apply word translations
			    for (std::map<std::string, std::string>::const_iterator it = _translations.wordTranslations.begin ();
				 it != _translations.wordTranslations.end (); ++it)
			    {
				if (tok == it->first) tok = it->second;
			    }
			    ppkey.tokens.push_back (tok);
			    ppkey.oneWord.append (tok);
			    ppkey.size += tok.size ();
			    ppkey.maxWwmBonus += getWwmBonus (0, tok.size ());

			}

			return ppkey;
		 
		}





		template<class T>
		int
		LexicalMatcher<T>::getWwmLwdThreshold (int nbLetters) const
		{
			if (nbLetters > 30) return WWM_LWD_THRESHOLD[29];
			return WWM_LWD_THRESHOLD[nbLetters];
		}



		template<class T>
		int
		LexicalMatcher<T>::getWwmBonus (int wordLwd, int nbLetters) const
		{
			if (nbLetters > 30) return WWM_BONUS[29];
			double weight = ((double) (nbLetters - wordLwd)) / ((double) nbLetters);
			// an exact match give maximal bonus. approximate match decrease bonus very quickly (^2).
			return (int) (WWM_BONUS[nbLetters] * weight * weight);
		}







		template<class T>
		double 
		LexicalMatcher<T>::computeScore (const PreprocessedKey& key, const PreprocessedKey& candidate) const
		{
			double maxLength = (double) std::max (key.size, candidate.size);

			double sumLWD = 0.0;
			int sumWWM = 0;

			if (_ignoreWordOrder == false)
			{
			std::vector<std::string>::const_iterator it1 = key.tokens.begin ();
			std::vector<std::string>::const_iterator it2 = candidate.tokens.begin ();
			while (it1 != key.tokens.end ())
			{
				if (it2 == candidate.tokens.end ()) 
				{
				sumLWD += (int) it1->size ();
				} 
				else
				{
				int wordLWD = computeLWD (*it1, *it2);
				sumLWD += wordLWD;
				if (wordLWD <= getWwmLwdThreshold (it2->size ()))
				{
					// Add the number of letters of the word
					// because a small WWM is less important than a big WWM.
					sumWWM += getWwmBonus (wordLWD, it2->size ()); 
				}
				++it2;
				}
				++it1;
			}
			}
			else
			{

			std::list<std::string> canWords (candidate.tokens.size ());
			std::copy (candidate.tokens.begin (), candidate.tokens.end (), canWords.begin ());

			// Heuristic : remove each time the best matching word of candidate
			// until no more word to match in key. Ignore too short words on both sides.
			std::vector<std::string>::const_iterator key_iter = key.tokens.begin ();
			while (key_iter != key.tokens.end ())
			{
				if (canWords.empty ()) 
				{
				// Extra words in key. Normal penalty.
				sumLWD += EXTRA_INPUT_WORD_PENALTY_FACTOR * ((double) key_iter->size ());
				++key_iter;
				continue;
				}
			
				// Compute minimal LWD
				double minLWD = std::numeric_limits<double>::max ();
				double bestScore = 0.0;

				std::list<std::string>::iterator it = canWords.begin ();
				std::list<std::string>::iterator bestit = canWords.end ();
				while (it != canWords.end ())
				{
				double lwd = computeLWD (*key_iter, *it);
				if (lwd < minLWD) 
				{
					minLWD = lwd;
					bestit = it;
					if (minLWD == 0.0) break;
				}
				else
				{
					++it;
				}
				}
				if (minLWD <= getWwmLwdThreshold (bestit->size ()))
				{
				// Add the number of letters of the word
				// because a small WWM is less important than a big WWM.
				sumWWM += getWwmBonus ((int) minLWD, bestit->size ()); 
				}
				sumLWD += minLWD;

				canWords.erase (bestit);

				++key_iter;
			}

			for (std::list<std::string>::iterator it = canWords.begin ();
				 it != canWords.end (); ++it) 
			{
				// Extra words in candidates. Smaller penalty. It is obvious that most
				// of the time, the input will contain fewer words than then best match.
				sumLWD += EXTRA_MATCH_WORD_PENALTY_FACTOR * ((double) it->size ());
			}

			}

			if (_ignoreWordSpacing)
			{
			// Do an extra test considering the one-word ppkey (as if there was only one token)
			// to cover bad word spacing...
			double oneWordLWD = computeLWD (key.oneWord, candidate.oneWord);
			
			// ... and keep it if better.
			if (oneWordLWD < sumLWD) sumLWD = oneWordLWD;
			}

			double lwdScore = (maxLength - sumLWD) / maxLength;

			// Given penalty heuristics, it can happen that the total distance is superior
			// to total length... In such a case this is really not a good match : 
			if (lwdScore < 0) lwdScore = 0;

		    
			double wwmScore = ((double) sumWWM) / candidate.maxWwmBonus;


		//    std::cerr << candidate.oneWord <<  "   " << "lwdScore=" << lwdScore << "  wwmScore=" << wwmScore << std::endl;

			if (wwmScore > lwdScore) 
			{
			// Whole word match can only bring a bonus.
			return LWD_SCORE_WEIGHT * lwdScore + WWM_SCORE_WEIGHT * wwmScore;
			} 
			return lwdScore;

		}






		template<class T>
		int 
		LexicalMatcher<T>::computeLWD (const std::string& s, const std::string& t) const
		{
			if ((s.size () > 256) || (t.size () > 256)) return std::numeric_limits<int>::max ();

			// Levenshtein Word Distance matrix.
			// Note that the dims are bounded to 256. It means that it is 
			// forbidden to compare words larger than 256 characters each!
			int matrix[256][256];

			int n = (int) s.length();
			int m = (int) t.length();

			if (n == 0) return m;
			if (m == 0) return n;


			for ( int i = 0; i <= n; matrix[i][0] = i++ );
			for ( int j = 1; j <= m; matrix[0][j] = j++ );
		    
			for ( int i = 1; i <= n; i++ ) 
			{
				char sc = s[i-1];
				for (int j = 1; j <= m;j++) 
				{
					int v = matrix[i-1][j-1];
					if ( t[j-1] !=  sc ) v++;
					matrix[i][j] = std::min( std::min( matrix[i-1][ j] + 1, matrix[i][j-1] + 1 ), v );
				}
			}
			return matrix[n][m];
		}






		template<class T>
		void 
		LexicalMatcher<T>::clear ()
		{
			_map.clear ();
			_ppKeys.clear ();
		}





		template<class T>
		void 
		LexicalMatcher<T>::add (const std::string& key, T ptr)
		{
			if (key.empty()) return;
			_map.insert (std::make_pair (key, ptr));
			_ppKeys.insert (std::make_pair (key, preprocessKey (key)));
		}





		template<class T>
		void 
		LexicalMatcher<T>::remove (const std::string& key)
		{
			_map.erase (key);
			_ppKeys.erase (key);
		}



		template<class T>
		const std::map<std::string, T>&
		LexicalMatcher<T>::entries () const
		{
			return _map;
		}





	}
}

#endif
