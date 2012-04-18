
/** LexicalMatcherTest class implementation.
	@file LexicalMatcherTest.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "07_lexical_matcher/LexicalMatcher.h"

#include <iostream>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::lexical_matcher;
using namespace std;

std::vector<std::string>
getTestBase1 ()
{
    std::vector<std::string> testBase;
    testBase.push_back ("Abreuvoir, rue de l'");
    testBase.push_back ("Anicenne Mare, cour de l'");
    testBase.push_back ("Boulangers, chemin des");
    testBase.push_back ("Buc, rue de");
    testBase.push_back ("Charles de Gaulle, rue");
    testBase.push_back ("Chateau, allée du");
    testBase.push_back ("Cimetière, chemin du");
    testBase.push_back ("Cocanne, rue de la");
    testBase.push_back ("Commanderie, rue de la");
    testBase.push_back ("Côtes Montbron, chemin des");
    testBase.push_back ("Couronne, rue de la");
    testBase.push_back ("Croix Blanche, rue de la");
    testBase.push_back ("Croix des Mortiers, rue de la");
    testBase.push_back ("Division Leclerc, rue de la");
    testBase.push_back ("Ferme de l'hopital, rue de la");
    testBase.push_back ("Folie, rue de la");
    testBase.push_back ("Fontaine, sente de la");
    testBase.push_back ("Garenne, rue de la");
    testBase.push_back ("Georges Guynemer, rue");
    testBase.push_back ("Grande Rue");
    testBase.push_back ("Guy Mocquet, rue");
    testBase.push_back ("Haies, rue des");
    testBase.push_back ("Haras de Vauptain, route du");
    testBase.push_back ("Jean Mermoz, rue");
    testBase.push_back ("Jouy, route de");
    testBase.push_back ("Juvinière, rue de la");
    testBase.push_back ("Logeraie, allée de la");
    testBase.push_back ("Louis Grenier, place");
    testBase.push_back ("Mansart, rue");
    testBase.push_back ("Marroniers, allée des");
    testBase.push_back ("Maryse Bastié, allée");
    testBase.push_back ("Midori, rue de");
    testBase.push_back ("Monument, place du");
    testBase.push_back ("Orme Rond, chemin de l'");
    testBase.push_back ("Pépinière, rue de la");
    testBase.push_back ("Petit Jouy, rue du");
    testBase.push_back ("Peupliers, allée des");
    testBase.push_back ("Pointe, rue de la");
    testBase.push_back ("Portes des Loges, chemin de la");
    testBase.push_back ("Poste, rue de la");
    testBase.push_back ("Saint Exupéry, rue");
    testBase.push_back ("Sophoras, allée des");
    testBase.push_back ("Tilleuls, rue des");
    testBase.push_back ("Trou Salé, rue du");
    testBase.push_back ("Vallée, chemin de la");
    testBase.push_back ("Vauptain, chemin de");
    return testBase;
}





BOOST_AUTO_TEST_CASE (testBasicMatches)
{
    LexicalMatcher<int> matcher;
    std::vector<std::string> testBase = getTestBase1 ();
    for (size_t i=0; i<testBase.size (); ++i) matcher.add (testBase[i], 0);

    BOOST_CHECK_EQUAL (FrenchSentence("Peupliers, allée des").getPhoneticString(), matcher.bestMatch ("all. peuplier").key.getPhoneticString());
    BOOST_CHECK_EQUAL (FrenchSentence("Peupliers, allée des").getPhoneticString(), matcher.bestMatch ("    all   .   peuplier   ").key.getPhoneticString());
    BOOST_CHECK_EQUAL (FrenchSentence("Peupliers, allée des").getPhoneticString(), matcher.bestMatch ("peupier").key.getPhoneticString());
    BOOST_CHECK_EQUAL (FrenchSentence("Peupliers, allée des").getPhoneticString(), matcher.bestMatch ("peupier allée").key.getPhoneticString());
    BOOST_CHECK_EQUAL (FrenchSentence("Peupliers, allée des").getPhoneticString(), matcher.bestMatch ("PeuPlieER").key.getPhoneticString());

    BOOST_CHECK_EQUAL (FrenchSentence("Division Leclerc, rue de la").getPhoneticString(), matcher.bestMatch ("rue div leclerc").key.getPhoneticString());
    BOOST_CHECK_EQUAL (FrenchSentence("Division Leclerc, rue de la").getPhoneticString(), matcher.bestMatch ("ruedivleclerc").key.getPhoneticString());
    BOOST_CHECK_EQUAL (FrenchSentence("Division Leclerc, rue de la").getPhoneticString(), matcher.bestMatch ("lecler").key.getPhoneticString());

    BOOST_CHECK_EQUAL (FrenchSentence("Sophoras, allée des").getPhoneticString(), matcher.bestMatch ("soforas").key.getPhoneticString());


    BOOST_CHECK_EQUAL (FrenchSentence("Portes des Loges, chemin de la").getPhoneticString(), matcher.bestMatch ("loge porte chemin").key.getPhoneticString());

}



BOOST_AUTO_TEST_CASE (testWholeWordMatchingCriterium)
{
    {
	std::vector<std::string> testBase;
	testBase.push_back ("ANPE trucmuche");
	testBase.push_back ("ANPE bidule");
	testBase.push_back ("Anges");


	LexicalMatcher<int> matcher;
	for (size_t i=0; i<testBase.size (); ++i) matcher.add (testBase[i], 0);

	BOOST_CHECK_EQUAL (FrenchSentence("ANPE bidule").getPhoneticString(), matcher.bestMatch ("ANPE").key.getPhoneticString());
    }
    {
	std::vector<std::string> testBase;
	testBase.push_back ("Rue Sainte-Marie");
	testBase.push_back ("Rue de Saint-Cyprien");
	testBase.push_back ("Rue Saint-Cyr");


	LexicalMatcher<int> matcher;
	for (size_t i=0; i<testBase.size (); ++i) matcher.add (testBase[i], 0);

	BOOST_CHECK_EQUAL (FrenchSentence("Rue de Saint-Cyprien").getPhoneticString(), matcher.bestMatch ("rue saint-siprien").key.getPhoneticString());
    }
}

BOOST_AUTO_TEST_CASE(comparison)
{
	FrenchSentence sanAntonio("S. Antonio (Val Morobbia) Arrêt");
	FrenchSentence valMorobbia("val morobbia");
	FrenchSentence cerneuxEtagesOK("Le Cerneux-Péquignot Etages");
	FrenchSentence cerneuxEtajes("Le cerneux pequignaux les etages");
	FrenchSentence cerneuxEtagesWithMistake("Le cerneux pequegnaux les etages");

	FrenchSentence::ComparisonScore sA_cE(sanAntonio.compare(cerneuxEtajes));
	BOOST_CHECK_CLOSE(sA_cE.phoneticScore, 0.359, 1);
	BOOST_CHECK_EQUAL(sA_cE.levenshtein, 29);

	FrenchSentence::ComparisonScore sA_vM(sanAntonio.compare(valMorobbia));
	BOOST_CHECK_EQUAL(sA_vM.phoneticScore, 1);
	BOOST_CHECK_EQUAL(sA_vM.levenshtein, 20);

	FrenchSentence::ComparisonScore sA_cEM(sanAntonio.compare(cerneuxEtagesWithMistake));
	BOOST_CHECK_CLOSE(sA_cEM.phoneticScore, 0.359, 1);
	BOOST_CHECK_EQUAL(sA_cEM.levenshtein, 29);

	FrenchSentence::ComparisonScore cEM_sA(cerneuxEtagesWithMistake.compare(sanAntonio));
	BOOST_CHECK_CLOSE(cEM_sA.phoneticScore, 0.114, 1);
	BOOST_CHECK_EQUAL(cEM_sA.levenshtein, 29);

	FrenchSentence::ComparisonScore cE_cEM(cerneuxEtajes.compare(cerneuxEtagesWithMistake));
	BOOST_CHECK_CLOSE(cE_cEM.phoneticScore, 0.952, 1);
	BOOST_CHECK_EQUAL(cE_cEM.levenshtein, 1);

	FrenchSentence::ComparisonScore cEM_cE(cerneuxEtagesWithMistake.compare(cerneuxEtajes));
	BOOST_CHECK_CLOSE(cEM_cE.phoneticScore, 0.952, 1);
	BOOST_CHECK_EQUAL(cEM_cE.levenshtein, 1);

	FrenchSentence::ComparisonScore cEM_cEOK(cerneuxEtagesWithMistake.compare(cerneuxEtagesOK));
	BOOST_CHECK_CLOSE(cEM_cEOK.phoneticScore, 0.952, 1);
	BOOST_CHECK_EQUAL(cEM_cEOK.levenshtein, 11);

	LexicalMatcher<int> lexmatcher;
	lexmatcher.add(sanAntonio.getSource(), 0);
	lexmatcher.add(cerneuxEtagesOK.getSource(), 2);

	string key(cerneuxEtagesWithMistake.getSource());
	LexicalMatcher<int>::MatchHit result(lexmatcher.bestMatch(key));
	BOOST_CHECK_EQUAL(result.value, 2);
}

