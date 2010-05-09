/*
 * bmp.h - ????
 *
 *   Copyright (C) 2009 rakuten 
 *     by crumb <hiroaki.kubota@rakuten.co.jp> 
 *     Date : 2009/08/04
 */
#ifndef BMP_H
#define BMP_H

#include <iostream>
#include "common.h"
extern "C" {
  typedef struct tagBITMAPFILEHEADER {
    uint16_t bfType __attribute__((packed));
    uint32_t bfSize  __attribute__((packed));
    uint16_t bfReserved1  __attribute__((packed));
    uint16_t bfReserved2 __attribute__((packed));
    uint32_t  bfOffBits __attribute__((packed));
  } BITMAPFILEHEADER;
  typedef struct tagBITMAPINFOHEADER{
    uint32_t biSize __attribute__((packed));
    int32_t  biWidth __attribute__((packed));
    int32_t  biHeight __attribute__((packed));
    uint16_t biPlanes __attribute__((packed));
    uint16_t biBitCount __attribute__((packed));
    uint32_t biCompression __attribute__((packed));
    uint32_t biSizeImage __attribute__((packed));
    int32_t  biXPixPerMeter __attribute__((packed));
    int32_t  biYPixPerMeter __attribute__((packed));
    uint32_t biClrUsed __attribute__((packed));
    uint32_t biClrImporant __attribute__((packed));
  } BITMAPINFOHEADER;
  typedef struct tagRGBQUAD{
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbReserved;
  } RGBQUAD;
  typedef struct tagBITMAP{
    tagBITMAPFILEHEADER fheader;
    tagBITMAPINFOHEADER iheader;
    //tagRGBQUAD rgbs[1];
  }BITMAP;
};
static tagRGBQUAD getRGB(const C & c) {
  tagRGBQUAD buf;
  buf.rgbRed = round(c.r*255);
  buf.rgbGreen = round(c.g*255);
  buf.rgbBlue = round(c.b*255);
  buf.rgbReserved = 0x00;
  return buf;
}
static C getC(const tagRGBQUAD & rgb) {
  return C(rgb.rgbRed/255.0,rgb.rgbGreen/255.0,rgb.rgbBlue/255.0);
}
static bool compRGB(const tagRGBQUAD &rgb1,const tagRGBQUAD &rgb2){
  return (memcmp(&rgb1,&rgb2,sizeof(tagRGBQUAD)) == 0);
}
struct BMP {
  tagBITMAP * bmp;
  uint32_t real_size;
  uint32_t width;
  uint32_t height;
  BMP () 
    : bmp(0),real_size(0),width(0),height(0){}
  BMP ( int32_t width , int32_t height ) 
    : bmp(0),real_size(0){ 
    resize(width,height);
  }
  BMP ( const BMP &in )
    : bmp(0),real_size(0){ 
    resize(in.bmp->iheader.biWidth,in.bmp->iheader.biHeight);
    memcpy(bmp,&in,in.size());
  }
  BMP (const void * in)
    : bmp(0),real_size(0){ 
    this->load(in);
  }
  ~BMP(){
    free();
  }
  BMP & load(const void * in){
    this->resize(((const tagBITMAP*)in)->iheader.biWidth,((const tagBITMAP*)in)->iheader.biHeight,((const tagBITMAP*)in)->fheader.bfOffBits);
    memcpy(bmp,in,((const tagBITMAP*)in)->fheader.bfSize );
    return *this;
  }
  
