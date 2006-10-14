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



namespace synthese
{
namespace lexmatcher
{


/** Generic map container on string keys which can return a set of pairs <value, score>.
    The score is equal to 1 if and only if it exists an identical key in the container.

@ingroup m01
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
	} PreprocessedKey;


    typedef std::map<std::string, std::string> TranslationMap;

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
		    const TranslationMap& translations = TranslationMap (),
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
    int computeLWD (const std::string& s, const std::string& t) const;
    std::pair<double, double> computeDistanceAndScore (const std::string& s1, const std::string& s2) const;
    double computeScore (const PreprocessedKey& key, const PreprocessedKey& candidate) const;


};


template<class T>
const double LexicalMatcher<T>::EXTRA_INPUT_WORD_PENALTY_FACTOR (1.0);

template<class T>
const double LexicalMatcher<T>::EXTRA_MATCH_WORD_PENALTY_FACTOR (0.7);


template<class T>
const double LexicalMatcher<T>::LWD_SCORE_WEIGHT (0.5);

template<class T>
const double LexicalMatcher<T>::WWM_SCORE_WEIGHT (0.5);

template<class T>
const double LexicalMatcher<T>::WWM_SCORE_THRESHOLD (0.75);


template<class T>
LexicalMatcher<T>::LexicalMatcher (bool ignoreCase,
				   bool ignoreWordOrder,
				   bool ignoreWordSpacing,
				   const TranslationMap& translations,
				   const std::string& separatorCharacters)
    : _ignoreCase (ignoreCase)
    , _ignoreWordOrder (ignoreWordOrder)
    , _ignoreWordSpacing (ignoreWordSpacing)
    , _translations (translations)
    , _separatorCharacters (separatorCharacters)
{
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

    // Do some filtering...
    if (_ignoreCase)
    {
	boost::algorithm::to_lower (tmpkey); 
    }

    // Apply translations
    for (std::map<std::string, std::string>::const_iterator it = _translations.begin ();
	 it != _translations.end (); ++it)
    {
	std::string search = _ignoreCase ? boost::algorithm::to_lower_copy (it->first) : it->first;
	boost::algorithm::trim (search);
	std::string replace = _ignoreCase ? boost::algorithm::to_lower_copy (it->second) : it->second;
	boost::algorithm::trim (replace);
	boost::algorithm::replace_all (tmpkey, search, replace);
    }

    // Trim the string
    boost::algorithm::trim (tmpkey);

    // Split the string
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep (_separatorCharacters.c_str ());
    tokenizer keyTokens (tmpkey, sep);
    
    for (tokenizer::iterator tok_iter = keyTokens.begin(); 
	 tok_iter != keyTokens.end (); 
	 ++tok_iter) 
    {
	ppkey.tokens.push_back (*tok_iter);
	ppkey.oneWord.append (*tok_iter);
	ppkey.size += tok_iter->size ();
    }
    return ppkey;
 
}




template<class T>
std::pair<double, double>
LexicalMatcher<T>::computeDistanceAndScore (const std::string& s1, const std::string& s2) const
{
    double maxLength = (double) std::max (s1.size (), s2.size ());
    double lwd = computeLWD (s1, s2);
    return std::make_pair (lwd, (maxLength - lwd) / maxLength);
}






template<class T>
double 
LexicalMatcher<T>::computeScore (const PreprocessedKey& key, const PreprocessedKey& candidate) const
{
    double maxLength = (double) std::max (key.size, candidate.size);

    double sumLWD = 0.0;
    int nbWWM = 0;

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
		std::pair<double, double> distanceAndScore = computeDistanceAndScore (*it1, *it2);
		sumLWD += distanceAndScore.first;
		if (distanceAndScore.second >= WWM_SCORE_THRESHOLD)
		{
		    ++nbWWM;
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
		std::pair<double, double> distanceAndScore = computeDistanceAndScore (*key_iter, *it);

		if (distanceAndScore.first < minLWD) 
		{
		    minLWD = distanceAndScore.first;
		    bestScore = distanceAndScore.second;
		    bestit = it;
		    if (minLWD == 0.0) break;
		}
		else
		{
		    ++it;
		}
	    }
	    canWords.erase (bestit);

	    if (bestScore >= WWM_SCORE_THRESHOLD)
	    {
		++nbWWM;
	    }

	    sumLWD += minLWD;
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

    if (lwdScore < 0) lwdScore = 0;

    
    double wwmScore = ((double) nbWWM) / candidate.tokens.size ();

    // Given penalty heuristics, it can happen that the total distance is superior
    // to total length... In such a case this is really not a good match : 
    // if (sumLWD > maxLength) return 0.0;


    return LWD_SCORE_WEIGHT * lwdScore + WWM_SCORE_WEIGHT * wwmScore;
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

