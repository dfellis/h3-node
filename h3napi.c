#include <node_api.h>
#include <stdio.h>
#include "h3api.h"

///////////////////////////////////////////////////////////////////////////////
// Indexing Functions                                                        //
///////////////////////////////////////////////////////////////////////////////

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
  lat = degsToRads(lat);

  if (napi_get_value_double(env, argv[1], (double *) &lng) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected numeric longitude");
    return NULL;
  }
  lng = degsToRads(lng);

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
    napi_throw_error(env, "ENOSPC", "Could not write H3 string");
  }

  return result;
}

napi_value napiH3ToGeo(napi_env env, napi_callback_info info) {
  napi_value argv[1];
  size_t argc = 1;

  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc < 1) {
    napi_throw_error(env, "EINVAL", "Too few arguments");
    return NULL;
  }

  char h3String[17];
  size_t writeCount;

  if (napi_get_value_string_utf8(env, argv[0], h3String, 17, &writeCount) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected string h3 index");
    return NULL;
  }

  H3Index h3 = stringToH3(h3String);

  GeoCoord geo = { 0 };
  h3ToGeo(h3, &geo);
  napi_value result;
  if (napi_create_array_with_length(env, 2, &result) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not create return array");
  }

  napi_value lat;
  napi_value lng;

  if (napi_create_double(env, radsToDegs(geo.lat), &lat) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not write latitude double");
  }

  if (napi_create_double(env, radsToDegs(geo.lon), &lng) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not write longitude double");
  }

  if (napi_set_element(env, result, 0, lat) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not store latitude in array");
  }

  if (napi_set_element(env, result, 1, lng) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not store longitude in array");
  }

  return result;
}

napi_value napiH3ToGeoBoundary(napi_env env, napi_callback_info info) {
  napi_value argv[1];
  size_t argc = 1;

  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc < 1) {
    napi_throw_error(env, "EINVAL", "Too few arguments");
    return NULL;
  }

  char h3String[17];
  size_t writeCount;

  if (napi_get_value_string_utf8(env, argv[0], h3String, 17, &writeCount) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected string h3 index");
    return NULL;
  }

  H3Index h3 = stringToH3(h3String);

  GeoBoundary geoBoundary = { 0 };
  h3ToGeoBoundary(h3, &geoBoundary);
  napi_value result;
  if (napi_create_array_with_length(env, geoBoundary.numVerts, &result) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not create return array");
  }

  for (int i = 0; i < geoBoundary.numVerts; i++) {
    napi_value latlngArray;
    napi_value lat;
    napi_value lng;

    if (napi_create_array_with_length(env, 2, &latlngArray) != napi_ok) {
      napi_throw_error(env, "ENOSPC", "Could not create return array");
    }

    if (napi_create_double(env, radsToDegs(geoBoundary.verts[i].lat), &lat) != napi_ok) {
      napi_throw_error(env, "ENOSPC", "Could not write latitude double");
    }

    if (napi_create_double(env, radsToDegs(geoBoundary.verts[i].lon), &lng) != napi_ok) {
      napi_throw_error(env, "ENOSPC", "Could not write longitude double");
    }

    if (napi_set_element(env, latlngArray, 0, lat) != napi_ok) {
      napi_throw_error(env, "ENOSPC", "Could not store latitude in array");
    }

    if (napi_set_element(env, latlngArray, 1, lng) != napi_ok) {
      napi_throw_error(env, "ENOSPC", "Could not store longitude in array");
    }

    if (napi_set_element(env, result, i, latlngArray) != napi_ok) {
      napi_throw_error(env, "ENOSPC", "Could not store lat, lng array in array");
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Inspection Functions                                                      //
///////////////////////////////////////////////////////////////////////////////

napi_value napiH3GetResolution(napi_env env, napi_callback_info info) {
  napi_value argv[1];
  size_t argc = 1;

  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc < 1) {
    napi_throw_error(env, "EINVAL", "Too few arguments");
    return NULL;
  }

  char h3String[17];
  size_t writeCount;

  if (napi_get_value_string_utf8(env, argv[0], h3String, 17, &writeCount) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected string h3 index");
    return NULL;
  }

  H3Index h3 = stringToH3(h3String);

  int res = h3GetResolution(h3);

  napi_value result;
  if (napi_create_int32(env, res, &result) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not create resolution integer");
  }

  return result;
}

napi_value napiH3GetBaseCell(napi_env env, napi_callback_info info) {
  napi_value argv[1];
  size_t argc = 1;

  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc < 1) {
    napi_throw_error(env, "EINVAL", "Too few arguments");
    return NULL;
  }

  char h3String[17];
  size_t writeCount;

  if (napi_get_value_string_utf8(env, argv[0], h3String, 17, &writeCount) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected string h3 index");
    return NULL;
  }

  H3Index h3 = stringToH3(h3String);

  int baseCell = h3GetBaseCell(h3);

  napi_value result;
  if (napi_create_int32(env, baseCell, &result) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not create base cell integer");
  }

  return result;
}