  void free(){
    if ( bmp ) {
      delete bmp;
    }
    bmp = 0;
  }
  static inline uint32_t calcSize ( int32_t width , int32_t height , int32_t offset ) {
    return offset  + height * width * sizeof(tagRGBQUAD);
  }
  uint32_t size() const {
    return this->bmp->fheader.bfSize;
  }
  void alloc(int32_t width , int32_t height , int32_t offset ) { 
    uint32_t newsize = calcSize(width,height,offset);
    if ( ! this->bmp || newsize >= this->real_size ) {
      this->real_size = newsize;
      this->bmp = reinterpret_cast<tagBITMAP*>(new char[this->real_size]);
    }
  }
  BMP & resize(int32_t width , int32_t height , int32_t offset =  sizeof(tagBITMAPFILEHEADER) + sizeof(tagBITMAPINFOHEADER) ) { 
    this->alloc(width,height,offset);
    this->width = width;
    this->height = height;
    this->bmp->fheader.bfType=0x4d42;
    this->bmp->fheader.bfSize=calcSize(width,height,offset);
    this->bmp->fheader.bfReserved1 = 0;
    this->bmp->fheader.bfReserved2 = 0;
    this->bmp->fheader.bfOffBits = offset;
    this->bmp->iheader.biSize = offset - sizeof(tagBITMAPFILEHEADER);
    this->bmp->iheader.biWidth = width;
    this->bmp->iheader.biHeight = height;
    this->bmp->iheader.biPlanes = 1;
    this->bmp->iheader.biBitCount = 32;
    this->bmp->iheader.biCompression = 0;
    this->bmp->iheader.biSizeImage = 0;
    this->bmp->iheader.biXPixPerMeter = 0;
    this->bmp->iheader.biYPixPerMeter = 0;
    this->bmp->iheader.biClrUsed = 0;
    this->bmp->iheader.biClrImporant = 0;
    return *this;
  }
  BMP & erase( const C & c = C()) {
    for ( int32_t h = 0 ; h < this->bmp->iheader.biHeight; h++ ) {
      for ( int32_t w = 0 ; w < this->bmp->iheader.biWidth; w++ ) {
        this->set(w,h,c);
      }
    }
    return *this;
  }
  BMP & set ( int32_t x , int32_t y , const C & c) {
    return this->set(x,y,getRGB(c));
  }
  BMP & set ( int32_t x , int32_t y , const tagRGBQUAD & c) {
    tagRGBQUAD * rgbs = reinterpret_cast<tagRGBQUAD*>(reinterpret_cast<uint8_t *>(this->bmp) + this->bmp->fheader.bfOffBits);
    rgbs[x+y*this->bmp->iheader.biWidth] = c;
    return *this;
  }
  tagRGBQUAD & get ( int32_t x , int32_t y)  {
    tagRGBQUAD * rgbs = reinterpret_cast<tagRGBQUAD*>(reinterpret_cast<uint8_t *>(this->bmp) + this->bmp->fheader.bfOffBits);
    return rgbs[x+y*this->bmp->iheader.biWidth];
  }
  const tagRGBQUAD & get ( int32_t x , int32_t y)  const {
    tagRGBQUAD * rgbs = reinterpret_cast<tagRGBQUAD*>(reinterpret_cast<uint8_t *>(this->bmp) + this->bmp->fheader.bfOffBits);
    return rgbs[x+y*this->bmp->iheader.biWidth];
  }
  void gradation (
    const C & penetration_target,
    const C & to , 
    const G & point,
    const double & acc , 
    const DIRECTION & direction) {
    struct gradation_line {
      BMP & bmp;
      DIRECTION d;
      int L;
      const C & penetration_target;
      gradation_line(BMP & bmp, DIRECTION d,int L,const C & penetration_target):bmp(bmp),d(d),L(L),penetration_target(penetration_target){}
      void operator()(int n,double a,const C & grad_color){
        tagRGBQUAD t = getRGB(penetration_target);
        for ( int l = 0 ; l <= L ; l++ ){
          if ( d == VERTICAL ) {
            if ( ! compRGB(t,bmp.get(l,n))) {
              const C & base = getC(bmp.get(l,n));
              bmp.set(l,n,base+(grad_color-base)*a);
            }
          } else if ( d == HORIZONTAL) {
            if ( ! compRGB(t,bmp.get(n,l))) {
              const C & base = getC(bmp.get(n,l));
              bmp.set(n,l,base+(grad_color-base)*a);
            }
          }
        }
      }
    };
//    C diff = (to - from);
    if ( direction == VERTICAL || direction == HORIZONTAL ) {
      int N; // Num of loop
      int L; // Line length
      int S; // Starting point
      if ( direction == VERTICAL ) {
        N = this->height-1;
        L = this->width-1;
        S = point.y;
      }else if ( direction == HORIZONTAL) {
        N = this->width-1;
        L = this->height-1;
        S = point.x;
      }
      int R = (S < (N-S))?(N-S):S; // Max range from starting point 
      double resolv = 1/(pow(R,acc));
      for ( int n = 0 ; n <= N ; n++ ){
        double a = pow(R-abs(n-S),acc)*resolv; 
        gradation_line(*this,direction,L,penetration_target)(n,a,to);
        // if ( a == 0 ) std::cerr << n << ":" << a << std::endl;
        // if ( a == 1 ) std::cerr << n << ":" << a << std::endl;
      }
    }else if ( direction == CIRCLE ) {
      tagRGBQUAD t = getRGB(penetration_target);
      int W = (point.x<(this->width-point.x-1))?(this->width-point.x-1):point.x;
      int H = (point.y<(this->height-point.y-1))?(this->height-point.y-1):point.y;
      double R = sqrt(W*W+H*H); // Max range from starting point 
      // if ( acc == 1 ) {
      //   double resolv = 1.0/R;
      //   for ( int x = 0 ; x < this->width ; x++ ) {
      //     for ( int y = 0 ; y < this->height ; y++ ) {
      //       int X = (point.x - x);
      //       int Y = (point.y - y);
      //       // Magick++'s bug !? Should be generate image self process.
      //       double a = sqrt(X*X+Y*Y)*resolv;
      //       if ( !compRGB(t,this->get(x,y))) this->set(x,y,(from+diff*a));
      //       if ( a == 0 ) std::cerr << x << "," << y << ":" << a << std::endl;
      //       if ( a == 1 ) std::cerr << x << "," << y << ":" << a << std::endl;
      //     }
      //   }
      // }else {
      double resolv = 1/(pow(R,acc));
      // double resolv = 1.0/(pow(acc,R)-1);
      for ( int x = 0 ; x < this->width ; x++ ) {
        for ( int y = 0 ; y < this->height ; y++ ) {
          int X = (point.x - x);
          int Y = (point.y - y);
          // Magick++'s bug !? Should be generate image self process.
          double a = pow(R-sqrt(X*X+Y*Y),acc)*resolv; 
          // double a = (pow(acc,sqrt(X*X+Y*Y))-1)*resolv;
          if ( !compRGB(t,this->get(x,y))) {
            const C & base = getC(this->get(x,y));
            this->set(x,y,base+(to-base)*a);
          }
          // if ( a == 0 ) std::cerr << x << "," << y << ":" << a << std::endl;
          // if ( a == 1 ) std::cerr << x << "," << y << ":" << a << std::endl;
        }
      }
    }
    // }else if ( direction == NO_DIRECTION ) {
    //   tagRGBQUAD t = getRGB(penetration_target);
    //   for ( int x = 0 ; x < this->width ; x++ ) {
    //     for ( int y = 0 ; y < this->height ; y++ ) {
    //       if ( !compRGB(t,this->get(x,y))) this->set(x,y,(from));
    //     }
    //   }          
    // }
  }
  void marge( const BMP & in , 
              const C & penetration_target )  {
    tagRGBQUAD t = getRGB(penetration_target);
    for ( int x = 0 ; x < this->width ; x++ ) {
      for ( int y = 0 ; y < this->height ; y++ ) {
        if ( !compRGB(t,this->get(x,y))) this->set(x,y,in.get(x,y));
      }
    }
  }
};

#endif
