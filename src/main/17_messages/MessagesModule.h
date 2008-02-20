
/** MessagesModule class header.
	@file MessagesModule.h

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

#ifndef SYNTHESE_MessagesModule_H__
#define SYNTHESE_MessagesModule_H__

#include "01_util/ModuleClass.h"
#include "01_util/UId.h"
#include "01_util/Constants.h"
#include "01_util/FactorableTemplate.h"

#include "17_messages/Types.h"

#include <vector>
#include <string>

namespace synthese
{
	/** @defgroup m17Actions Actions
		@ingroup m17

		@defgroup m17 17 Messages
		@ingroup m1
	@{	*/

	/** 17 Messages module namespace.
	*/
	namespace messages
	{
		/** 17 Messages module class.
		*/
		class MessagesModule : public util::FactorableTemplate<util::ModuleClass, MessagesModule>
		{
		public:
			void initialize();

			/** Labels list containing each scenario template ordered by folder, indicating the full path in the folder tree.
				@param withAll true = add a link "all scenarios"
				@param folderId id of the main parent folder (optional) :
					- 0/default value is the root folder
					- UNKWNOWN_VALUE = do not use this criteria : return all templates without their full path
				@param prefix text to add at the beginning of each item (optional)
				@return std::vector<std::pair<uid, std::string> > The list
				@author Hugues Romain
				@date 2008
			*/
			static std::vector<std::pair<uid, std::string> > GetScenarioTemplatesLabels(
				bool withAll = false
				, uid folderId = 0
				, std::string prefix = std::string()
			);



			/** Labels list containing each scenario template folder, indicating the full path in the folder tree.
				@param folderId id of the main parent folder (optional)
				@param prefix text to add at the beginning of each item (optional)
				@param forbiddenFolderId id of a folder which must not be present in the result
				@return std::vector<std::pair<uid, std::string> > The list
				@author Hugues Romain
				@date 2008
			*/
			static std::vector<std::pair<uid, std::string> > GetScenarioFoldersLabels(
				uid folderId = 0
				, std::string prefix = std::string()
				, uid forbiddenFolderId = UNKNOWN_VALUE
			);

			static std::vector<std::pair<AlarmLevel, std::string> >		getLevelLabels(bool withAll = false);
			static std::vector<std::pair<AlarmConflict, std::string> >	getConflictLabels(bool withAll = false);
			static std::vector<std::pair<uid, std::string> >			getTextTemplateLabels(const AlarmLevel& level);

			static std::string							getLevelLabel(const AlarmLevel& level);
			static std::string							getConflictLabel(const AlarmConflict& conflict);

			virtual std::string getName() const;
		};
	}
	/** @} */
}

#endif // SYNTHESE_MessagesModule_H__
