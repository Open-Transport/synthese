#include "FrenchPhoneticString.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <limits>
#include <sstream>

#undef max
#undef min

using namespace std;
using namespace boost;

namespace synthese
{
	namespace lexmatcher
	{
		FrenchPhoneticString::FrenchPhoneticString()
		{}
		
		
		
		FrenchPhoneticString::FrenchPhoneticString(const std::string& source)
		{
			setSource(source);
		}
		
		
		
		void FrenchPhoneticString::setSource(const std::string& ssource)
		{
			_source = ssource;
			
			string source(to_lower_copy(ssource));
			PhoneticString result;
			for(size_t pos(0); pos < source.size(); ++pos)
			{
				switch(source[pos])
				{
				case '0':
					result.push_back(Z);
					result.push_back(E);
					result.push_back(R);
					result.push_back(O);
					break;

				case '1':
					result.push_back(AN);
					break;

				case '2':
					result.push_back(D);
					result.push_back(E);
					break;

				case '3':
					result.push_back(T);
					result.push_back(R);
					result.push_back(O);
					result.push_back(A);
					break;

				case '4':
					result.push_back(K);
					result.push_back(A);
					result.push_back(T);
					result.push_back(R);
					break;

				case '5':
					result.push_back(S);
					result.push_back(AN);
					result.push_back(K);
					break;

				case '6':
					result.push_back(S);
					result.push_back(I);
					result.push_back(S);
					break;

				case '7':
					result.push_back(S);
					result.push_back(E);
					result.push_back(T);
					break;

				case '8':
					result.push_back(U);
					result.push_back(I);
					result.push_back(T);
					break;

				case '9':
					result.push_back(N);
					result.push_back(E);
					result.push_back(F);
					break;
					
				case 'a':
				case 'à':
				case 'ä':
				case 'â':
					if(	_IsFollowedBy(source, pos, "in") &&
						!_IsFollowedBy(source, pos, "ine")
					){
						pos += 2;
						result.push_back(AN);
						break;
					}

					if(	(	_IsFollowedBy(source, pos, "i") ||
							_IsFollowedBy(source, pos, "ï") ||
							_IsFollowedBy(source, pos, "ì") ||
							_IsFollowedBy(source, pos, "y") ||
							_IsFollowedBy(source, pos, "e")
						) && !_IsFollowedBy(source, pos, "il")
					){
						++pos;
						result.push_back(E);
						break;
					}
					
					if(	_IsFollowedBy(source, pos, "u"))
					{
						++pos;
						result.push_back(O);
						break;
					}

					if(	(	_IsFollowedBy(source, pos, "n") &&
							!_IsFollowedBy(source, pos, "na") &&
							!_IsFollowedBy(source, pos, "ne") &&
							!_IsFollowedBy(source, pos, "né") &&
							!_IsFollowedBy(source, pos, "nè") &&
							!_IsFollowedBy(source, pos, "në") &&
							!_IsFollowedBy(source, pos, "nê") &&
							!_IsFollowedBy(source, pos, "ni") &&
							!_IsFollowedBy(source, pos, "nì") &&
							!_IsFollowedBy(source, pos, "nï") &&
							!_IsFollowedBy(source, pos, "no") &&
							!_IsFollowedBy(source, pos, "nu") &&
							!_IsFollowedBy(source, pos, "ny") &&
							!_IsFollowedBy(source, pos, "nn")
						) ||
						_IsFollowedBy(source, pos, "mb") ||
						_IsFollowedBy(source, pos, "mp")
					){
						++pos;
						result.push_back(AN);
						break;
					}
					
					if(	_IsFollowedBy(source, pos, "en")
					){
						pos += 2;
						result.push_back(AN);
						break;
					}
	
					result.push_back(A);
					break;
					
				case 'b':
					result.push_back(B);
					break;
					
				case 'c':
					if(	_IsFollowedBy(source, pos, "h"))
					{
						++pos;
						result.push_back(CH);
						break;
					}
					
					if(	_IsFollowedBy(source, pos, "e") ||
						_IsFollowedBy(source, pos, "é") ||
						_IsFollowedBy(source, pos, "è") ||
						_IsFollowedBy(source, pos, "ë") ||
						_IsFollowedBy(source, pos, "ê") ||
						_IsFollowedBy(source, pos, "i") ||
						_IsFollowedBy(source, pos, "ï") ||
						_IsFollowedBy(source, pos, "î") ||
						_IsFollowedBy(source, pos, "ì") ||
						_IsFollowedBy(source, pos, "y")
					){
						result.push_back(S);
						break;
					}
					
					result.push_back(K);
					break;
					
				case 'ç':
					result.push_back(S);
					break;
					
				case 'd':
					if(	_IsLast(source, pos)
					){
						break;
					}
					
					result.push_back(D);
					break;
					
				case 'e':
					if(	_IsFollowedBy(source, pos, "s") &&
						_IsLast(source, pos, 2)
					){
						++pos;
						break;
					}
					
					if(	_IsLast(source, pos))
					{
						break;
					}
					
				case 'é':
				case 'è':
				case 'ë':
				case 'ê':
					if(	_IsFollowedBy(source, pos, "in") &&
						!_IsFollowedBy(source, pos, "ine")
					){
						pos += 2;
						result.push_back(AN);
						break;
					}

					if(	(	_IsFollowedBy(source, pos, "i") ||
							_IsFollowedBy(source, pos, "y") ||
							_IsFollowedBy(source, pos, "u") ||
							_IsFollowedBy(source, pos, "e")
						) && !_IsFollowedBy(source, pos, "il")
					){
						++pos;
						result.push_back(E);
						break;
					}
					
					if(	_IsFollowedBy(source, pos, "n") ||
						_IsFollowedBy(source, pos, "mp") ||
						_IsFollowedBy(source, pos, "mb")
					){
						++pos;
						result.push_back(AN);
						break;
					}

					if(	(	_IsFollowedBy(source, pos, "t") ||
							_IsFollowedBy(source, pos, "r")
						) &&
						_IsLast(source, pos, 2)
					){
						++pos;
						result.push_back(E);
						break;
					}

					if(	(	_IsFollowedBy(source, pos, "ts") ||
						_IsFollowedBy(source, pos, "rs")
						) &&
						_IsLast(source, pos, 3)
					){
						pos += 2;
						result.push_back(E);
						break;
					}

					result.push_back(E);
					break;
					
				case 'f':
					result.push_back(F);
					break;
					
				case 'g':
					if(	_IsFollowedBy(source, pos, "e") ||
						_IsFollowedBy(source, pos, "é") ||
						_IsFollowedBy(source, pos, "è") ||
						_IsFollowedBy(source, pos, "ë") ||
						_IsFollowedBy(source, pos, "i") ||
						_IsFollowedBy(source, pos, "ï") ||
						_IsFollowedBy(source, pos, "î") ||
						_IsFollowedBy(source, pos, "y")
					){
						result.push_back(J);
						break;
					}
					
					if(	_IsFollowedBy(source, pos, "u")
					){
						++pos;
						result.push_back(G);
						break;
					}
					
					if(	_IsFollowedBy(source, pos, "n")
					){
						++pos;
						result.push_back(N);
						result.push_back(I);
						break;
					}
	
					result.push_back(G);
					break;
					
				case 'h':
					break;
					
				case 'i':
				case 'ì':
				case 'î':
					if(	_IsFollowedBy(source, pos, "ng") &&
						_IsLast(source, pos, 3)
					){
						result.push_back(I);
						break;
					}
					
					if(	_IsFollowedBy(source, pos, "ll")
					){
						pos += 2;
						result.push_back(I);
						break;
					}
					
					if(	_IsFollowedBy(source, pos, "l")
					){
						++pos;
						result.push_back(I);
						break;
					}
	
					if(	(	_IsFollowedBy(source, pos, "n") &&
							!_IsFollowedBy(source, pos, "na") &&
							!_IsFollowedBy(source, pos, "ne") &&
							!_IsFollowedBy(source, pos, "né") &&
							!_IsFollowedBy(source, pos, "nè") &&
							!_IsFollowedBy(source, pos, "në") &&
							!_IsFollowedBy(source, pos, "nê") &&
							!_IsFollowedBy(source, pos, "ni") &&
							!_IsFollowedBy(source, pos, "nì") &&
							!_IsFollowedBy(source, pos, "nï") &&
							!_IsFollowedBy(source, pos, "nî") &&
							!_IsFollowedBy(source, pos, "no") &&
							!_IsFollowedBy(source, pos, "nu") &&
							!_IsFollowedBy(source, pos, "ny") &&
							!_IsFollowedBy(source, pos, "nn")
						) ||
						_IsFollowedBy(source, pos, "mb") ||
						_IsFollowedBy(source, pos, "mp")
					){
						++pos;
						result.push_back(AN);
						break;
					}
	
				case 'ï':
					result.push_back(I);
					break;
					
				case 'j':
					result.push_back(J);
					break;
					
				case 'k':
					result.push_back(K);
					break;
				
				case 'l':
					if(	_IsFollowedBy(source, pos, "l")
					){
						++pos;
						result.push_back(L);
						break;
					}
	
					result.push_back(L);
					break;
					
				case 'm':
					if(	_IsFollowedBy(source, pos, "m")
					){
						++pos;
						result.push_back(M);
						break;
					}
	
					result.push_back(M);
					break;
				
				case 'n':
					if(	_IsFollowedBy(source, pos, "n")
					){
						++pos;
						result.push_back(N);
						break;
					}
	
					result.push_back(N);
					break;
				
				case 'o':
				case 'ò':
				case 'ô':
					if(	_IsFollowedBy(source, pos, "n") ||
						_IsFollowedBy(source, pos, "mp") ||
						_IsFollowedBy(source, pos, "mb")
					){
						++pos;
						result.push_back(AN);
						break;
					}
	
					if(	_IsFollowedBy(source, pos, "u") ||
						_IsFollowedBy(source, pos, "ù")
					){
						++pos;
						result.push_back(U);
						break;
					}
	
					if(	_IsFollowedBy(source, pos, "i") ||
						_IsFollowedBy(source, pos, "ì") ||
						_IsFollowedBy(source, pos, "y")
					){
						++pos;
						result.push_back(O);
						result.push_back(A);
						break;
					}
					
				case 'ö':
					result.push_back(O);
					break;
	
				case 'p':
					if(	_IsLast(source, pos)
					){
						break;
					}
					
					if(	_IsFollowedBy(source, pos, "p")
					){
						++pos;
						result.push_back(P);
						break;
					}

					if(	_IsFollowedBy(source, pos, "t") &&
						_IsLast(source, pos, 2)
					){
						++pos;
						result.push_back(T);
						break;
					}

					result.push_back(P);
					break;
					
				case 'q':
					if(	_IsFollowedBy(source, pos, "u")
					){
						++pos;
						result.push_back(K);
						break;
					}
					
					result.push_back(K);
					break;
					
				case 'r':
					if(	_IsFollowedBy(source, pos, "r")
					){
						++pos;
						result.push_back(R);
						break;
					}
				
					result.push_back(R);
					break;

				case 's':
					if(	(	_IsFollowedBy(source, pos, "a") ||
							_IsFollowedBy(source, pos, "à") ||
							_IsFollowedBy(source, pos, "ä") ||
							_IsFollowedBy(source, pos, "e") ||
							_IsFollowedBy(source, pos, "é") ||
							_IsFollowedBy(source, pos, "è") ||
							_IsFollowedBy(source, pos, "ë") ||
							_IsFollowedBy(source, pos, "i") ||
							_IsFollowedBy(source, pos, "ì") ||
							_IsFollowedBy(source, pos, "ï") ||
							_IsFollowedBy(source, pos, "o") ||
							_IsFollowedBy(source, pos, "ò") ||
							_IsFollowedBy(source, pos, "ö") ||
							_IsFollowedBy(source, pos, "u") ||
							_IsFollowedBy(source, pos, "ù") ||
							_IsFollowedBy(source, pos, "ü") ||
							_IsFollowedBy(source, pos, "y")
					)	&&
						(	_IsPrecededBy(source, pos, "a") ||
							_IsPrecededBy(source, pos, "à") ||
							_IsPrecededBy(source, pos, "ä") ||
							_IsPrecededBy(source, pos, "e") ||
							_IsPrecededBy(source, pos, "é") ||
							_IsPrecededBy(source, pos, "è") ||
							_IsPrecededBy(source, pos, "ë") ||
							_IsPrecededBy(source, pos, "i") ||
							_IsPrecededBy(source, pos, "ì") ||
							_IsPrecededBy(source, pos, "ï") ||
							_IsPrecededBy(source, pos, "o") ||
							_IsPrecededBy(source, pos, "ò") ||
							_IsPrecededBy(source, pos, "ö") ||
							_IsPrecededBy(source, pos, "u") ||
							_IsPrecededBy(source, pos, "ù") ||
							_IsPrecededBy(source, pos, "ü") ||
							_IsPrecededBy(source, pos, "y")
					)	){
						result.push_back(Z);
						break;
					}
					
					if(	_IsFollowedBy(source, pos, "s")
					){
						++pos;
						result.push_back(S);
						break;
					}
					
					if(	_IsLast(source, pos))
					{
						break;
					}
					
					result.push_back(S);
					break;
				
				case 't':
					if(	_IsFollowedBy(source, pos, "t")
					){
						++pos;
						result.push_back(T);
						break;
					}
					
					if(	(	_IsFollowedBy(source, pos, "ien") ||
							_IsFollowedBy(source, pos, "ion")
					)	&&
						(	_IsPrecededBy(source, pos, "a") ||
							_IsPrecededBy(source, pos, "à") ||
							_IsPrecededBy(source, pos, "ä") ||
							_IsPrecededBy(source, pos, "e") ||
							_IsPrecededBy(source, pos, "é") ||
							_IsPrecededBy(source, pos, "è") ||
							_IsPrecededBy(source, pos, "ë") ||
							_IsPrecededBy(source, pos, "i") ||
							_IsPrecededBy(source, pos, "ì") ||
							_IsPrecededBy(source, pos, "ï") ||
							_IsPrecededBy(source, pos, "o") ||
							_IsPrecededBy(source, pos, "ù") ||
							_IsPrecededBy(source, pos, "ö") ||
							_IsPrecededBy(source, pos, "u") ||
							_IsPrecededBy(source, pos, "ù") ||
							_IsPrecededBy(source, pos, "ü") ||
							_IsPrecededBy(source, pos, "y")
					)	){
						result.push_back(S);
						break;
					}
				
					if(	_IsLast(source, pos))
					{
						break;
					}
					
					result.push_back(T);
					break;

				case 'u':
				case 'ù':
				case 'ü':
				case 'û':
					result.push_back(U);
					break;

				case 'v':
					if(	_IsFollowedBy(source, pos, "ille")
					){
						pos +=4;
						result.push_back(V);
						result.push_back(I);
						result.push_back(L);
						break;
					}

					result.push_back(V);
					break;
				
				case 'w':
					result.push_back(V);
					break;
				
				case 'x':
					if(	_IsLast(source, pos))
					{
						break;
					}
					
					result.push_back(K);
					result.push_back(S);
					break;
				
				case 'y':
					result.push_back(I);
					break;
				
				case 'z':
					result.push_back(Z);
					break;
				}
			}
			_phonetic = result;
		}
		
		
		
