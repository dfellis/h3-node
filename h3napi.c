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

#define napiArgsWithOptional(N, M) \
  napi_value argv[N + M];\
  size_t argc = N + M;\
  \
  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);\
  \
  if (argc < N) {\
    napi_throw_error(env, "EINVAL", "Too few arguments");\
    return NULL;\
  }

#define napiGetNapiArg(I, N) \
  napi_value N = argv[I];

#define napiGetValue(I, J, T, N) \
  T N;\
  if (napi_get_value_ ## J(env, argv[I], (T *) &N) != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected " #J " in arg " #I);\
    return NULL;\
  }

#define napiGet(V, J, T, N) \
  T N;\
  napi_status napiGet ## V ## N = napi_get_value_ ## J(env, V, &N);\
  if (napiGet ## V ## N != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Could not get " #J " value from " #V);\
  }\
  if (napiGet ## V ## N != napi_ok)

#define napiGetNapiValue(A, I, N) \
  napi_value N;\
  napi_status napiGetNapiValue ## N = napi_get_element(env, A, I, &N);\
  if (napiGetNapiValue ## N != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Could not get element from array at index " #I);\
  }\
  if (napiGetNapiValue ## N != napi_ok)

#define napiGetH3Index(I, O) \
  char O ## Str[17];\
  size_t O ## StrCount;\
  \
  if (napi_get_value_string_utf8(env, argv[I], O ## Str, 17, &O ## StrCount) != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected string h3 index in arg " #I);\
    return NULL;\
  }\
  \
  H3Index O = stringToH3(O ## Str);

#define napiGetNapiH3Index(I, O) \
  char O ## Str[17];\
  size_t O ## StrCount;\
  napi_status napiGetNapiH3Index ## I = napi_get_value_string_utf8(env, I, O ## Str, 17, &O ## StrCount);\
  if (napiGetNapiH3Index ## I != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected string h3 index in arg " #I);\
  }\
  H3Index O = stringToH3(O ## Str);\
  if (napiGetNapiH3Index ## I != napi_ok)

#define napiFixedArray(V, L) \
  napi_value V;\
  if (napi_create_array_with_length(env, L, &V) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create fixed length array");\
    return NULL;\
  }

#define napiVarArray(V) \
  napi_value V;\
  napi_status napiVarArray ## V = napi_create_array(env, &V);\
  if (napiVarArray ## V != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create variable array");\
  }\
  if (napiVarArray ## V != napi_ok)

#define napiSetNapiValue(A, I, N) \
  napi_status napiSetNapiValue ## N = napi_set_element(env, A, I, N);\
  if (napiSetNapiValue ## N != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not store " #N " in array");\
  }\
  if (napiSetNapiValue ## N != napi_ok)

#define napiSetValue(A, I, J, V, N) \
  napi_value N;\
  \
  if (napi_create_ ## J(env, V, &N) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not write " #N #J);\
    return NULL;\
  }\
  napiSetNapiValue(A, I, N);

#define napiStoreH3Index(V, O) \
  char V ## String[17];\
  h3ToString(V, V ## String, 17);\
  napi_value O;\
  napi_status napiStoreH3Index ## V = napi_create_string_utf8(env, V ## String, 15, &O);\
  if (napiStoreH3Index ## V != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not write H3 string");\
  }\
  if (napiStoreH3Index ## V != napi_ok)

#define napiStoreValue(N, T, V) \
  napi_value N;\
  if (napi_create_ ## T(env, V, &N) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create " #N);\
    return NULL;\
  }

#define napiStoreBool(N, V) \
  napi_value N;\
  if (napi_get_boolean(env, V, &N) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create boolean");\
    return NULL;\
  }

#define napiObject(O) \
  napi_value O;\
  if (napi_create_object(env, &O) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create object");\
    return NULL;\
  }

#define napiStoreNapiInObject(O, N, V) \
  napi_set_named_property(env, O, N, V);

#define napiStoreInObject(O, N, T, V) \
  napi_value N;\
  if (napi_create_ ## T(env, V, &N) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create " #N);\
    return NULL;\
  }\
  napiStoreNapiInObject(O, #N, N);

#define napiGetFromObject(O, N, T, R) \
  napi_value N;\
  if (napi_get_named_property(env, O, #N, &N) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not retrieve " #N);\
    return NULL;\
  }\
  if (napi_get_value_ ## T(env, N, R) != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected " #N " in object");\
    return NULL;\
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
  napiGetH3Index(0, h3);

  GeoCoord geo = { 0 };
  h3ToGeo(h3, &geo);

  napiFixedArray(result, 2);
  napiSetValue(result, 0, double, radsToDegs(geo.lat), lat);
  napiSetValue(result, 1, double, radsToDegs(geo.lon), lng);

  return result;
}

napiFn(h3ToGeoBoundary) {
  napiArgs(1);
  napiGetH3Index(0, h3);

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
  napiGetH3Index(0, h3);

  int res = h3GetResolution(h3);

  napiStoreValue(result, int32, res);

  return result;
}

napiFn(h3GetBaseCell) {
  napiArgs(1);
  napiGetH3Index(0, h3);

  int baseCell = h3GetBaseCell(h3);

  napiStoreValue(result, int32, baseCell);

  return result;
}

napiFn(h3IsValid) {
  napiArgs(1);
  napiGetH3Index(0, h3);

  int isValid = h3IsValid(h3);

  napiStoreBool(result, isValid);

  return result;
}

napiFn(h3IsResClassIII) {
  napiArgs(1);
  napiGetH3Index(0, h3);

  int isResClassIII = h3IsResClassIII(h3);

  napiStoreBool(result, isResClassIII);

  return result;
}

napiFn(h3IsPentagon) {
  napiArgs(1);
  napiGetH3Index(0, h3);

  int isPentagon = h3IsPentagon(h3);

  napiStoreBool(result, isPentagon);

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Traversal Functions                                                       //
///////////////////////////////////////////////////////////////////////////////

napiFn(kRing) {
  napiArgs(2);
  napiGetH3Index(0, h3);
  napiGetValue(1, int32, int, k);

  int maxSize = maxKringSize(k);
  H3Index* kRingOut = calloc(maxSize, sizeof(H3Index));
  kRing(h3, k, kRingOut);

  int arrayIndex = 0;
  napiVarArray(result) {
    free(kRingOut);
    return NULL;
  }
  for (int i = 0; i < maxSize; i++) {
    if (kRingOut[i] == 0) continue;

    H3Index h3Num = kRingOut[i];
    napiStoreH3Index(h3Num, h3Val) {
      free(kRingOut);
      return NULL;
    }
    napiSetNapiValue(result, arrayIndex, h3Val) {
      free(kRingOut);
      return NULL;
    }

    arrayIndex++;
  }

  free(kRingOut);

  return result;
}

napiFn(kRingDistances) {
  napiArgs(2);
  napiGetH3Index(0, h3);
  napiGetValue(1, int32, int, k);

  int maxSize = maxKringSize(k);
  H3Index* kRingOut = calloc(maxSize, sizeof(H3Index));
  int* distances = calloc(maxSize, sizeof(int));
  kRingDistances(h3, k, kRingOut, distances);

  napiVarArray(result) {
    free(distances);
    free(kRingOut);
    return NULL;
  }
  for (int i = 0; i < k + 1; i++) {
    napiVarArray(ring) {
      free(distances);
      free(kRingOut);
      return NULL;
    }
    napiSetNapiValue(result, i, ring) {
      free(distances);
      free(kRingOut);
      return NULL;
    }
  }

  int* arrayIndices = calloc(k + 1, sizeof(int));

  for (int i = 0; i < maxSize; i++) {
    if (kRingOut[i] == 0) continue;

    H3Index h3Num = kRingOut[i];
    int ring = distances[i];
    napiStoreH3Index(h3Num, h3Val) {
      free(arrayIndices);
      free(distances);
      free(kRingOut);
      return NULL;
    }
    napiGetNapiValue(result, ring, ringArray) {
      free(arrayIndices);
      free(distances);
      free(kRingOut);
      return NULL;
    }
    napiSetNapiValue(ringArray, arrayIndices[ring], h3Val) {
      free(arrayIndices);
      free(distances);
      free(kRingOut);
      return NULL;
    }

    arrayIndices[ring]++;
  }

  free(arrayIndices);
  free(distances);
  free(kRingOut);

  return result;
}

napiFn(hexRing) {
  napiArgs(2);
  napiGetH3Index(0, h3);
  napiGetValue(1, int32, int, k);

  int maxSize = k == 0 ? 1 : 6 * k;
  H3Index* hexRingOut = calloc(maxSize, sizeof(H3Index));
  if (hexRing(h3, k, hexRingOut) != 0) {
    napi_throw_error(env, "EINVAL", "Pentagon encountered in range of ring");
  }

  int arrayIndex = 0;
  napiVarArray(result) {
    free(hexRingOut);
    return NULL;
  }
  for (int i = 0; i < maxSize; i++) {
    if (hexRingOut[i] == 0) continue;

    H3Index h3Num = hexRingOut[i];
    napiStoreH3Index(h3Num, h3Val) {
      free(hexRingOut);
      return NULL;
    }
    napiSetNapiValue(result, arrayIndex, h3Val) {
      free(hexRingOut);
      return NULL;
    }

    arrayIndex++;
  }

  free(hexRingOut);

  return result;
}

napiFn(h3Distance) {
  napiArgs(2);
  napiGetH3Index(0, origin);
  napiGetH3Index(1, destination);

  int distance = h3Distance(origin, destination);

  napiStoreValue(result, int32, distance);

  return result;
}

napiFn(experimentalH3ToLocalIj) {
  napiArgs(2);
  napiGetH3Index(0, origin);
  napiGetH3Index(1, destination);

  CoordIJ ij = { 0 };
  int errorCode = experimentalH3ToLocalIj(origin, destination, &ij);
  switch (errorCode) {
    case 0:
      break;
    case 1:
      napi_throw_error(env, "EINVAL", "Incompatible origin and index");
      return NULL;
    case 2:
    default:
      napi_throw_error(env, "EINVAL", "Local IJ coordinates undefined for this origin and index"
        " pair. The index may be too far from the origin.");
      return NULL;
    case 3:
    case 4:
    case 5:
      napi_throw_error(env, "EINVAL", "Encountered possible pentagon distortion.");
      return NULL;
  }
  napiObject(result);
  napiStoreInObject(result, i, int32, ij.i);
  napiStoreInObject(result, j, int32, ij.j);
  return result;
}

napiFn(experimentalLocalIjToH3) {
  napiArgs(2);
  napiGetH3Index(0, origin);
  napiGetNapiArg(1, coords);

  CoordIJ ij = { 0 };
  H3Index h3;
  napiGetFromObject(coords, i, int32, &ij.i);
  napiGetFromObject(coords, j, int32, &ij.j);
  int errorCode = experimentalLocalIjToH3(origin, &ij, &h3);
  if (errorCode != 0) {
    napi_throw_error(env, "EINVAL", "Index not defined for this origin and IJ coordinate pair. "
      "IJ coordinates may be too far from origin, or a pentagon distortion was encountered.");
    return NULL;
  }
  napiStoreH3Index(h3, result);

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Hierarchy Functions                                                       //
///////////////////////////////////////////////////////////////////////////////

napiFn(h3ToParent) {
  napiArgs(2);
  napiGetH3Index(0, h3);
  napiGetValue(1, int32, int, res);

  H3Index h3Parent = h3ToParent(h3, res);

  napiStoreH3Index(h3Parent, result);

  return result;
}

napiFn(h3ToChildren) {
  napiArgs(2);
  napiGetH3Index(0, h3);
  napiGetValue(1, int32, int, res);

  int maxSize = maxH3ToChildrenSize(h3, res);
  H3Index* children = calloc(maxSize, sizeof(H3Index));
  h3ToChildren(h3, res, children);

  napiVarArray(result) {
    free(children);
    return NULL;
  }
  int arrayIndex = 0;
  for (int i = 0; i < maxSize; i++) {
    H3Index child = children[i];
    if (child == 0) continue;
    napiStoreH3Index(child, childObj) {
      free(children);
      return NULL;
    }
    napiSetNapiValue(result, arrayIndex, childObj) {
      free(children);
      return NULL;
    }
    arrayIndex++;
  }
  
  free(children);
  return result;
}

napiFn(compact) {
  napiArgs(1);
  napiGetNapiArg(0, h3IndexArrayObj);
  uint32_t arrayLen;
  if (napi_get_array_length(env, h3IndexArrayObj, &arrayLen) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected an array of h3 indexes");
    return NULL;
  }
  H3Index* uncompactedArray = calloc(arrayLen, sizeof(H3Index));
  H3Index* compactedArray = calloc(arrayLen, sizeof(H3Index));
  for (int i = 0; i < arrayLen; i++) {
    napiGetNapiValue(h3IndexArrayObj, i, h3IndexObj) {
      free(uncompactedArray);
      free(compactedArray);
      return NULL;
    }
    napiGetNapiH3Index(h3IndexObj, h3Index) {
      free(uncompactedArray);
      free(compactedArray);
      return NULL;
    }
    uncompactedArray[i] = h3Index;
  }
  int errorCode = compact(uncompactedArray, compactedArray, arrayLen);
  if (errorCode != 0) {
    napi_throw_error(env, "EINVAL", "Unexpected non-hexagon in provided set");
    free(compactedArray);
    free(uncompactedArray);
    return NULL;
  }
  napiVarArray(result) {
    free(compactedArray);
    free(uncompactedArray);
    return NULL;
  }
  int arrayIndex = 0;
  for (int i = 0; i < arrayLen; i++) {
    H3Index compacted = compactedArray[i];
    if (compacted == 0) continue;
    napiStoreH3Index(compacted, compactedObj) {
      free(compactedArray);
      free(uncompactedArray);
      return NULL;
    }
    napiSetNapiValue(result, arrayIndex, compactedObj) {
      free(compactedArray);
      free(uncompactedArray);
      return NULL;
    }
    arrayIndex++;
  }

  free(compactedArray);
  free(uncompactedArray);
  return result;
}

napiFn(uncompact) {
  napiArgs(2);
  napiGetNapiArg(0, h3IndexArrayObj);
  napiGetValue(1, int32, int, res);
  uint32_t arrayLen;
  if (napi_get_array_length(env, h3IndexArrayObj, &arrayLen) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected an array of h3 indexes");
    return NULL;
  }
  H3Index* compactedArray = calloc(arrayLen, sizeof(H3Index));
  for (int i = 0; i < arrayLen; i++) {
    napiGetNapiValue(h3IndexArrayObj, i, h3IndexObj) {
      free(compactedArray);
      return NULL;
    }
    napiGetNapiH3Index(h3IndexObj, h3Index) {
      free(compactedArray);
      return NULL;
    }
    compactedArray[i] = h3Index;
  }
  int maxSize = maxUncompactSize(compactedArray, arrayLen, res);
  H3Index* uncompactedArray = calloc(maxSize, sizeof(H3Index));
  int errorCode = uncompact(compactedArray, arrayLen, uncompactedArray, maxSize, res);
  if (errorCode != 0) {
    napi_throw_error(env, "EINVAL", "Unexpected non-hexagon in provided set");
    free(compactedArray);
    free(uncompactedArray);
    return NULL;
  }
  napiVarArray(result) {
    free(compactedArray);
    free(uncompactedArray);
    return NULL;
  }
  int arrayIndex = 0;
  for (int i = 0; i < maxSize; i++) {
    H3Index uncompacted = uncompactedArray[i];
    if (uncompacted == 0) continue;
    napiStoreH3Index(uncompacted, uncompactedObj) {
      free(compactedArray);
      free(uncompactedArray);
      return NULL;
    }
    napiSetNapiValue(result, arrayIndex, uncompactedObj) {
      free(compactedArray);
      free(uncompactedArray);
      return NULL;
    }
    arrayIndex++;
  }

  free(compactedArray);
  free(uncompactedArray);
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Region Functions                                                          //
///////////////////////////////////////////////////////////////////////////////

napiFn(polyfill) {
  napiArgsWithOptional(2, 1);
  napiGetNapiArg(0, geofenceArrayObj);
  napiGetValue(1, int32, int, res);
  napi_value isGeoJsonObj;
  if (napi_coerce_to_bool(env, argv[2], &isGeoJsonObj) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Somehow cannot coerce a boolean?");
    return NULL;
  }
  bool isGeoJson;
  if (napi_get_value_bool(env, isGeoJsonObj, &isGeoJson) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Somehow cannot extract boolean from JS object");
    return NULL;
  }
  napi_valuetype typepoke;
  if (napi_typeof(env, geofenceArrayObj, &typepoke) != napi_ok) {
    napi_throw_error(env, "EINVAL", "No geofence array provided?");
    return NULL;
  }
  if (typepoke != napi_object) {
    napi_throw_error(env, "EINVAL", "Non-array value provided as geofence array.");
    return NULL;
  }
  uint32_t arrayLength;
  if (napi_get_array_length(env, geofenceArrayObj, &arrayLength) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Could not read length of geofence array");
    return NULL;
  }
  napiVarArray(result);
  if (arrayLength == 0) return result;
  napiGetNapiValue(geofenceArrayObj, 0, firstInnerArray);
  if (napi_get_array_length(env, firstInnerArray, &arrayLength) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Could not read length of the first inner array");
    return NULL;
  }
  if (arrayLength == 0) return result;
  napiGetNapiValue(firstInnerArray, 0, nestingTest);
  if (napi_typeof(env, nestingTest, &typepoke) != napi_ok) {
    napi_throw_error(env, "EINVAL", "No geofence data provided?");
    return NULL;
  }
  GeoPolygon polygon = { 0 };
  if (typepoke == napi_number) {
    // Single polygon shape, no holes
    polygon.numHoles = 0;
    if (napi_get_array_length(env, geofenceArrayObj, &arrayLength) != napi_ok) {
      napi_throw_error(env, "EINVAL", "Could not read length of geofence array");
      return NULL;
    }
    polygon.geofence.numVerts = arrayLength;
    polygon.geofence.verts = calloc(arrayLength, sizeof(GeoCoord));
    for (int i = 0; i < arrayLength; i++) {
      napiGetNapiValue(geofenceArrayObj, i, latlngArray) {
        free(polygon.geofence.verts);
        return NULL;
      }
      napiGetNapiValue(latlngArray, 0, a) {
        free(polygon.geofence.verts);
        return NULL;
      }
      napiGetNapiValue(latlngArray, 1, b) {
        free(polygon.geofence.verts);
        return NULL;
      }
      if (isGeoJson) {
        napiGet(b, double, double, lat) {
          free(polygon.geofence.verts);
          return NULL;
        }
        napiGet(a, double, double, lng) {
          free(polygon.geofence.verts);
          return NULL;
        }
        polygon.geofence.verts[i].lat = degsToRads(lat);
        polygon.geofence.verts[i].lon = degsToRads(lng);
      } else {
        napiGet(a, double, double, lat) {
          free(polygon.geofence.verts);
          return NULL;
        }
        napiGet(b, double, double, lng) {
          free(polygon.geofence.verts);
          return NULL;
        }
        polygon.geofence.verts[i].lat = degsToRads(lat);
        polygon.geofence.verts[i].lon = degsToRads(lng);
      }
    }
  } else {
    // Multiple polygon shapes, has holes
    if (napi_get_array_length(env, geofenceArrayObj, &arrayLength) != napi_ok) {
      napi_throw_error(env, "EINVAL", "Could not read length of geofence array");
      return NULL;
    }
    polygon.numHoles = arrayLength - 1;
    if (polygon.numHoles > 0) {
      polygon.holes = calloc(polygon.numHoles, sizeof(Geofence));
    }
    for (int i = 0; i < arrayLength; i++) {
      napiGetNapiValue(geofenceArrayObj, i, polygonArray) {
        if (polygon.geofence.numVerts > 0) {
          free(polygon.geofence.verts);
        }
        if (polygon.numHoles > 0) {
          for (int x = 0; x < polygon.numHoles; x++) {
            if (polygon.holes[x].numVerts > 0) {
              free(polygon.holes[x].verts);
            }
          }
          free(polygon.holes);
        }
        return NULL;
      }
      int polygonLength;
      if (napi_get_array_length(env, polygonArray, &polygonLength) != napi_ok) {
        napi_throw_error(env, "EINVAL", "Could not read length of geofence array");
        return NULL;
      }
      if (i == 0) {
        // Primary geofence case
        polygon.geofence.numVerts = polygonLength;
        polygon.geofence.verts = calloc(polygonLength, sizeof(GeoCoord));
        for (int j = 0; j < polygonLength; j++) {
          napiGetNapiValue(polygonArray, j, latlngArray) {
            if (polygon.geofence.numVerts > 0) {
              free(polygon.geofence.verts);
            }
            if (polygon.numHoles > 0) {
              for (int x = 0; x < polygon.numHoles; x++) {
                if (polygon.holes[x].numVerts > 0) {
                  free(polygon.holes[x].verts);
                }
              }
              free(polygon.holes);
            }
            return NULL;
          }
          napiGetNapiValue(latlngArray, 0, a) {
            if (polygon.geofence.numVerts > 0) {
              free(polygon.geofence.verts);
            }
            if (polygon.numHoles > 0) {
              for (int x = 0; x < polygon.numHoles; x++) {
                if (polygon.holes[x].numVerts > 0) {
                  free(polygon.holes[x].verts);
                }
              }
              free(polygon.holes);
            }
            return NULL;
          }
          napiGetNapiValue(latlngArray, 1, b) {
            if (polygon.geofence.numVerts > 0) {
              free(polygon.geofence.verts);
            }
            if (polygon.numHoles > 0) {
              for (int x = 0; x < polygon.numHoles; x++) {
                if (polygon.holes[x].numVerts > 0) {
                  free(polygon.holes[x].verts);
                }
              }
              free(polygon.holes);
            }
            return NULL;
          }
          if (isGeoJson) {
            napiGet(b, double, double, lat) {
              if (polygon.geofence.numVerts > 0) {
                free(polygon.geofence.verts);
              }
              if (polygon.numHoles > 0) {
                for (int x = 0; x < polygon.numHoles; x++) {
                  if (polygon.holes[x].numVerts > 0) {
                    free(polygon.holes[x].verts);
                  }
                }
                free(polygon.holes);
              }
              return NULL;
            }
            napiGet(a, double, double, lng) {
              if (polygon.geofence.numVerts > 0) {
                free(polygon.geofence.verts);
              }
              if (polygon.numHoles > 0) {
                for (int x = 0; x < polygon.numHoles; x++) {
                  if (polygon.holes[x].numVerts > 0) {
                    free(polygon.holes[x].verts);
                  }
                }
                free(polygon.holes);
              }
              return NULL;
            }
            polygon.geofence.verts[j].lat = degsToRads(lat);
            polygon.geofence.verts[j].lon = degsToRads(lng);
          } else {
            napiGet(a, double, double, lat) {
              if (polygon.geofence.numVerts > 0) {
                free(polygon.geofence.verts);
              }
              if (polygon.numHoles > 0) {
                for (int x = 0; x < polygon.numHoles; x++) {
                  if (polygon.holes[x].numVerts > 0) {
                    free(polygon.holes[x].verts);
                  }
                }
                free(polygon.holes);
              }
              return NULL;
            }
            napiGet(b, double, double, lng) {
              if (polygon.geofence.numVerts > 0) {
                free(polygon.geofence.verts);
              }
              if (polygon.numHoles > 0) {
                for (int x = 0; x < polygon.numHoles; x++) {
                  if (polygon.holes[x].numVerts > 0) {
                    free(polygon.holes[x].verts);
                  }
                }
                free(polygon.holes);
              }
              return NULL;
            }
            polygon.geofence.verts[j].lat = degsToRads(lat);
            polygon.geofence.verts[j].lon = degsToRads(lng);
          }
        }
      } else {
        // Hole case
        polygon.holes[i - 1].numVerts = polygonLength;
        polygon.holes[i - 1].verts = calloc(polygonLength, sizeof(GeoCoord));
        for (int j = 0; j < polygonLength; j++) {
          napiGetNapiValue(polygonArray, j, latlngArray) {
            if (polygon.geofence.numVerts > 0) {
              free(polygon.geofence.verts);
            }
            if (polygon.numHoles > 0) {
              for (int x = 0; x < polygon.numHoles; x++) {
                if (polygon.holes[x].numVerts > 0) {
                  free(polygon.holes[x].verts);
                }
              }
              free(polygon.holes);
            }
            return NULL;
          }
          napiGetNapiValue(latlngArray, 0, a) {
            if (polygon.geofence.numVerts > 0) {
              free(polygon.geofence.verts);
            }
            if (polygon.numHoles > 0) {
              for (int x = 0; x < polygon.numHoles; x++) {
                if (polygon.holes[x].numVerts > 0) {
                  free(polygon.holes[x].verts);
                }
              }
              free(polygon.holes);
            }
            return NULL;
          }
          napiGetNapiValue(latlngArray, 1, b) {
            if (polygon.geofence.numVerts > 0) {
              free(polygon.geofence.verts);
            }
            if (polygon.numHoles > 0) {
              for (int x = 0; x < polygon.numHoles; x++) {
                if (polygon.holes[x].numVerts > 0) {
                  free(polygon.holes[x].verts);
                }
              }
              free(polygon.holes);
            }
            return NULL;
          }
          if (isGeoJson) {
            napiGet(b, double, double, lat) {
              if (polygon.geofence.numVerts > 0) {
                free(polygon.geofence.verts);
              }
              if (polygon.numHoles > 0) {
                for (int x = 0; x < polygon.numHoles; x++) {
                  if (polygon.holes[x].numVerts > 0) {
                    free(polygon.holes[x].verts);
                  }
                }
                free(polygon.holes);
              }
              return NULL;
            }
            napiGet(a, double, double, lng) {
              if (polygon.geofence.numVerts > 0) {
                free(polygon.geofence.verts);
              }
              if (polygon.numHoles > 0) {
                for (int x = 0; x < polygon.numHoles; x++) {
                  if (polygon.holes[x].numVerts > 0) {
                    free(polygon.holes[x].verts);
                  }
                }
                free(polygon.holes);
              }
              return NULL;
            }
            polygon.holes[i - 1].verts[j].lat = degsToRads(lat);
            polygon.holes[i - 1].verts[j].lon = degsToRads(lng);
          } else {
            napiGet(a, double, double, lat) {
              if (polygon.geofence.numVerts > 0) {
                free(polygon.geofence.verts);
              }
              if (polygon.numHoles > 0) {
                for (int x = 0; x < polygon.numHoles; x++) {
                  if (polygon.holes[x].numVerts > 0) {
                    free(polygon.holes[x].verts);
                  }
                }
                free(polygon.holes);
              }
              return NULL;
            }
            napiGet(b, double, double, lng) {
              if (polygon.geofence.numVerts > 0) {
                free(polygon.geofence.verts);
              }
              if (polygon.numHoles > 0) {
                for (int x = 0; x < polygon.numHoles; x++) {
                  if (polygon.holes[x].numVerts > 0) {
                    free(polygon.holes[x].verts);
                  }
                }
                free(polygon.holes);
              }
              return NULL;
            }
            polygon.holes[i - 1].verts[j].lat = degsToRads(lat);
            polygon.holes[i - 1].verts[j].lon = degsToRads(lng);
          }
        }
      }
    }
  }
  int maxSize = maxPolyfillSize(&polygon, res);
  H3Index* fill = calloc(maxSize, sizeof(H3Index));
  polyfill(&polygon, res, fill);

  int arrayIndex = 0;
  for (int i = 0; i < maxSize; i++) {
    H3Index h3 = fill[i];
    if (h3 == 0) continue;
    napiStoreH3Index(h3, h3Obj) {
      if (polygon.geofence.numVerts > 0) {
        free(polygon.geofence.verts);
      }
      if (polygon.numHoles > 0) {
        for (int x = 0; x < polygon.numHoles; x++) {
          if (polygon.holes[x].numVerts > 0) {
            free(polygon.holes[x].verts);
          }
        }
        free(polygon.holes);
      }
      return NULL;
    }
    napiSetNapiValue(result, arrayIndex, h3Obj) {
      if (polygon.geofence.numVerts > 0) {
        free(polygon.geofence.verts);
      }
      if (polygon.numHoles > 0) {
        for (int x = 0; x < polygon.numHoles; x++) {
          if (polygon.holes[x].numVerts > 0) {
            free(polygon.holes[x].verts);
          }
        }
        free(polygon.holes);
      }
      return NULL;
    }
    arrayIndex++;
  }

  if (polygon.geofence.numVerts > 0) {
    free(polygon.geofence.verts);
  }
  if (polygon.numHoles > 0) {
    for (int x = 0; x < polygon.numHoles; x++) {
      if (polygon.holes[x].numVerts > 0) {
        free(polygon.holes[x].verts);
      }
    }
    free(polygon.holes);
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Unidirectional Edge Functions                                             //
///////////////////////////////////////////////////////////////////////////////

napiFn(h3IndexesAreNeighbors) {
  napiArgs(2);
  napiGetH3Index(0, origin);
  napiGetH3Index(1, destination);

  int areNeighbors = h3IndexesAreNeighbors(origin, destination);

  napiStoreBool(result, areNeighbors);

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
  napiExport(h3Distance);
  napiExport(experimentalH3ToLocalIj);
  napiExport(experimentalLocalIjToH3);

  // Hierarchy Functions
  napiExport(h3ToParent);
  napiExport(h3ToChildren);
  napiExport(compact);
  napiExport(uncompact);

  // Region Functions
  napiExport(polyfill);

  // Unidirectional Edge Functions
  napiExport(h3IndexesAreNeighbors);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
