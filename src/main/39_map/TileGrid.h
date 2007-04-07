#ifndef SYNTHESE_CARTO_TILEGRID_H
#define SYNTHESE_CARTO_TILEGRID_H

#include <vector>


namespace synthese
{
namespace env
{
    class Point;
}


namespace map
{




/** This class is aimed at maintaining a grid
   of non painted tiles. 
   The grid is generated here and then dumped in the postscript file
   for interrogation when drawing non overlapping objects.
	@ingroup m39
*/
class TileGrid
{
private:

    double _step;
    double _tileWidth;
    double _tileHeight;

    std::vector<std::vector<bool> > _tiles;
	
protected:

public:

    TileGrid (double width, 
		double height,
		double tileWidth,
		double tileHeight);

    ~TileGrid ();

    bool isMarked (int tileX, int tileY) const;
    
	
    void markTilesForPoint (const synthese::env::Point& p);

    void markTilesForLine (const synthese::env::Point& from, 
			   const synthese::env::Point& to);

    void markTilesForRectangle (const synthese::env::Point& from, 
				const synthese::env::Point& to, 
				bool filled = true);
	

    
};

}
}

#endif

