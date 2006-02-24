#include "cTrajets.h"
#include "cMateriel.h"



/*!	\brief Destructeur
	\author Hugues Romain
	\date 2005
	
*/
cTrajets::~cTrajets()
{
}



/*!	\brief Fabrication de la liste des points d'arr�ts pour repr�sentation des trajets dans une fiche horaire
	\author Hugues Romain
	\date 2001-2005

	Le but de la m�thode est de fournir une liste ordonn�e de points d'arr�t de taille minimale d�terminant les lignes du tableau de fiche horaire.

	Exemples de r�sultats apr�s ajout de trajets :

	Pas 0 : Service ABD (cr�ation de B)

	<table class="Tableau" cellspacing="0" cellpadding="5">
 		<tr><td>A</td><td>X</td></tr>
		<tr><td>B</td><td>X</td></tr>
		<tr><td>D</td><td>X</td></tr>
	</table>

	Pas 1 : Service ACD (cr�ation de C)

	<table class="Tableau" cellspacing="0" cellpadding="5">
		<tr><td>A</td><td>X</td><td>X</td></tr>
		<tr><td>B</td><td>X</td><td|</td></tr>
		<tr><td>C</td><td>|</td><td>X</td></tr>
		<tr><td>D</td><td>X</td><td>X</td></tr>
	</table>

	Pas 2 : Service ACBD (modification de l'ordre accept�e : descente de B au rang C+1)

	<table class="Tableau" cellspacing="0" cellpadding="5">
		<tr><td>A</td><td>X</td><td>X</td><td>X</td></tr>
		<tr><td>B</td><td>X</td><td>|</td><td>|</td></tr>
		<tr><td>C</td><td>|</td><td>X</td><td>X</td></tr>
		<tr><td>D</td><td>X</td><td>X</td><td>-</td></tr>
	</table>

	(permutation)

	<table class="Tableau" cellspacing="0" cellpadding="5">
		<tr><td>A</td><td>X</td><td>X</td><td>X</td></tr>
		<tr><td>C</td><td>|</td><td>X</td><td>X</td></tr>
		<tr><td>B</td><td>X</td><td>|</td><td>X</td></tr>
		<tr><td>D</td><td>X</td><td>X</td><td>X</td></tr>
	</table>

	Pas 3 : Service ABCD (modification de l'ordre refus�e : cr�ation d'un second C)

	<table class="Tableau" cellspacing="0" cellpadding="5">
		<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
		<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td></tr>
		<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td></tr>
		<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td></tr>
		<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
	</table>
	
	Pas 4 : Service AB->CD (service continu BC)

	<table class="Tableau" cellspacing="0" cellpadding="5">
		<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
		<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td><td>|</td></tr>
		<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td><td>V</td></tr>
		<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td><td>V</td></tr>
		<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
	</table>
	
	Pas 5 : Service AED (E ins�r� avant B pour ne pas rompre la continuit� BC)
	
	<table class="Tableau" cellspacing="0" cellpadding="5">
		<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
		<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td><td>|</td><td>|</td></tr>
		<tr><td>E</td><td>|</td><td>|</td><td>|</td><td>|</td><td>|</td><td>X</td></tr>
		<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td><td>V</td><td>|</td></tr>
		<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td><td>V</td><td>|</td></tr>
		<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
	</table>
	
Pour chaque trajet, on proc�de donc par balayage dans l'ordre des gares existantes. Si la gare � relier n�est pas trouv�e entre la position de la gare pr�c�dente et la fin, deux solutions :
 - soit la gare n�est pr�sente nulle part (balayage avant la position de la pr�c�dente) auquel cas elle est cr��e et rajout�e � la position de la gare pr�c�dente + 1
 - soit la gare est pr�sente avant la gare pr�c�dente. Dans ce cas, on tente de descendre la ligne de la gare recherch�e au niveau de la position de la gare pr�c�dente + 1. On contr�le sur chacun des trajets pr�c�dents que la chronologie n'en serait pas affect�e. Si elle ne l'est pas, alors la ligne est descendue. Sinon une nouvelle ligne est cr��e.

Contr�le de l'�changeabilit� :

Soit \f$ \delta_{l,c}:(l,c)\mapsto\{{1\mbox{~si~le~trajet~}c\mbox{~dessert~la~ligne~}l\atop 0~sinon} \f$

Deux lignes l et m sont �changeables si et seulement si l'ordre des lignes dont \f$ \delta_{l,c}=1 \f$ pour chaque colonne est respect�.

Cet ordre s�exprime par la propri�t� suivante : Si \f$ \Phi \f$ est la permutation pr�vue, alors

<img width=283 height=27
src="interface.doxygen_fichiers/image008.gif">

Il est donc n�cessaire � la fois de contr�ler la possibilit� de permutation, et de la d�terminer �ventuellement.

<p class=MsoNormal>Si <sub><img width=25 height=24
src="interface.doxygen_fichiers/image009.gif"></sub>est la ligne de la gare
pr�c�demment trouv�e, et <sub><img width=24 height=24
src="interface.doxygen_fichiers/image010.gif"></sub>�l�emplacement de la gare
souhait�e pour permuter, alors les permutations � op�rer ne peuvent concerner
que des lignes comprises entre <sub><img width=24 height=24
src="interface.doxygen_fichiers/image010.gif"></sub>�et <sub><img width=25
height=24 src="interface.doxygen_fichiers/image009.gif"></sub>. En effet, les
autres lignes n�influent pas.</p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal>En premier lieu il est n�cessaire de d�terminer l�ensemble
des lignes � permuter. Cet ensemble est construit en explorant chaque colonne.
Si <sub><img width=16 height=24 src="interface.doxygen_fichiers/image011.gif"></sub>�est
l�ensemble des lignes � permuter pour assurer l�int�grit� des colonnes <sub><img
width=36 height=27 src="interface.doxygen_fichiers/image012.gif"></sub>, on
peut d�finir cet ensemble en fonction du pr�c�dent <sub><img width=25
height=24 src="interface.doxygen_fichiers/image013.gif"></sub>&nbsp;: <sub><img
width=308 height=35 src="interface.doxygen_fichiers/image014.gif"></sub></p>

<p class=MsoNormal>Le but �tant de faire descendre la ligne <sub><img width=24
height=24 src="interface.doxygen_fichiers/image010.gif"></sub>�vers <sub><img
width=25 height=24 src="interface.doxygen_fichiers/image009.gif"></sub>, les
lignes appartenant � L doivent �tre �changeables avec� les positions <sub><img
width=216 height=27 src="interface.doxygen_fichiers/image015.gif"></sub>.
L�ensemble de ces tests doit �tre r�alis�. Au moindre �chec, l�ensemble de la
permutation est rendu impossible.</p>

<p class=MsoNormal>L��changeabilit� binaire entre deux lignes l et m revient �
contr�ler la propri�t�&nbsp;<sub><img width=89 height=28
src="interface.doxygen_fichiers/image016.gif"></sub>.</p>

<p class=MsoNormal>L��changeabilit� totale s��crit donc <sub><img width=145
height=28 src="interface.doxygen_fichiers/image017.gif"></sub></p>

<p class=MsoNormal>L�algorithme est donc le suivant&nbsp;:</p>

<p class=MsoListBullet><span style='font-size:9.0pt;font-family:Wingdings;
color:red'>n<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Construction de L</p>

<p class=MsoListBullet><span style='font-size:9.0pt;font-family:Wingdings;
color:red'>n<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Contr�le d��changeabilit� binaire pour chaque �l�ment de L avec
sa future position</p>

<p class=MsoListBullet><span style='font-size:9.0pt;font-family:Wingdings;
color:red'>n<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Permutation</p>

<p class=MsoNormal><b>Echange</b>&nbsp;:</p>

<p class=MsoNormal>Exemple d��change&nbsp;:</p>

<p class=MsoNormal>&nbsp;</p>

<table class=MsoNormalTable border=1 cellspacing=0 cellpadding=0 width=340
 style='width:254.95pt;margin-left:141.6pt;border-collapse:collapse;border:
 none'>
 <tr>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=29 valign=top style='width:21.65pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>X</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=29 valign=top style='width:21.65pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>X</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=29 valign=top style='width:21.65pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>X</p>
  </td>
 </tr>
</table>

<p class=MsoNormal><span style='position:relative;z-index:16'><span
style='left:0px;position:absolute;left:398px;top:-1px;width:67px;height:53px'><img
width=67 height=53 src="interface.doxygen_fichiers/image018.gif"></span></span><span
style='position:relative;z-index:13'><span style='left:0px;position:absolute;
left:371px;top:-1px;width:67px;height:52px'><img width=67 height=52
src="interface.doxygen_fichiers/image019.gif"></span></span><span
style='position:relative;z-index:12'><span style='left:0px;position:absolute;
left:349px;top:-1px;width:62px;height:53px'><img width=62 height=53
src="interface.doxygen_fichiers/image020.gif"></span></span><span
style='position:relative;z-index:11'><span style='left:0px;position:absolute;
left:322px;top:-1px;width:69px;height:52px'><img width=69 height=52
src="interface.doxygen_fichiers/image021.gif"></span></span><span
style='position:relative;z-index:10'><span style='left:0px;position:absolute;
left:269px;top:-1px;width:97px;height:53px'><img width=97 height=53
src="interface.doxygen_fichiers/image022.gif"></span></span><span
style='position:relative;z-index:14'><span style='left:0px;position:absolute;
left:455px;top:-1px;width:37px;height:51px'><img width=37 height=51
src="interface.doxygen_fichiers/image023.gif"></span></span><span
style='position:relative;z-index:15'><span style='left:0px;position:absolute;
left:482px;top:-1px;width:33px;height:51px'><img width=33 height=51
src="interface.doxygen_fichiers/image024.gif"></span></span><span
style='position:relative;z-index:6'><span style='left:0px;position:absolute;
left:248px;top:-1px;width:262px;height:53px'><img width=262 height=53
src="interface.doxygen_fichiers/image025.gif"></span></span><span
style='position:relative;z-index:5'><span style='left:0px;position:absolute;
left:221px;top:-1px;width:206px;height:53px'><img width=206 height=53
src="interface.doxygen_fichiers/image026.gif"></span></span><span
style='position:relative;z-index:7'><span style='left:0px;position:absolute;
left:242px;top:-1px;width:97px;height:52px'><img width=97 height=52
src="interface.doxygen_fichiers/image027.gif"></span></span><span
style='position:relative;z-index:9'><span style='left:0px;position:absolute;
left:216px;top:-1px;width:96px;height:52px'><img width=96 height=52
src="interface.doxygen_fichiers/image028.gif"></span></span><span
style='position:relative;z-index:8'><span style='left:0px;position:absolute;
left:193px;top:-1px;width:96px;height:52px'><img width=96 height=52
src="interface.doxygen_fichiers/image029.gif"></span></span><span
style='position:relative;z-index:4'><span style='left:0px;position:absolute;
left:194px;top:-1px;width:103px;height:52px'><img width=103 height=52
src="interface.doxygen_fichiers/image030.gif"></span></span></p>

<p class=MsoNormal>&nbsp;</p>

<table class=MsoNormalTable border=1 cellspacing=0 cellpadding=0 width=340
 style='width:254.95pt;margin-left:141.6pt;border-collapse:collapse;border:
 none'>
 <tr>
  <td width=29 valign=top style='width:21.65pt;border:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>X</p>
  </td>
  <td width=29 valign=top style='width:21.65pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>X</p>
  </td>
  <td width=29 valign=top style='width:21.65pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>X</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
  <td width=25 valign=top style='width:19.0pt;border:solid windowtext 1.0pt;
  border-left:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=Tableau>-</p>
  </td>
 </tr>
</table>

*/
void cTrajets::GenererListeOrdonneePointsArrets()
{
	// Variables locales
	size_t i;
	size_t dernieri;

	// Allocation
	_LigneVerrouillee.clear();
	_ListeOrdonneePointsArret.clear();
	
	// Boucle horizontale
	for (size_t __Col=0; __Col< Taille(); __Col++)
	{
		i = 0;
		dernieri = -1;

		// Boucle verticale
		for(cElementTrajet* curET = _Element[__Col].getPremierElement(); curET != NULL; curET = curET->getSuivant())
		{
			// Recherche de la gare � partir de la gare pr�c�dente
			if (OrdrePARechercheGare(i, curET->getOrigin()->getLogicalPlace()))
			{
				if (i<dernieri)
					i = OrdrePAEchangeSiPossible(dernieri, i);
			}
			else
				i = OrdrePAInsere(curET->getOrigin()->getLogicalPlace(), dernieri+1);
			dernieri = i;
			i++;

			// Controle gare suivante pour trajet � pied
			if (curET->getLigne()->Materiel()->Code() == MATERIELPied && _ListeOrdonneePointsArret[i] != curET->getDestination()->getLogicalPlace() && curET->getSuivant() != NULL)
			{
				if (OrdrePARechercheGare(i, curET->getDestination()->getLogicalPlace()))
				{
					OrdrePAEchangeSiPossible(dernieri, i);
					i = dernieri + 1;
				}
				else
				{
					i = dernieri + 1;
					OrdrePAInsere(curET->getDestination()->getLogicalPlace(), i);
				}
				_LigneVerrouillee.insert(_LigneVerrouillee.begin()+ i, true);
			}
		}
	}

	// Ajout de la destination finale en fin de tableau
	if (Taille())
		_ListeOrdonneePointsArret.push_back(operator[](0).getDestination()->getLogicalPlace());
}


