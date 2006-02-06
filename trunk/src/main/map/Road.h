#ifndef ROAD_H_
#define ROAD_H_

#include "Referrant.h"
#include <string>



namespace synmap
{


class Topography;


 class Road : public Referrant
{
public:
	
private:
	
  const std::string _name;
  const std::string _discriminant;


public:

  Road(Topography* topography,
       int key,
       const std::string& name,
       const std::string& discriminant);
	
  virtual ~Road();

  const std::string& getName () const { return _name; }
  const std::string& getDiscriminant () const { return _discriminant; }


};

}

#endif /*ROAD_H_*/
