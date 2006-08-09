#include "cTrajets.h"

#include "33_route_planner/JourneyLeg.h"
#include "15_env/Vertex.h"


using synthese::routeplanner::JourneyLeg;
using synthese::env::Vertex;
using synthese::env::ConnectionPlace;



/*! \brief Destructeur
 \author Hugues Romain
 \date 2005
 
*/
cTrajets::~cTrajets()
{}



/*! \brief Fabrication de la liste des points d'arrï¿½ts pour reprï¿½sentation des trajets dans une fiche horaire
 \author Hugues Romain
 \date 2001-2005
 
 Le but de la mï¿½thode est de fournir une liste ordonnï¿½e de points d'arrï¿½t de taille minimale dï¿½terminant les lignes du tableau de fiche horaire.
 
 Exemples de rï¿½sultats aprï¿½s ajout de trajets :
 
 Pas 0 : Service ABD (crï¿½ation de B)
 
 <table class="Tableau" cellspacing="0" cellpadding="5">
   <tr><td>A</td><td>X</td></tr>
  <tr><td>B</td><td>X</td></tr>
  <tr><td>D</td><td>X</td></tr>
 </table>
 
 Pas 1 : Service ACD (crï¿½ation de C)
 
 <table class="Tableau" cellspacing="0" cellpadding="5">
  <tr><td>A</td><td>X</td><td>X</td></tr>
  <tr><td>B</td><td>X</td><td|</td></tr>
  <tr><td>C</td><td>|</td><td>X</td></tr>
  <tr><td>D</td><td>X</td><td>X</td></tr>
 </table>
 
 Pas 2 : Service ACBD (modification de l'ordre acceptï¿½e : descente de B au rang C+1)
 
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
 
 Pas 3 : Service ABCD (modification de l'ordre refusï¿½e : crï¿½ation d'un second C)
 
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
 
 Pas 5 : Service AED (E insï¿½rï¿½ avant B pour ne pas rompre la continuitï¿½ BC)
 
 <table class="Tableau" cellspacing="0" cellpadding="5">
  <tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
  <tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td><td>|</td><td>|</td></tr>
  <tr><td>E</td><td>|</td><td>|</td><td>|</td><td>|</td><td>|</td><td>X</td></tr>
  <tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td><td>V</td><td>|</td></tr>
  <tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td><td>V</td><td>|</td></tr>
  <tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
 </table>
 
Pour chaque trajet, on procï¿½de donc par balayage dans l'ordre des gares existantes. Si la gare ï¿½ relier nï¿½est pas trouvï¿½e entre la position de la gare prï¿½cï¿½dente et la fin, deux solutions :
 - soit la gare nï¿½est prï¿½sente nulle part (balayage avant la position de la prï¿½cï¿½dente) auquel cas elle est crï¿½ï¿½e et rajoutï¿½e ï¿½ la position de la gare prï¿½cï¿½dente + 1
 - soit la gare est prï¿½sente avant la gare prï¿½cï¿½dente. Dans ce cas, on tente de descendre la ligne de la gare recherchï¿½e au niveau de la position de la gare prï¿½cï¿½dente + 1. On contrï¿½le sur chacun des trajets prï¿½cï¿½dents que la chronologie n'en serait pas affectï¿½e. Si elle ne l'est pas, alors la ligne est descendue. Sinon une nouvelle ligne est crï¿½ï¿½e.
 
Contrï¿½le de l'ï¿½changeabilitï¿½ :
 
Soit \f$ \delta_{l,c}:(l,c)\mapsto\{{1\mbox{~si~le~trajet~}c\mbox{~dessert~la~ligne~}l\atop 0~sinon} \f$
 
Deux lignes l et m sont ï¿½changeables si et seulement si l'ordre des lignes dont \f$ \delta_{l,c}=1 \f$ pour chaque colonne est respectï¿½.
 
Cet ordre sï¿½exprime par la propriï¿½tï¿½ suivante : Si \f$ \Phi \f$ est la permutation prï¿½vue, alors
 
<img width=283 height=27
src="interface.doxygen_fichiers/image008.gif">
 
Il est donc nï¿½cessaire ï¿½ la fois de contrï¿½ler la possibilitï¿½ de permutation, et de la dï¿½terminer ï¿½ventuellement.
 
<p class=MsoNormal>Si <sub><img width=25 height=24
src="interface.doxygen_fichiers/image009.gif"></sub>est la ligne de la gare
prï¿½cï¿½demment trouvï¿½e, et <sub><img width=24 height=24
src="interface.doxygen_fichiers/image010.gif"></sub>ï¿½lï¿½emplacement de la gare
souhaitï¿½e pour permuter, alors les permutations ï¿½ opï¿½rer ne peuvent concerner
que des lignes comprises entre <sub><img width=24 height=24
src="interface.doxygen_fichiers/image010.gif"></sub>ï¿½et <sub><img width=25
height=24 src="interface.doxygen_fichiers/image009.gif"></sub>. En effet, les
autres lignes nï¿½influent pas.</p>
 
<p class=MsoNormal>&nbsp;</p>
 
<p class=MsoNormal>En premier lieu il est nï¿½cessaire de dï¿½terminer lï¿½ensemble
des lignes ï¿½ permuter. Cet ensemble est construit en explorant chaque colonne.
Si <sub><img width=16 height=24 src="interface.doxygen_fichiers/image011.gif"></sub>ï¿½est
lï¿½ensemble des lignes ï¿½ permuter pour assurer lï¿½intï¿½gritï¿½ des colonnes <sub><img
width=36 height=27 src="interface.doxygen_fichiers/image012.gif"></sub>, on
peut dï¿½finir cet ensemble en fonction du prï¿½cï¿½dent <sub><img width=25
height=24 src="interface.doxygen_fichiers/image013.gif"></sub>&nbsp;: <sub><img
width=308 height=35 src="interface.doxygen_fichiers/image014.gif"></sub></p>
 
<p class=MsoNormal>Le but ï¿½tant de faire descendre la ligne <sub><img width=24
height=24 src="interface.doxygen_fichiers/image010.gif"></sub>ï¿½vers <sub><img
width=25 height=24 src="interface.doxygen_fichiers/image009.gif"></sub>, les
lignes appartenant ï¿½ L doivent ï¿½tre ï¿½changeables avecï¿½ les positions <sub><img
width=216 height=27 src="interface.doxygen_fichiers/image015.gif"></sub>.
Lï¿½ensemble de ces tests doit ï¿½tre rï¿½alisï¿½. Au moindre ï¿½chec, lï¿½ensemble de la
permutation est rendu impossible.</p>
 
<p class=MsoNormal>Lï¿½ï¿½changeabilitï¿½ binaire entre deux lignes l et m revient ï¿½
contrï¿½ler la propriï¿½tï¿½&nbsp;<sub><img width=89 height=28
src="interface.doxygen_fichiers/image016.gif"></sub>.</p>
 
<p class=MsoNormal>Lï¿½ï¿½changeabilitï¿½ totale sï¿½ï¿½crit donc <sub><img width=145
height=28 src="interface.doxygen_fichiers/image017.gif"></sub></p>
 
<p class=MsoNormal>Lï¿½algorithme est donc le suivant&nbsp;:</p>
 
<p class=MsoListBullet><span style='font-size:9.0pt;font-family:Wingdings;
color:red'>n<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Construction de L</p>
 
<p class=MsoListBullet><span style='font-size:9.0pt;font-family:Wingdings;
color:red'>n<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Contrï¿½le dï¿½ï¿½changeabilitï¿½ binaire pour chaque ï¿½lï¿½ment de L avec
sa future position</p>
 
<p class=MsoListBullet><span style='font-size:9.0pt;font-family:Wingdings;
color:red'>n<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;
</span></span>Permutation</p>
 
<p class=MsoNormal><b>Echange</b>&nbsp;:</p>
 
<p class=MsoNormal>Exemple dï¿½ï¿½change&nbsp;:</p>
 
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
    for ( size_t __Col = 0; __Col < Taille(); __Col++ )
    {
        i = 0;
        dernieri = -1;

        // Boucle verticale
        for (int l = 0; l < _Element[__Col].getJourneyLegCount (); ++l)
	{
	    const JourneyLeg * curET = _Element[__Col].getJourneyLeg (l);

            // Recherche de la gare a partir de la gare precedente
            if ( OrdrePARechercheGare( i, curET->getOrigin() ->getConnectionPlace() ) )
            {
                if ( i < dernieri )
                    i = OrdrePAEchangeSiPossible( dernieri, i );
            }
            else
	    {
                i = OrdrePAInsere( curET->getOrigin() ->getConnectionPlace(), dernieri + 1 );
	    }

            dernieri = i;
            i++;

            // Controle gare suivante pour trajet a pied
            if ( /* MJ que deviennent les lignes à pied ??? curET->getLigne() ->Materiel() ->Code() == MATERIELPied && */   _ListeOrdonneePointsArret[ i ] != curET->getDestination() ->getConnectionPlace() && (l != _Element[__Col].getJourneyLegCount ()-1) )
            {
                if ( OrdrePARechercheGare( i, curET->getDestination() ->getConnectionPlace() ) )
                {
                    OrdrePAEchangeSiPossible( dernieri, i );
                    i = dernieri + 1;
                }
                else
                {
                    i = dernieri + 1;
                    OrdrePAInsere( curET->getDestination() ->getConnectionPlace(), i );
                }
                _LigneVerrouillee.insert( _LigneVerrouillee.begin() + i, true );
            }
        }
    }

    // Ajout de la destination finale en fin de tableau
    if ( Taille() )
        _ListeOrdonneePointsArret.push_back( operator[] ( 0 ).getDestination() ->getConnectionPlace() );
}



