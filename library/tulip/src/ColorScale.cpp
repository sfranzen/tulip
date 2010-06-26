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

#include "tulip/ColorScale.h"

using namespace std;

namespace tlp {

ColorScale::ColorScale() :
  gradient(true), colorScaleSet(false) {
  colorMap[0.0f] = Color(255, 255, 255, 255);
  colorMap[1.0f] = Color(255, 255, 255, 255);
}

ColorScale::ColorScale(const ColorScale& scale) {
  colorMap = scale.colorMap;
  gradient = scale.gradient;
  colorScaleSet = scale.colorScaleSet;
}

ColorScale::~ColorScale() {
}

void ColorScale::setColorScale(const std::vector<Color> colors,
    const bool gradientV) {
  gradient = gradientV;
  colorMap.clear();
  if (!colors.empty()) {
    colorScaleSet = true;
    if (colors.size() == 1) {
      colorMap[0.0f] = colors[0];
      colorMap[1.0f] = colors[0];
    } else {
      float shift;
      if (gradient) {
        shift = 1.0f / (colors.size() - 1);
      } else {
        shift = 1.0f / colors.size();
      }
      for (unsigned int i = 0; i < colors.size(); ++i) {
        //Ensure that the last color will be set to 1
        if (i == colors.size() - 1) {
          if (!gradient) {
            colorMap[1.0f - shift] = colors[i];
          }
          colorMap[1.0f] = colors[i];
        } else {
          colorMap[(float) (i * shift)] = colors[i];
          if (!gradient) {
            colorMap[(float) (((i + 1) * shift) - 1E-6)] = colors[i];
          }
        }
      }
    }
    notifyObservers();
  }
}

Color ColorScale::getColorAtPos(const float pos) const {
  if (colorMap.size() == 0) {
    return Color(255, 255, 255, 255);
  } else {
    Color startColor;
    Color endColor;
    float startPos, endPos;
    map<float, Color>::const_iterator it = colorMap.begin();
    startPos = endPos = it->first;
    startColor = endColor = it->second;
    for (++it; it != colorMap.end(); ++it) {
      endColor = it->second;
      endPos = it->first;
      if (pos >= startPos && pos <= endPos) {
        break;
      } else {
        startColor = endColor;
        startPos = endPos;
      }
    }

    Color ret;
    if (gradient) {
      double ratio = (pos - startPos) / (endPos - startPos);
      for (unsigned int i = 0; i < 4; ++i) {
        ret[i] = (unsigned char) (double(startColor[i]) + (double(endColor[i])
            - double(startColor[i])) * ratio);
      }
    } else {
      return startColor;
    }
    return ret;
  }
}

}