/*!	\brief Contr�le de la compatibilit� entre l'ordre des arr�ts dans la grille horaire et les arr�ts du trajet
*/
void cTrajets::OrdrePAConstruitLignesAPermuter(const cTrajet& __TrajetATester, bool* Resultat, size_t LigneMax) const
{
	const cElementTrajet* curET = __TrajetATester.PremierElement();
	for (int i = 0; _ListeOrdonneePointsArret[i] != NULL && i<= LigneMax; i++)
		if (curET != NULL && _ListeOrdonneePointsArret[i] == curET->getOrigin()->getLogicalPlace())
		{
			Resultat[i]=true;
			curET = curET->getSuivant();
		}
		else
			Resultat[i]=false;
}


//! \todo A revoir
size_t cTrajets::OrdrePAEchangeSiPossible(size_t PositionActuelle, size_t PositionGareSouhaitee)
{
	bool* LignesAPermuter = (bool*) calloc(PositionActuelle+1,sizeof(bool));
	bool* curLignesET = (bool*) malloc((PositionActuelle+1)*sizeof(bool));
	bool Echangeable = true;
	LogicalPlace* tempGare;
	size_t i;
	size_t j;
	
	// Construction de l'ensemble des lignes a permuter
	LignesAPermuter[PositionActuelle]=true;
	for (int __i=0; __i<Taille(); __i++)
	{
		OrdrePAConstruitLignesAPermuter(getElement(__i), curLignesET, PositionActuelle);
		for (i=PositionActuelle; i>PositionGareSouhaitee; i--)
			if (curLignesET[i] && LignesAPermuter[i])
				break;
		for (; i>PositionGareSouhaitee; i--)
			if (curLignesET[i])
				LignesAPermuter[i]=true;
	}

	// Tests d'�changeabilit� binaire
	// A la premiere contradiction on s'arrete
	for (size_t __i=0; __i<Taille(); __i++)
	{
		OrdrePAConstruitLignesAPermuter(getElement(__i), curLignesET, PositionActuelle);
		i = PositionGareSouhaitee;
		for (j=PositionGareSouhaitee; true; j++)
		{
			for(; !LignesAPermuter[i]; i++)
			{ }
			
			if (i>PositionActuelle)
				break;

			if (curLignesET[i] && curLignesET[j] && !LignesAPermuter[j])
			{
				Echangeable = false;
				break;
			}
			i++;
		}
		if (!Echangeable)
			break;
	}

	// Echange ou insertion
	if (Echangeable)
	{
		for (j=0; true; j++)
		{
			for (i=j; !LignesAPermuter[i] && i<=PositionActuelle; i++)
			{ }

			if (i>PositionActuelle)
				break;

			LignesAPermuter[i]=false;

			tempGare = _ListeOrdonneePointsArret[i];
			for (; i>PositionGareSouhaitee+j; i--)
				_ListeOrdonneePointsArret[i-1] = _ListeOrdonneePointsArret[i];
			_ListeOrdonneePointsArret[i] = tempGare;
		}
		return PositionGareSouhaitee+j;
	}
	else
		return OrdrePAInsere(_ListeOrdonneePointsArret[PositionGareSouhaitee], PositionActuelle+1);
}



