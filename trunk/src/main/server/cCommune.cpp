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
cCommune::cCommune(tIndex id, const cEnvironnement& environment, std::string name)
  : cPoint()
  , _id(id)
  , _name(name)
  , _mainLogicalPlace(this)
  , _environment (environment)
  
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
	vector<Interpretor::Result> matches = _logicalPlaces.search(name, n);

	// METTRE ICI UNE DISCUSSION SUR L'AMBIGUITE AVEC CRITERES METIER, AINSI QU'UN FILTRAGE SI LIEUX DONNANT ARRETS IDENTIQUES

	// Sortie
	vector<LogicalPlace*> result;
	for (size_t i=0; i<matches.size(); i++)
		result.push_back(_logicalPlaces[matches[i]]);
	return result;
}



/** Ajout de lieu logique.
	@param logicalPlace Lieu logique à ajouter
*/
void cCommune::addLogicalPlace(const LogicalPlace* logicalPlace)
{
	_logicalPlaces.Ajout(logicalPlace->getName(), logicalPlace);
}


/** Intégration d'un arrêt tout lieu
	@param allPlaces Tout lieu à intégrer
*/
void cCommune::setAtAllPlaces(const LogicalPlace* allPlaces)
{
	// Liaison
	_allPlaces = allPlaces;

	// Mise à jour de tous les lieux logiques
	for (Interpretor::Index i=0; i<_logicalPlaces.size(); i++)
		_logicalPlaces[i]->addAliasedLogicalPlace(_allPlaces);
}


/** Destructeur.
*/
cCommune::~cCommune()
{
	delete _allPlaces;
	for (tIndex i=0; i<_logicalPlaces.size(); i++)
		delete _logicalPlaces[i];
}
