
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
    testBase.push_back ("Ancienne Mare, cour de l'");
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


BOOST_AUTO_TEST_CASE(FrenchPhoneticTest)
{
	std::vector<std::string> testBase = getTestBase1 ();

// 0	testBase.push_back ("Abreuvoir, rue de l'");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[0]).getPhoneticString(), "A-B-R-E-V-O-A-R R-U");
//	testBase.push_back ("Ancienne Mare, cour de l'");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[1]).getPhoneticString(), "AN-S-I-E-N M-A-R K-U-R");
//	testBase.push_back ("Boulangers, chemin des");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[2]).getPhoneticString(), "B-U-L-AN-J-E CH-E-M-AN");
//	testBase.push_back ("Buc, rue de");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[3]).getPhoneticString(), "B-U-K R-U");
//	testBase.push_back ("Charles de Gaulle, rue");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[4]).getPhoneticString(), "CH-A-R-L G-O-L R-U");
//	testBase.push_back ("Chateau, allée du");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[5]).getPhoneticString(), "CH-A-T-O A-L-E");
//	testBase.push_back ("Cimetière, chemin du");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[6]).getPhoneticString(), "S-I-M-E-T-I-E-R CH-E-M-AN");
//	testBase.push_back ("Cocanne, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[7]).getPhoneticString(), "K-O-K-A-N R-U");
//	testBase.push_back ("Commanderie, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[8]).getPhoneticString(), "K-O-M-AN-D-E-R-I R-U");
//	testBase.push_back ("Côtes Montbron, chemin des");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[9]).getPhoneticString(), "K-O-T M-ON-B-R-ON CH-E-M-AN");
// 10	testBase.push_back ("Couronne, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[10]).getPhoneticString(), "K-U-R-O-N R-U");
//	testBase.push_back ("Croix Blanche, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[11]).getPhoneticString(), "K-R-O-A B-L-AN-CH R-U");
//	testBase.push_back ("Croix des Mortiers, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[12]).getPhoneticString(), "K-R-O-A M-O-R-T-I-E R-U");
// 	testBase.push_back ("Division Leclerc, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[13]).getPhoneticString(), "D-I-V-I-Z-I-ON L-E-K-L-E-R R-U");
// 	testBase.push_back ("Ferme de l'hopital, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[14]).getPhoneticString(), "F-E-R-M O-P-I-T-A-L R-U");
// 	testBase.push_back ("Folie, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[15]).getPhoneticString(), "F-O-L-I R-U");
// 	testBase.push_back ("Fontaine, sente de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[16]).getPhoneticString(), "F-ON-T-E-N S-AN-T");
// 	testBase.push_back ("Garenne, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[17]).getPhoneticString(), "G-A-R-E-N R-U");
// 	testBase.push_back ("Georges Guynemer, rue");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[18]).getPhoneticString(), "J-O-R-J G-U-I-N-E-M-E-R R-U");
// 	testBase.push_back ("Grande Rue");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[19]).getPhoneticString(), "G-R-AN-D R-U");
// 20 	testBase.push_back ("Guy Mocquet, rue");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[20]).getPhoneticString(), "G-I M-O-K-E R-U");
// 	testBase.push_back ("Haies, rue des");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[21]).getPhoneticString(), "E R-U");
// 	testBase.push_back ("Haras de Vauptain, route du");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[22]).getPhoneticString(), "A-R-A V-O-P-T-AN R-U-T");
// 	testBase.push_back ("Jean Mermoz, rue");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[23]).getPhoneticString(), "J-AN M-E-R-M-O-Z R-U");
// 	testBase.push_back ("Jouy, route de");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[24]).getPhoneticString(), "J-U-I R-U-T");
// 	testBase.push_back ("Juvinière, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[25]).getPhoneticString(), "J-U-V-I-N-I-E-R R-U");
// 	testBase.push_back ("Logeraie, allée de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[26]).getPhoneticString(), "L-O-J-E-R-E A-L-E");
// 	testBase.push_back ("Louis Grenier, place");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[27]).getPhoneticString(), "L-U-I G-R-E-N-I-E P-L-A-S");
// 	testBase.push_back ("Mansart, rue");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[28]).getPhoneticString(), "M-AN-S-A-R R-U");
// 	testBase.push_back ("Marroniers, allée des");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[29]).getPhoneticString(), "M-A-R-O-N-I-E A-L-E");
// 30 	testBase.push_back ("Maryse Bastié, allée");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[30]).getPhoneticString(), "M-A-R-I-Z B-A-S-T-I-E A-L-E");
// 	testBase.push_back ("Midori, rue de");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[31]).getPhoneticString(), "M-I-D-O-R-I R-U");
// 	testBase.push_back ("Monument, place du");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[32]).getPhoneticString(), "M-O-N-U-M-AN P-L-A-S");
// 	testBase.push_back ("Orme Rond, chemin de l'");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[33]).getPhoneticString(), "O-R-M R-ON CH-E-M-AN");
// 	testBase.push_back ("Pépinière, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[34]).getPhoneticString(), "P-E-P-I-N-I-E-R R-U");
// 	testBase.push_back ("Petit Jouy, rue du");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[35]).getPhoneticString(), "P-E-T-I J-U-I R-U");
// 	testBase.push_back ("Peupliers, allée des");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[36]).getPhoneticString(), "P-E-P-L-I-E A-L-E");
// 	testBase.push_back ("Pointe, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[37]).getPhoneticString(), "P-O-AN-T R-U");
// 	testBase.push_back ("Portes des Loges, chemin de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[38]).getPhoneticString(), "P-O-R-T L-O-J CH-E-M-AN");
// 	testBase.push_back ("Poste, rue de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[39]).getPhoneticString(), "P-O-S-T R-U");
// 40	testBase.push_back ("Saint Exupéry, rue");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[40]).getPhoneticString(), "S-AN E-K-S-U-P-E-R-I R-U");
// 	testBase.push_back ("Sophoras, allée des");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[41]).getPhoneticString(), "S-O-F-O-R-A A-L-E");
// 	testBase.push_back ("Tilleuls, rue des");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[42]).getPhoneticString(), "T-I-E-L R-U");
// 	testBase.push_back ("Trou Salé, rue du");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[43]).getPhoneticString(), "T-R-U S-A-L-E R-U");
// 	testBase.push_back ("Vallée, chemin de la");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[44]).getPhoneticString(), "V-A-L-E CH-E-M-AN");
// 	testBase.push_back ("Vauptain, chemin de");
	BOOST_CHECK_EQUAL (FrenchSentence(testBase[45]).getPhoneticString(), "V-O-P-T-AN CH-E-M-AN");
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
	FrenchSentence lascelle("lascelle");
	FrenchSentence lascelleOK("Lascelle 15096");
	FrenchSentence mandailles("Mandailles-Saint-Julien 15113");

	FrenchSentence::ComparisonScore sA_cE(sanAntonio.compare(cerneuxEtajes));
	BOOST_CHECK_CLOSE(sA_cE.phoneticScore, 0.287, 1);
	BOOST_CHECK_EQUAL(sA_cE.levenshtein, 29ULL);

	FrenchSentence::ComparisonScore sA_vM(sanAntonio.compare(valMorobbia));
	BOOST_CHECK_EQUAL(sA_vM.phoneticScore, 1);
	BOOST_CHECK_EQUAL(sA_vM.levenshtein, 20ULL);

	FrenchSentence::ComparisonScore sA_cEM(sanAntonio.compare(cerneuxEtagesWithMistake));
	BOOST_CHECK_CLOSE(sA_cEM.phoneticScore, 0.287, 1);
	BOOST_CHECK_EQUAL(sA_cEM.levenshtein, 29ULL);

	FrenchSentence::ComparisonScore cEM_sA(cerneuxEtagesWithMistake.compare(sanAntonio));
	BOOST_CHECK_CLOSE(cEM_sA.phoneticScore, 0.083, 1);
	BOOST_CHECK_EQUAL(cEM_sA.levenshtein, 29ULL);

	FrenchSentence::ComparisonScore cE_cEM(cerneuxEtajes.compare(cerneuxEtagesWithMistake));
	BOOST_CHECK_CLOSE(cE_cEM.phoneticScore, 0.952, 1);
	BOOST_CHECK_EQUAL(cE_cEM.levenshtein, 1ULL);

	FrenchSentence::ComparisonScore cEM_cE(cerneuxEtagesWithMistake.compare(cerneuxEtajes));
	BOOST_CHECK_CLOSE(cEM_cE.phoneticScore, 0.952, 1);
	BOOST_CHECK_EQUAL(cEM_cE.levenshtein, 1ULL);

	FrenchSentence::ComparisonScore cEM_cEOK(cerneuxEtagesWithMistake.compare(cerneuxEtagesOK));
	BOOST_CHECK_CLOSE(cEM_cEOK.phoneticScore, 0.952, 1);
	BOOST_CHECK_EQUAL(cEM_cEOK.levenshtein, 11ULL);

	LexicalMatcher<int> lexmatcher;
	lexmatcher.add(sanAntonio.getSource(), 0);
	lexmatcher.add(cerneuxEtagesOK.getSource(), 2);

	string key(cerneuxEtagesWithMistake.getSource());
	LexicalMatcher<int>::MatchHit result(lexmatcher.bestMatch(key));
	BOOST_CHECK_EQUAL(result.value, 2);

	FrenchSentence::ComparisonScore lascelleTest(lascelle.compare(lascelleOK));
	BOOST_CHECK_CLOSE(lascelleTest.phoneticScore, 0.5, 1);

	FrenchSentence::ComparisonScore lascelleTest2(lascelleOK.compare(lascelle));
	BOOST_CHECK_EQUAL(lascelleTest2.phoneticScore, 1);

	LexicalMatcher<int> lexmatcher2;
	lexmatcher2.add(lascelleOK.getSource(), 0);
	lexmatcher2.add(sanAntonio.getSource(), 1);
	string key2(lascelle.getSource());
	LexicalMatcher<int>::MatchHit result2(lexmatcher2.bestMatch(key2));
	BOOST_CHECK_EQUAL(result2.value, 0);
}