/*!	\brief Insertion d'un arr�t de passage dans la liste des arr�ts d'une fiche horaire
	\param ArretLogique Arr�t � ins�rer
	\param Position Position minimale � donner � l'arr�t

	L'insertion d�cale les arr�ts suivants une ligne plus bas. Si un trajet pi�ton (repr�sent� par deux fl�ches devant �tre attenantes) se trouve � la position demand�e, alors l'arr�t est plac� en suivant pour ne pas rompre le cheminement pi�ton.
*/
size_t cTrajets::OrdrePAInsere(LogicalPlace* const ArretLogique, size_t Position)
{
	// Saut de ligne v�rouill�e par un cheminement pi�ton
	for (; Position<_LigneVerrouillee.size() && _LigneVerrouillee[Position]; Position++);

	// D�calage des arr�ts suivants
	for (size_t i=_ListeOrdonneePointsArret.size(); i>Position; i--)
		_ListeOrdonneePointsArret[i-1] = _ListeOrdonneePointsArret[i];

	// Stockage de l'arr�t demand�
	_ListeOrdonneePointsArret[Position] = ArretLogique;
	
	// Retour de la position choisie
	return Position;
}



/*!	\brief Recherche de point d'arr�t dans la liste des points d'arr�t
*/
bool cTrajets::OrdrePARechercheGare(size_t& i, LogicalPlace* const GareAChercher)
{
	// Recherche de la gare en suivant � partir de la position i
	for (; i<_ListeOrdonneePointsArret.size() && _ListeOrdonneePointsArret[i] != NULL && _ListeOrdonneePointsArret[i] != GareAChercher; ++i);

	// Gare trouv�e en suivant avant la fin du tableau
	if (i < _ListeOrdonneePointsArret.size() && _ListeOrdonneePointsArret[i] != NULL)
		return true;

	// Recherche de position ant�rieure � i
	for (i = 0; i < _ListeOrdonneePointsArret.size() && _ListeOrdonneePointsArret[i] != NULL && _ListeOrdonneePointsArret[i] != GareAChercher; ++i);

	return i <_ListeOrdonneePointsArret.size() && _ListeOrdonneePointsArret[i] != NULL;
}