/*! \brief Contrï¿½le de la compatibilitï¿½ entre l'ordre des arrï¿½ts dans la grille horaire et les arrï¿½ts du trajet
*/
void cTrajets::OrdrePAConstruitLignesAPermuter( const synthese::routeplanner::Journey& __TrajetATester, bool* Resultat, size_t LigneMax ) const
{
    int l = 0;
    const JourneyLeg* curET = (l >= __TrajetATester.getJourneyLegCount ()) ? NULL : __TrajetATester.getJourneyLeg (l);
    for ( int i = 0; _ListeOrdonneePointsArret[ i ] != NULL && i <= LigneMax; i++ )
    {
        if ( curET != NULL && _ListeOrdonneePointsArret[ i ] == curET->getOrigin() ->getConnectionPlace() )
        {
            Resultat[ i ] = true;
	    ++l;
	    curET = (l >= __TrajetATester.getJourneyLegCount ()) ? NULL : __TrajetATester.getJourneyLeg (l);
        }
        else
	{
            Resultat[ i ] = false;
	}
    }
}




//! \todo A revoir
size_t cTrajets::OrdrePAEchangeSiPossible( size_t PositionActuelle, size_t PositionGareSouhaitee )
{
    bool * LignesAPermuter = ( bool* ) calloc( PositionActuelle + 1, sizeof( bool ) );
    bool* curLignesET = ( bool* ) malloc( ( PositionActuelle + 1 ) * sizeof( bool ) );
    bool Echangeable = true;
    const ConnectionPlace* tempGare;
    size_t i;
    size_t j;

    // Construction de l'ensemble des lignes a permuter
    LignesAPermuter[ PositionActuelle ] = true;
    for ( int __i = 0; __i < Taille(); __i++ )
    {
        OrdrePAConstruitLignesAPermuter( getElement( __i ), curLignesET, PositionActuelle );
        for ( i = PositionActuelle; i > PositionGareSouhaitee; i-- )
            if ( curLignesET[ i ] && LignesAPermuter[ i ] )
                break;
        for ( ; i > PositionGareSouhaitee; i-- )
            if ( curLignesET[ i ] )
                LignesAPermuter[ i ] = true;
    }

    // Tests d'ï¿½changeabilitï¿½ binaire
    // A la premiere contradiction on s'arrete
    for ( size_t __i = 0; __i < Taille(); __i++ )
    {
        OrdrePAConstruitLignesAPermuter( getElement( __i ), curLignesET, PositionActuelle );
        i = PositionGareSouhaitee;
        for ( j = PositionGareSouhaitee; true; j++ )
        {
            for ( ; !LignesAPermuter[ i ]; i++ )
            { }

            if ( i > PositionActuelle )
                break;

            if ( curLignesET[ i ] && curLignesET[ j ] && !LignesAPermuter[ j ] )
            {
                Echangeable = false;
                break;
            }
            i++;
        }
        if ( !Echangeable )
            break;
    }

    // Echange ou insertion
    if ( Echangeable )
    {
        for ( j = 0; true; j++ )
        {
            for ( i = j; !LignesAPermuter[ i ] && i <= PositionActuelle; i++ )
            { }

            if ( i > PositionActuelle )
                break;

            LignesAPermuter[ i ] = false;

            tempGare = _ListeOrdonneePointsArret[ i ];
            for ( ; i > PositionGareSouhaitee + j; i-- )
                _ListeOrdonneePointsArret[ i - 1 ] = _ListeOrdonneePointsArret[ i ];
            _ListeOrdonneePointsArret[ i ] = tempGare;
        }
        return PositionGareSouhaitee + j;
    }
    else
        return OrdrePAInsere( _ListeOrdonneePointsArret[ PositionGareSouhaitee ], PositionActuelle + 1 );
}