		const string& FrenchPhoneticString::getSource() const
		{
			return _source;
		}
		
		
		
		const FrenchPhoneticString::PhoneticString& FrenchPhoneticString::getPhonetic() const
		{
			return _phonetic;
		}
		
		
		
		bool FrenchPhoneticString::_IsLast(const std::string& source, size_t pos, size_t len)
		{
			return pos + len == source.size();
		}
		
		bool FrenchPhoneticString::_IsFollowedBy(const std::string& source, size_t pos, const std::string& text)
		{
			if(pos + text.size() + 1 > source.size()) return false;
			try
			{
				return source.substr(pos + 1, text.size()) == text;
			}
			catch(...)
			{
				return false;
			}
		}
		
		bool FrenchPhoneticString::_IsPrecededBy(const std::string& source, size_t pos, const std::string& text)
		{
			if(text.size() > pos) return false;
			try
			{
				return source.substr(pos - text.size(), text.size()) == text;
			}
			catch(...)
			{
				return false;
			}
		}
		
		
		FrenchPhoneticString::LevenshteinDistance FrenchPhoneticString::levenshtein(
			const FrenchPhoneticString& s
		) const {
			if (s._phonetic.size () > 256 || _phonetic.size() > 256) return numeric_limits<LevenshteinDistance>::max ();

			// Levenshtein Word Distance matrix.
			// Note that the dims are bounded to 256. It means that it is 
			// forbidden to compare words larger than 256 characters each!
			LevenshteinDistance matrix[256][256];

			LevenshteinDistance n = s._phonetic.size();
			LevenshteinDistance m = _phonetic.size();

			if (n == 0) return m;
			if (m == 0) return n;


			for(LevenshteinDistance i = 0; i <= n; matrix[i][0] = i++) ;
			for(LevenshteinDistance j = 1; j <= m; matrix[0][j] = j++) ;
			
			for (LevenshteinDistance i = 1; i <= n; i++ ) 
			{
				char sc = s._phonetic[i-1];
				for (LevenshteinDistance j = 1; j <= m;j++) 
				{
					LevenshteinDistance v = matrix[i-1][j-1];
					if ( _phonetic[j-1] !=  sc ) v++;
					matrix[i][j] = min(min(matrix[i-1][j] + 1, matrix[i][j-1] + 1 ), v );
				}
			}
			return matrix[n][m];
		}



		string FrenchPhoneticString::getPhoneticString() const
		{
			stringstream s;
			bool first(true);
			BOOST_FOREACH(Phoneme p, _phonetic)
			{
				if(!first) s << "-";
				switch(p)
				{
				case A: s << "A"; break;
				case AN: s << "AN"; break;
				case B: s << "B"; break;
				case CH: s << "CH"; break;
				case D: s << "D"; break;
				case E: s << "E"; break;
				case F: s << "F"; break;
				case G: s << "G"; break;
				case I: s << "I"; break;
				case J: s << "J"; break;
				case K: s << "K"; break;
				case L: s << "L"; break;
				case M: s << "M"; break;
				case N: s << "N"; break;
				case O: s << "O"; break;
				case P: s << "P"; break;
				case R: s << "R"; break;
				case S: s << "S"; break;
				case T: s << "T"; break;
				case U: s << "U"; break;
				case V: s << "V"; break;
				case Z: s << "Z"; break;
				default: s << "*"; break;
				}
				first = false;
			}
			return s.str();
		}
	}
}