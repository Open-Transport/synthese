

#include "Interpretor.h"

using namespace interpretor;


/** Constructeur.
*/
template <class T>
Interpretor<T>::Interpretor()
{
}

/** Ajout d'élément dans le réseau.

	@param key Clé texte de l'élément
	@param element Elément à ajouter
	@param id Index de l'élément (vide/INCONNU = auto : création d'un index si nouvel élément, rien si élement déjà existant)
	@return tIndex L'index de l'élément ajouté, ICONNU si ajout impossible

	Si l'index est fourni et si il existe, alors l'ajout consiste en l'ajout d'une clé texte dans le réseau vers celui-ci.
	Si l'index est fourni et si il n'existe pas, alors l'ajout consiste en l'ajout de l'élément dans la liste, puis à l'ajout de la clé texte dans le réseau.
	Si l'index n'est pas fourni, et si aucun élément de la liste n'est identique à l'élément fourni, alors l'élément est ajouté en queue de liste
	Si l'index n'est pas fourni et si un élément de la liste est identique, alors une clé texte est ajoutée au réseau vers cet élement

	Dans tous les cas l'index de l'élément est retourné, qu'il ait été créé ou non, sauf en cas de problème ou la valeur de retour est INCONNU (-1).

	@todo Mettre une section critique + CR implémenter le lien réseau neurone
*/
template <class T>
Index Interpretor<T>::add(const Key& key, const T& element, Index id)
{
	// Stockage dans le vecteur
	if (id == INCONNU)
	{
		_vector.push_back(element);
		id = _vector.size() - 1;
	}
	else
		_vector[id] = element;

	// Etablissement du lien dans le réseau de neurone


	return id;
}


/** Suppression d'un lien de correspondance clé => élément.

	La suppression par clé texte ne supprime que le lien texte => élément et non la totalité des liens vers l'élément pointé par la clé texte.
	
	@warning Un élément peut ne plus posséder de clé texte vers celui-ci si la dernière a été supprimée.

	@param __Index Index de l'élément à supprimer
	@return true si Suppression OK (peut être changé)

	@todo CR : Implémenter, ça ne concerne que le réseau de neurone
*/
template <class T>
void Interpretor<T>::erase(const Key& key)
{
	
}


/** Suppression d'élément dans le réseau d'après son index.

	La suppression par élément supprime la totalité des liens vers l'élément.

	@param id Index de l'élément à supprimer
	@return true si Suppression OK (peut être changé)

	@todo CR : faire la partie réseau neurone
*/
template <class T>
bool Interpretor<T>::erase(Index id)
{
	// CR : ici l'effacement de toutes les clés dans le réseau
	
	_vector.erase(id);
}


/** Accesseur d'après l'index numérique
	@param __Index Index de l'objet à accéder
	@return Objet demandé
*/
template <class T>
T& Interpretor<T>::operator[](Index id)
{
	return _vector.operator [](id);
}


/** Nombre d'enregistrements contenus.
*/
template <class T>
Index Interpretor<T>::size () const
{
	return _vector.size();
}


/** Interprétation de texte.

	L'interprétation s'effectue par action de l'associateur sur le réseau de neurones

	(détailler le fonctionnement à l'intérieur du source avec tags doxygen pour prise en compte)

	@param key Texte à interpréter
	@param size Nombre de solutions à produire
	@return couple index élément trouvé / score, trié dans l'ordre drécroissant de score

	@todo CR : A implémenter
*/
template <class T>
vector<Result> Interpretor<T>::search(const Key& key, Index size) const
{
	vector<Result> result;

	return result;
}