napi_value napiH3IsValid(napi_env env, napi_callback_info info) {
  napi_value argv[1];
  size_t argc = 1;

  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc < 1) {
    napi_throw_error(env, "EINVAL", "Too few arguments");
    return NULL;
  }

  char h3String[17];
  size_t writeCount;

  if (napi_get_value_string_utf8(env, argv[0], h3String, 17, &writeCount) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected string h3 index");
    return NULL;
  }

  H3Index h3 = stringToH3(h3String);

  int isValid = h3IsValid(h3);

  napi_value result;
  if (napi_get_boolean(env, isValid, &result) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not create boolean");
  }

  return result;
}

napi_value napiH3IsResClassIII(napi_env env, napi_callback_info info) {
  napi_value argv[1];
  size_t argc = 1;

  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc < 1) {
    napi_throw_error(env, "EINVAL", "Too few arguments");
    return NULL;
  }

  char h3String[17];
  size_t writeCount;

  if (napi_get_value_string_utf8(env, argv[0], h3String, 17, &writeCount) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected string h3 index");
    return NULL;
  }

  H3Index h3 = stringToH3(h3String);

  int isResClassIII = h3IsResClassIII(h3);

  napi_value result;
  if (napi_get_boolean(env, isResClassIII, &result) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not create boolean");
  }

  return result;
}

napi_value napiH3IsPentagon(napi_env env, napi_callback_info info) {
  napi_value argv[1];
  size_t argc = 1;

  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

  if (argc < 1) {
    napi_throw_error(env, "EINVAL", "Too few arguments");
    return NULL;
  }

  char h3String[17];
  size_t writeCount;

  if (napi_get_value_string_utf8(env, argv[0], h3String, 17, &writeCount) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected string h3 index");
    return NULL;
  }

  H3Index h3 = stringToH3(h3String);

  int isPentagon = h3IsPentagon(h3);

  napi_value result;
  if (napi_get_boolean(env, isPentagon, &result) != napi_ok) {
    napi_throw_error(env, "ENOSPC", "Could not create boolean");
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Initialization Function                                                   //
///////////////////////////////////////////////////////////////////////////////

napi_value init_all (napi_env env, napi_value exports) {

  // Indexing Functions
  napi_value geoToH3Fn;
  napi_create_function(env, NULL, 0, napiGeoToH3, NULL, &geoToH3Fn);
  napi_set_named_property(env, exports, "geoToH3", geoToH3Fn);

  napi_value h3ToGeoFn;
  napi_create_function(env, NULL, 0, napiH3ToGeo, NULL, &h3ToGeoFn);
  napi_set_named_property(env, exports, "h3ToGeo", h3ToGeoFn);

  napi_value h3ToGeoBoundaryFn;
  napi_create_function(env, NULL, 0, napiH3ToGeoBoundary, NULL, &h3ToGeoBoundaryFn);
  napi_set_named_property(env, exports, "h3ToGeoBoundary", h3ToGeoBoundaryFn);

  // Inspection Functions
  napi_value h3GetResolutionFn;
  napi_create_function(env, NULL, 0, napiH3GetResolution, NULL, &h3GetResolutionFn);
  napi_set_named_property(env, exports, "h3GetResolution", h3GetResolutionFn);

  napi_value h3GetBaseCellFn;
  napi_create_function(env, NULL, 0, napiH3GetBaseCell, NULL, &h3GetBaseCellFn);
  napi_set_named_property(env, exports, "h3GetBaseCell", h3GetBaseCellFn);

  napi_value h3IsValidFn;
  napi_create_function(env, NULL, 0, napiH3IsValid, NULL, &h3IsValidFn);
  napi_set_named_property(env, exports, "h3IsValid", h3IsValidFn);

  napi_value h3IsResClassIIIFn;
  napi_create_function(env, NULL, 0, napiH3IsResClassIII, NULL, &h3IsResClassIIIFn);
  napi_set_named_property(env, exports, "h3IsResClassIII", h3IsResClassIIIFn);

  napi_value h3IsPentagonFn;
  napi_create_function(env, NULL, 0, napiH3IsPentagon, NULL, &h3IsPentagonFn);
  napi_set_named_property(env, exports, "h3IsPentagon", h3IsPentagonFn);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
