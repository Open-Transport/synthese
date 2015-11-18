#include "FrenchPhoneticString.h"
#include "IConv.hpp"
#include "Log.h"

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
		IConv FrenchPhoneticString::ICONV("UTF-8","CP1252");
		std::string FrenchPhoneticString::ACCENTUATED_A(IConv("UTF-8","CP1252").convert("àáâäÀÁÂÄ"));
		std::string FrenchPhoneticString::ACCENTUATED_E(IConv("UTF-8","CP1252").convert("èéêëÈÉÊË"));
		std::string FrenchPhoneticString::ACCENTUATED_I(IConv("UTF-8","CP1252").convert("ìíîïÌÍÎÏ"));
		std::string FrenchPhoneticString::ACCENTUATED_O(IConv("UTF-8","CP1252").convert("òóôöÒÓÔÖ"));
		std::string FrenchPhoneticString::ACCENTUATED_U(IConv("UTF-8","CP1252").convert("ùúûüÙÚÛÜ"));
		std::string FrenchPhoneticString::VOWELS(IConv("UTF-8","CP1252").convert("aàäáâeèëéêiìïíîoòöóôuùüúûy"));
		std::string FrenchPhoneticString::C_VOWELS(IConv("UTF-8","CP1252").convert("eèëéêiìïíîy"));

		mutex FrenchPhoneticString::_IConvMutex;

		FrenchPhoneticString::FrenchPhoneticString()
		{}



		FrenchPhoneticString::FrenchPhoneticString(const std::string& source)
		{
			setSource(source);
		}



		std::string FrenchPhoneticString::_convertTo8bits(const std::string& text)
		{
			try
			{
				return to_lower_copy(ICONV.convert(text));
			}
			catch(IConv::ImpossibleConversion& e)
			{
				util::Log::GetInstance().warn("Failed to convert string to CP1252 in FrenchPhoneticString", e);
				return "";
			};
		}

		std::string FrenchPhoneticString::_to_plain_lower_copy_8bit(const std::string& source)
		{
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

		std::string FrenchPhoneticString::to_plain_lower_copy(const std::string& text)
		{
			mutex::scoped_lock lock(_IConvMutex);
			return _to_plain_lower_copy_8bit(_convertTo8bits(text));
		}




		void FrenchPhoneticString::setSource(const std::string& ssource)
		{
			mutex::scoped_lock lock(_IConvMutex);

			_source = ssource;
			string source(_convertTo8bits(ssource));
			_plainLowerSource = _to_plain_lower_copy_8bit(source);

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
				case 224:
				case 225:
				case 226:
				case 227:
				case 228:
				case 229:
				case 192:
				case 193:
				case 194:
				case 195:
				case 196:
				case 197:
					if(	_IsFollowedBy(source, pos, "in") &&
						!_IsFollowedBy(source, pos, "ine")
					){
						pos += 2;
						result.push_back(AN);
						break;
					}

					if( (unsigned char)source[pos+1] == 239) // ï
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

					if(	_IsFollowedBy(source, pos, "n") &&
						_IsFollowedBy(source, pos+1, "n")
					){
						++pos;
						++pos;
						result.push_back(A);
						result.push_back(N);
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
					if(	_IsFollowedBy(source, pos, "b"))
					{
						++pos;
					}
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

					if(	_IsFollowedBy(source, pos, "k") ||
						_IsFollowedBy(source, pos, "q")
					){
						break;
					}

					result.push_back(K);
					break;

				case 0xC7: // Capital cedilla C in ISO-8859-1 and Windows CP1252
				case 0xE7: // Minus cedilla c
					result.push_back(S);
					break;

				case 'd':
					if(	_IsLast(source, pos)
					){
						break;
					}
					if(	_IsFollowedBy(source, pos, "d"))
					{
						++pos;
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

					if(	_IsFollowedBy(source, pos, "r") &&
						_IsFollowedBy(source, pos+1, "c") &&
						_IsLast(source, pos, 3)
					){
						result.push_back(E);
						result.push_back(R);
						++pos;
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

				case 200:
				case 201:
				case 202:
				case 203:
				case 232:
				case 233:
				case 234:
				case 235:
				case 198:
				case 230:
				case 140:
				case 156:
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

					if(	_IsFollowedBy(source, pos, "n") &&
						_IsFollowedBy(source, pos+1, "n")
					){
						++pos;
						++pos;
						result.push_back(E);
						result.push_back(N);
						break;
					}

					if(	(_IsFollowedBy(source, pos, "n") &&
						(	!_IsAnyOf(source, pos+2, VOWELS) ||
							_IsAnyOf(source, pos, ACCENTUATED_E)
						)) ||
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
					if(	_IsFollowedBy(source, pos, "f"))
					{
						++pos;
					}
					result.push_back(F);
					break;

				case 'g':
					if(	_IsFollowedBy(source, pos, "g"))
					{
						++pos;
						result.push_back(G);
						break;
					}
					if(	_IsFollowedBy(source, pos, "e") &&
						_IsAnyOf(source, pos+2, "oau")
					){
						++pos;
						result.push_back(J);
						break;
					}

					if(	_IsFollowedBy(source, pos, "e") ||
						_IsAnyOf(source, pos+1, ACCENTUATED_E) ||
						_IsFollowedBy(source, pos, "i") ||
						_IsAnyOf(source, pos+1, ACCENTUATED_I) ||
						_IsFollowedBy(source, pos, "y")
					){
						result.push_back(J);
						break;
					}

					if(	_IsFollowedBy(source, pos, "u") &&
						(	!_IsFollowedBy(source, pos+1, "y") ||
							_IsLast(source, pos, 3)
					)	){
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
				case 204:
				case 205:
				case 206:
				case 236:
				case 237:
				case 238:
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
						result.push_back(L);
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

				case 207: // ï
				case 239: // Ï
					result.push_back(I);
					break;

				case 'j':
					if(	_IsFollowedBy(source, pos, "e") &&
						_IsAnyOf(source, pos+2, VOWELS)
					){
						++pos;
					}

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

					if(	_IsFollowedBy(source, pos, "e") &&
						_IsFollowedBy(source, pos+1, "r") &&
						_IsLast(source, pos, 3)
					){
						++pos;
						++pos;
						result.push_back(M);
						result.push_back(E);
						result.push_back(R);
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
				case 210:
				case 211:
				case 212:
				case 213:
				case 240:
				case 242:
				case 243:
				case 244:
				case 245:
					if(	_IsFollowedBy(source, pos, "n") &&
						_IsFollowedBy(source, pos+1, "n")
					){
						++pos;
						++pos;
						result.push_back(O);
						result.push_back(N);
						break;
					}

					if(	(_IsFollowedBy(source, pos, "n") && !_IsAnyOf(source, pos+2, VOWELS)) ||
						_IsFollowedBy(source, pos, "mp") ||
						_IsFollowedBy(source, pos, "mb")
					){
						++pos;
						result.push_back(ON);
						break;
					}

					if(	_IsFollowedBy(source, pos, "u") ||
						_IsAnyOf(source, pos+1, ACCENTUATED_U)
					){
						++pos;
						result.push_back(U);
						break;
					}

					if(	(	_IsFollowedBy(source, pos, "i") ||
							_IsAnyOf(source, pos+1, ACCENTUATED_I) ||
							_IsFollowedBy(source, pos, "y")
						) && !_IsAnyOf(source, pos+2, "nm")
					){
						++pos;
						result.push_back(O);
						result.push_back(A);
						break;
					}

				case 214: // ö
				case 246:
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

					if(	_IsFollowedBy(source, pos, "h")
					){
						++pos;
						result.push_back(F);
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
					if(pos == 0)
					{
						result.push_back(T);
						break;
					}

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
				case 217:
				case 218:
				case 219:
				case 249:
				case 250:
				case 251:
					if(	_IsFollowedBy(source, pos, "n") &&
						_IsLast(source, pos+1)
					){
						++pos;
						result.push_back(AN);
						break;
					}

				case 220: // ü
				case 252: // Ü
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
			if(pos >= source.size())
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

		bool FrenchPhoneticString::startsWith(
			const FrenchPhoneticString& s
		) const {
			if(s._phonetic.size() > _phonetic.size()) return false;

			size_t pos(0);
			for(;pos < s._phonetic.size();pos++)
				if(s._phonetic[pos] != _phonetic[pos])
					return false;

			return true;
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
				case ON: s << "ON"; break;
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
