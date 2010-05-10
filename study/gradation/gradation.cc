/*
 * gradation.cc - ????
 *  
 *   Copyright (C) 2009 rakuten 
 *     by hiroaki.kubota <hiroaki.kubota@rakuten.co.jp> 
 *     Date : 2009/08/19
 */

#include <iostream>
#include <ft2build.h>
#include FT_BITMAP_H
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include "gradation.h"

// void dumpC(const C & c){
//   std::cerr << c.r << ":" << c.g << ":" << c.b << std::endl;
// }

void gen_base_img (
  BMP & bmp,
  const G & img_size,
  const C & color
  ){
  bmp.resize(img_size.x,img_size.y);
  bmp.erase(color);
}
void gen_gradation_img ( 
  BMP & bmp,
  const C & to , 
  const G & point,
  const double & acc , 
  const DIRECTION & direction) 
{
  bmp.gradation(C(),to,point,acc,direction);

  return; // @@@ No magic routine.
  // img.size(img_size.get());
  // if ( direction == NO_DIRECTION ) {
  //   img.backgroundColor(from.get());
  //   img.erase();
  //   return;
  // }
  // C diff = (to - from);
  // // DrawableList
  // Magick::DrawableList list;
  // list.push_back(Magick::DrawableStrokeAntialias(false));
  // struct gradation_line {
  //   Magick::DrawableList &list;
  //   int d,N,L;
  //   gradation_line(Magick::DrawableList &list,int d,int N,int L):list(list),d(d),N(N),L(L){}
  //   void operator()(int i){
  //     if ( d == VERTICAL )        list.push_back(Magick::DrawableLine(0,i,L,i));
  //     else if ( d == HORIZONTAL)  list.push_back(Magick::DrawableLine(i,0,i,L));
  //   }
  // };
  // if ( direction == VERTICAL || direction == HORIZONTAL ) {
  //   std::cerr << "???" << std::endl;      
  //   list.push_back(Magick::DrawableStrokeWidth(1));
  //   int N; // Num of loop
  //   int L; // Line length
  //   int S; // Starting point
  //   if ( direction == VERTICAL ) {
  //     N = img_size.y-1;
  //     L = img_size.x-1;
  //     S = point.y;
  //   }else if ( direction == HORIZONTAL) {
  //     N = img_size.x-1;
  //     L = img_size.y-1;
  //     S = point.x;
  //   }
  //   int R = (S < (N-S))?(N-S):S; // Max range from starting point 
  //   if ( acc == 1 ) {
  //     std::cerr << "ACC=1" << std::endl;      
  //     double resolv = 1.0/R;
  //     for ( int i = 0 ; i <= N ; i++ ){
  //       double a = abs(i-S)*resolv;
  //       list.push_back(Magick::DrawableStrokeColor((from+diff*a).get()));
  //       gradation_line(list,direction,N,L)(i);
  //       if ( a == 0 ) std::cerr << i << ":" << a << std::endl;
  //       if ( a == 1 ) std::cerr << i << ":" << a << std::endl;
  //     }
  //   }else {
  //     std::cerr << "ACC!=1" << std::endl;      
  //     double resolv = 1.0/(pow(acc,R)-1);
  //     for ( int i = 0 ; i <= N ; i++ ){
  //       double a = (pow(acc,abs(i-S))-1)*resolv;
  //       list.push_back(Magick::DrawableStrokeColor((from+diff*a).get()));
  //       gradation_line(list,direction,N,L)(i);
  //       if ( a == 0 ) std::cerr << i << ":" << a << std::endl;
  //       if ( a == 1 ) std::cerr << i << ":" << a << std::endl;
  //     }
  //   }
  // }else if ( direction == CIRCLE ) {
  //   list.push_back(Magick::DrawablePointSize(1));
  //   int W = (point.x<(img_size.x-point.x-1))?(img_size.x-point.x-1):point.x;
  //   int H = (point.y<(img_size.y-point.y-1))?(img_size.y-point.y-1):point.y;
  //   double R = sqrt(W*W+H*H); // Max range from starting point 
  //   if ( acc == 1 ) {
  //     double resolv = 1.0/R;
  //     for ( int x = 0 ; x < img_size.x ; x++ ) {
  //       for ( int y = 0 ; y < img_size.y ; y++ ) {
  //         int X = (point.x - x);
  //         int Y = (point.y - y);
  //         // Magick++'s bug !? Should be generate image self process.
  //         double a = sqrt(X*X+Y*Y)*resolv;
  //         list.push_back(Magick::DrawableStrokeColor((from+diff*a).get()));
  //         list.push_back(Magick::DrawableLine(x,y,x+1,y));
  //         if ( a == 0 ) std::cerr << x << "," << y << ":" << a << std::endl;
  //         if ( a == 1 ) std::cerr << x << "," << y << ":" << a << std::endl;
  //       }
  //     }
  //   }else {
  //     double resolv = 1.0/(pow(acc,R)-1);
  //     for ( int x = 0 ; x < img_size.x ; x++ ) {
  //       for ( int y = 0 ; y < img_size.y ; y++ ) {
  //         int X = (point.x - x);
  //         int Y = (point.y - y);
  //         // Magick++'s bug !? Should be generate image self process.
  //         double a = (pow(acc,sqrt(X*X+Y*Y))-1)*resolv;
  //         list.push_back(Magick::DrawableStrokeColor((from+diff*a).get()));
  //         list.push_back(Magick::DrawableLine(x,y,x+1,y));
  //         if ( a == 0 ) std::cerr << x << "," << y << ":" << a << std::endl;
  //         if ( a == 1 ) std::cerr << x << "," << y << ":" << a << std::endl;
  //       }
  //     }
  //   }
  // }
  // img.draw(list);
}
void gen_font_img ( 
  Magick::Image & img_base ,
  const char * font_str,
  const char * font_name,
  const int & font_size,
  const G & font_point,
  const int & font_line_width,
  C font_color,
  C font_fill_color,
  C font_grad_color,
  const G & font_grad_point,
  double font_grad_acc,
  DIRECTION font_grad_dir,
  const Blur & font_blur)
{
  if (font_fill_color == C(0,0,0) ) {
    font_fill_color = C(1.0/255.0 , 1.0/255.0 , 1.0/255.0);
  }
  if (font_color == C(0,0,0) ) {
    font_color = font_fill_color = C(1.0/255.0,1.0/255.0,1.0/255.0);
  }
  Magick::Color font_bg_color = Magick::Color(0,0,0);
  // gen dummy data
  Magick::Image img_font_edge(img_base.size(),font_bg_color);
  Magick::Image img_font(img_base);
  // img_font_dummy.backgroundColor(font_bg_color);
  // img_font_dummy.size(Magick::Geometry(img_base.size()));
  // img_font_dummy.erase();

  Magick::DrawableList dlist_font;
  dlist_font.push_back(Magick::DrawableStrokeWidth(font_line_width));
  dlist_font.push_back(Magick::DrawableStrokeColor(font_color.get()));
  dlist_font.push_back(Magick::DrawableFillColor(font_fill_color.get()));
  dlist_font.push_back(Magick::DrawableTextAntialias(false));
  dlist_font.push_back(Magick::DrawablePointSize(font_size));
// $ identify -list font
  dlist_font.push_back(Magick::DrawableFont(font_name,Magick::ItalicStyle,700,Magick::NormalStretch));
  dlist_font.push_back(Magick::DrawableText(font_point.x,font_point.y,font_str));
  img_font_edge.draw(dlist_font);
  img_font.draw(dlist_font);
  img_font_edge.blur(font_blur.radius,font_blur.sigma);
  // Gradation
  Magick::Blob blob;
  img_font.write(&blob,"BMP");
  BMP bmp(blob.data());
  // bmp.gradation(font_fill_color,font_fill_color,font_grad_color,font_grad_point,font_grad_acc,font_grad_dir);
  bmp.gradation(font_fill_color,font_grad_color,font_grad_point,font_grad_acc,font_grad_dir);
  blob.update(bmp.bmp,bmp.size());
  img_font.read(blob,img_font.size(),"BMP");
  img_font.blur(font_blur.radius,font_blur.sigma);
  // Marge ( font -> edge -> base )
  Magick::Blob blob_edge;
  img_font.write(&blob,"BMP");
  img_font_edge.write(&blob_edge,"BMP");
  bmp.load(blob.data());
  BMP bmp_edge(blob_edge.data());
  bmp_edge.marge(bmp,C());
  blob.update(bmp_edge.bmp,bmp_edge.size());
  img_font.read(blob,Magick::Geometry(bmp_edge.width,bmp_edge.height),"BMP");
  img_font.transparent(font_bg_color);
  img_base.composite(img_font,0,0,Magick::AtopCompositeOp);
}