/*! \brief Insertion d'un arrï¿½t de passage dans la liste des arrï¿½ts d'une fiche horaire
 \param ArretLogique Arrï¿½t ï¿½ insï¿½rer
 \param Position Position minimale ï¿½ donner ï¿½ l'arrï¿½t
 
 L'insertion dï¿½cale les arrï¿½ts suivants une ligne plus bas. Si un trajet piï¿½ton (reprï¿½sentï¿½ par deux flï¿½ches devant ï¿½tre attenantes) se trouve ï¿½ la position demandï¿½e, alors l'arrï¿½t est placï¿½ en suivant pour ne pas rompre le cheminement piï¿½ton.
*/
size_t cTrajets::OrdrePAInsere( const ConnectionPlace* ArretLogique, size_t Position )
{
    // Saut de ligne vï¿½rouillï¿½e par un cheminement piï¿½ton
    for ( ; Position < _LigneVerrouillee.size() && _LigneVerrouillee[ Position ]; Position++ )
        ;

    // Dï¿½calage des arrï¿½ts suivants
    for ( size_t i = _ListeOrdonneePointsArret.size(); i > Position; i-- )
        _ListeOrdonneePointsArret[ i - 1 ] = _ListeOrdonneePointsArret[ i ];

    // Stockage de l'arrï¿½t demandï¿½
    _ListeOrdonneePointsArret[ Position ] = ArretLogique;

    // Retour de la position choisie
    return Position;
}



