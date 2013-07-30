
/** PDF class implementation.
	@file PDF.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "PDF.hpp"

#include "Exception.h"
#include "RGBColor.h"

#include <boost/lexical_cast.hpp>
#include <hpdf_utils.h>

using namespace boost;
using namespace std;

namespace synthese
{
	namespace pdf_cms
	{
		PDF::PDF():
			_curDoc(HPDF_New(_errorHandler, NULL)),
			_curPage(NULL),
			_curFont(NULL),
			_curTextSize(3),
			_curLineWidth(1),
			_curStrokeColor(0,0,0),
			_curFillColor(1,1,1)
		{
			setTextFont("Helvetica");
			HPDF_SetInfoAttr(_curDoc, HPDF_INFO_CREATOR, "SYNTHESE");
			HPDF_SetCompressionMode(_curDoc, HPDF_COMP_ALL);
		}



		PDF::~PDF()
		{
			HPDF_Free(_curDoc);
		}



		void PDF::_errorHandler( HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data )
		{
			throw Exception(lexical_cast<string>(error_no) +"/"+ lexical_cast<string>(detail_no));
		}



		//////////////////////////////////////////////////////////////////////////
		/// Adds a page into the PDF.
		/// @param width the width of the page
		/// @param height the height of the page
		void PDF::addPage(
			HPDF_REAL width,
			HPDF_REAL height
		){
			_curPage = HPDF_AddPage(_curDoc);
			HPDF_Page_SetWidth(_curPage, width);
			HPDF_Page_SetHeight(_curPage, height);
			HPDF_Page_SetRGBStroke(_curPage, _curStrokeColor.red, _curStrokeColor.green, _curStrokeColor.blue);
			HPDF_Page_SetRGBFill(_curPage, _curFillColor.red, _curFillColor.green, _curFillColor.blue);
			HPDF_Page_SetLineWidth(_curPage, _curLineWidth);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Generates the PDF content into a stream.
		/// @param stream the stream
		void PDF::output( std::ostream& stream ) const
		{
			HPDF_STATUS status(HPDF_SaveToStream(_curDoc));
			if(status != HPDF_OK)
			{
				return;
			}

			// Rewind the stream
			HPDF_ResetStream (_curDoc);

			// Get the data from the stream and output it to the output stream.
			while(true)
			{
				HPDF_BYTE buf[4096];
				HPDF_UINT32 siz = 4096;
				HPDF_ReadFromStream (_curDoc, buf, &siz);

				if (siz == 0)
				{
					break;
				}

				stream.write(reinterpret_cast<const char*>(buf), siz);
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Writes some text onto the PDF.
		/// @param text the text to write. Must use the same encoding than the fonts (generally iso-8859-1)
		/// @param x x coordinate of the text
		/// @param y y coordinate of the text
		void PDF::drawText(
			const std::string& text,
			HPDF_REAL x,
			HPDF_REAL y
		){
			HPDF_Page_SetTextRenderingMode (_curPage, HPDF_FILL);
			HPDF_Page_BeginText(_curPage);
			HPDF_Page_SetFontAndSize(_curPage, _curFont, _curTextSize);
			HPDF_Page_TextOut(_curPage, x, y, text.c_str());
			HPDF_Page_EndText(_curPage);
		}



		void PDF::setTextFont( const std::string& font )
		{
			_curFont = HPDF_GetFont(
				_curDoc,
				font.c_str(),
				HPDF_ENCODING_CP1252
			);
		}



		void PDF::setTextSize( HPDF_REAL value)
		{
			_curTextSize = value;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Embeds a True Type font into the PDF.
		/// @param content the content of the font file
		void PDF::embedTTFFont(
			const std::string& content
		){
			HPDF_Stream font_data;

			/* create file stream */
			font_data = HPDF_MemStream_New(_curDoc->mmgr, content.size());

			if (!HPDF_Stream_Validate (font_data))
			{
				HPDF_RaiseError (&_curDoc->error, HPDF_INVALID_STREAM, 0);
				return;
			}

			if(HPDF_Stream_Write(font_data, reinterpret_cast<const HPDF_BYTE*>(content.c_str()), content.size()) != HPDF_OK)
			{
				HPDF_Stream_Free(font_data);
				return;
			}

			if (HPDF_Stream_Validate (font_data))
			{
				// Code copied from hpdf_doc.c:LoadTTFontFromStream
				// (LoadTTFontFromStream is a C static function)

				HPDF_FontDef def;

				def = HPDF_TTFontDef_Load (_curDoc->mmgr, font_data, true);
				if (def) {
					HPDF_FontDef  tmpdef = HPDF_Doc_FindFontDef (_curDoc, def->base_font);
					if (tmpdef) {
						HPDF_FontDef_Free (def);
						HPDF_SetError (&_curDoc->error, HPDF_FONT_EXISTS, 0);
						return;
					}

					if (HPDF_List_Add (_curDoc->fontdef_list, def) != HPDF_OK) {
						HPDF_FontDef_Free (def);
						return;
					}
				} else
					return;

				if (_curDoc->ttfont_tag[0] == 0) {
					HPDF_MemCpy (_curDoc->ttfont_tag, (HPDF_BYTE *)"HPDFAA", 6);
				} else {
					HPDF_INT i;

					for (i = 5; i >= 0; i--) {
						_curDoc->ttfont_tag[i] += 1;
						if (_curDoc->ttfont_tag[i] > 'Z')
							_curDoc->ttfont_tag[i] = 'A';
						else
							break;
					}
				}

				HPDF_TTFontDef_SetTagName (def, (char *)_curDoc->ttfont_tag);
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Converts milimeters to pixels.
		/// @param mm milimeters
		/// @return pixels
		HPDF_REAL PDF::GetPixelsFromMM( float mm )
		{
			return mm * 2.83464566929F;
		}



		HPDF_Image PDF::embedPNGImage(
			const std::string& key,
			const std::string& content
		){
			// Check if the key is free
			Images::const_iterator it(_images.find(key));
			if(it != _images.end())
			{
				throw Exception("Image key "+ key +" already used");
			}

			// Insertion
			HPDF_Image image(
				HPDF_LoadPngImageFromMem(
					_curDoc,
					reinterpret_cast<const HPDF_BYTE*>(content.c_str()),
					content.size()
			)	);

			// Check if the image has been properly embedded
			if(!image)
			{
				throw Exception("Error at "+ key +" image load.");
			}

			// Store the link to the image
			_images[key] = image;

			// Return the image
			return image;
		}



		void PDF::drawImage(
			HPDF_Image image,
			HPDF_REAL x,
			HPDF_REAL y,
			HPDF_REAL width,
			HPDF_REAL height
		){
			// Draw the image
			HPDF_Page_DrawImage(
				_curPage,
				image,
				x,
				y,
				width,
				height
			);
		}



		HPDF_Image PDF::getImage( const std::string& key ) const
		{
			Images::const_iterator it(_images.find(key));
			if( it == _images.end())
			{
				return NULL;
			}
			return it->second;
		}



		void PDF::setStrokeColor( RGBColor value )
		{
			if(_curStrokeColor != value)
			{
				_curStrokeColor = value;
				HPDF_Page_SetRGBStroke(_curPage, value.red, value.green, value.blue);
			}
		}



		void PDF::setFillColor( RGBColor value )
		{
			if(_curFillColor != value)
			{
				_curFillColor = value;
				HPDF_Page_SetRGBFill(_curPage, value.red, value.green, value.blue);
			}
		}



		void PDF::setLineWidth( HPDF_REAL value )
		{
			if(_curLineWidth != value)
			{
				_curLineWidth = value;
				HPDF_Page_SetLineWidth(_curPage, value);
			}
		}



		void PDF::drawLine( HPDF_REAL x1, HPDF_REAL y1, HPDF_REAL x2, HPDF_REAL y2 )
		{
			HPDF_Page_MoveTo (_curPage, x1, y1);
			HPDF_Page_LineTo (_curPage, x2, y2);
			HPDF_Page_Stroke (_curPage);
		}



		void PDF::drawRectangle(
			HPDF_REAL x1,
			HPDF_REAL y1,
			HPDF_REAL x2,
			HPDF_REAL y2,
			bool stroke,
			bool fill
		){
			HPDF_Page_Rectangle(_curPage, x1, y1, x2, y2);
			if(stroke && fill)
			{
				HPDF_Page_FillStroke(_curPage);
			}
			else if(stroke)
			{
				HPDF_Page_Stroke(_curPage);
			}
			else if(fill)
			{
				HPDF_Page_Fill(_curPage);
			}
		}



		PDF::RGBColor::RGBColor( HPDF_REAL red_, HPDF_REAL green_, HPDF_REAL blue_ ):
			red(red_),
			green(green_),
			blue(blue_)
		{}



		PDF::RGBColor::RGBColor(
			const util::RGBColor& value
		):	red(float(value.r) / 256),
			green(float(value.g) / 256),
			blue(float(value.b) / 256)
		{}



		bool PDF::RGBColor::operator!=( const RGBColor& rhs ) const
		{
			return red != rhs.red || green != rhs.green || blue != rhs.blue;
		}
}	}
