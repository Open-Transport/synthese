
/** TreeFolderAdmin class header.
	@file TreeFolderAdmin.hpp

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

#ifndef SYNTHESE_tree_TreeFolderAdmin_hpp__
#define SYNTHESE_tree_TreeFolderAdmin_hpp__

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "HTMLForm.h"
#include "HTMLList.h"
#include "HTMLModule.h"
#include "ObjectCreateAction.hpp"
#include "ObjectUpdateAction.hpp"
#include "PropertiesHTMLTable.h"
#include "RemoveObjectAction.hpp"
#include "TreeFolder.hpp"
#include "TreeFolderRoot.hpp"

namespace synthese
{
	namespace tree
	{
		////////////////////////////////////////////////////////////////////////////
		/// Tree folder administration panel base class.
		///	@ingroup m20
		/// @author Hugues Romain
		/// @date 2012
		template<
			class RootType,
			class AdminPageType
		>
		class TreeFolderAdmin
		{
		private:
			const TreeFolderUpNode* _node;
			

		protected:
			///////////////////////////////////////////////////////////////////////////
			/// Protected constructor : this class is a base class only.
			TreeFolderAdmin():
				 _node(NULL)
			{}


		public:
			/// Setters
			//@{
				void setFolder(const TreeFolder& value){ _node = static_cast<const TreeFolderUpNode*>(&value); }
				void setRoot(const RootType& value){ _node = static_cast<const TreeFolderUpNode*>(&value); }
				void setNode(const TreeFolderUpNode& value){ _node = &value; }
			//@}

			/// Getters
			//@{
				const TreeFolder* getFolder() const { return dynamic_cast<const TreeFolder*>(_node); }
				const RootType* getRoot() const { return dynamic_cast<const RootType*>(_node); }
				const TreeFolderUpNode* getNode() const { return _node; }
				const RootType* getNodeRoot() const { return getRoot() ? getRoot() : dynamic_cast<const RootType*>(getFolder()->_getRoot()); }
			//@}



		protected:
			void _getNodeParametersMap(
				util::ParametersMap& map
			) const {
				if(_node)
				{
					map.insert(server::Request::PARAMETER_OBJECT_ID, _node->getKey());
				}
			}



			//////////////////////////////////////////////////////////////////////////
			/// Loads the node from the specified id defined in the parameters map.
			/// @param map the map to read
			void _loadNodeFromMainEnv(
				const util::ParametersMap& map
			){
				util::RegistryKeyType id(
					map.get<util::RegistryKeyType>(server::Request::PARAMETER_OBJECT_ID)
				);

				if(util::decodeTableId(id) == TreeFolder::CLASS_NUMBER)
				{ // The current object is a folder
					try
					{
						_node = static_cast<const TreeFolderUpNode*>(
							util::Env::GetOfficialEnv().get<TreeFolder>(id).get()
						);
					}
					catch(util::ObjectNotFoundException<TreeFolder>& e)
					{
						throw admin::AdminParametersException(e.getMessage());
					}
				}
				else
				{ // The current object is a root
					try
					{
						_node = static_cast<const TreeFolderUpNode*>(
							util::Env::GetOfficialEnv().get<RootType>(id).get()
						);
					}
					catch(util::ObjectNotFoundException<RootType>& e)
					{
						throw admin::AdminParametersException(e.getMessage());
					}
				}
			}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the sub folders list with folder creation form.
			/// @param stream the stream to write on
			/// @param request the current request
			void _displaySubFoldersList(
				std::ostream& stream,
				const server::Request& request
			) const {

				// Sub-folders load
				TreeFolder::SubFolders folders(
					_node->getSubFolders()
				);

				// Sub-folder opening request
				admin::AdminFunctionRequest<AdminPageType> goFolderRequest(request);

				// Sub-folder creation request
				typename admin::AdminActionFunctionRequest<db::ObjectCreateAction, AdminPageType> addFolderRequest(request);
				//addFolderRequest.setActionFailedPage<AdminPageType>();
				addFolderRequest.getAction()->template set<Parent>(const_cast<TreeFolderUpNode*>(_node));
				addFolderRequest.getAction()->template setTable<TreeFolder>();
				addFolderRequest.setActionWillCreateObject();

				// Sub-folder removal request
				admin::AdminActionFunctionRequest<db::RemoveObjectAction, AdminPageType> removeFolderRequest(request);

				// Section title
				stream << "<h1>Sous-répertoires</h1>";
				if(folders.empty())
				{
					stream << "<p>Aucun sous-répertoire.</p>";
				}

				// Form and list headers
				html::HTMLForm f(addFolderRequest.getHTMLForm());
				html::HTMLList l;
				stream << f.open() << l.open();

				// Loop on sub folders
				BOOST_FOREACH(TreeFolder* folder, folders)
				{
					// Prevent infinite loop if corrupted data
					if(static_cast<TreeFolderUpNode*>(folder) == _node)
					{
						continue;
					}

					// List bullet
					stream << l.element("folder");

					// Remove button only for empty folders and for granted users
					removeFolderRequest.getAction()->setObjectId(
						folder->getKey()
					);
					if(	folder->empty() &&
						removeFolderRequest.getAction()->isAuthorized(request.getSession().get())
					){
						stream <<
							html::HTMLModule::getHTMLLink(
								removeFolderRequest.getURL(),
								html::HTMLModule::getHTMLImage("/admin/img/remove.png", "Supprimer"),
								"Etes-vous sûr de vouloir supprimer le répertoire "+ folder->get<Name>() +" ?"
							);
					}

					// Link to the folder
					goFolderRequest.getPage()->setFolder(
						*folder
					);
					stream << html::HTMLModule::getHTMLLink(goFolderRequest.getURL(), folder->get<Name>());
				}

				// Folder creation form
				stream << l.element("folder");
				stream << f.getImageSubmitButton("/admin/img/add.png", "Ajouter");
				stream << f.getTextInput(db::ObjectCreateAction::GetInputName<Name>(), std::string(), "(Entrez le nom du répertoire ici)");

				// Form and list footer
				stream << l.close() << f.close();
			}



			///////////////////////////////////////////////////////////////////////////
			/// Display of the folder properties with edition form.
			/// @param stream the stream to write on
			/// @param request the current request
			void _displayFolderProperties(
				std::ostream& stream,
				const server::Request& request
			) const {

				// No display if the page is not loaded on a folder
				if(!getFolder())
				{
					return;
				}
				const TreeFolder& folder(*getFolder());

				// Section title
				stream << "<h1>Répertoire</h1>";

				// Folder removal button
				if(	folder.empty()
				){
					admin::AdminActionFunctionRequest<db::RemoveObjectAction, AdminPageType> removeFolderRequest(request);
					removeFolderRequest.getAction()->setObjectId(folder.getKey());
					removeFolderRequest.getPage()->setNode(*folder._getParent());

					stream <<
						"<p>" <<
						html::HTMLModule::getLinkButton(
							removeFolderRequest.getURL(),
							"Supprimer",
							"Etes-vous sûr de vouloir supprimer le répertoire "+ folder.getName() +" ?",
							"/admin/img/folder_delete.png"
						) << "</p>"
					;
				}

				// Update form request
				typename admin::AdminActionFunctionRequest<db::ObjectUpdateAction, AdminPageType> updateFolderRequest(request);
				updateFolderRequest.getAction()->setObject(folder);

				// The form
				html::PropertiesHTMLTable t(updateFolderRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Nom", t.getForm().getTextInput(db::ObjectUpdateAction::GetInputName<Name>(), folder.get<Name>()));
				stream << t.cell(
					"Parent",
					t.getForm().getSelectInput(
						db::ObjectUpdateAction::GetInputName<Parent>(),
						folder._getRoot()->getSubFoldersLabels(),
						boost::optional<util::RegistryKeyType>(folder._getParent()->getKey())
				)	);
				stream << t.close();
			}
		};
}	}

#endif // SYNTHESE_tree_TreeFolderAdmin_hpp__

