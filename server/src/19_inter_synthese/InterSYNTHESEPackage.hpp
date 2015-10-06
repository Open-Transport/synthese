
/** InterSYNTHESEPackage class header.
	@file InterSYNTHESEPackage.hpp

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

#ifndef SYNTHESE_cms_InterSYNTHESEPackage_hpp__
#define SYNTHESE_cms_InterSYNTHESEPackage_hpp__

#include "Object.hpp"

#include "Import.hpp"
#include "PointerField.hpp"
#include "PointersVectorField.hpp"
#include "PtimeField.hpp"
#include "SchemaMacros.hpp"
#include "StringField.hpp"
#include "TablesOrObjectsVectorField.hpp"
#include "User.h"

#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>

namespace synthese
{
	FIELD_POINTER(LockUser, security::User)
	FIELD_PTIME(LockTime)
	FIELD_STRING(LockServerName)
	FIELD_STRING(LastJSON)
	FIELD_TABLES_OR_OBJECTS_VECTOR(Objects)
	FIELD_BOOL(Public)
	
	namespace inter_synthese
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Code),
			FIELD(Objects),
			FIELD(impex::Import),
			FIELD(LockUser),
			FIELD(LockTime),
			FIELD(LockServerName),
			FIELD(LastJSON),
			FIELD(Public)
		> InterSYNTHESEPackageRecord;



		//////////////////////////////////////////////////////////////////////////
		/// InterSYNTHESE Package class.
		/// The inter-SYNTHESE package stores the informations about a collection
		/// of objects. Each object of the package is the root of a dependency
		/// tree.
		///
		/// A special SYNTHESE instance is considered as the package repository.
		/// In normal case, the writing access to the package content is allowed
		/// only in the repository. Other instances can do read-only copies of
		/// the package, from the repository : such copies are named working copies.
		/// For a package, the current server is considered as the repository if
		/// the Import field is empty. On a working copy, the Import field points
		/// to an Import object which contains the necessary informations to get
		/// the package from the repository.
		/// 
		/// It is possible to delegate temporarily the writing right to a working
		/// copy. In this case, the repository looses its writing right to avoid
		/// conflicts. On the repository, the writing right is loosed when the
		/// LockTime field is not empty. On a working copy, the writing right is
		/// granted when the LockTime field is not empty. The LastJSON field
		/// contains a dump of the data obtained when the lock was obtained.
		/// When the working copy has the writing right, it can send updates 
		/// to the repository or revert to the initial status as stored in the
		/// LastJSON field. When an update is sent, a new version of the LastJSON
		/// field is stored. After that, the working copy cannot be reverted at
		/// the initial content.
		///
		/// The working copy can release the lock at any time, but must contain
		/// the same content as in the repository : the lock release is
		/// necessarily preceded by an update or a revert, except if no local
		/// modifications were done on it since the lock was obtained.
		//////////////////////////////////////////////////////////////////////////
		/// @date 2013
		///	@ingroup m19
		/// @author Hugues Romain
		/// @since 3.9.0
		class InterSYNTHESEPackage:
			public Object<InterSYNTHESEPackage, InterSYNTHESEPackageRecord>
		{
		public:
			static const std::string TAG_IMPORT;
			static const std::string TAG_LOCK_USER;
			static const std::string SEPARATOR;
		
		private:
			class ItemDumper:
				public boost::static_visitor<>
			{
			private:
				util::ParametersMap& _pm;
				boost::optional<std::stringstream &> _binaryStream;

			public:
				ItemDumper(util::ParametersMap& pm,
					boost::optional<std::stringstream &> binaryStream = boost::optional<std::stringstream &>()
				);

				void operator()(boost::shared_ptr<db::DBTableSync> value) const;
				void operator()(boost::shared_ptr<util::Registrable> value) const;
				void operator()(util::Registrable* value) const;
			};

			static void _dumpItem(
				const db::TableOrObject& item,
				util::ParametersMap& pm,
				boost::optional<std::stringstream &> binaryStream = boost::optional<std::stringstream &>()
			);

			boost::mutex _lockMutex;

			std::string _buildDump(
				const boost::posix_time::ptime& lockTime
			) const;

		public:
			InterSYNTHESEPackage(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Adds parameters that are not intended to be saved (i.e. generated content).
				/// The default implementation adds nothing. This method may be overloaded
				/// @param map the map to populate
				/// @param prefix prefix to add to the keys of the map items
				virtual void addAdditionalParameters(
					util::ParametersMap& map,
					std::string prefix = std::string()
				) const;

				bool isRepository() const;
				bool isWorkingCopy() const;
				bool isWritable() const;

				std::string buildDump(
				) const;

				std::string getNonBinaryDump(
				) const;

				std::string lockAndBuildDump(
					const security::User& user,
					const std::string& serverName
				);

				void localLock(
					const security::User& user
				);

				void unlock();

				boost::mutex& getLockMutex(){ return _lockMutex; }

				struct PackageAddress
				{
					std::string host;
					std::string port;
					std::string smartURL;

					PackageAddress(const std::string& url);
				};
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_cms_InterSYNTHESEPackage_hpp__
