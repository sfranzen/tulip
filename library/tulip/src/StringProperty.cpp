/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */

#include "tulip/StringProperty.h"
#include "tulip/PluginContext.h"
#include "tulip/Observable.h"
#include "tulip/StringAlgorithm.h"
#include "tulip/AbstractProperty.h"

using namespace std;
using namespace tlp;

//=================================================================================
PropertyInterface* StringProperty::clonePrototype(Graph * g, const std::string& n)
{
	if( !g )
		return 0;
	StringProperty * p = g->getLocalProperty<StringProperty>( n );
	p->setAllNodeValue( getNodeDefaultValue() );
	p->setAllEdgeValue( getEdgeDefaultValue() );
	return p;
}
//=================================================================================
PropertyInterface* StringVectorProperty::clonePrototype(Graph * g, const std::string& n) {
  if( !g )
    return 0;
  StringVectorProperty * p = g->getLocalProperty<StringVectorProperty>( n );
  p->setAllNodeValue( getNodeDefaultValue() );
  p->setAllEdgeValue( getEdgeDefaultValue() );
  return p;
}

