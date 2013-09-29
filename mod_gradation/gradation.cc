/*
 * gradation.cc - ????
 *  
 *   Copyright (C) 2009 rakuten 
 *     by hiroaki.kubota <hiroaki.kubota@rakuten.co.jp> 
 *     Date : 2009/08/19
 */

#include <iostream>
#include "Magick++.h"
#include <math.h>
#include <string.h>
#include <vector>

enum DIRECTION {
  VERTICAL   = 0x01,
  HORIZONTAL = 0x02,
  CIRCLE     = 0x03,
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
};
struct Blur {
  double radius;
  double sigma;
  Blur(double r=0,double s=0):radius(r),sigma(s){}
};

struct C {
  double r,g,b;
  C(double r=0,double g=0,double b=0):r(r),g(g),b(b){}
  bool operator == ( const C & in ) const { return r==in.r && g==in.g && b==in.b;}
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
};
C getC( const char * str ) {
  unsigned long c = strtoul(str,0,16);
  double r = ((0xFF0000&c)>>16)/255.0;
  double g = ((0x00FF00&c)>>8)/255.0;
  double b = (0x0000FF&c)/255.0;
  return C(r,g,b);
}
// void dumpC(const C & c){
//   std::cerr << c.r << ":" << c.g << ":" << c.b << std::endl;
// }


