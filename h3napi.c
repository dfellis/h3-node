#include <node_api.h>
#include <stdio.h>
#include "h3api.h"

#define napiFn(N) napi_value N ## Napi(napi_env env, napi_callback_info info)

#define napiArgs(N) \
  napi_value argv[N];\
  size_t argc = N;\
  \
  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);\
  \
  if (argc < N) {\
    napi_throw_error(env, "EINVAL", "Too few arguments");\
    return NULL;\
  }

#define napiGetValue(I, J, T, N) \
  T N;\
  if (napi_get_value_ ## J(env, argv[I], (T *) &N) != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected " #J " in arg " #I);\
    return NULL;\
  }

#define napiGetNapiValue(A, I, N) \
  napi_value N;\
  if (napi_get_element(env, A, I, &N) != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Could not get element from array at index " #I);\
  }

#define napiGetH3Index(I, V, O) \
  char V[17];\
  size_t V ## Count;\
  \
  if (napi_get_value_string_utf8(env, argv[I], V, 17, &V ## Count) != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected string h3 index in arg " #I);\
  }\
  \
  H3Index O = stringToH3(V);

#define napiFixedArray(V, L) \
  napi_value V;\
  if (napi_create_array_with_length(env, L, &V) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create fixed length array");\
  }

#define napiVarArray(V) \
  napi_value V;\
  if (napi_create_array(env, &V) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create variable array");\
  }

#define napiSetNapiValue(A, I, N) \
  if (napi_set_element(env, A, I, N) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not store " #N " in array");\
  }

#define napiSetValue(A, I, J, V, N) \
  napi_value N;\
  \
  if (napi_create_ ## J(env, V, &N) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not write " #N #J);\
  }\
  napiSetNapiValue(A, I, N);

#define napiStoreH3Index(V, O) \
  char V ## String[17];\
  h3ToString(V, V ## String, 17);\
  napi_value O;\
  if (napi_create_string_utf8(env, V ## String, 15, &O) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not write H3 string");\
  }

#define napiStoreValue(N, T, V) \
  napi_value N;\
  if (napi_create_ ## T(env, V, &N) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create " #N);\
  }

#define napiStoreBool(N, V) \
  napi_value N;\
  if (napi_get_boolean(env, V, &N) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create boolean");\
  }

#define napiExport(N) \
  napi_value N ## Fn;\
  napi_create_function(env, NULL, 0, N ## Napi, NULL, &N ## Fn);\
  napi_set_named_property(env, exports, #N, N ## Fn);


///////////////////////////////////////////////////////////////////////////////
// Indexing Functions                                                        //
///////////////////////////////////////////////////////////////////////////////

napiFn(geoToH3) {
  napiArgs(3);
  napiGetValue(0, double, double, lat);
  napiGetValue(1, double, double, lng);
  napiGetValue(2, int32, int, res);

  GeoCoord geo = { degsToRads(lat), degsToRads(lng) };
  H3Index h3 = geoToH3(&geo, res);

  napiStoreH3Index(h3, result);

  return result;
}

napiFn(h3ToGeo) {
  napiArgs(1);
  napiGetH3Index(0, h3String, h3);

  GeoCoord geo = { 0 };
  h3ToGeo(h3, &geo);

  napiFixedArray(result, 2);
  napiSetValue(result, 0, double, radsToDegs(geo.lat), lat);
  napiSetValue(result, 1, double, radsToDegs(geo.lon), lng);

  return result;
}

napiFn(h3ToGeoBoundary) {
  napiArgs(1);
  napiGetH3Index(0, h3String, h3);

  GeoBoundary geoBoundary = { 0 };
  h3ToGeoBoundary(h3, &geoBoundary);

  napiFixedArray(result, geoBoundary.numVerts);
  for (int i = 0; i < geoBoundary.numVerts; i++) {
    napiFixedArray(latlngArray, 2);
    napiSetValue(latlngArray, 0, double, radsToDegs(geoBoundary.verts[i].lat), lat);
    napiSetValue(latlngArray, 1, double, radsToDegs(geoBoundary.verts[i].lon), lng);
    napiSetNapiValue(result, i, latlngArray);
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Inspection Functions                                                      //
///////////////////////////////////////////////////////////////////////////////

napiFn(h3GetResolution) {
  napiArgs(1);
  napiGetH3Index(0, h3String, h3);

  int res = h3GetResolution(h3);

  napiStoreValue(result, int32, res);

  return result;
}

napiFn(h3GetBaseCell) {
  napiArgs(1);
  napiGetH3Index(0, h3String, h3);

  int baseCell = h3GetBaseCell(h3);

  napiStoreValue(result, int32, baseCell);

  return result;
}

napiFn(h3IsValid) {
  napiArgs(1);
  napiGetH3Index(0, h3String, h3);

  int isValid = h3IsValid(h3);

  napiStoreBool(result, isValid);

  return result;
}

napiFn(h3IsResClassIII) {
  napiArgs(1);
  napiGetH3Index(0, h3String, h3);

  int isResClassIII = h3IsResClassIII(h3);

  napiStoreBool(result, isResClassIII);

  return result;
}

napiFn(h3IsPentagon) {
  napiArgs(1);
  napiGetH3Index(0, h3String, h3);

  int isPentagon = h3IsPentagon(h3);

  napiStoreBool(result, isPentagon);

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Traversal Functions                                                       //
///////////////////////////////////////////////////////////////////////////////

napiFn(kRing) {
  napiArgs(2);
  napiGetH3Index(0, h3String, h3);
  napiGetValue(1, int32, int, k);

  int maxSize = maxKringSize(k);
  H3Index* kRingOut = calloc(maxSize, sizeof(H3Index));
  kRing(h3, k, kRingOut);

  int arrayIndex = 0;
  napiVarArray(result);
  for (int i = 0; i < maxSize; i++) {
    if (kRingOut[i] == 0) continue;

    H3Index h3Num = kRingOut[i];
    napiStoreH3Index(h3Num, h3Val);
    napiSetNapiValue(result, arrayIndex, h3Val);

    arrayIndex++;
  }

  free(kRingOut);

  return result;
}

napiFn(kRingDistances) {
  napiArgs(2);
  napiGetH3Index(0, h3String, h3);
  napiGetValue(1, int32, int, k);

  int maxSize = maxKringSize(k);
  H3Index* kRingOut = calloc(maxSize, sizeof(H3Index));
  int* distances = calloc(maxSize, sizeof(int));
  kRingDistances(h3, k, kRingOut, distances);

  napiVarArray(result);
  for (int i = 0; i < k + 1; i++) {
    napiVarArray(ring);
    napiSetNapiValue(result, i, ring);
  }

  int* arrayIndices = calloc(k + 1, sizeof(int));

  for (int i = 0; i < maxSize; i++) {
    if (kRingOut[i] == 0) continue;

    H3Index h3Num = kRingOut[i];
    int ring = distances[i];
    napiStoreH3Index(h3Num, h3Val);
    napiGetNapiValue(result, ring, ringArray);
    napiSetNapiValue(ringArray, arrayIndices[ring], h3Val);

    arrayIndices[ring]++;
  }

  free(arrayIndices);
  free(distances);
  free(kRingOut);

  return result;
}

napiFn(hexRing) {
  napiArgs(2);
  napiGetH3Index(0, h3String, h3);
  napiGetValue(1, int32, int, k);

  int maxSize = k == 0 ? 1 : 6 * k;
  H3Index* hexRingOut = calloc(maxSize, sizeof(H3Index));
  if (hexRing(h3, k, hexRingOut) != 0) {
    napi_throw_error(env, "EINVAL", "Pentagon encountered in range of ring");
  }

  int arrayIndex = 0;
  napiVarArray(result);
  for (int i = 0; i < maxSize; i++) {
    if (hexRingOut[i] == 0) continue;

    H3Index h3Num = hexRingOut[i];
    napiStoreH3Index(h3Num, h3Val);
    napiSetNapiValue(result, arrayIndex, h3Val);

    arrayIndex++;
  }

  free(hexRingOut);

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Initialization Function                                                   //
///////////////////////////////////////////////////////////////////////////////

napi_value init_all (napi_env env, napi_value exports) {
  // Indexing Functions
  napiExport(geoToH3);
  napiExport(h3ToGeo);
  napiExport(h3ToGeoBoundary);

  // Inspection Functions
  napiExport(h3GetResolution);
  napiExport(h3GetBaseCell);
  napiExport(h3IsValid);
  napiExport(h3IsResClassIII);
  napiExport(h3IsPentagon);

  // Traversal Functions
  napiExport(kRing);
  napiExport(kRingDistances);
  napiExport(hexRing);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
