/*!	\file cCommune.cpp
	\brief Impl�mentation classe Commune
*/

#include "cCommune.h"
#include "Interpretor.h"


/** Constructeur.
	@param name Nom de la commune
	@param id ID de la commune
	@author Hugues Romain
	@date 2001-2006
*/
cCommune::cCommune(tIndex const id, std::string name)
  : cPoint()
  , _id(id)
  , _name(name)
  , _mainLogicalPlace(this)
{
	_allPlaces = NULL;
}



/** Recherche de lieux en fonction d'une entrée texte.
	@param n nombre de points d'arret en sortie en cas de doute non controlé
	@param text Texte sur lequel fonder la recherche
	@author Hugues Romain
	@date 2003-2006

	@return Liste des lieux correspondant à la demande. Si un lieu a pu être déterminé sans ambiguité, alors la liste est de taille 1.

	@todo HR Terminer l'implémentation
*/
vector<LogicalPlace*> cCommune::searchLogicalPlaces(std::string name, size_t n) const
{
	// Recherche
	set<interpretor::Result> matches = _logicalPlaces.search(name, n);

	// METTRE ICI UNE DISCUSSION SUR L'AMBIGUITE AVEC CRITERES METIER, AINSI QU'UN FILTRAGE SI LIEUX DONNANT ARRETS IDENTIQUES

	// Sortie
	vector<LogicalPlace*> result;
//	for (size_t i=0; i<matches.size(); i++)
//		result.push_back(_logicalPlaces[matches[i]]);
	return result;
}



/** Ajout de lieu logique.
	@param logicalPlace Lieu logique à ajouter
*/
void cCommune::addLogicalPlace(LogicalPlace* const logicalPlace)
{
	_logicalPlaces.add(logicalPlace->getName(), logicalPlace, logicalPlace->getId());
}


/** Intégration d'un arrêt tout lieu
	@param allPlaces Tout lieu à intégrer
*/
void cCommune::setAtAllPlaces(LogicalPlace* const allPlaces)
{
	// Liaison
	_allPlaces = allPlaces;

	// Mise à jour de tous les lieux logiques
	for (LogicalPlaceInterpretor::MapType::const_iterator iter = _logicalPlaces.getMap().begin();
		iter != _logicalPlaces.getMap().end();
		++iter)
			iter->second->addAliasedLogicalPlace(_allPlaces);
}


/** Destructeur.
*/
cCommune::~cCommune()
{
	delete _allPlaces;
	for (LogicalPlaceInterpretor::MapType::const_iterator iter = _logicalPlaces.getMap().begin();
		iter != _logicalPlaces.getMap().end();
		++iter)
		delete iter->second;
}


const string& cCommune::getName() const
{
	return _name;
}

void cCommune::addToMainLogicalPlace(LogicalPlace* const logicalPlace)
{
	_mainLogicalPlace.addAliasedLogicalPlace(logicalPlace);
}