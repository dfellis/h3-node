#include <node_api.h>
#include <stdio.h>
#include "h3api.h"

napi_value napiGeoToH3(napi_env env, napi_callback_info info) {
  napi_value argv[3];
  size_t argc = 3;

  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc < 3) {
    napi_throw_error(env, "EINVAL", "Too few arguments");
    return NULL;
  }

  double lat;
  double lng;
  int res;

  if (napi_get_value_double(env, argv[0], (double *) &lat) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected numeric latitude");
    return NULL;
  }

  if (napi_get_value_double(env, argv[1], (double *) &lng) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected numeric longitude");
    return NULL;
  }

  if (napi_get_value_int32(env, argv[2], (int *) &res) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected numeric resolution");
    return NULL;
  }

  GeoCoord geo = { lat, lng };
  H3Index h3 = geoToH3(&geo, res);
  char h3String[17];
  h3ToString(h3, h3String, 17);
  napi_value result;
  if (napi_create_string_utf8(env, h3String, 15, &result) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Could not write H3 string");
  }

  return result;
}


napi_value init_all (napi_env env, napi_value exports) {
  napi_value geoToH3Fn;
  napi_create_function(env, NULL, 0, napiGeoToH3, NULL, &geoToH3Fn);
  napi_set_named_property(env, exports, "geoToH3", geoToH3Fn);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
