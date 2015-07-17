
/** Tree Test implementation.
	@file TreeTest.cpp

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

#include "TreeMultiClassRootPolicy.hpp"
#include "TreeNode.hpp"
#include "TreeOtherClassRootPolicy.hpp"
#include "TreeRoot.hpp"
#include "TreeRankOrderingPolicy.hpp"
#include "TreeUniqueRootPolicy.hpp"

#pragma GCC diagnostic ignored "-Wsign-compare"
#include <boost/test/auto_unit_test.hpp>
#pragma GCC diagnostic pop

using namespace synthese::tree;
using namespace boost;

class TR;
class TN;

class TR:
	public TreeRoot<TN, TreeRankOrderingPolicy>
{

};

class TN:
	public TreeNode<TN, TreeRankOrderingPolicy, TreeOtherClassRootPolicy<TR> >
{

};

BOOST_AUTO_TEST_CASE(Tree_Rank_OtherClassRoot)
{
	TR root;
	BOOST_CHECK(root.getChildren().empty());

	TN node1;
	BOOST_CHECK(root.getChildren().empty());
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(node1.getRoot() == NULL);
//	TODO
//	BOOST_CHECK_THROW(node1.getParent(), InconsistentTreeException);
//	BOOST_CHECK_THROW(node1.getRank(), InconsistentTreeException);
//	BOOST_CHECK_THROW(node1.getDepth(), InconsistentTreeException);
//	BOOST_CHECK_THROW(node1.getPreviousSibling(), InconsistentTreeException);
//	BOOST_CHECK_THROW(node1.getNextSibling(), InconsistentTreeException);

	node1.setRoot(&root);
	BOOST_CHECK(root.getChildren().empty());
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(node1.getRoot() == &root);
//	BOOST_CHECK_THROW(node1.getParent(), InconsistentTreeException);
//	BOOST_CHECK_THROW(node1.getRank(), InconsistentTreeException);
//	BOOST_CHECK_THROW(node1.getDepth(), InconsistentTreeException);
//	BOOST_CHECK_THROW(node1.getPreviousSibling(), InconsistentTreeException);
//	BOOST_CHECK_THROW(node1.getNextSibling(), InconsistentTreeException);

	node1.setParent(NULL);
	node1.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(root.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(root.getChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(root.getChildren().begin()->second, &node1);
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(node1.getRoot() == &root);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);

	TN node2;
	node2.setParent(&node1);
	node2.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(root.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(root.getChildren().begin()->second, &node1);
	BOOST_REQUIRE_EQUAL(node1.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getChildren().begin()->second, &node2);
	BOOST_CHECK(node1.getRoot() == &root);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);
	BOOST_CHECK(node2.getChildren().empty());
	BOOST_CHECK(node2.getRoot() == &root);
	BOOST_CHECK(node2.getParent() == &node1);
	BOOST_CHECK_EQUAL(node2.getRank(), 0);
	BOOST_CHECK_EQUAL(node2.getDepth(), 2);
	BOOST_CHECK(node2.getPreviousSibling() == NULL);
	BOOST_CHECK(node2.getNextSibling() == NULL);

	TN node3;
	node3.setRank(1);
	node3.setParent(&node1);
	node3.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(root.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(root.getChildren().begin()->second, &node1);
	BOOST_REQUIRE_EQUAL(node1.getChildren().size(), 2);
	TN::ChildrenType::const_iterator it(node1.getChildren().begin());
	BOOST_CHECK_EQUAL(it->second, &node2);
	++it;
	BOOST_CHECK_EQUAL(it->second, &node3);
	BOOST_CHECK(node1.getRoot() == &root);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);
	BOOST_CHECK(node2.getChildren().empty());
	BOOST_CHECK(node2.getRoot() == &root);
	BOOST_CHECK(node2.getParent() == &node1);
	BOOST_CHECK_EQUAL(node2.getRank(), 0);
	BOOST_CHECK_EQUAL(node2.getDepth(), 2);
	BOOST_CHECK(node2.getPreviousSibling() == NULL);
	BOOST_CHECK(node2.getNextSibling() == &node3);
	BOOST_CHECK(node3.getChildren().empty());
	BOOST_CHECK(node3.getRoot() == &root);
	BOOST_CHECK(node3.getParent() == &node1);
	BOOST_CHECK_EQUAL(node3.getRank(), 1);
	BOOST_CHECK_EQUAL(node3.getDepth(), 2);
	BOOST_CHECK(node3.getPreviousSibling() == &node2);
	BOOST_CHECK(node3.getNextSibling() == NULL);

	TN node4;
	node4.setRoot(&root);
	BOOST_CHECK_EQUAL(root.getChildren().size(), 1);
	BOOST_CHECK(node4.getChildren().empty());
	BOOST_CHECK(node4.getRoot() == &root);

	node4.setRank(1);
	node4.setParent(NULL);
	node4.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(root.getChildren().size(), 2);
	TN::ChildrenType::const_iterator it4(root.getChildren().begin());
	BOOST_CHECK_EQUAL(it4->first, node1.getRank());
	BOOST_CHECK_EQUAL(it4->second, &node1);
	++it4;
	BOOST_CHECK_EQUAL(it4->first, node4.getRank());
	BOOST_CHECK_EQUAL(it4->second, &node4);
	BOOST_CHECK(node4.getChildren().empty());
	BOOST_CHECK(node4.getRoot() == &root);
	BOOST_CHECK(node4.getParent() == NULL);
	BOOST_CHECK_EQUAL(node4.getRank(), 1);
	BOOST_CHECK_EQUAL(node4.getDepth(), 1);
	BOOST_CHECK(node4.getPreviousSibling() == &node1);
	BOOST_CHECK(node4.getNextSibling() == NULL);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == &node4);
}


class TU:
	public TreeNode<TU, TreeRankOrderingPolicy, TreeUniqueRootPolicy<TreeRankOrderingPolicy, TU> >
{

};

BOOST_AUTO_TEST_CASE(Tree_Rank_UniqueRoot)
{
	TU node1;
	BOOST_CHECK(node1.getRoot()->getChildren().empty());
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(node1.getChildren().empty());

	//	TODO
	//	BOOST_CHECK_THROW(node1.getParent(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getRank(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getDepth(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getPreviousSibling(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getNextSibling(), InconsistentTreeException);

	node1.setParent(NULL);
	node1.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(node1.getRoot()->getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getRoot()->getChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(node1.getRoot()->getChildren().begin()->second, &node1);
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);

	TU node2;
	node2.setParent(&node1);
	node2.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(node1.getRoot()->getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getRoot()->getChildren().begin()->second, &node1);
	BOOST_REQUIRE_EQUAL(node1.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getChildren().begin()->second, &node2);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);
	BOOST_CHECK(node2.getChildren().empty());
	BOOST_CHECK(node2.getRoot() == node1.getRoot());
	BOOST_CHECK(node2.getParent() == &node1);
	BOOST_CHECK_EQUAL(node2.getRank(), 0);
	BOOST_CHECK_EQUAL(node2.getDepth(), 2);
	BOOST_CHECK(node2.getPreviousSibling() == NULL);
	BOOST_CHECK(node2.getNextSibling() == NULL);

	TU node3;
	node3.setRank(1);
	node3.setParent(&node1);
	node3.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(node1.getRoot()->getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getRoot()->getChildren().begin()->second, &node1);
	BOOST_REQUIRE_EQUAL(node1.getChildren().size(), 2);
	TU::ChildrenType::const_iterator it(node1.getChildren().begin());
	BOOST_CHECK_EQUAL(it->second, &node2);
	++it;
	BOOST_CHECK_EQUAL(it->second, &node3);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);
	BOOST_CHECK(node2.getChildren().empty());
	BOOST_CHECK(node2.getRoot() == node1.getRoot());
	BOOST_CHECK(node2.getParent() == &node1);
	BOOST_CHECK_EQUAL(node2.getRank(), 0);
	BOOST_CHECK_EQUAL(node2.getDepth(), 2);
	BOOST_CHECK(node2.getPreviousSibling() == NULL);
	BOOST_CHECK(node2.getNextSibling() == &node3);
	BOOST_CHECK(node3.getChildren().empty());
	BOOST_CHECK(node3.getRoot() == node1.getRoot());
	BOOST_CHECK(node3.getParent() == &node1);
	BOOST_CHECK_EQUAL(node3.getRank(), 1);
	BOOST_CHECK_EQUAL(node3.getDepth(), 2);
	BOOST_CHECK(node3.getPreviousSibling() == &node2);
	BOOST_CHECK(node3.getNextSibling() == NULL);
}

class TN1;

class TR1:
	public TreeRoot<TN1, TreeRankOrderingPolicy>
{
};

class TR2:
	public TreeRoot<TN1, TreeRankOrderingPolicy>
{
};

class TN1:
	public TreeNode<TN1, TreeRankOrderingPolicy, TreeMultiClassRootPolicy<TR1, TR2> >
{
};


BOOST_AUTO_TEST_CASE(Tree_Rank_MultiClassRoot1)
{
	TR1 root;
	BOOST_CHECK(root.getChildren().empty());

	TN1 node1;
	BOOST_CHECK(root.getChildren().empty());
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(!node1.hasRoot());
	BOOST_CHECK(node1.getRoot<TR1>() == NULL);
	BOOST_CHECK(node1.getRoot<TR2>() == NULL);

	//	TODO
	//	BOOST_CHECK_THROW(node1.getParent(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getRank(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getDepth(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getPreviousSibling(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getNextSibling(), InconsistentTreeException);

	node1.setRoot(&root);
	BOOST_CHECK(root.getChildren().empty());
	BOOST_CHECK(node1.getRootChildren().empty());
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(node1.hasRoot());
	BOOST_CHECK(node1.getRoot<TR1>() == &root);
	BOOST_CHECK(node1.getRoot<TR2>() == NULL);
	//	BOOST_CHECK_THROW(node1.getParent(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getRank(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getDepth(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getPreviousSibling(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getNextSibling(), InconsistentTreeException);

	node1.setParent(NULL);
	node1.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(root.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().size(), 1);
	BOOST_CHECK_EQUAL(root.getChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(root.getChildren().begin()->second, &node1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->second, &node1);
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(node1.hasRoot());
	BOOST_CHECK(node1.getRoot<TR1>() == &root);
	BOOST_CHECK(node1.getRoot<TR2>() == NULL);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);

	TN1 node2;
	node2.setParent(&node1);
	node2.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(root.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().size(), 1);
	BOOST_CHECK_EQUAL(root.getChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(root.getChildren().begin()->second, &node1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->second, &node1);
	BOOST_REQUIRE_EQUAL(node1.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getChildren().begin()->second, &node2);
	BOOST_CHECK(node1.hasRoot());
	BOOST_CHECK(node1.getRoot<TR1>() == &root);
	BOOST_CHECK(node1.getRoot<TR2>() == NULL);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);
	BOOST_CHECK(node2.getChildren().empty());
	BOOST_CHECK(node2.hasRoot());
	BOOST_CHECK(node2.getRoot<TR1>() == &root);
	BOOST_CHECK(node2.getRoot<TR2>() == NULL);
	BOOST_CHECK(node2.getParent() == &node1);
	BOOST_CHECK_EQUAL(node2.getRank(), 0);
	BOOST_CHECK_EQUAL(node2.getDepth(), 2);
	BOOST_CHECK(node2.getPreviousSibling() == NULL);
	BOOST_CHECK(node2.getNextSibling() == NULL);

	TN1 node3;
	node3.setRank(1);
	node3.setParent(&node1);
	node3.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(root.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().size(), 1);
	BOOST_CHECK_EQUAL(root.getChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(root.getChildren().begin()->second, &node1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->second, &node1);
	BOOST_REQUIRE_EQUAL(node1.getChildren().size(), 2);
	TN1::ChildrenType::const_iterator it(node1.getChildren().begin());
	BOOST_CHECK_EQUAL(it->second, &node2);
	++it;
	BOOST_CHECK_EQUAL(it->second, &node3);
	BOOST_CHECK(node1.hasRoot());
	BOOST_CHECK(node1.getRoot<TR1>() == &root);
	BOOST_CHECK(node1.getRoot<TR2>() == NULL);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);
	BOOST_CHECK(node2.getChildren().empty());
	BOOST_CHECK(node2.hasRoot());
	BOOST_CHECK(node2.getRoot<TR1>() == &root);
	BOOST_CHECK(node2.getRoot<TR2>() == NULL);
	BOOST_CHECK(node2.getParent() == &node1);
	BOOST_CHECK_EQUAL(node2.getRank(), 0);
	BOOST_CHECK_EQUAL(node2.getDepth(), 2);
	BOOST_CHECK(node2.getPreviousSibling() == NULL);
	BOOST_CHECK(node2.getNextSibling() == &node3);
	BOOST_CHECK(node3.getChildren().empty());
	BOOST_CHECK(node3.hasRoot());
	BOOST_CHECK(node3.getRoot<TR1>() == &root);
	BOOST_CHECK(node3.getRoot<TR2>() == NULL);
	BOOST_CHECK(node3.getParent() == &node1);
	BOOST_CHECK_EQUAL(node3.getRank(), 1);
	BOOST_CHECK_EQUAL(node3.getDepth(), 2);
	BOOST_CHECK(node3.getPreviousSibling() == &node2);
	BOOST_CHECK(node3.getNextSibling() == NULL);
}

class TN2;

class TR21:
	public TreeRoot<TN2, TreeRankOrderingPolicy>
{
};

class TR22:
	public TreeRoot<TN2, TreeRankOrderingPolicy>
{
};

class TN2:
	public TreeNode<TN2, TreeRankOrderingPolicy, TreeMultiClassRootPolicy<TR21, TR22> >
{
};

BOOST_AUTO_TEST_CASE(Tree_Rank_MultiClassRoot2)
{
	TR22 root;
	BOOST_CHECK(root.getChildren().empty());

	TN2 node1;
	BOOST_CHECK(root.getChildren().empty());
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(!node1.hasRoot());
	BOOST_CHECK(node1.getRoot<TR21>() == NULL);
	BOOST_CHECK(node1.getRoot<TR22>() == NULL);

	//	TODO
	//	BOOST_CHECK_THROW(node1.getParent(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getRank(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getDepth(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getPreviousSibling(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getNextSibling(), InconsistentTreeException);

	node1.setRoot(&root);
	BOOST_CHECK(root.getChildren().empty());
	BOOST_CHECK(node1.getRootChildren().empty());
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(node1.hasRoot());
	BOOST_CHECK(node1.getRoot<TR21>() == NULL);
	BOOST_CHECK(node1.getRoot<TR22>() == &root);
	//	BOOST_CHECK_THROW(node1.getParent(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getRank(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getDepth(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getPreviousSibling(), InconsistentTreeException);
	//	BOOST_CHECK_THROW(node1.getNextSibling(), InconsistentTreeException);

	node1.setParent(NULL);
	node1.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(root.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().size(), 1);
	BOOST_CHECK_EQUAL(root.getChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(root.getChildren().begin()->second, &node1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->second, &node1);
	BOOST_CHECK(node1.getChildren().empty());
	BOOST_CHECK(node1.hasRoot());
	BOOST_CHECK(node1.getRoot<TR21>() == NULL);
	BOOST_CHECK(node1.getRoot<TR22>() == &root);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);

	TN2 node2;
	node2.setParent(&node1);
	node2.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(root.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().size(), 1);
	BOOST_CHECK_EQUAL(root.getChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(root.getChildren().begin()->second, &node1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->second, &node1);
	BOOST_REQUIRE_EQUAL(node1.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getChildren().begin()->second, &node2);
	BOOST_CHECK(node1.hasRoot());
	BOOST_CHECK(node1.getRoot<TR21>() == NULL);
	BOOST_CHECK(node1.getRoot<TR22>() == &root);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);
	BOOST_CHECK(node2.getChildren().empty());
	BOOST_CHECK(node2.hasRoot());
	BOOST_CHECK(node2.getRoot<TR21>() == NULL);
	BOOST_CHECK(node2.getRoot<TR22>() == &root);
	BOOST_CHECK(node2.getParent() == &node1);
	BOOST_CHECK_EQUAL(node2.getRank(), 0);
	BOOST_CHECK_EQUAL(node2.getDepth(), 2);
	BOOST_CHECK(node2.getPreviousSibling() == NULL);
	BOOST_CHECK(node2.getNextSibling() == NULL);

	TN2 node3;
	node3.setRank(1);
	node3.setParent(&node1);
	node3.registerInParentOrRoot();
	BOOST_REQUIRE_EQUAL(root.getChildren().size(), 1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().size(), 1);
	BOOST_CHECK_EQUAL(root.getChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(root.getChildren().begin()->second, &node1);
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->first, node1.getRank());
	BOOST_CHECK_EQUAL(node1.getRootChildren().begin()->second, &node1);
	BOOST_REQUIRE_EQUAL(node1.getChildren().size(), 2);
	TN2::ChildrenType::const_iterator it(node1.getChildren().begin());
	BOOST_CHECK_EQUAL(it->second, &node2);
	++it;
	BOOST_CHECK_EQUAL(it->second, &node3);
	BOOST_CHECK(node1.hasRoot());
	BOOST_CHECK(node1.getRoot<TR21>() == NULL);
	BOOST_CHECK(node1.getRoot<TR22>() == &root);
	BOOST_CHECK(node1.getParent() == NULL);
	BOOST_CHECK_EQUAL(node1.getRank(), 0);
	BOOST_CHECK_EQUAL(node1.getDepth(), 1);
	BOOST_CHECK(node1.getPreviousSibling() == NULL);
	BOOST_CHECK(node1.getNextSibling() == NULL);
	BOOST_CHECK(node2.getChildren().empty());
	BOOST_CHECK(node2.hasRoot());
	BOOST_CHECK(node2.getRoot<TR21>() == NULL);
	BOOST_CHECK(node2.getRoot<TR22>() == &root);
	BOOST_CHECK(node2.getParent() == &node1);
	BOOST_CHECK_EQUAL(node2.getRank(), 0);
	BOOST_CHECK_EQUAL(node2.getDepth(), 2);
	BOOST_CHECK(node2.getPreviousSibling() == NULL);
	BOOST_CHECK(node2.getNextSibling() == &node3);
	BOOST_CHECK(node3.getChildren().empty());
	BOOST_CHECK(node3.hasRoot());
	BOOST_CHECK(node3.getRoot<TR21>() == NULL);
	BOOST_CHECK(node3.getRoot<TR22>() == &root);
	BOOST_CHECK(node3.getParent() == &node1);
	BOOST_CHECK_EQUAL(node3.getRank(), 1);
	BOOST_CHECK_EQUAL(node3.getDepth(), 2);
	BOOST_CHECK(node3.getPreviousSibling() == &node2);
	BOOST_CHECK(node3.getNextSibling() == NULL);
}
