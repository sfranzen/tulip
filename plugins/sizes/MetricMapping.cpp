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
#include <cmath>
#include <tulip/TulipPlugin.h>

using namespace std;
using namespace tlp;

namespace {
const char * paramHelp[] = {
  // property
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "DoubleProperty" ) \
  HTML_HELP_BODY() \
  "This metric is used to affect scalar values to graph items." \
  HTML_HELP_CLOSE(),
  // input
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "SizeProperty" ) \
  HTML_HELP_BODY() \
  "This size property is used to affect values to unselected dimensions (width, height, depth)." \
  HTML_HELP_CLOSE(),
  // width, height, depth
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "boolean" ) \
  HTML_HELP_DEF( "values", "true/false" ) \
  HTML_HELP_DEF( "default", "true" ) \
  HTML_HELP_BODY() \
  "Indicates if this parameter will be computed(box is checked) or kept(box is unchecked) from the value of input size property" \
  HTML_HELP_CLOSE(),
  // min
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "double" ) \
  HTML_HELP_BODY() \
  "Gives the minimum value of the range of computed sizes." \
  HTML_HELP_CLOSE(),
  // max
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "double" ) \
  HTML_HELP_BODY() \
  "Gives the maximum value of the range of computed sizes." \
  HTML_HELP_CLOSE(),
  // Mapping type
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "Boolean" ) \
  HTML_HELP_DEF( "values", "true / false" ) \
  HTML_HELP_DEF( "default", "true" ) \
  HTML_HELP_BODY() \
  "This value defines the type of mapping. Following values are valid :" \
  "<ul><li>true : linear mapping</li><li>false: uniform quantification</li></ul>" \
  HTML_HELP_CLOSE(),
  // Mapping type
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "Boolean" ) \
  HTML_HELP_DEF( "values", "true / false" ) \
  HTML_HELP_DEF( "default", "true" ) \
  HTML_HELP_BODY() \
  "If true the algorithm will compute the size of nodes else it will compute the size of edges :" \
  "<ul><li>true : node size</li><li>false: edge size</li></ul>" \
  HTML_HELP_CLOSE(),

  //proportional mapping
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "string" ) \
  HTML_HELP_DEF( "default", "Area Proportional" ) \
  HTML_HELP_BODY() \
  "The mapping can either be area/volume proportional, or square/cubic;" \
  "i.e. the areas/volumes will be proportional, or the dimensions (width, height and depth) will be." \
  HTML_HELP_CLOSE(),
};
}

// error msg for invalid range value
static const char* rangeSizeErrorMsg = "max size must be greater than min size";
static const char* rangeMetricErrorMsg = "All values are the same";
static const std::string AREA_PROPORTIONAL = "Area Proportional";
/** \addtogroup size */
/*@{*/
/// Metric Mapping - Compute size of elements according to a metric.
/** This plugin enables to set the size of the graph's elements
 *  according to a metric.
 *
 *  \author David Auber University Bordeaux I France: Email:auber@tulip-software.org
 */
class MetricSizeMapping:public SizeAlgorithm {
public:
  MetricSizeMapping(const PropertyContext &context):SizeAlgorithm(context) {
    addParameter<DoubleProperty>("property", paramHelp[0]);
    addParameter<SizeProperty>("input", paramHelp[1]);
    addParameter<bool>("width", paramHelp[2],"true");
    addParameter<bool>("height", paramHelp[2],"true");
    addParameter<bool>("depth", paramHelp[2],"false");
    addParameter<double>("min size",paramHelp[3],"1");
    addParameter<double>("max size",paramHelp[4],"10");
    addParameter<bool>("type", paramHelp[5],"true");
    addParameter<bool>("node/edge", paramHelp[6],"true");
    addParameter<StringCollection>("area proportional", paramHelp[7], "Area Proportional;Quadratic/Cubic");
  }

  ~MetricSizeMapping() {}

