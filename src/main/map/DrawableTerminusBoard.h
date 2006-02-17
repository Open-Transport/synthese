#ifndef DRAWABLETERMINUSBOARD_H_
#define DRAWABLETERMINUSBOARD_H_

#include "Drawable.h"
#include <vector>
#include <string>


namespace synmap
{

class DrawableBusLine;
class Vertex;

class DrawableTerminusBoard : public Drawable
{
private:
    const std::string _caption;
    const Vertex* _vertex;    
    std::vector<const DrawableBusLine*> _lines;



public:
	DrawableTerminusBoard(const std::string& caption, const Vertex* vertex);
	virtual ~DrawableTerminusBoard();

    void addDrawableBusLine (const DrawableBusLine* dbl);
    
    void preDraw (Map& map) const;
    
    void draw (Map& map) const;

    void postDraw (Map& map) const;
    
    
    
};

}

#endif /*DRAWABLETERMINUSBOARD_H_*/
