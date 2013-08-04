
/** PDF class header.
	@file PDF.hpp

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

#ifndef SYNTHESE_pdf_cms_PDF_hpp__
#define SYNTHESE_pdf_cms_PDF_hpp__

#include <hpdf.h>
#include <map>
#include <ostream>

namespace synthese
{
	namespace util
	{
		struct RGBColor;
	}

	namespace pdf_cms
	{
		/** PDF class.
			@ingroup m40
		*/
		class PDF
		{
		public:
			struct RGBColor
			{
				HPDF_REAL red;
				HPDF_REAL green;
				HPDF_REAL blue;
				RGBColor(HPDF_REAL red_, HPDF_REAL green_, HPDF_REAL blue_);
				RGBColor(const util::RGBColor& value);
				bool operator!=(const RGBColor& rhs) const;
			};

		private:
			HPDF_Doc _curDoc;
			HPDF_Page _curPage;
			HPDF_Font _curFont;
			HPDF_REAL _curTextSize;
			HPDF_REAL _curLineWidth;
			RGBColor _curStrokeColor;
			RGBColor _curFillColor;

			typedef std::map<std::string, HPDF_Image> Images;
			Images _images;

			static void _errorHandler(
				HPDF_STATUS   error_no,
				HPDF_STATUS   detail_no,
				void         *user_data
			);

		public:
			PDF();
			~PDF();

			void addPage(
				HPDF_REAL width,
				HPDF_REAL height
			);

			void embedTTFFont(
				const std::string& content
			);

			HPDF_Image embedPNGImage(
				const std::string& key,
				const std::string& content
			);

			HPDF_Image getImage(
				const std::string& key
			) const;

			void setTextFont(
				const std::string& font
			);

			void setTextSize(
				HPDF_REAL value
			);

			void setStrokeColor(
				RGBColor value
			);

			void setFillColor(
				RGBColor value
			);

			void setLineWidth(
				HPDF_REAL value
			);

			void drawText(
				const std::string& text,
				HPDF_REAL x,
				HPDF_REAL y,
				float angle = 0
			);

			void drawImage(
				HPDF_Image image,
				HPDF_REAL x,
				HPDF_REAL y,
				HPDF_REAL width,
				HPDF_REAL height
			);

			void drawLine(
				HPDF_REAL x1,
				HPDF_REAL y1,
				HPDF_REAL x2,
				HPDF_REAL y2
			);

			void drawRectangle(
				HPDF_REAL x1,
				HPDF_REAL y1,
				HPDF_REAL x2,
				HPDF_REAL y2,
				bool stroke,
				bool fill
			);
			void output(
				std::ostream& stream
			) const;

			static HPDF_REAL GetPixelsFromMM(float mm);
		};
	}
}

#endif // SYNTHESE_pdf_cms_PDF_hpp__

