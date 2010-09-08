
/** HTMLFilter class header.
	@file HTMLFilter.h

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

#ifndef SYNTHESE_UTIL_HTMLFILTER_H
#define SYNTHESE_UTIL_HTMLFILTER_H

#include "05_html/Constants.h"

#include <string>
#include <iostream>

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/write.hpp>

namespace synthese
{
	namespace html
	{
		/** Filters any stressed or special alphabetical character, which are replaced 
			by the equivalent HTML entity.
			@ingroup m05
		*/
		class HTMLFilter : public boost::iostreams::multichar_output_filter {
		private:


		public:
		    
			HTMLFilter () {}

			/** @todo Complete filtering implementation for other special characters
			 */
			template<typename Sink> 
			std::streamsize write(Sink& dest, const char* s, std::streamsize n);

		};




		template<typename Sink>
		std::streamsize 
		HTMLFilter::write(Sink& dest, const char* s, std::streamsize n)
		{
			int i = 0;
			    
			while (i != n) {
			const std::string* ref = 0;

			if ((s[i] & 0x80) == 0) // Is first char ASCII ?
			{
				char c = s[i];

				// Test ASCII characters
				// if (c == 'X') ref = &XXXX_HTML_ENTITY; // 

				// to be completed...

				if (ref != 0) {
				boost::iostreams::write(dest, ref->c_str (), ref->size ());
				} else {
				boost::iostreams::put(dest, c);
				}
				i++;
			} 
			else if ( n-i >= 2 )
			{
				// Test some special characters encoded on 2 bytes (UTF-8)
				// Naive algorithm, but special characters to test
				// take two bytes max...
				int c = ((unsigned char) s[i]);
				c = (c << 8) + (unsigned char) s[i+1];

				if (c == 0xC3A1) ref = &AACUTE_HTML_ENTITY; // á
				else if (c == 0xC3A0) ref = &AGRAVE_HTML_ENTITY; // à
				else if (c == 0xC3A2) ref = &ACIRC_HTML_ENTITY; // â
				else if (c == 0xC3A4) ref = &AUML_HTML_ENTITY; // ä

				else if (c == 0xC381) ref = &UPCASE_AACUTE_HTML_ENTITY; // Á
				else if (c == 0xC380) ref = &UPCASE_AGRAVE_HTML_ENTITY; // À
				else if (c == 0xC382) ref = &UPCASE_ACIRC_HTML_ENTITY; // Â
				else if (c == 0xC384) ref = &UPCASE_AUML_HTML_ENTITY; // Ä

				else if (c == 0xC3A9) ref = &EACUTE_HTML_ENTITY; // é 
				else if (c == 0xC3A8) ref = &EGRAVE_HTML_ENTITY; // è
				else if (c == 0xC3AA) ref = &ECIRC_HTML_ENTITY; // ê
				else if (c == 0xC3AB) ref = &EUML_HTML_ENTITY; // ë

				else if (c == 0xC389) ref = &UPCASE_EACUTE_HTML_ENTITY; // É
				else if (c == 0xC388) ref = &UPCASE_EGRAVE_HTML_ENTITY; // È
				else if (c == 0xC38A) ref = &UPCASE_ECIRC_HTML_ENTITY; // Ê
				else if (c == 0xC38B) ref = &UPCASE_EUML_HTML_ENTITY; // Ë

				else if (c == 0xC3AD) ref = &IACUTE_HTML_ENTITY; // í 
				else if (c == 0xC3AC) ref = &IGRAVE_HTML_ENTITY; // ì
				else if (c == 0xC3AF) ref = &IUML_HTML_ENTITY; // ï
				else if (c == 0xC3AE) ref = &ICIRC_HTML_ENTITY; // î

				else if (c == 0xC38D) ref = &UPCASE_IACUTE_HTML_ENTITY; // Í
				else if (c == 0xC38C) ref = &UPCASE_IGRAVE_HTML_ENTITY; // Ì
				else if (c == 0xC38F) ref = &UPCASE_IUML_HTML_ENTITY; // Ï
				else if (c == 0xC38E) ref = &UPCASE_ICIRC_HTML_ENTITY; // Î

				else if (c == 0xC3B3) ref = &OACUTE_HTML_ENTITY; // ó
				else if (c == 0xC3B2) ref = &OGRAVE_HTML_ENTITY; // ò
				else if (c == 0xC3B6) ref = &OUML_HTML_ENTITY; // ö
				else if (c == 0xC3B4) ref = &OCIRC_HTML_ENTITY; // ô

				else if (c == 0xC393) ref = &UPCASE_OACUTE_HTML_ENTITY; // Ó
				else if (c == 0xC392) ref = &UPCASE_OGRAVE_HTML_ENTITY; // Ò
				else if (c == 0xC396) ref = &UPCASE_OUML_HTML_ENTITY; // Ö
				else if (c == 0xC394) ref = &UPCASE_OCIRC_HTML_ENTITY; // Ô

				else if (c == 0xC3BA) ref = &UACUTE_HTML_ENTITY; // ú
				else if (c == 0xC3B9) ref = &UGRAVE_HTML_ENTITY; // ù
				else if (c == 0xC3BC) ref = &UUML_HTML_ENTITY; // ü
				else if (c == 0xC3BB) ref = &UCIRC_HTML_ENTITY; // û

				else if (c == 0xC39A) ref = &UPCASE_UACUTE_HTML_ENTITY; // Ú
				else if (c == 0xC399) ref = &UPCASE_UGRAVE_HTML_ENTITY; // Ù
				else if (c == 0xC39C) ref = &UPCASE_UUML_HTML_ENTITY; // Ü
				else if (c == 0xC39B) ref = &UPCASE_UCIRC_HTML_ENTITY; // Û

				else if (c == 0xC3A7) ref = &CCEDIL_HTML_ENTITY; // ç
				else if (c == 0xC387) ref = &UPCASE_CCEDIL_HTML_ENTITY; // ç

			    
				if (ref != 0) {
				boost::iostreams::write(dest, ref->c_str (), ref->size ());
				} else {
				boost::iostreams::put(dest, s[i]);
				boost::iostreams::put(dest, s[i+1]);
				}
				

				i++;
				i++;

			} else {
				boost::iostreams::put(dest, s[i]);
				i++;
			}

			
			}
			return i;
		}

		
	}
}

#endif
