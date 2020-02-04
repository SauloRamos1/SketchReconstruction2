#ifndef POLYCURVE_H
#define POLYCURVE_H

#include <vector>
#include <limits>

#include "./polygonalcurve/CurveN.hpp"





class PolyCurve
{
    public:

        PolyCurve() = default;

        PolyCurve( const std::vector< Curve2D >& curve_ ){
            curve = curve_;
          }

        PolyCurve( const std::vector< double >& vs_, const std::vector< std::size_t >& es_ = std::vector< std::size_t >() ){
            fromVector( vs_, es_ );
        }


        Curve2D getSubcurve( std::size_t index_ ) const{
            bool valid_ = !curve.empty();
            valid_ &= ( ( index_ < curve.size() ) && ( index_ >= 0 ) );

            if( valid_ == false )
                return Curve2D();

            return curve.at( index_ );
        }


        bool isEmpty() const{
            if( curve.empty() == true ) return true;

            std::size_t size_ = 0;
            for( auto c_: curve )
            {
                size_ += c_.size();
            }

            if( size_ <= CURVE_MIN_LENGHT )
                return true;

            return false;
        }


        void clear(){
            for( auto c_: curve )
            {
                c_.clear();
            }
            curve.clear();
        }


        void fromVector( const std::vector< double >& vs_, const std::vector< std::size_t >& es_ = std::vector< std::size_t >() ){
            std::size_t number_of_edges_ = es_.size()/2;
            std::size_t last_id_ = UINT_MAX;


            Curve2D c_;

            for( std::size_t i = 0; i < number_of_edges_; ++i )
            {

                std::size_t id0_ = es_[ 2*i ];
                std::size_t id1_ = es_[ 2*i + 1 ];

                double x0 = vs_[ 2*id0_ ];
                double y0 = vs_[ 2*id0_ + 1 ];
                double x1 = vs_[ 2*id1_ ];
                double y1 = vs_[ 2*id1_ + 1 ];

                if( last_id_ != id0_ )
                {
                    curve.push_back( c_ );
                    c_ = Curve2D();
                }

                c_.add( Point2D( x0, y0 ) );
                c_.add( Point2D( x1, y1 ) );

                last_id_ = id1_;

            }

        }


        void toVector( std::vector< double >& vs_, std::vector< std::size_t >& es_ ) const{
            int index_ = 0;
            for( auto c_: curve )
            {
                if( c_.size() < CURVE_MIN_LENGHT ) continue;

                for( unsigned int i = 0; i < c_.size(); ++i )
                {
                    Point2D p_ = c_.at( i );

                    vs_.push_back( p_.x() );
                    vs_.push_back( p_.y() );

                    if( ( i + 1 ) == c_.size() )
                    {
                        es_.push_back( index_ );
                        es_.push_back( ++index_ );
                    }

                }

            }
        }


        static bool overSketch( Curve2D& sketch_, const Curve2D& subpath_ )
        {
            if( subpath_.size() < CURVE_MIN_LENGHT ) return false;

            Curve2D cpy_subpath_ = subpath_;
            Curve2D rest_;

            double err_ = 1.0;
            double aprox_factor_ = 36.0;

            sketch_ = cpy_subpath_.overSketch( sketch_, rest_, err_, aprox_factor_ );
            return true;

        }


        static Curve2D smooth( Curve2D& sketch_ )
        {
            Curve2D cpy_ = sketch_;
            cpy_.lineFilter();
            cpy_.meanFilter();
            cpy_.meanFilter();
            return cpy_;
        }


    protected:

        static const std::size_t CURVE_MIN_LENGHT = 3;
        std::vector< Curve2D > curve;

};

#endif // POLYCURVE_H
