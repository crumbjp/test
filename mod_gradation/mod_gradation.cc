#include "httpd.h"
#include "http_log.h"
#include "http_core.h"
#include "http_config.h"
#include "http_protocol.h"
#include "http_request.h"
#include "apr_strings.h"

#include <vector>
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

void gradation_handler_proc( 
  std::vector<unsigned char> &buf ,
  const char * q_w,
  const char * q_h,
  const char * q_f, 
  const char * q_t, 
  const char * q_d,
  const char * q_a,
  const char * q_aa
  );

static int gradation_handler(request_rec *r)
{
  if (strcmp(r->handler, "gradation")) {
    return DECLINED;
  }
  r->content_type = "image/png";

  if (r->header_only)
    return OK;
  
  const apr_table_t * query_parms = parse_parms(r,r->args);
  const char *q_d = apr_table_get(query_parms, "d");
  const char *q_f = apr_table_get(query_parms, "f");
  const char *q_t = apr_table_get(query_parms, "t");
  const char *q_h = apr_table_get(query_parms, "h");
  const char *q_w = apr_table_get(query_parms, "w");
  const char *q_c = apr_table_get(query_parms, "c");
  const char *q_a = apr_table_get(query_parms, "a");
  const char *q_aa = apr_table_get(query_parms, "aa");

//   Magick::Blob in ( t1gif_d,sizeof(t1gif_d) );
//   Magick::Image img;
//   img.read ( in , "GIF");
  std::vector<unsigned char> buf;
  gradation_handler_proc(buf,q_w,q_h,q_f,q_t,q_a,q_d,q_aa);
//   Magick::Image img(Magick::Geometry(100,100),Magick::Color(MaxRGB,MaxRGB,MaxRGB));
//  img.fillColor ( Magick::Color(0,MaxRGB,MaxRGB) );
//  img.lineWidth(1);
//   list.push_back(Magick::DrawableStrokeColor(Magick::Color(red*MaxRGB/2,green*MaxRGB/2,blue*MaxRGB/2)));
//   list.push_back(Magick::DrawableLine(10,10,10,90));
//   img.strokeColor ( Magick::Color(red*MaxRGB,green*MaxRGB,blue*MaxRGB) );
//   img.strokeAntiAlias((aa!=0));
//   Magick::DrawableLine line1(10,10,90,90);
//   img.draw(line1);
//   Magick::DrawableLine line2(10,10,10,90);
//   img.draw(line2);
//   Magick::CoordinateList draw_list;
//   draw_list.push_back(Magick::Coordinate(10,10));
//   draw_list.push_back(Magick::Coordinate(10,90));
//   draw_list.push_back(Magick::Coordinate(90,90));
//   draw_list.push_back(Magick::Coordinate(90,10));
//   draw_list.push_back(Magick::Coordinate(10,10));
//   img.draw(&draw_list);
  ap_rwrite(&buf[0],buf.size(), r);

  
//   const char * out = apr_psprintf(r->pool,"w = %s , h = %s", q_w , q_h);
//   ap_rputs(out, r);

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

