
#include "cReseau.h"


cReseau::cReseau(const size_t& id)
: _id(id)
{ }

cReseau::~cReseau()
{}

void cReseau::setNom(const std::string &newVal)
{
	vNom = newVal;
}


void cReseau::setDoc(const std::string &newVal)
{
	vDoc = newVal;
}


void cReseau::setURL(const std::string &newVal)
{
	vURL = newVal;
}

const size_t& cReseau::getId() const
{
	return _id;
}
