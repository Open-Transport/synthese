/**********************************************************************
 * $Id: AbstractNode.cpp 3717 2012-09-17 08:40:42Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/index/strtree/AbstractNode.h>

#include <vector>
#include <cassert>

using namespace std;

namespace geos {
namespace index { // geos.index
namespace strtree { // geos.index.strtree

/*
 * Constructs an AbstractNode at the given level in the tree
 * @param level 0 if this node is a leaf, 1 if a parent of a leaf, and so on;
 * the root node will have the highest level
 */
AbstractNode::AbstractNode(int newLevel, int capacity) {
	childBoundables.reserve(capacity);
	bounds=0;
	level=newLevel;
}

AbstractNode::~AbstractNode() {
}

const void *
AbstractNode::getBounds() const
{
	if (bounds==0) {
		bounds = computeBounds();
	}
	return bounds;
}

/**
* Returns 0 if this node is a leaf, 1 if a parent of a leaf, and so on; the
* root node will have the highest level
*/
int AbstractNode::getLevel() {
	return level;
}

/**
 * Adds either an AbstractNode, or if this is a leaf node, a data object
 * (wrapped in an ItemBoundable)
 */
void AbstractNode::addChildBoundable(Boundable *childBoundable) {
	assert(bounds==0);
	childBoundables.push_back(childBoundable);
}

} // namespace geos.index.strtree
} // namespace geos.index
} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.13  2006/03/21 10:47:34  strk
 * indexStrtree.h split
 *
 * Revision 1.12  2006/03/06 19:40:47  strk
 * geos::util namespace. New GeometryCollection::iterator interface, many cleanups.
 *
 **********************************************************************/

