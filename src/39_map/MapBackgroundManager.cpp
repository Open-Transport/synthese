
/** MapBackgroundManager class implementation.
	@file MapBackgroundManager.cpp

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

#include "MapBackgroundManager.h"
#include "Exception.h"
#include "MapBackground.h"
#include "Log.h"

#include <cmath>
#include <limits>
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

#undef min
#undef max

namespace fs = boost::filesystem;

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace map
	{


		std::map<std::string, MapBackgroundManager*> MapBackgroundManager::_managers;



		MapBackgroundManager::MapBackgroundManager(const boost::filesystem::path& backgroundDir)
		{
			Log::GetInstance ().debug ("Initializing backgrounds in " + backgroundDir.string ());

			// Go through each scale directory
			fs::directory_iterator end_iter;
		   for ( fs::directory_iterator dir_itr( backgroundDir );
				  dir_itr != end_iter;
				  ++dir_itr )
			{
				std::string dirpath = dir_itr->path().string();

				const MapBackground* background = new MapBackground (dirpath);
				_backgrounds.push_back (background);
			}
		}




		MapBackgroundManager::~MapBackgroundManager()
		{
			clearBackgrounds ();
		}




		void
		MapBackgroundManager::clearBackgrounds ()
		{
			for (std::vector<const MapBackground*>::const_iterator iter = _backgrounds.begin ();
				 iter < _backgrounds.end ();
				 ++iter)
			{
				delete *iter;
			}
			_backgrounds.clear ();
		}



		const MapBackground*
		MapBackgroundManager::getBestScalingBackground (double mapScaleX,
														double mapScaleY) const
		{
			double currentDiff = std::numeric_limits<double>::max ();
			const MapBackground* currentBackground = 0;
			for (std::vector<const MapBackground*>::const_iterator iter = _backgrounds.begin ();
				 iter < _backgrounds.end ();
				 ++iter)
			{
				const MapBackground* mbg = *iter;
				double diffScaleX = mapScaleX - mbg->getScaleX ();
				double diffScaleY = mapScaleY - mbg->getScaleY ();
				double meanDiff = std::abs ((diffScaleX + diffScaleY) / 2.0);
				if ((meanDiff < currentDiff) && (meanDiff > 0)) {
					currentDiff = meanDiff;
					currentBackground = mbg;
				}
			}
			if (currentBackground != 0)
			{
				Log::GetInstance ().info ("Best scale for " +
					lexical_cast<string>(mapScaleX) + ", " +
					lexical_cast<string>(mapScaleY) + " : "
					+ lexical_cast<string>(currentBackground->getScaleX ()) + ", "
					+ lexical_cast<string>(currentBackground->getScaleY ()));
			}


			return currentBackground;
		}



		void
		MapBackgroundManager::Initialize (const boost::filesystem::path& backgroundsDir)
		{
			for (std::map<std::string, MapBackgroundManager*>::const_iterator iter = _managers.begin ();
				 iter != _managers.end ();
				 ++iter)
			{
				delete iter->second;
			}
			_managers.clear ();

			if (fs::exists (backgroundsDir) == false)
			{
			Log::GetInstance ().warn ("Map backgrounds dir does not exist : " + backgroundsDir.string ());
			return;
			}

			// Create all managers
			fs::directory_iterator end_iter;
			for ( fs::directory_iterator dir_itr( backgroundsDir );
			  dir_itr != end_iter;
			  ++dir_itr )
			{
			  std::string dirpath = dir_itr->path().filename().string();

			MapBackgroundManager* manager = new MapBackgroundManager (backgroundsDir / dirpath);
			_managers.insert (std::make_pair (dirpath, manager));
			}

		}





		const MapBackgroundManager*
		MapBackgroundManager::GetMapBackgroundManager (const std::string& id)
		{
			if (_managers.find (id) == _managers.end ())
			{
				throw synthese::Exception ("Missing background directory " + id);
			}

			return _managers.find (id)->second;
		}
}	}