struct bmp_data {
  int b;
  int l;
  int w;
  int h;
  FT_Bitmap *t;
};
void raster_span_func(
  int       y,
  int       count,
  FT_Span*  spans,
  void*     user ){
  bmp_data *bd = (bmp_data*)user;
  //std::cerr << "raster y:" << y << " c:" << count << std::endl;
  int n = bd->w*bd->h;
  for ( int i = 0 ; i < count ; i++ ) {
    int a = n-1-(y+bd->b+1)*bd->w + spans[i].x-bd->l;
    std::cerr << "raster y:" << y << " x:" << spans[i].x << " l:" << spans[i].len << " c:" << (int)spans[i].coverage << " a:" << a <<std::endl;
    bd->t->buffer[a] = 0;
    //spans[i].coverage=128;
  }
}

void gradation_handler_proc( 
  buf_t & buf,
  const base_image_params & base,
  const grad_params & grad1,
  const grad_params & grad2,
  const pattern_params & pattern,
  const font_params & font,
  const all_params & all )
{
  G img_size = base.get_size();
  BMP bmp;
  // gen_base_img (
  //   bmp,
  //   base.get_size(),
  //   base.get_color());
  
  // gen_gradation_img (
  //   bmp,
  //   grad1.get_grad_color(),
  //   grad1.get_grad_point(),
  //   grad1.get_grad_accel(),
  //   grad1.get_grad_direction() );
  // gen_gradation_img (
  //   bmp,
  //   grad2.get_grad_color(),
  //   grad2.get_grad_point(),
  //   grad2.get_grad_accel(),
  //   grad2.get_grad_direction() );


    if (!setlocale(LC_CTYPE, "")) {
      throw std::logic_error("Cannot set locale !");
    }
    FT_Library library;
    if ( FT_Init_FreeType(&library) ) {
      throw std::logic_error("Failure : FT_Init_FreeType");
    }
    FT_Face face;
    const char * fontpath = "/usr/share/fonts/vlgothic/VL-Gothic-Regular.ttf";
    if ( FT_New_Face(library, fontpath, 0, &face) ) {
      throw std::logic_error("Failure : FT_New_Face");
    }
    if (!FT_IS_SCALABLE(face)) {
      throw std::logic_error("Failure : FT_IS_SCALABLE");
    }
    int size = 200;
    FT_Set_Pixel_Sizes(face, 0, size);
    if (!face->charmap) {
      // if the font has no charmap, freetype provides one at
      // first glyph load. A charmap is needed for correct
      // target bitmap sizing
      FT_Load_Glyph(face, 'A', 0);
    }
    //const char * text = "fog,あb.r";
    const char * text = "うあb.r";
    //const char * text = "f";

    int char_count = mbstowcs(NULL, text, 0);
    if (char_count == -1) {
      throw std::logic_error("Failure : invalid multibyte sequence");
    }
    std::vector<wchar_t> to_render_buf(char_count + 1);
    wchar_t *to_render = &to_render_buf[0];
    mbstowcs(to_render, text, char_count + 1);

    FT_Pos pen = 0;
    FT_Pos glyph_above = 0;
    FT_Pos glyph_below = 0;
    FT_Pos above = 0;
    FT_Pos below = 0;
    FT_Pos after_glyph = 0;
    int prev_glyph = 0;
    FT_Pos x_start = 0;
    for (int i = 0; i < char_count; i++) {
      FT_Pos cur_glyph = FT_Get_Char_Index(face, to_render[i]);

      //if ( FT_Load_Glyph(face, cur_glyph, FT_LOAD_LINEAR_DESIGN) ) {
      if ( FT_Load_Glyph(face, cur_glyph, FT_LOAD_DEFAULT ) ) {
        //if ( FT_Load_Glyph(face, cur_glyph, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) ) {
        throw std::logic_error("Failure : FT_Load_Glyph");
      }
      if ( i == 0 ) {
      //   x_start = face->glyph->metrics.horiBearingX;
      }else{
        // if(FT_HAS_KERNING(face)) {
        //   FT_Vector v;
        //   FT_Get_Kerning(face, cur_glyph, prev_glyph, 0, &v);
        //   std::cerr << "kerning x:" << v.x << " y:" << v.y << std::endl;
        // }
      }
      if ( face->glyph->format == FT_GLYPH_FORMAT_OUTLINE){
        // // Bold
        // FT_Outline_Embolden(&face->glyph->outline, 1000);
        // // Itaric
        // FT_Matrix matrix;
        // matrix.xx = 0x10000;
        // matrix.xy = 0x5800;
        // matrix.yx = 0;
        // matrix.yy = 0x10000;
        // FT_Outline_Transform(&face->glyph->outline, &matrix);
      }
      int baseline = (face->glyph->metrics.height-face->glyph->metrics.horiBearingY)/64;
      int height = face->glyph->metrics.height/64;
      int width  = face->glyph->metrics.width/64;
      int left = face->glyph->metrics.horiBearingX/64;
      
//      if ( FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO) ) {
      if ( FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) ) {
        throw std::logic_error("Failure : FT_Render_Glyph");
      }
      FT_Bitmap &bitmap = face->glyph->bitmap;

      // FT_Bitmap bitmap;
      // FT_Bitmap_New(&bitmap);
      // bitmap.width=face->glyph->bitmap.width;
      // bitmap.rows=face->glyph->bitmap.rows;
      // bitmap.pitch=face->glyph->bitmap.pitch;
      // bitmap.pixel_mode=FT_PIXEL_MODE_GRAY;
      // bitmap.num_grays=256;
      // std::vector<unsigned char> bmp_buf(bitmap.width*bitmap.rows);
      // bitmap.buffer=&bmp_buf[0];
      // FT_Outline_Get_Bitmap(library,&face->glyph->outline,&bitmap);
      bmp_data bd;
      bd.h = height;
      bd.w = width;
      bd.l = left;
      bd.b = baseline;
      bd.t = &bitmap;
      FT_Raster_Params  params;
      params.flags      = ft_raster_flag_aa | ft_raster_flag_direct;
      params.gray_spans = (FT_Raster_Span_Func)raster_span_func;
      params.target = &bitmap;
      params.source = &face->glyph->outline;
      params.user       = &bd;
      if ( FT_Outline_Render( library, &face->glyph->outline, &params ) ) {
        throw std::logic_error("Failure : FT_Outline_Render");
      }
      std::cerr << "bwidth " << bitmap.width << std::endl;
      std::cerr << "mwidth " << width << std::endl;
      std::cerr << "mrows " << height << std::endl;
//    std::cerr << "bearingY " << face->glyph->metrics.horiBearingY << std::endl;
      std::cerr << "baseline " << baseline << std::endl;
      std::cerr << "bleft " << face->glyph->bitmap_left << std::endl;
      std::cerr << "btop " << face->glyph->bitmap_top << std::endl;
      std::cerr << "badb " << face->glyph->advance.x/64 << std::endl;
      std::cerr << "out_contours " << face->glyph->outline.n_contours << std::endl;
      std::cerr << "out_points " << face->glyph->outline.n_points << std::endl;
      // glyph_above = face->glyph->metrics.horiBearingY;
      // glyph_below = face->glyph->metrics.height - glyph_above;
      // if (glyph_above > above) above = glyph_above;
      // if (glyph_below > below) below = glyph_below;
      // after_glyph = pen + face->glyph->metrics.width + face->glyph->metrics.horiBearingX;
      // pen += face->glyph->advance.x;
      // prev_glyph = cur_glyph;
    bmp.resize(bitmap.width,bitmap.rows);
    int npixels = bitmap.width*bitmap.rows;
    for(int r=0;r<npixels;r++){
      //std::cerr << "***********" << std::endl;
      //std::cerr << bitmap.buffer[r] << std::endl;
      tagRGBQUAD rgb;
      rgb.rgbBlue = 0;
      rgb.rgbGreen = 0;
      rgb.rgbRed = 0;
      if ( bitmap.buffer[r] & 0xff ) {
        rgb.rgbBlue = bitmap.buffer[r] & 0xff;
        rgb.rgbGreen = bitmap.buffer[r] & 0xff;
        rgb.rgbRed = bitmap.buffer[r] & 0xff;
        // if ( bitmap.buffer[r] != 0xff ) {
        //   rgb.rgbRed = 0;
        // }
      }
      if ( bitmap.buffer[r] & 0x1 ) {
        //std::cerr << bitmap.buffer[r] << std::endl;
//        rgb.rgbGreen = 0;
      }
      if ( bitmap.buffer[r] & 0x2 ) {
//        rgb.rgbGreen = 0;
      }
      if ( bitmap.buffer[r] & 0x4 ) {
        //rgb.rgbBlue = 0;
      }
      if ( bitmap.buffer[r] & 0x8 ) {
//        rgb.rgbGreen = 0;
      }
      if ( bitmap.buffer[r] & 0xf0 ) {
//        rgb.rgbGreen = 0;
      }
      if ( (bitmap.rows-1-r/bitmap.width) == (face->glyph->metrics.height-face->glyph->metrics.horiBearingY)/64 ){
        rgb.rgbRed = 128;
        rgb.rgbGreen = 128;
        rgb.rgbBlue = 128;
      }
//      bmp.set(r,rgb);
      bmp.set(r%bitmap.width,bitmap.rows-1-r/bitmap.width,rgb);
    }
    // tagRGBQUAD rgb;
    // rgb.rgbRed=255;
    // rgb.rgbBlue=0;
    // rgb.rgbGreen=0;
    // for( int o = 0 ; o < face->glyph->outline.n_points ; o++ ) {
    //   bmp.set(face->glyph->outline.points[o].x/64,face->glyph->outline.points[o].y/64,rgb);
    // //   std::cerr << "out (" << face->glyph->outline.points[o].x/64<<","<<face->glyph->outline.points[o].y/64<<")" << std::endl;
    // }
    break;
    
    }

    // int width = (after_glyph - x_start)/64;
    // int height = (above+below)/64;
    // std::cerr << "after " << after_glyph << std::endl;
    // std::cerr << "x_start " << x_start << std::endl;
    // std::cerr << "above " << above << std::endl;
    // std::cerr << "below " << below << std::endl;
    // std::cerr << "width " << width << std::endl;
    // std::cerr << "height " << height << std::endl;

    



    //bmp.resize(width,height);
    //bmp.load(bitmap);
    // std::cerr << "size " << bmp.size() << std::endl;




  Magick::Image img_base(Magick::Geometry(1,1),Magick::Color(0,0,0));
  Magick::Blob in(bmp.bmp,bmp.size());
  img_base.read(in,img_size.get(),"BMP");

  // if ( font.get_name()) {
  //   if ( font.get_shadow() != NO_SHADOW ) {
  //     gen_font_img(
  //       img_base,
  //       font.get_string(),
  //       font.get_name(),
  //       font.get_size(),
  //       font.get_point() + font.get_shadow_point(),
  //       font.get_line_width(),
  //       font.get_color(),
  //       font.get_fill_color(),
  //       font.q_grad_parms.get_grad_color(),
  //       font.q_grad_parms.get_grad_point(),
  //       font.q_grad_parms.get_grad_accel(),
  //       font.q_grad_parms.get_grad_direction(),
  //       font.get_shadow_blur()
  //       );
  //   }
  //   gen_font_img(
  //     img_base,
  //     font.get_string(),
  //     font.get_name(),
  //     font.get_size(),
  //     font.get_point(),
  //     font.get_line_width(),
  //     font.get_color(),
  //     font.get_fill_color(),
  //     font.q_grad_parms.get_grad_color(),
  //     font.q_grad_parms.get_grad_point(),
  //     font.q_grad_parms.get_grad_accel(),
  //     font.q_grad_parms.get_grad_direction(),
  //     font.get_blur()
  //     );
  // }
  
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
  try { 
    std::vector<unsigned char> buf;
    // gradation_handler_proc(
    //   buf,
    //   base_image_params ( 
    //     "3","200",    // width , height
    //     "F1F3F4"       // color
    //     ),
    //   grad_params ( 
    //     "FFFFFF",       // gradation-color
    //     "70","200",     // gradation start point(x,y)
    //     "1",            // gradation direction
    //     "1.5"         // gradation accel
    //     ),
    //   grad_params ( 
    //     "",       // gradation-color
    //     "","",     // gradation start point(x,y)
    //     "",            // gradation direction
    //     ""         // gradation accel
    //     ),
    //   pattern_params (
    //     "","","","","","","","","","","","","","","","","",""
    //     ),
    //   font_params (
    //     "", // string
    //     "","", // font (name,size)
    //     "","",    // width , height
    //     "",          // line-width
    //     "",     // color
    //     "",     // fill-color
    //     grad_params ( 
    //       "",       // gradation-color
    //       "","",     // gradation start point(x,y)
    //       "",            // gradation direction
    //       ""         // gradation accel
    //       ),
    //     "","",    // blur ( radius , sigma )
    //     "",          // shadow-type
    //     "","",     // shadow point
    //     "",""     // shadow blur ( radius , sigma )
    //     ),
    //   all_params (
    //     "","" 
    //     )
    //   );
    gradation_handler_proc(
      buf,
      base_image_params ( 
        "250","70",    // width , height
        "FF6868"       // color
        ),
      grad_params ( 
        "FFFFFF",       // gradation-color
        "120","20",     // gradation start point(x,y)
        "1",            // gradation direction
        "5.0"         // gradation accel
        ),
      grad_params ( 
        "FFFFFF",       // gradation-color
        "55","20",     // gradation start point(x,y)
        "2",            // gradation direction
        "10.0"         // gradation accel
        ),
      pattern_params (
        "","","","","","","","","","","","","","","","","",""
        ),
      font_params (
        "Hirano", // string
        "Times","50", // font (name,size)
        "65","50",    // width , height
        "1",          // line-width
        "D0E030",     // color
        "FFFFFF",     // fill-color
        grad_params ( 
          "FFFF90",       // gradation-color
          "125","35",     // gradation start point(x,y)
          "3",            // gradation direction
          "6.0"         // gradation accel
          ),
        "0","0.0",    // blur ( radius , sigma )
        "1",          // shadow-type
        "5","10",     // shadow point
        "5","2.5"     // shadow blur ( radius , sigma )
        ),
      all_params (
        "","" 
        )
      );
    
    std::ofstream fp("/usr/local/apache2/htdocs/foo.png");
    fp.write((const char*)&buf[0],buf.size());
    fp.close();
  }catch ( const std::exception &ex ) {
    std::cerr << ex.what() << std::endl;
  }
  return 0;
}