  void computeNodeSize() {
    Iterator<node> *itN=graph->getNodes();

    while(itN->hasNext()) {
      node itn=itN->next();

      double sizos = 0;
      if (proportional == AREA_PROPORTIONAL) {
        const double power = 1.0 / (float(xaxis) + float(yaxis) + float(zaxis));
        sizos = pow((entryMetric->getNodeValue(itn)-shift)*(max-min)/range, power);
      }
      else {
        sizos = min + (entryMetric->getNodeValue(itn)-shift)*(max-min)/range;
      }
      Size result=entrySize->getNodeValue(itn);

      if (xaxis) result[0]=static_cast<float>(sizos);

      if (yaxis) result[1]=static_cast<float>(sizos);

      if (zaxis) result[2]=static_cast<float>(sizos);

      sizeResult->setNodeValue(itn, result);
    }

    delete itN;
  }

  void computeEdgeSize() {
    Iterator<edge> *itE=graph->getEdges();

    while(itE->hasNext()) {
      edge ite=itE->next();
      double sizos = min+(entryMetric->getEdgeValue(ite)-shift)*(max-min)/range;
      Size result  = entrySize->getEdgeValue(ite);
      result[0] = static_cast<float>(sizos);
      result[1] = static_cast<float>(sizos);
      sizeResult->setEdgeValue(ite, result);
    }

    delete itE;
  }

  bool check(std::string &errorMsg) {
    xaxis=yaxis=zaxis=true;
    min=1;
    max=10;
    nodeoredge = true;
    proportional = "Area Proportional";
    entryMetric=graph->getProperty<DoubleProperty>("viewMetric");
    entrySize=graph->getProperty<SizeProperty>("viewSize");
    mappingType = true;
    StringCollection proportionalType;

    if ( dataSet!=0 ) {
      dataSet->get("property",entryMetric);
      dataSet->get("input",entrySize);
      dataSet->get("width",xaxis);
      dataSet->get("height",yaxis);
      dataSet->get("depth",zaxis);
      dataSet->get("min size",min);
      dataSet->get("max size",max);
      dataSet->get("type",mappingType);
      dataSet->get("node/edge",nodeoredge);
      dataSet->get("area proportional",proportionalType);
      proportional = proportionalType.getCurrentString();
    }
    std::cout << proportional << std::endl;

    if (min >= max) {
      /*cerr << rangeErrorMsg << endl;
        pluginProgress->setError(rangeSizeErrorMsg); */
      errorMsg = std::string(rangeSizeErrorMsg);
      return false;
    }

    if (nodeoredge)
      range = entryMetric->getNodeMax(graph) - entryMetric->getNodeMin(graph);
    else
      range = entryMetric->getEdgeMax(graph) - entryMetric->getEdgeMin(graph);

    if (!range) {
      errorMsg = std::string(rangeMetricErrorMsg);
      return false;
    }

    if(!xaxis && !yaxis && !zaxis) {
      errorMsg = "You need at least one axis to map on.";
      return false;
    }

    if(proportional == AREA_PROPORTIONAL) {
      max = max*max;
    }

    return true;
  }

  bool run() {
    DoubleProperty *tmp = 0;

    if (!mappingType) {
      tmp = new DoubleProperty(graph);
      *tmp = *entryMetric;
      tmp->uniformQuantification(300);
      entryMetric = tmp;
    }

    if(nodeoredge) {
      shift = entryMetric->getNodeMin(graph);
      computeNodeSize();
      edge e;
      forEach(e, graph->getEdges())
      sizeResult->setEdgeValue(e, entrySize->getEdgeValue(e));
    }
    else {
      shift = entryMetric->getEdgeMin(graph);
      computeEdgeSize();
      node n;
      forEach(n, graph->getNodes())
      sizeResult->setNodeValue(n, entrySize->getNodeValue(n));
    }

    if (!mappingType) delete tmp;

    return true;
  }

private:
  DoubleProperty *entryMetric;
  SizeProperty *entrySize;
  bool xaxis,yaxis,zaxis,mappingType;
  double min,max;
  double range;
  double shift;
  bool nodeoredge;
  std::string proportional;
};
/*@}*/
SIZEPLUGIN(MetricSizeMapping,"Metric Mapping","Auber","08/08/2003","","2.0");
