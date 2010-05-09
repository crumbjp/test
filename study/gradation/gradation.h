/*
 * gradation.h - ????
 *
 *   Copyright (C) 2009 rakuten 
 *     by crumb <hiroaki.kubota@rakuten.co.jp> 
 *     Date : 2009/08/03
 */
#ifndef GRADATION_H
#define GRADATION_H

#include "Magick++.h"
#include <string.h>
#include <stdexcept>
#include "common.h"
#include "bmp.h"

struct base_image_params {
  const char * q_width;
  const char * q_height;
  const char * q_color;
  G get_size() const {
    unsigned long w = strtoul(q_width,0,10);
    unsigned long h = strtoul(q_height,0,10);
    return G(w,h);
  }
  C get_color() const {
    return C::get(q_color);
  }
  base_image_params (
    const char * q_width,
    const char * q_height,
    const char * q_color ) : 
    q_width(q_width),
    q_height(q_height),
    q_color(q_color) {}
};

struct grad_params {
  const char * q_grad_color;
  const char * q_grad_x;
  const char * q_grad_y;
  const char * q_grad_dir;
  const char * q_grad_acc;
  C get_grad_color () const { 
    return C::get(q_grad_color);
  }
  G get_grad_point () const { 
    unsigned long x = strtoul(q_grad_x,0,10);
    unsigned long y = strtoul(q_grad_y,0,10);
    return G(x,y);
  }
  double get_grad_accel () const { 
    double acc = strtod(q_grad_acc,0);
    return acc;
  }
  DIRECTION get_grad_direction () const { 
    return getD(q_grad_dir);
  }
  grad_params (
    const char * q_grad_color,
    const char * q_grad_x,
    const char * q_grad_y,
    const char * q_grad_dir,
    const char * q_grad_acc ):
    q_grad_color(q_grad_color),
    q_grad_x(q_grad_x),
    q_grad_y(q_grad_y),
    q_grad_dir(q_grad_dir),
    q_grad_acc(q_grad_acc) { }
};
  
struct pattern_params {
  const char *q_p1_id;
  const char *q_p1_x;
  const char *q_p1_y;
  const char *q_p1_width;
  const char *q_p1_height;
  const char *q_p1_color;
  const char *q_p1_fill_color;
  const char *q_p1_grad_color;
  const char *q_p1_grad_x;
  const char *q_p1_grad_y;
  const char *q_p1_grad_dir;
  const char *q_p1_grad_acc;
  const char *q_p1_blur_radius;
  const char *q_p1_blur_sigma;
  const char *q_p1_shadow_x;
  const char *q_p1_shadow_y;
  const char *q_p1_shadow_blur_radius;
  const char *q_p1_shadow_blur_sigma;
  pattern_params (
    const char *q_p1_id,
    const char *q_p1_x,
    const char *q_p1_y,
    const char *q_p1_width,
    const char *q_p1_height,
    const char *q_p1_color,
    const char *q_p1_fill_color,
    const char *q_p1_grad_color,
    const char *q_p1_grad_x,
    const char *q_p1_grad_y,
    const char *q_p1_grad_dir,
    const char *q_p1_grad_acc,
    const char *q_p1_blur_radius,
    const char *q_p1_blur_sigma,
    const char *q_p1_shadow_x,
    const char *q_p1_shadow_y,
    const char *q_p1_shadow_blur_radius,
    const char *q_p1_shadow_blur_sigma ):
    q_p1_id(q_p1_id),
    q_p1_x(q_p1_x),
    q_p1_y(q_p1_y),
    q_p1_width(q_p1_width),
    q_p1_height(q_p1_height),
    q_p1_color(q_p1_color),
    q_p1_fill_color(q_p1_fill_color),
    q_p1_grad_color(q_p1_grad_color),
    q_p1_grad_x(q_p1_grad_x),
    q_p1_grad_y(q_p1_grad_y),
    q_p1_grad_dir(q_p1_grad_dir),
    q_p1_grad_acc(q_p1_grad_acc),
    q_p1_blur_radius(q_p1_blur_radius),
    q_p1_blur_sigma(q_p1_blur_sigma),
    q_p1_shadow_x(q_p1_shadow_x),
    q_p1_shadow_y(q_p1_shadow_y),
    q_p1_shadow_blur_radius(q_p1_shadow_blur_radius),
    q_p1_shadow_blur_sigma(q_p1_shadow_blur_sigma) { }
};

struct font_params {
  const char * q_string;
  const char * q_name;
  const char * q_size;
  const char * q_x;
  const char * q_y;
  const char * q_line;
  const char * q_color;
  const char * q_fill_color;
  const grad_params & q_grad_parms;
  const char * q_blur_radius;
  const char * q_blur_sigma;
  const char * q_shadow;
  const char * q_shadow_x;
  const char * q_shadow_y;
  const char * q_shadow_blur_radius;
  const char * q_shadow_blur_sigma;
  const char * get_string() const {
    return q_string;
  }
  const char * get_name() const {
    return q_name;
  }
  int get_size() const {
    unsigned long size = strtoul(q_size,0,10);
    return size;
  }
  G get_point() const {
    unsigned long x = strtoul(q_x,0,10);
    unsigned long y = strtoul(q_y,0,10);
    return G(x,y);
  }
  int get_line_width() const {
    unsigned long w = strtoul(q_line,0,10);
    return w;
  }
  C get_color() const {
    return C::get(q_color);
  }
  C get_fill_color() const {
    return C::get(q_fill_color);
  }
  Blur get_blur() const {
    return Blur::get(q_blur_radius,q_blur_sigma);
  }
  SHADOW_TYPE get_shadow() const {
    return getS(q_shadow);
  }
  G get_shadow_point() const {
    unsigned long x = strtoul(q_shadow_x,0,10);
    unsigned long y = strtoul(q_shadow_y,0,10);
    return G(x,y);
  }
  Blur get_shadow_blur() const {
    return Blur::get(q_shadow_blur_radius,q_shadow_blur_sigma);
  }

  font_params (
    const char * q_string,
    const char * q_name,
    const char * q_size,
    const char * q_x,
    const char * q_y,
    const char * q_line,
    const char * q_color,
    const char * q_fill_color,
    const grad_params & q_grad_parms,
    const char * q_blur_radius,
    const char * q_blur_sigma,
    const char * q_shadow,
    const char * q_shadow_x,
    const char * q_shadow_y,
    const char * q_shadow_blur_radius,
    const char * q_shadow_blur_sigma ):
    q_string(q_string),
    q_name(q_name),
    q_size(q_size),
    q_x(q_x),
    q_y(q_y),
    q_line(q_line),
    q_color(q_color),
    q_fill_color(q_fill_color),
    q_grad_parms(q_grad_parms),
    q_blur_radius(q_blur_radius),
    q_blur_sigma(q_blur_sigma),
    q_shadow(q_shadow),
    q_shadow_x(q_shadow_x),
    q_shadow_y(q_shadow_y),
    q_shadow_blur_radius(q_shadow_blur_radius),
    q_shadow_blur_sigma(q_shadow_blur_sigma) { }
};

struct all_params {
  const char *q_al_blur_radius;
  const char *q_al_blur_sigma;
  all_params (
    const char *q_al_blur_radius,
    const char *q_al_blur_sigma ) :
    q_al_blur_radius(q_al_blur_radius),
    q_al_blur_sigma(q_al_blur_sigma) { }
};

void gradation_handler_proc( 
  buf_t & buf,
  const base_image_params & base,
  const grad_params & grad1,
  const grad_params & grad2,
  const pattern_params & pattern,
  const font_params & font,
  const all_params & all );

#endif
