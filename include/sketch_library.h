#ifndef SKETCHLIBRARY_H
#define SKETCHLIBRARY_H


#include <QPainterPath>


class SketchLibrary
{
    public:
        static bool overSketch( QPolygonF& sketch, const QPolygonF& new_segment );

        static QPolygonF smooth( const QPolygonF& sketch );
};


#endif // SKETCHLIBRARY_H
