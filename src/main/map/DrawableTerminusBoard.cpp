#include "DrawableTerminusBoard.h"

#include "DrawableBusLine.h"

#include <iostream>

using namespace std;


namespace synmap
{

DrawableTerminusBoard::DrawableTerminusBoard(const std::string& caption, 
                                             const Vertex* vertex)
: _caption (caption)
, _vertex (vertex)
{
    
}




DrawableTerminusBoard::~DrawableTerminusBoard()
{
}


    
void 
DrawableTerminusBoard::preDraw (Map& map) const
{
    
}

void 
DrawableTerminusBoard::draw (Map& map) const
{
    for (int i=0; i<_lines.size(); ++i) {
        cout << _lines[i]->getLineNumber() << ",";   
    }
    cout << endl;
}


void 
DrawableTerminusBoard::postDraw (Map& map) const
{
    
}


void 
DrawableTerminusBoard::addDrawableBusLine (const DrawableBusLine* dbl)
{
    _lines.push_back (dbl);
}


    




}
