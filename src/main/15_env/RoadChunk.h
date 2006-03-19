#ifndef SYNTHESE_ENV_ROADCHUNK_H
#define SYNTHESE_ENV_ROADCHUNK_H


#include "Edge.h"



namespace synthese
{
namespace env
{

    class Road;

class RoadChunk : public Edge 
{
private:
  
  const Road* _road;

public:

  RoadChunk (const Road* road);
  
  virtual ~RoadChunk ();


  //! @name Getters/Setters
  //@{
  const Path* getParentPath () const;

  const Vertex* getFrom () const;
  const Vertex* getTo () const;
  //@}
    

  //! @name Query methods.
  //@{
  //@}

};


}
}

#endif 