void gen_gradation_img ( 
  Magick::Image & img , 
  const G & img_size,
  const C & from,
  const C & to , 
  const G & point,
  const double & acc = 1.0 , 
  const int & direction = VERTICAL) 
{
  // img.backgroundColor(Magick::Color());
  // img.resize(img_size.get());
  // img.erase();

  C diff = (to - from);
  // DrawableList
  Magick::DrawableList list;
  list.push_back(Magick::DrawableStrokeAntialias(false));
  struct gradation_line {
    Magick::DrawableList &list;
    int d,N,L;
    gradation_line(Magick::DrawableList &list,int d,int N,int L):list(list),d(d),N(N),L(L){}
    void operator()(int i){
      if ( d == VERTICAL )        list.push_back(Magick::DrawableLine(0,i,L,i));
      else if ( d == HORIZONTAL)  list.push_back(Magick::DrawableLine(i,0,i,L));
    }
  };
  if ( direction == VERTICAL || direction == HORIZONTAL ) {
    list.push_back(Magick::DrawableStrokeWidth(1));
    int N; // Num of loop
    int L; // Line length
    int S; // Starting point
    if ( direction == VERTICAL ) {
      N = img_size.y-1;
      L = img_size.x-1;
      S = point.y;
    }else if ( direction == HORIZONTAL) {
      N = img_size.x-1;
      L = img_size.y-1;
      S = point.x;
    }
    int R = (S < (N-S))?(N-S):S; // Max range from starting point 
    for ( int i = 0 ; i <= N ; i++ ){
      double a = pow(abs(i-S)*(1.0/R),acc);
      list.push_back(Magick::DrawableStrokeColor((from+diff*a).get()));
      gradation_line(list,direction,N,L)(i);
        if ( a == 0 ) std::cerr << i << ":" << a << std::endl;
        if ( a == 1 ) std::cerr << i << ":" << a << std::endl;
    }
  }else if ( direction == CIRCLE ) {
    list.push_back(Magick::DrawablePointSize(1));
    int W = (point.x<(img_size.x-point.x-1))?(img_size.x-point.x-1):point.x;
    int H = (point.y<(img_size.y-point.y-1))?(img_size.y-point.y-1):point.y;
    double R = sqrt(W*W+H*H); // Max range from starting point 
//    if ( acc == 1 ) {
      double resolv = 1.0/R;
      for ( int x = 0 ; x < img_size.x ; x++ ) {
        for ( int y = 0 ; y < img_size.y ; y++ ) {
          int X = (point.x - x);
          int Y = (point.y - y);
          // Magick++'s bug !? Should be generate image self process.
          double a = pow(sqrt(X*X+Y*Y)*(1.0/R),acc);
          //double a = sqrt(X*X+Y*Y)*resolv;
          list.push_back(Magick::DrawableStrokeColor((from+diff*a).get()));
          list.push_back(Magick::DrawableLine(x,y,x+1,y));
          if ( a == 0 ) std::cerr << x << "," << y << ":" << a << std::endl;
          if ( a == 1 ) std::cerr << x << "," << y << ":" << a << std::endl;
        }
      }
    // }else {
    //   double resolv = 1.0/(pow(acc,R)-1);
    //   for ( int x = 0 ; x < img_size.x ; x++ ) {
    //     for ( int y = 0 ; y < img_size.y ; y++ ) {
    //       int X = (point.x - x);
    //       int Y = (point.y - y);
    //       // Magick++'s bug !? Should be generate image self process.
    //       double a = (pow(acc,sqrt(X*X+Y*Y))-1)*resolv;
    //       list.push_back(Magick::DrawableStrokeColor((from+diff*a).get()));
    //       list.push_back(Magick::DrawableLine(x,y,x+1,y));
    //       if ( a == 0 ) std::cerr << x << "," << y << ":" << a << std::endl;
    //       if ( a == 1 ) std::cerr << x << "," << y << ":" << a << std::endl;
    //     }
    //   }
    // }
  }
  img.draw(list);
}
void gen_font_img ( 
  const Magick::Image & img_base ,
  Magick::Image & img_font,
  const ::std::string & font_str,
  C font_color,
  C font_fill_color,
  const G & font_point,
  const int & font_size,
  const int & font_line_width,
  const Blur & font_blur,
  const G & font_shadow_point,
  const Blur & font_shadow_blur
)
{
  if (font_fill_color == C(1.0,1.0,1.0) ) {
    font_fill_color = C(1.0-1.0/255.0 , 1.0-1.0/255.0 , 1.0-1.0/255.0);
  }
  if (font_color == C(255,255,255) ) {
    font_color = font_fill_color = C(1.0-1.0/255.0,1.0-1.0/255.0,1.0-1.0/255.0);
  }
  const static double FONT_MARGIN_X = 10;
//   Magick::Image img2(Magick::Geometry(400,50),Magick::Color(MaxRGB,0,0));
  Magick::Image img_font_tmp(Magick::Geometry(img_base.size().width(),font_size*2),img_font.backgroundColor());
  Magick::DrawableList dlist_font;
//   list2.push_back(Magick::DrawableStrokeOpacity(0));
//   list2.push_back(Magick::DrawableFillOpacity(0));
//   list2.push_back(Magick::DrawableRectangle(0,0,10,100));

  dlist_font.push_back(Magick::DrawableStrokeWidth(font_line_width));
  dlist_font.push_back(Magick::DrawableStrokeColor(font_color.get()));
  dlist_font.push_back(Magick::DrawableFillColor(font_fill_color.get()));
  dlist_font.push_back(Magick::DrawableTextAntialias(true));
//  dlist_font.push_back(Magick::DrawableTextUnderColor(Magick::Color(0,0,0)));
//  dlist_font.push_back(Magick::DrawableTextDecoration(Magick::UnderlineDecoration));
  dlist_font.push_back(Magick::DrawablePointSize(font_size));
// $ identify -list font
  dlist_font.push_back(Magick::DrawableFont("Times",Magick::ItalicStyle,700,Magick::NormalStretch));

//  dlist_font.push_back(Magick::DrawableText(0,h/2,"あいう漢字"));
  dlist_font.push_back(Magick::DrawableText(font_point.x,(font_size*2.0)*(2.0/3.0),font_str));
  img_font_tmp.draw(dlist_font);
  img_font_tmp.transparent(img_font.backgroundColor());
  img_font_tmp.blur(font_blur.radius,font_blur.sigma);
//   img_font.transparent(font_bg_color);
//  img_font.transparentChroma(Magick::Color(MaxRGB*0.9,MaxRGB*0.9,MaxRGB*0.9),Magick::Color(MaxRGB,MaxRGB,MaxRGB));
  Magick::Image img_font_shadow(img_font_tmp.size(),img_font.backgroundColor());
  img_font_shadow.composite(img_font_tmp,font_shadow_point.x,font_shadow_point.y,Magick::AtopCompositeOp);
  img_font_shadow.blur(font_shadow_blur.radius,font_shadow_blur.sigma);
  img_font_shadow.transparent(img_font.backgroundColor());

//   Magick::Image img4(img_font_shadow);
//   img4.composite(img,font_x*(-1)-FONT_MARGIN_X,font_y*(-1),Magick::MultiplyCompositeOp);

  img_font_shadow.composite(img_base,0,(font_point.y-font_size)*(-1),Magick::MultiplyCompositeOp);
  img_font_shadow.composite(img_font_tmp,0,0,Magick::AtopCompositeOp);
  img_font = img_font_shadow; // @@@

// img.composite(img4,0,(font_size*2)*2,Magick::OverCompositeOp);

//   img.composite(img4,0,200,Magick::AtopCompositeOp);
//   img.texture(img_font);
// img.composite(img_font,0,200,Magick::AtopCompositeOp);

//  img.composite(img_font_shadow,10,200,Magick::AtopCompositeOp);

//  img.composite(img_font,0,0,Magick::OverlayCompositeOp);

//  img.composite(img_font_shadow,10,200,Magick::MultiplyCompositeOp);
//  img.composite(img_font,10,200,Magick::AtopCompositeOp);

//  list.push_back(Magick::DrawableColor(Magick::Color(MaxRGB,0,0)));
//   UndefinedMethod,
//   PointMethod,
//   ReplaceMethod,
//   FloodfillMethod,
//   FillToBorderMethod,
//   ResetMethod



//   UndefinedDecoration,
//   NoDecoration,
//   UnderlineDecoration,
//   OverlineDecoration,
//   LineThroughDecoration


// UndefinedStyle 	
// NormalStyle 	
// ItalicStyle 	
// ObliqueStyle 	
// AnyStyle 	

// UndefinedStretch 	
// NormalStretch 	
// UltraCondensedStretch 	
// ExtraCondensedStretch 	
// CondensedStretch 	
// SemiCondensedStretch 	
// SemiExpandedStretch 	
// ExpandedStretch 	
// ExtraExpandedStretch 	
// UltraExpandedStretch 	
// AnyStretch 

//   DrawableFont ( const std::string &family_,
//                  StyleType style_,
//                  const unsigned long weight_,
//                  StretchType stretch_ );

  


//  img.blur(2,0.5);

}

