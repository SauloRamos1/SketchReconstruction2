#ifndef SKETCHUTILITIES_H
#define SKETCHUTILITIES_H

#include <QPainterPath>

#include "polygonalcurve/CurveN.hpp"
#include "polycurve.h"


class SketchUtilities
{
    public:
        static Curve2D fromQtToCurve2D( const QPolygonF& pol_ )
        {
            Curve2D curve_;
            std::size_t start = ( pol_.isClosed() ) ? 1 : 0 ;

            for( int i = start ; i < pol_.size() ; ++i )
                curve_.add( QPointFToPoint2D( pol_[ i ] ) );

            curve_.close( pol_.isClosed() );
            return curve_;
        }

        static QPolygonF fromCurve2DToQt( const Curve2D& curve_ )
        {

            QPolygonF pol_;

            for( unsigned int i = 0 ; i < curve_.size() ; ++i )
                pol_ << Point2DToQPointF( curve_[ i ] ) ;

            if( curve_.isClosed() && curve_.size() > 0 )
                pol_ << Point2DToQPointF( curve_[ 0 ] ) ;

            return pol_;
        }

        static QPointF Point2DToQPointF( const Point2D & p )
        {
            return QPointF( p.x() , p.y() ) ;
        }

        static Point2D QPointFToPoint2D( const QPointF &p )
        {
            return Point2D( p.x() , p.y() ) ;
        }

};


#endif // SKETCHLIBRARY_H
