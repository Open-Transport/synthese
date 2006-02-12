/** En-tête classe interpréteur de textes.
	@file Interpretor.h
*/

#ifndef SYNTHESE_INTERPRETOR_H
#define SYNTHESE_INTERPRETOR_H

#include <utility>
#include <string>
#include <vector>
#include "cAssociator.h"

using namespace std;


/** @defgroup m07 07 Interpréteur de textes
	@{

	@section m07integr Intégration du module Interpréteur de textes

	Le module interpréteur de texte implémente une liste permettant la recherche via l'associateur basé sur un réseau de neurones.

	Il est prévu pour être utilisé dans toute liste susceptible d'être interrogée par une requête utilisateur inexacte :
		- Liste des communes dans chaque environnement : cInterpreteur<cCommune*>
		- Liste des arrêts/adresses/lieux dans chaque commune : cInterpreteur<cAccesPADe*>

	Les méthodes de recherche correspondantes doivent être mises à jour pour utiliser l'interpréteur. Les méthodes suivantes sont concernées: 
		- cEnvironnement::TextToCommune
		- cCommune::textToPADe

	Les méthode d'ajout d'élément sont impactées par l'ajout d'une commande _interpreteur.Ajout() en fin de code :
		- cEnvironnement::Enregistre(cCommune*)
		- cCommune::addDesignation

	Les méthodes de suppression seront uitilisées dans le cadre de la mise en service des modifications temps réel de la base.
*/




/** Classe interpréteur de textes (template générique).

	L'interpréteur générique fonctionne comme un map où les objets sont doublement indexés :
		- un inde textuel permet principalement une méthode d'interrogation d'après un texte entré inexact
		- un index numérique simple
	
	Un même élément peut être indexé sous plusieurs indexs textes, mais sous un seul index numérique.

	La recherche d'un élément peut se faire de deux manières :
		- par la fourniture directe de l'index
		- par la recherche préalable de l'index d'après un texte à rechercher

	CR : tu gères tes attributs comme tu veux (avec peut etre une liste normale d'éléments en plus pour s'en servir comme tableau avec opérateur [])
*/
template <class T>
class Interpretor
{
public:

	/** Score d'un résultat (à déplacer dans associator je pense) */
	typedef int Score;

	/** Index */
	typedef int Index;

	/** Résultat */
	typedef pair<Index, Score> Result;

	/** Clé */
	typedef string Key;


protected:
	//! @name Attributs
	//{
		cAssociator		_associator;	//!< Réseau de neurones support de l'interprétation (CR : met le bon typage du dump : il fait index_texte=>index_numerique)
		vector<T>	_vector;		//!< Liste d'objets indexée numériquement
	//}

public:
	//! @name Modificateurs
	//@{
		Index add(const Key& key, const T& element, Index id=INCONNU);
		void erase(const Key& key);
		bool erase(Index id);
	//@}


	//! @name Accesseurs
	//@{
		T& operator[](Index id);
		Index size() const;
		vector<Result> search(const Key&, Index) const;
	//@}

	//!	@name Constructeur et destructeur.
	//@{
		Interpretor();
		~Interpretor();
	//@}

};

/** @} */

#endif