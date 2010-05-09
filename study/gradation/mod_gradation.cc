#include "httpd.h"
#include "http_log.h"
#include "http_core.h"
#include "http_config.h"
#include "http_protocol.h"
#include "http_request.h"
#include "apr_strings.h"

#include "gradation.h"

static const apr_table_t * parse_parms( request_rec * r , const char * data ) {
  apr_table_t *params = apr_table_make( r->pool , 1);
  if ( data ) {
    const char * command = data;
    const char * token;
    while ((*command != '\0') && (token = ap_getword(r->pool, &command, '&'))) {
      char *key;
      char *value;

      key = ap_getword(r->pool, &token, '=');
      value = ap_getword(r->pool, &token, '=');
      ap_unescape_url(key);
      ap_unescape_url(value);

      apr_table_set(params, key, value);
    }
  }
  return params;
}


static int gradation_handler(request_rec *r)
{
  if (strcmp(r->handler, "gradation")) {
    return DECLINED;
  }
  r->content_type = "image/png";

  if (r->header_only)
    return OK;

  const apr_table_t * query_parms = parse_parms(r,r->args);
  // base image
  const char *q_b_width               = apr_table_get(query_parms, "b-width"); // px
  const char *q_b_height              = apr_table_get(query_parms, "b-height"); // px
  const char *q_b_color               = apr_table_get(query_parms, "b-color"); // rgb
  const char *q_grad1_color          = apr_table_get(query_parms, "b-grad1-color"); // rgb
  const char *q_grad1_x              = apr_table_get(query_parms, "b-grad1-x"); // px
  const char *q_grad1_y              = apr_table_get(query_parms, "b-grad1-y"); // px
  const char *q_grad1_dir            = apr_table_get(query_parms, "b-grad1-dir"); // direction
  const char *q_grad1_acc            = apr_table_get(query_parms, "b-grad1-acc"); // double
  const char *q_grad2_color          = apr_table_get(query_parms, "b-grad2-color"); // rgb
  const char *q_grad2_x              = apr_table_get(query_parms, "b-grad2-x"); // px
  const char *q_grad2_y              = apr_table_get(query_parms, "b-grad2-y"); // px
  const char *q_grad2_dir            = apr_table_get(query_parms, "b-grad2-dir"); // direction
  const char *q_grad2_acc            = apr_table_get(query_parms, "b-grad2-acc"); // double
  // pattern image 1
  const char *q_p1_id                 = apr_table_get(query_parms, "p1-id"); // pattern id
  const char *q_p1_x                  = apr_table_get(query_parms, "p1-x"); // px
  const char *q_p1_y                  = apr_table_get(query_parms, "p1-y"); // px
  const char *q_p1_width              = apr_table_get(query_parms, "p1-width"); // px
  const char *q_p1_height             = apr_table_get(query_parms, "p1-height"); // px
  const char *q_p1_color              = apr_table_get(query_parms, "p1-color"); // rgb
  const char *q_p1_fill_color         = apr_table_get(query_parms, "p1-fill-color"); // rgb
  const char *q_p1_grad_color         = apr_table_get(query_parms, "p1-grad-color"); // rgb
  const char *q_p1_grad_dir           = apr_table_get(query_parms, "p1-grad-dir"); // direction
  const char *q_p1_grad_x             = apr_table_get(query_parms, "p1-grad-x"); // px
  const char *q_p1_grad_y             = apr_table_get(query_parms, "p1-grad-y"); // px
  const char *q_p1_grad_acc           = apr_table_get(query_parms, "p1-grad-acc"); // double
  const char *q_p1_blur_radius        = apr_table_get(query_parms, "p1-blur-radius"); // double
  const char *q_p1_blur_sigma         = apr_table_get(query_parms, "p1-blur-sigma"); // double
  const char *q_p1_shadow_x           = apr_table_get(query_parms, "p1-shadow-x"); // px
  const char *q_p1_shadow_y           = apr_table_get(query_parms, "p1-shadow-y"); // px
  const char *q_p1_shadow_blur_radius = apr_table_get(query_parms, "p1-shadow-blur-radius"); // double
  const char *q_p1_shadow_blur_sigma  = apr_table_get(query_parms, "p1-shadow-blur-sigma"); // double
  // font image 1
  const char *q_f1_string             = apr_table_get(query_parms, "f1-string"); // font-string
  const char *q_f1_name               = apr_table_get(query_parms, "f1-name"); // font-name
  const char *q_f1_size               = apr_table_get(query_parms, "f1-size"); // px
  const char *q_f1_x                  = apr_table_get(query_parms, "f1-x"); // px
  const char *q_f1_y                  = apr_table_get(query_parms, "f1-y"); // px
  const char *q_f1_line               = apr_table_get(query_parms, "f1-line"); // px
  const char *q_f1_color              = apr_table_get(query_parms, "f1-color"); // rgb
  const char *q_f1_fill_color         = apr_table_get(query_parms, "f1-fill-color"); // rgb
  const char *q_f1_grad_color         = apr_table_get(query_parms, "f1-grad-color"); // rgb
  const char *q_f1_grad_x             = apr_table_get(query_parms, "f1-grad-x"); // px
  const char *q_f1_grad_y             = apr_table_get(query_parms, "f1-grad-y"); // px
  const char *q_f1_grad_dir           = apr_table_get(query_parms, "f1-grad-dir"); // direction
  const char *q_f1_grad_acc           = apr_table_get(query_parms, "f1-grad-acc"); // double
  const char *q_f1_blur_radius        = apr_table_get(query_parms, "f1-blur-radius"); // double
  const char *q_f1_blur_sigma         = apr_table_get(query_parms, "f1-blur-sigma"); // double
  const char *q_f1_shadow             = apr_table_get(query_parms, "f1-shadow"); // shadow-type
  const char *q_f1_shadow_x           = apr_table_get(query_parms, "f1-shadow-x"); // px
  const char *q_f1_shadow_y           = apr_table_get(query_parms, "f1-shadow-y"); // px
  const char *q_f1_shadow_blur_radius = apr_table_get(query_parms, "f1-shadow-blur-radius"); // double
  const char *q_f1_shadow_blur_sigma  = apr_table_get(query_parms, "f1-shadow-blur-sigma"); // double
  // font image 2
  // font image 3
  // all
  const char *q_al_blur_radius        = apr_table_get(query_parms, "al-blur-radius"); // double
  const char *q_al_blur_sigma         = apr_table_get(query_parms, "al-blur-radius"); // double

  buf_t buf;
  gradation_handler_proc( 
    buf,
    // Base
    base_image_params (
      q_b_width,
      q_b_height,
      q_b_color),
    grad_params ( 
      q_grad1_color,
      q_grad1_x,
      q_grad1_y,
      q_grad1_dir,
      q_grad1_acc ),
    grad_params ( 
      q_grad2_color,
      q_grad2_x,
      q_grad2_y,
      q_grad2_dir,
      q_grad2_acc ),
    // Pattern
    pattern_params (
      q_p1_id,
      q_p1_x,
      q_p1_y,
      q_p1_width,
      q_p1_height,
      q_p1_color,
      q_p1_fill_color,
      q_p1_grad_color,
      q_p1_grad_x,
      q_p1_grad_y,
      q_p1_grad_dir,
      q_p1_grad_acc,
      q_p1_blur_radius,
      q_p1_blur_sigma,
      q_p1_shadow_x,
      q_p1_shadow_y,
      q_p1_shadow_blur_radius,
      q_p1_shadow_blur_sigma ),
    // Font
    font_params (
      q_f1_string,
      q_f1_name,
      q_f1_size,
      q_f1_x,
      q_f1_y,
      q_f1_line,
      q_f1_color,
      q_f1_fill_color,
      grad_params ( 
        q_f1_grad_color,
        q_f1_grad_x,
        q_f1_grad_y,
        q_f1_grad_dir,
        q_f1_grad_acc),
      q_f1_blur_radius,
      q_f1_blur_sigma,
      q_f1_shadow,
      q_f1_shadow_x,
      q_f1_shadow_y,
      q_f1_shadow_blur_radius,
      q_f1_shadow_blur_sigma ),
    // All
    all_params (
      q_al_blur_radius,
      q_al_blur_sigma )
    );
  ap_rwrite(&buf[0],buf.size(), r);

  
  return OK;
}

static void gradation_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(gradation_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA gradation_module = {
    STANDARD20_MODULE_STUFF, 
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    gradation_register_hooks  /* register hooks                      */
};

