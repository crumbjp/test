/*
 * common.h - ????
 *
 *   Copyright (C) 2009 rakuten 
 *     by crumb <hiroaki.kubota@rakuten.co.jp> 
 *     Date : 2009/08/04
 */
#ifndef COMMON_H
#define COMMON_H

#include <inttypes.h>
#include <math.h>
#include <vector>


enum DIRECTION {
  NO_DIRECTION = 0x00,
  VERTICAL   = 0x01,
  HORIZONTAL = 0x02,
  CIRCLE     = 0x03,
};
static inline DIRECTION getD( const char * str ) {
    unsigned long d = strtoul(str,0,10);
    if ( d == 0 ) return NO_DIRECTION;
    else if ( d == 1 ) return VERTICAL;
    else if ( d == 2 ) return HORIZONTAL;
    else if ( d == 3 ) return CIRCLE;
    else throw std::logic_error("Unknown direction");
}
enum SHADOW_TYPE {
  NO_SHADOW = 0x00,
  NORMAL_SHADOW   = 0x01,
};
static inline SHADOW_TYPE getS( const char * str ) {
    unsigned long d = strtoul(str,0,10);
    if ( d == 0 ) return NO_SHADOW;
    else if ( d == 1 ) return NORMAL_SHADOW;
    else throw std::logic_error("Unknown shadow type");
}

struct Blur {
  double radius;
  double sigma;
  Blur(double r=0,double s=0):radius(r),sigma(s){}
  static Blur get( const char * strr , const char * strs  ) {
    double r = strtod(strr,0);
    double s = strtod(strs,0);
    return Blur(r,s);
  }
};

struct G {
  int x;
  int y;
  G(int x=0,int y=0):x(x),y(y){}
  G & operator += ( const G & in ) { x+=in.x; y+=in.y; return *this;}
  G & operator -= ( const G & in ) { x-=in.x; y-=in.y; return *this;}
  G & operator *= ( double in) { x*=in; y*=in; return *this;}
  G & operator /= ( double in) { x/=in; y/=in; return *this;}
  G operator +  ( const G & in ) const { G ret(*this); ret += in; return ret;}
  G operator -  ( const G & in ) const { G ret(*this); ret -= in; return ret;}
  G operator *  ( double in ) const { G ret(*this); ret *= in; return ret;}
  G operator /  ( double in ) const { G ret(*this); ret /= in; return ret;}
  Magick::Geometry get() const { return Magick::Geometry(x,y);}
  double distance(const G & in) const { 
    double X = x-in.x; 
    double Y = y-in.x; 
    return sqrt(X*X+Y*Y); 
  }
  double distance(const G & in,double angle) const {  
    double X = (x-in.x) * sin(angle); 
    double Y = (y-in.x) * cos(angle);
    return sqrt(X*X+Y*Y); 
  }
};
struct C {
  enum{ MAXC = 255};
  double r,g,b;
  C(double r=0,double g=0,double b=0):r(r),g(g),b(b){}
  bool operator == ( const C & in ) const { return MAXC*r==MAXC*in.r && MAXC*g==MAXC*in.g && MAXC*b==MAXC*in.b;}
  bool operator != ( const C & in ) const { return !(*this == in);}
  C & operator += ( const C & in ) { r+=in.r; g+=in.g; b+=in.b; return *this;}
  C & operator -= ( const C & in ) { r-=in.r; g-=in.g; b-=in.b; return *this;}
  C & operator *= ( double in ) { r*=in; g*=in; b*=in; return *this;}
  C & operator /= ( double in ) { r/=in; g/=in; b/=in; return *this;}
  C operator +  ( const C & in ) const { C ret(*this); ret += in; return ret;}
  C operator -  ( const C & in ) const { C ret(*this); ret -= in; return ret;}
  C operator *  ( double in ) const { C ret(*this); ret *= in; return ret;}
  C operator /  ( double in ) const { C ret(*this); ret /= in; return ret;}
  C( const Magick::Color & in )
    : r(Magick::Color::scaleQuantumToDouble(in.redQuantum())),
      g(Magick::Color::scaleQuantumToDouble(in.greenQuantum())),
      b(Magick::Color::scaleQuantumToDouble(in.blueQuantum())){}
  Magick::Color get() const { 
    return Magick::Color( Magick::Color::scaleDoubleToQuantum(r),
                          Magick::Color::scaleDoubleToQuantum(g),
                          Magick::Color::scaleDoubleToQuantum(b));
  }
  static C get(const char * str ) {
    unsigned long c = strtoul(str,0,16);
    double r = ((0xFF0000&c)>>16)/255.0;
    double g = ((0x00FF00&c)>>8)/255.0;
    double b = (0x0000FF&c)/255.0;
    return C(r,g,b);
  }
};

typedef std::vector<unsigned char> buf_t;

#endif
