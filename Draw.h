
#include "Vector.h"

#include <iterator>
#include <GL/gl.h>

namespace draw {

// opengl_traits code from
// http://www.gamedev.net/reference/programming/features/ogltypetraits/
template< class T > struct opengl_traits {};

template<> struct opengl_traits< unsigned int >
{
    enum { GL_TYPE = GL_UNSIGNED_INT };
};

template<> struct opengl_traits< int >
{
    enum { GL_TYPE = GL_INT };
};

template<> struct opengl_traits< float >
{
    enum { GL_TYPE = GL_FLOAT };
};

template< typename V >
class Verts
{
    std::vector<V> verts;
    size_t n;
    
  public:
    typedef typename V::value_type type;

    enum { 
        stride = sizeof( V ),
        size   = V::static_size,
    };

    Verts( V* data, unsigned int n )
        : n( n )
    {
        verts.resize( n );

        while( n-- )
            verts[ n ] = data[ n ];
    }

    const type* data() const
    {
        return verts.begin()->begin();
    }

    unsigned int count() const
    {
        return n;
    }
};

template< typename V >
class TexCoords
{
    std::vector<V> coords;
    int texture;
    
  public:
    typedef typename V::value_type type;

    TexCoords( V* data, int texture=0, unsigned int n=4 )
        : texture( texture )
    {
        coords.resize( n );
        std::copy( data, data+n, coords.begin() );
    }

    void handle( int tex )
    {
        texture = tex;
    }

    int handle()
    {
        return texture;
    }

    const type* data() const
    {
        return coords.begin()->begin();
    }

    unsigned int count() const
    {
        return coords.size();
    }
};

// PROTOTYPES
template< typename V >
void draw( Verts<V> verts, GLenum mode=GL_QUADS );

template< typename V1, typename V2 >
void draw( Verts<V1> verts, TexCoords<V2> texCoords, GLenum mode=GL_QUADS );

template< typename Iter, typename T >
Iter loop( Iter begin, Iter end, float radA, float radB );

// DEFINITIONS
template< typename V >
void draw( Verts<V> verts , GLenum mode )
{
    glEnableClientState( GL_VERTEX_ARRAY );

    int glType = opengl_traits< typename Verts<V>::type >::GL_TYPE;

    glVertexPointer( verts.size, glType, verts.stride, verts.data() );
    glDrawArrays( mode, 0, verts.count() );

    glDisableClientState( GL_VERTEX_ARRAY );
}

template< typename V1, typename V2 >
void draw( Verts<V1> verts, TexCoords<V2> texCoords, GLenum mode )
{
    glEnable( GL_TEXTURE_2D );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    if( texCoords.handle() ) {
        typedef typename TexCoords<V2>::type T;

        glBindTexture( GL_TEXTURE_2D, texCoords.handle() );
        glTexCoordPointer( 2, opengl_traits<T>::GL_TYPE, 0, texCoords.data() );
    }

    draw( verts, mode );

    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisable( GL_TEXTURE_2D );
}

template< typename T, typename U >
void draw( T* verts, size_t nVerts, int texture, U* coords, GLenum mode )
{
    if( texture ) {
        glEnable( GL_TEXTURE_2D );

        glEnableClientState( GL_TEXTURE_COORD_ARRAY );

        glBindTexture( GL_TEXTURE_2D, texture );
        glTexCoordPointer( 2, opengl_traits<U>::GL_TYPE, 0, coords );
    }

    draw( verts, nVerts, mode );

    if( texture ) {
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        glDisable( GL_TEXTURE_2D );
    }
}

template< typename Iter, typename T >
Iter circle( Iter begin, size_t size, T r )
{
    Iter start = begin;

    unsigned int nVertecies = (size-2)/2;
    const float ROTATION = 2 * 3.145 / nVertecies;

    const float TANGENTAL_FACTOR = std::tan( ROTATION );
    const float RADIAL_FACTOR    = std::cos( ROTATION );

    Vector<float,2> rv; // radial vector.
    rv.x( r ); rv.y( 0.0f );

    // Make the bottom half.
    begin = std::copy( rv.begin(), rv.end(), begin );
    while( nVertecies-- > 0 );
    {
        Vector<float,2> tv; // Tangent vector.
        tv.x( -rv.y() ); tv.y( rv.x() );

        rv += tv * TANGENTAL_FACTOR;
        rv *= RADIAL_FACTOR;
        begin = std::copy( rv.begin(), rv.end(), begin );
    }

//    Iter underside = begin;
//    for( int i=0 ; begin != end; underside--, begin++, i++ ) {
//        *begin = *underside;
//        if( i & 1 )
//            ;
//        else
//            *begin = -*begin;
//    }

    return begin;
}

template< typename Iter, typename T >
Iter loop( Iter begin, Iter end, T radA, T radB )
{
    unsigned int nVertecies = std::distance( begin, end )/4-1;
    const float ROTATION = 2 * 3.145 / nVertecies;

    const float TANGENTAL_FACTOR = std::tan( ROTATION );
    const float RADIAL_FACTOR    = std::cos( ROTATION );

    Vector<float,2> rv1; // 1 radial vector.
    Vector<float,2> rv2; // 2 radial vector.

    rv1.x( radA ); rv1.y( 0.0f );
    rv2.x( radB ); rv1.y( 0.0f );

    // Make the bottom half.
    begin = std::copy( rv1.begin(), rv1.end(), begin );
    begin = std::copy( rv2.begin(), rv2.end(), begin );
    while( begin < end-4 );
    {
        Vector<float,2> tv1; 
        tv1.x( -rv1.y() ); tv1.y( rv1.x() );

        Vector<float,2> tv2; 
        tv2.x( -rv2.y() ); tv2.y( rv2.x() );

        rv1 += tv1 * TANGENTAL_FACTOR;
        rv1 *= RADIAL_FACTOR;

        rv2 += tv2 * TANGENTAL_FACTOR;
        rv2 *= RADIAL_FACTOR;

        begin = std::copy( rv1.begin(), rv1.end(), begin );
        begin = std::copy( rv2.begin(), rv2.end(), begin );
    }

//    Iter underside = begin;
//    for( int i=0 ; begin != end; underside--, begin++, i++ ) {
//        *begin = *underside;
//        if( i & 1 )
//            ;
//        else
//            *begin = -*begin;
//    }

    return begin;
}

} // namespace draw
