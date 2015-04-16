#ifndef SYNTHESE_CARTO_RENDERINGCONFIG_H
#define SYNTHESE_CARTO_RENDERINGCONFIG_H

#include "01_util/RGBColor.h"
#include <string>

namespace synthese
{
namespace map
{


/** @ingroup m39 */
class RenderingConfig
{
private:

    bool _enableCurves;
    double _radius;
    double _radiusDelta;
    int _lineWidth;
    double _spacing;
    synthese::util::RGBColor _borderColor;
    int _borderWidth;

public:

    RenderingConfig ();
    ~RenderingConfig ();

    bool getEnableCurves () const;
    void setEnableCurves (bool enableCurves);

    double getRadius () const;
    void setRadius (double radius);

    double getRadiusDelta () const;
    void setRadiusDelta (double radiusDelta);

    int getLineWidth () const;
    void setLineWidth (int lineWidth);

    double getSpacing () const;
    void setSpacing (double spacing);

    synthese::util::RGBColor getBorderColor () const;
    void setBorderColor (const synthese::util::RGBColor& color);

    int getBorderWidth () const;
    void setBorderWidth (int borderWidth);


};

}
}

#endif

