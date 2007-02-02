/** @file thwarptrans.h
 * 
 * @author marco corvi
 * @date   nov 2006
 * 
 * @brief warping transformation
 *
 * ---------------------------------------------------
 * GNU GPL licence ...
 */
#ifndef THWARPTRANS_H
#define THWARPTRANS_H

#include <vector>

#include "thtrans.h"
#include "thwarppme.h"

/**
 * therion warping transformation.
 *
 * The "from" data are in the sketch image reference frame
 * The "to" data are in the survey reference frame
 * The "warped image" is scaled/translated with respect to the survey frame
 *
 * The X and U centers and units are computed when the mapping is
 * initialized.  The warped image center and units are computed when a
 * source image is mapped (before that point they have the same values as the
 * U center and units).  Until the map has been initialized the centers and
 * units have undefined values.
 */
class thwarptrans : public thinserter
{
  private:
    bool m_initialized;          //!< whether the map has been initialized
    std::vector< thmorph_pair * > mPairs;         //!< vector of point pairs
    std::vector< thmorph_line * > mLines;         //!< vector of legs (lines)
    std::vector< thmorph_element * > mPlaquettes; //!< vector of 4-plaquettes
    
    double m_bound2;   //!< bound for segments
    double m_bound3;   //!< bound for triangles
    double m_bound4;   //!< bound for plaquettes

  public:
    thvec2 mX0;    //!< X-Y center
    thvec2 mU0;    //!< U-V center
    double mXUnit; //!< X-Y unit
    double mUUnit; //!< U-V unit
    thvec2 mUC;    //!< U-V center in the warped image
    double mUCUnit; //!< U-V unit in the warped image
    
  public:
    /** default cstr
     */
    thwarptrans( );


    /** cstr
     * @param bound2   bound for segments
     * @param bound3   bound for triangles
     * @param bound4   bound for plaquettes
     */
    thwarptrans( double bound2, double bound3, double bound4 );
    
    /** dstr
     */
    ~thwarptrans();
    
    /** clear internal mapping data
     */
    void reset();


    /** accessor to the "from" origin 
     * @return origin in the "from" image
     */
    thvec2 from_origin() const { return mX0; }
    
    /** accessor to the "to" origin 
     * @return origin in the "to" image
     */
    thvec2 to_origin() const { return mU0; }
    
    /** accessor to the "from" unit 
     * @return unit in the "from" image
     */
    double from_unit() const { return mXUnit; }
    
    /** accessor to the "to" unit 
     * @return unit in the "to" image
     */
    double to_unit() const { return mUUnit; }
    
    /** accessor to the warped image origin 
     * @return origin in the warped image (zero at the upper-left corner)
     */
    thvec2 warped_origin() const { return mUC; }
    
    /** accessor to the warped image unit 
     * @return unit in the warped image
     */
    double warped_unit() const { return mUCUnit; }


    
    /** insert a point and a line of extra type
     * @param p1  first line point
     * @param index index used to make the new point name
     * @param x3    new point X (sketch frame)
     * @param u3    new point U (survey frame)
     */
    thmorph_line * add_extra_line( thmorph_pair * p1, unsigned int index, 
                                   thvec2 & x3, thvec2 & u3 );


    /** insert a point
     * @param t    point type
     * @param name point name
     * @param src  point coords in the source (from) frame (sketch)
     * @param dst  point coords in the destination (to) frame (survey)
     * @return pointer to the new point-pair
     */
    thmorph_pair * insert_point( thmorph_type t, std::string & name, 
                                 const thvec2 & src, const thvec2 & dst );

    /** insert a line
     * @param t    line type
     * @param from name of the first line endpoint 
     * @param to   name of the second line endpoint 
     */
    void insert_line( thmorph_type t, std::string & from, std::string & to );
    
    /** initialize internal data for the mapping
     * @param proj  projection type
     * @return true if initialized successfully
     */
    bool initialize( thwarp_proj proj = THWARP_PLAN );
    
    /** map a 2D point forward 
     * @param src     point in the source ("from") frame
     * @param dst     point in the "to" frame
     * @return true if successful
     */
    bool forward( const thvec2 & src, thvec2 & dst )
    {
        return forward( src, dst, mU0, mUUnit );
    }
    
    /** map a 2D point backward 
     * @param dst     point in the "to" frame
     * @param src     point in the "from" frame
     * @return true if successful
     */
    bool backward( const thvec2 & dst, thvec2 & src )
    {
        return backward( dst, src, mU0, mUUnit );
    }
    
    /** map a 2D point backward 
     * @param dst     point in the "to" frame (survey)
     * @param src     point in the "from" frame (sketch)
     * @param origin  origin in the "to" frame
     * @param unit    unit in the "to" frame
     * @return true if successfulA
     */
    bool backward( const thvec2 & dst, thvec2 & src, const thvec2 & origin, double unit );
    
    /** map a 2D point forward
     * @param src     point in the "from" frame
     * @param dst     point in the "to" frame
     * @param origin  origin in the "to" frame
     * @param unit    unit in the "to" frame
     * @return true if successful
     */
    bool forward( const thvec2 & src, thvec2 & dst, const thvec2 & origin, double unit );
    
    /** map an image. The result is centered in the destination image
     * @param src  source image (RGB)
     * @param ws   source image width
     * @param hs   source image height 
     * @param dst  destination image (RGB) must be preallocaed by the caller
     * @param wd   destination image width
     * @param hd   destination image height
     * @param origin target origin in the warped image frame
     * @param unit   target units
     * @param depth image depth (bytes per pixel).
     * @param proj  sketch projection type
     * @return true if successful
     *
     * @note this method, as a side effect, sets the warped image origin and units
     */
    bool map_image( const unsigned char * src, unsigned int ws, unsigned int hs,
                    unsigned char * dst, unsigned int wd, unsigned int hd,
		    thvec2 const & origin, double unit,
                    int depth = 3, 
		    thwarp_proj proj = THWARP_PLAN );

    /** debug: print the content of this warptrans
     */
    void print();

  private:
    /** update line features - compute the lines' coeffs.
     */
    void update_lines();
    
    /** prepare the plaquettes
     * @param proj  projection type
     */
    void make_plaquettes( thwarp_proj proj );
    
    /** map by the plaquette
     * @param k   plaquette index
     * @param w   from point
     * @param z   to point
     * @return true if successful
     */
    bool map_backward_plaquette( unsigned int k, const thvec2 & w, thvec2 & z );

    /** add a segment
     * @param A   first endpoint
     * @param B   second endpoint
     */
    void add_segment( thmorph_pair * A, thmorph_pair * B );

    /** add a triangle
     * @param A   first point (left)
     * @param B   second point (vertex)
     * @param C   third point (right)
     */
    void add_triangle( thmorph_pair * A, thmorph_pair * B, thmorph_pair * C );
    
    /** add a quadrilater
     * @param A   first point 
     * @param B   second point
     * @param C   third point 
     * @param D   fourth point 
     */
    void add_quadrilater( thmorph_pair * A, thmorph_pair * B, thmorph_pair * C,
                          thmorph_pair * D );
};
    
#endif