/*! \brief Recherche de point d'arrï¿½t dans la liste des points d'arrï¿½t
*/
bool cTrajets::OrdrePARechercheGare( size_t& i, const ConnectionPlace* GareAChercher )
{
    // Recherche de la gare en suivant ï¿½ partir de la position i
    for ( ; i < _ListeOrdonneePointsArret.size() && _ListeOrdonneePointsArret[ i ] != NULL && _ListeOrdonneePointsArret[ i ] != GareAChercher; ++i )
        ;

    // Gare trouvï¿½e en suivant avant la fin du tableau
    if ( i < _ListeOrdonneePointsArret.size() && _ListeOrdonneePointsArret[ i ] != NULL )
        return true;

    // Recherche de position antï¿½rieure ï¿½ i
    for ( i = 0; i < _ListeOrdonneePointsArret.size() && _ListeOrdonneePointsArret[ i ] != NULL && _ListeOrdonneePointsArret[ i ] != GareAChercher; ++i )
        ;

    return i < _ListeOrdonneePointsArret.size() && _ListeOrdonneePointsArret[ i ] != NULL;
}



/*! \brief Crï¿½ation des niveaux d'alerte des trajets en fonction des donnï¿½es lignes et arrï¿½ts
 \author Hugues Romain
 \date 2005
*/
void cTrajets::GenererNiveauxEtAuMoinsUneAlerte()
{
    // Variables locales
    bool __AuMoinsUneAlerte = false;

    // Calcul des niveaux d'alerte pour chaque trajet et collecte du resultat
    for ( int __i = 0; __i < Taille(); __i++ )
    {
        __AuMoinsUneAlerte = getElement( __i ).getMaxAlarmLevel () || __AuMoinsUneAlerte;
    }

    // Stockage du resultat final de la liste de trajets sous format texte pour 
    // exploitabilite directe par module d'interface
    if ( __AuMoinsUneAlerte )
        _AuMoinsUneAlerte = "1";
    else
        _AuMoinsUneAlerte.clear();
}



/** Lancement des traitements permettant d'offrir l'ensemble des donnï¿½es prï¿½tes ï¿½ utiliser.
 \author Hugues Romain
 \date 2005
*/
void cTrajets::Finalise()
{
    GenererListeOrdonneePointsArrets();
    GenererNiveauxEtAuMoinsUneAlerte();
    _txtTaille = Taille();
}



/** Accesseur propriï¿½tï¿½ il y a une alerte pour au moins un ï¿½lï¿½ment de la collection de trajet.
 @return propriï¿½tï¿½ il y a une alerte pour au moins un ï¿½lï¿½ment de la collection de trajet
*/
const std::string& 
cTrajets::getAuMoinsUneAlerte() const
{
    return _AuMoinsUneAlerte;
}



size_t 
cTrajets::TailleListeOrdonneePointsArret() const
{
    return _ListeOrdonneePointsArret.size();
}



const ConnectionPlace* 
cTrajets::getListeOrdonneePointsArret ( size_t __i ) const
{
    return _ListeOrdonneePointsArret[ __i ];
}



const std::string& 
cTrajets::GetTailleTexte() const
{
    return _txtTaille;
}


