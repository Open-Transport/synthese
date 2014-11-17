#include "MapBackground.h"

#include <boost/filesystem/operations.hpp>
#include <boost/tokenizer.hpp>

#include <assert.h>

#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>

#include "01_util/Log.h"

#undef min
#undef max

using namespace std;


namespace fs = boost::filesystem;


namespace synthese
{
	using namespace util;

namespace map
{


MapBackground::MapBackground(const std::string& tileDir)
: _tileDir (tileDir)
{
  std::string tileDirStr = _tileDir.filename().string();

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("_");

    tokenizer scaleTokens(tileDirStr, sep);
    tokenizer::iterator scaleTok_iter = scaleTokens.begin();

    // Parse scalex and scaley
    _scaleX = atof ((++scaleTok_iter)->c_str ());
    _scaleY = atof ((++scaleTok_iter)->c_str ());

    Log::GetInstance ().debug ("Initializing scaled background in " + tileDir);

   std::vector<MapBackgroundTile*> tiles;

   // Iterate through the tile directory
   double minX = std::numeric_limits<double>::max ();
   double maxX = std::numeric_limits<double>::min ();
   double minY = std::numeric_limits<double>::max ();
   double maxY = std::numeric_limits<double>::min ();


   fs::directory_iterator end_iter;
   for ( fs::directory_iterator dir_itr( _tileDir );
          dir_itr != end_iter;
          ++dir_itr )
    {
      std::string filepath = dir_itr->path().string();
      std::string filename = dir_itr->path().filename().string();

        if (filename.substr (filename.size ()-3, 3) == ".ps") {
            filename = filename.substr (0, filename.size ()-3);

            // Parse top left and bottom right coordinates.
            tokenizer tokens(filename, sep);

            tokenizer::iterator tok_iter = tokens.begin();
            int pixelWidth = atoi ((++tok_iter)->c_str ());
            int pixelHeight = atoi ((++tok_iter)->c_str ());
            double topLeftX = atoi ((++tok_iter)->c_str ());
            double topLeftY = atoi ((++tok_iter)->c_str ());
            double bottomRightX = atoi ((++tok_iter)->c_str ());
            double bottomRightY = atoi ((++tok_iter)->c_str ());

            if (topLeftX < minX) minX = topLeftX;
            if (topLeftY > maxY) maxY = topLeftY;
            if (bottomRightX > maxX) maxX = bottomRightX;
            if (bottomRightY < minY) minY = bottomRightY;

            MapBackgroundTile* tile = new MapBackgroundTile (
				fs::path (filepath),
				pixelWidth, pixelHeight, topLeftX, topLeftY, bottomRightX, bottomRightY);

            tiles.push_back (tile);

        }


    }

    if (tiles.size () == 0) return;

    _minX = minX;
    _maxX = maxX;
    _minY = minY;
    _maxY = maxY;
    _tileWidth = tiles[0]->getWidth ();
    _tileHeight = tiles[0]->getHeight ();
    _tilePixelWidth = tiles[0]->getPixelWidth ();
    _tilePixelHeight = tiles[0]->getPixelHeight ();

    _numTilesX = (int) ceil((_maxX - _minX) / _tileWidth);
    _numTilesY = (int) ceil((_maxY - _minY) / _tileHeight);

    _tileArray = new MapBackgroundTile** [_numTilesX];
    for (int i=0; i<_numTilesX; ++i) {
        _tileArray[i] = new MapBackgroundTile* [_numTilesY];
        for (int j=0; j<_numTilesY; ++j) {
            _tileArray[i][j] = 0;
        }
    }

    for (unsigned int i=0; i<tiles.size(); ++i) {
        MapBackgroundTile* tile = tiles[i];
        assert (tile->getWidth () == _tileWidth);
        assert (tile->getHeight () == _tileHeight);

        // Do not consider top-left point because some tiles are slightly
        // overlapping (ie: 25k/ 3213 and 3214).
        // Instead consider middle of the tile
        int tileIndexX = (int) ((tile->getTopLeftX() - _minX + (_tileWidth / 2)) / _tileWidth);
        int tileIndexY = (int) ((tile->getTopLeftY() - _minY - (_tileHeight / 2)) / _tileHeight);

        // cout << tileIndexX << " " << tileIndexY << " " << tile->getPath ().string () << endl;
        assert (_tileArray[tileIndexX][tileIndexY] == 0);
        _tileArray[tileIndexX][tileIndexY] = tile;
    }

}


MapBackground::~MapBackground()
{
    for (int i=0; i<_numTilesX; ++i) {
        for (int j=0; j<_numTilesY; ++j) {
            delete _tileArray[i][j];
        }
		delete[] _tileArray[i];
    }
	delete[] _tileArray;
}


const MapBackgroundTile*
MapBackground::getTileContaining (double x, double y) const
{
    std::pair<int,int> indexes = getIndexesOfTileContaining (x, y);
    return _tileArray[indexes.first][indexes.second];
}


std::pair<int,int>
MapBackground::getIndexesOfTileContaining (double x, double y) const
{
    int tileIndexX = (int) ((x - _minX) / _tileWidth);
    int tileIndexY = (int) ((y - _minY) / _tileHeight);
    return std::make_pair (tileIndexX, tileIndexY);
}


const MapBackgroundTile*
MapBackground::getTile (int indexx, int indexy) const
{
    return _tileArray[indexx][indexy];
}


void
MapBackground::dumpTileGrid ()
{
    for (int j=0; j<_numTilesY; ++j) {
        for (int i=0; i<_numTilesX; ++i) {
            cout << ((_tileArray[i][j] == 0) ? "." : "X");
        }
        cout << endl;
    }


}



}
}

