#include "FrenchPhoneticString.h"
#include "IConv.hpp"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	
	namespace lexical_matcher
	{
		IConv FrenchPhoneticString::ICONV("UTF-8","CP850");
		std::string FrenchPhoneticString::ACCENTUATED_A(IConv::IConv("UTF-8","CP850").convert("àäáâÀÄÁÂ"));
		std::string FrenchPhoneticString::ACCENTUATED_E(IConv::IConv("UTF-8","CP850").convert("èëéêÈËÉÊ"));
		std::string FrenchPhoneticString::ACCENTUATED_I(IConv::IConv("UTF-8","CP850").convert("ìíîÌÍÎ"));
		std::string FrenchPhoneticString::ACCENTUATED_O(IConv::IConv("UTF-8","CP850").convert("òóôÒÓÔ"));
		std::string FrenchPhoneticString::ACCENTUATED_U(IConv::IConv("UTF-8","CP850").convert("ùúûÙÚÛ"));
		std::string FrenchPhoneticString::VOWELS(IConv::IConv("UTF-8","CP850").convert("aàäáâeèëéêiìïíîoòöóôuùüúûy"));
		std::string FrenchPhoneticString::C_VOWELS(IConv::IConv("UTF-8","CP850").convert("aàäáâeèëéêiìïíîy"));

		mutex FrenchPhoneticString::_IConvMutex;

		FrenchPhoneticString::FrenchPhoneticString()
		{}



		FrenchPhoneticString::FrenchPhoneticString(const std::string& source)
		{
			setSource(source);
		}



		std::string FrenchPhoneticString::to_plain_lower_copy(const std::string& text)
		{
			mutex::scoped_lock lock(_IConvMutex);

			string source(to_lower_copy(ICONV.convert(text)));
			stringstream result;
			for(size_t pos(0); pos < source.size(); ++pos)
			{
				if(_IsAnyOf(source, pos, ACCENTUATED_A)) result << "a";
				else if(_IsAnyOf(source, pos, ACCENTUATED_E)) result << "e";
				else if(_IsAnyOf(source, pos, ACCENTUATED_I)) result << "i";
				else if(_IsAnyOf(source, pos, ACCENTUATED_O)) result << "o";
				else if(_IsAnyOf(source, pos, ACCENTUATED_U)) result << "u";
				else result << source[pos];
			}
			return result.str();
		}




		void FrenchPhoneticString::setSource(const std::string& ssource)
		{
			mutex::scoped_lock lock(_IConvMutex);

			_source = ssource;

			string source(to_lower_copy(ICONV.convert(ssource)));
			PhoneticString result;
			for(size_t pos(0); pos < source.size(); ++pos)
			{
				switch(static_cast<unsigned char>(source[pos]))
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
				case 131:
				case 132:
				case 133:
				case 134:
				case 142:
				case 143:
				case 181:
				case 182:
				case 183:
					if(	_IsFollowedBy(source, pos, "in") &&
						!_IsFollowedBy(source, pos, "ine")
					){
						pos += 2;
						result.push_back(AN);
						break;
					}

					if( source[pos+1] == 139) // ï
					{
						pos += 1;
						result.push_back(A);
						result.push_back(I);
						break;
					}

					if(_IsFollowedBy(source, pos, "ill"))
					{
						pos += 3;
						result.push_back(A);
						result.push_back(I);
						break;
					}

					if(	_IsFollowedBy(source, pos, "il") && _IsLast(source, pos, 3)
					){
						pos += 2;
						result.push_back(A);
						result.push_back(I);
						break;
					}

					if(	_IsAnyOf(source, pos+1, ACCENTUATED_I) ||
						_IsFollowedBy(source, pos, "i") ||
						_IsFollowedBy(source, pos, "y") ||
						_IsFollowedBy(source, pos, "e")
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

					if(	(	_IsFollowedBy(source, pos, "n") && !_IsAnyOf(source, pos+2, VOWELS)) ||
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

					if(	_IsAnyOf(source, pos+1, C_VOWELS)
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
					if(	_IsFollowedBy(source, pos, "au")
					){
						result.push_back(O);
						pos += 2;
						break;
					}
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

					if(	_IsFollowedBy(source, pos, "in") &&
						!_IsFollowedBy(source, pos, "ine")
					){
						pos += 2;
						result.push_back(AN);
						break;
					}

				case 130:
				case 136:
				case 137:
				case 138:
				case 144:
				case 210:
				case 211:
				case 212:
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

					if(	_IsFollowedBy(source, pos, "n") && !_IsAnyOf(source, pos+2, VOWELS) ||
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
						_IsAnyOf(source, pos+1, ACCENTUATED_E) ||
						_IsFollowedBy(source, pos, "i") ||
						_IsAnyOf(source, pos+1, ACCENTUATED_I) ||
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
				case 140:
				case 141:
				case 161:
				case 214:
				case 215:
				case 222:
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
							!_IsAnyOf(source, pos+2, VOWELS)
						) ||
						_IsFollowedBy(source, pos, "mb") ||
						_IsFollowedBy(source, pos, "mp")
					){
						++pos;
						result.push_back(AN);
						break;
					}

				case 139: // ï
				case 216: // Ï
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
				case 147:
				case 149:
				case 224:
				case 226: // Ô
				case 227:
					if(	_IsFollowedBy(source, pos, "n") && !_IsAnyOf(source, pos+2, VOWELS) ||
						_IsFollowedBy(source, pos, "mp") ||
						_IsFollowedBy(source, pos, "mb")
					){
						++pos;
						result.push_back(AN);
						break;
					}

					if(	_IsFollowedBy(source, pos, "u") ||
						_IsAnyOf(source, pos+1, ACCENTUATED_U)
					){
						++pos;
						result.push_back(U);
						break;
					}

					if(	_IsFollowedBy(source, pos, "i") ||
						_IsAnyOf(source, pos+1, ACCENTUATED_I) ||
						_IsFollowedBy(source, pos, "y")
					){
						++pos;
						result.push_back(O);
						result.push_back(A);
						break;
					}

				case 148: // ö
				case 153:
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
					if(	_IsAnyOf(source, pos+1, VOWELS)	&&
						_IsAnyOf(source, pos-1, VOWELS)
					){
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
					)	&& _IsAnyOf(source, pos-1, VOWELS)
					){
						result.push_back(S);
						break;
					}

					if(	_IsLast(source, pos) ||
						_IsFollowedBy(source, pos, "b") ||
						_IsFollowedBy(source, pos, "c") ||
						_IsFollowedBy(source, pos, "d") ||
						_IsFollowedBy(source, pos, "f") ||
						_IsFollowedBy(source, pos, "g") ||
						_IsFollowedBy(source, pos, "j") ||
						_IsFollowedBy(source, pos, "k") ||
						_IsFollowedBy(source, pos, "l") ||
						_IsFollowedBy(source, pos, "m") ||
						_IsFollowedBy(source, pos, "n") ||
						_IsFollowedBy(source, pos, "p") ||
						_IsFollowedBy(source, pos, "q") ||
						_IsFollowedBy(source, pos, "r") ||
						_IsFollowedBy(source, pos, "s") ||
						_IsFollowedBy(source, pos, "v")
					){
						break;
					}

					result.push_back(T);
					break;

				case 'u':
				case 150:
				case 151:
				case 234: // Û
				case 235: // Ù
					if(	_IsFollowedBy(source, pos, "n") &&
						_IsLast(source, pos+1)
					){
						++pos;
						result.push_back(AN);
						break;
					}
				
				case 129: // ü
				case 154: // Ü
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



		bool FrenchPhoneticString::_IsAnyOf( const std::string& source, size_t pos, const std::string& chars )
		{
			if(pos < 0 || pos >= source.size())
			{
				return false;
			}

			char t(source[pos]);
			BOOST_FOREACH(char c, chars)
			{
				if(c == t)
				{
					return true;
				}
			}
			return false;
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
			return Levenshtein(_phonetic, s._phonetic);
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
}	}
