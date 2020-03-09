#include <QPainterPath>

#include "polygonalcurve/CurveN.hpp"
/// NOTE: we don't use polycurve.h anymore, remove?
#include "polycurve.h"

#include "sketch_library.h"
#include "sketch_utilities.h"


bool SketchLibrary::overSketch( QPolygonF& current_sketch, const QPolygonF& new_segment )
{
    Curve2D sketch_ = SketchUtilities::fromQtToCurve2D( current_sketch );
    Curve2D segment_ = SketchUtilities::fromQtToCurve2D( new_segment );

    // CurveN::oversketch() uses this variable to store whatever is removed from sketch_
    Curve2D rest_;

    // Magic variables
    double err_ = 1.0;
    double aprox_factor_ = 36.0;

    // Valores Normalizados - TO DO

    Curve2D modified_sketch_ = segment_.overSketch( sketch_, rest_, err_, aprox_factor_ );

    bool success = ( sketch_ != modified_sketch_ );

    if ( success )
    {
        current_sketch = SketchUtilities::fromCurve2DToQt( modified_sketch_ );
    }

    return success;
}

QPolygonF SketchLibrary::smooth( const QPolygonF& sketch )
{
    Curve2D sketch_ = SketchUtilities::fromQtToCurve2D( sketch );

    sketch_.lineFilter();
    sketch_.meanFilter();
    sketch_.meanFilter();

    return SketchUtilities::fromCurve2DToQt( sketch_ );
}