struct FontData {
  const char * font_str;
  const char * font_color;
  const char * font_fill_color;
  const double font_size;
  const double font_line_width;
  const G &    font_point;
  const Blur & font_blur;
  const G &    font_shadow_point;
  const Blur & font_shadow_blur;
  FontData(  
    const char * font_str,
    const char * font_color,
    const char * font_fill_color,
    const double font_size,
    const double font_line_width,
    const G &    font_point,
    const Blur   font_blur,
    const G &    font_shadow_point,
    const Blur   font_shadow_blur
    ):
    font_str          ( font_str),
    font_color        ( font_color),
    font_fill_color   ( font_fill_color),
    font_size         ( font_size), 
    font_line_width   ( font_line_width),
    font_point        ( font_point),
    font_blur         ( font_blur),
    font_shadow_point ( font_shadow_point),
    font_shadow_blur  ( font_shadow_blur) {
  };
};
void gradation_handler_proc( 
  std::vector<unsigned char> &buf , 
  const G & imgSize,
  const char * fromColor,
  const char * toColor,
  const G & basePoint,
  const double & acceralation,
  const int & direction,
  const FontData & fontData
  // const char * q_w ,
  // const char * q_h ,
  // const char * q_f , 
  // const char * q_t , 
  // const char * q_px,
  // const char * q_py,
  // const char * q_a,
  // const char * q_d,
  // const char * q_aa )
  ){
  // unsigned long h = strtoul(q_h,0,0);
  // unsigned long w = strtoul(q_w,0,0);
  // unsigned long px = strtoul(q_px,0,0);
  // unsigned long py = strtoul(q_py,0,0);
  // unsigned long aa = strtoul(q_aa,0,16);
  // unsigned long d = strtoul(q_d,0,16);
  // double a = strtod(q_a,0);




//  Magick::Image img_base(Magick::Geometry(1,1),Magick::Color(0,0,0));
  Magick::Image img_base(imgSize.get(),Magick::Color(0,0,0));
  gen_gradation_img(img_base,
                    imgSize,
                    getC(fromColor),
                    getC(toColor),
                    basePoint,
                    acceralation,
                    direction);
  if ( fontData.font_str ) {
    Magick::Image img_font(Magick::Geometry(img_base.size().width(),fontData.font_size*2),Magick::Color(MaxRGB,MaxRGB,MaxRGB));
    gen_font_img(img_base,
                 img_font,
                 fontData.font_str,
                 getC(fontData.font_color),
                 getC(fontData.font_fill_color),
                 fontData.font_point,
                 fontData.font_size,
                 fontData.font_line_width,
                 fontData.font_blur,
                 fontData.font_shadow_point,
                 fontData.font_shadow_blur);
    img_base.composite(img_font,0,fontData.font_point.y-img_font.size().height()/2,Magick::AtopCompositeOp);
  }

  Magick::Blob out;
  img_base.write( &out , "PNG" );
  buf.resize(out.length());
  memcpy(&buf[0],out.data(),out.length());
}

/*
  base    : width,height,color1,color2,grad,blur
  pattern : x,y,pattern,color1,color2,grad,blur
  string1 : x,y,font,size,string,color1,color2,shadow,shadow-color,line-under,line-over,line-through
  string2 : x,y,font,size,string,color1,color2,shadow,shadow-color,line-under,line-over,line-through
  string3 : x,y,font,size,string,color1,color2,shadow,shadow-color,line-under,line-over,line-through
 */
#include <fstream>
int main(int argc,char *argv[]) {
  std::vector<unsigned char> buf;

  gradation_handler_proc(
    buf,
    G(640,480),
    "404040",
    "CCCCCC",
    G(50,50),
    0.35,
    VERTICAL,
    FontData(  
      "Crumbjp",
      "000000",
      "FFFFFF",
      30.0,
      1.0,
      G(485,455),
      Blur(2,0.7),
      G(2,4),
      Blur(4,3.0)
      )
    );

  std::ofstream fp("/mnt/hgfs/shared/foo.png");
  fp.write((const char*)&buf[0],buf.size());
  fp.close();
  return 0;
}
