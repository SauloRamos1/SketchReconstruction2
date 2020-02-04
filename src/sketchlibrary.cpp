#include <QPainterPath>

#include "./include/sketchlibrary.h"
#include "./sketch_utilities.h"

#include "./libs/polycurve.h"

QPainterPath SketchLibrary::overSketchQt(const QPolygonF &seg0_, const QPolygonF &seg1_){

    Curve2D sketch_ = SketchUtilities::fromQtToCurve2D( seg0_ );
    Curve2D segment_ = SketchUtilities::fromQtToCurve2D( seg1_ );

    PolyCurve::overSketch( sketch_, segment_ );

    QPainterPath curve;
    curve.addPolygon( SketchUtilities::fromCurve2DToQt( sketch_ ));
    return curve;

}

QPainterPath SketchLibrary::smooth(const QPolygonF &seg0_){

    Curve2D sketch_ = SketchUtilities::fromQtToCurve2D( seg0_);
    Curve2D smoothedp_ = PolyCurve::smooth( sketch_ );

    QPainterPath curve;
    curve.addPolygon( SketchUtilities::fromCurve2DToQt( smoothedp_ ));
    return curve;

}