/*!	\brief Cr�ation des niveaux d'alerte des trajets en fonction des donn�es lignes et arr�ts
	\author Hugues Romain
	\date 2005
*/
void cTrajets::GenererNiveauxEtAuMoinsUneAlerte()
{
	// Variables locales
	bool __AuMoinsUneAlerte = false;
	
	// Calcul des niveaux d'alerte pour chaque trajet et collecte du r�sultat
	for (int __i=0; __i<Taille(); __i++)
		__AuMoinsUneAlerte = getElement(__i).GenererNiveauxAlerte() || __AuMoinsUneAlerte;
	
	// Stockage du r�sultat final de la liste de trajets sous format texte pour exploitabilit� directe par module d'interface
	if (__AuMoinsUneAlerte)
		_AuMoinsUneAlerte = "1";
	else
		_AuMoinsUneAlerte.Vide();
}



/** Lancement des traitements permettant d'offrir l'ensemble des donn�es pr�tes � utiliser.
	\author Hugues Romain
	\date 2005
*/
void cTrajets::Finalise()
{
	GenererListeOrdonneePointsArrets();
	GenererNiveauxEtAuMoinsUneAlerte();
	_txtTaille.Vide();
	_txtTaille << Taille();
}

/** Accesseur propri�t� il y a une alerte pour au moins un �l�ment de la collection de trajet.
	@return propri�t� il y a une alerte pour au moins un �l�ment de la collection de trajet
*/
const cTexte& cTrajets::getAuMoinsUneAlerte() const
{
	return _AuMoinsUneAlerte;
}

size_t cTrajets::TailleListeOrdonneePointsArret() const
{
	return _ListeOrdonneePointsArret.size();
}

LogicalPlace* cTrajets::getListeOrdonneePointsArret(size_t __i) const
{
	return _ListeOrdonneePointsArret[__i];
}

const cTexte& cTrajets::GetTailleTexte() const
{
	return _txtTaille;
}
