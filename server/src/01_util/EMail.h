
/** EMail class header.
	@file EMail.h

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

#ifndef SYNTHESE_util_EMail_h__
#define SYNTHESE_util_EMail_h__

#include <string>
#include <vector>
#include <exception>
#include <utility>

namespace synthese
{
	namespace util
	{
		/** E-Mail class.
			@ingroup m01
		*/
		class EMail
		{
		public:
			typedef enum
			{
				EMAIL_TEXT,
				EMAIL_HTML
			} Format;

			class Exception : public std::exception
			{

			};

		private:
			typedef std::vector<std::pair<std::string, std::string> > _Recipients;

			const std::string			_smtpServer;
			const std::string			_smtpPort;
			Format						_format;
			std::string					_content;
			_Recipients					_recipients;
			std::string					_sender;
			std::string					_senderName;
			std::string					_subject;

		public:
			EMail(
				const std::string& smtpServer,
				const std::string smtpPort = "mail"
			);
			void setFormat(Format value);
			void setContent(const std::string& value);
			void addRecipient(
				const std::string& email,
				std::string name = std::string()
			);
			void setSender(const std::string& value);
			void setSenderName(const std::string& value);
			void setSubject(const std::string& value);

			void send() const;
		};
	}
}

#endif // SYNTHESE_util_EMail_h__
