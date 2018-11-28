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

#define napiGetArg(I, J, T, N) \
  T N;\
  if (napi_get_value_ ## J(env, argv[I], (T *) &N) != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected " #J " in arg " #I);\
    return NULL;\
  }

#define napiToVar(V, J, T, N) \
  T N;\
  napi_status napiToVar ## V ## N = napi_get_value_ ## J(env, V, &N);\
  if (napiToVar ## V ## N != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Could not get " #J " value from " #V);\
  }\
  if (napiToVar ## V ## N != napi_ok)

#define napiFromArray(A, I, N) \
  napi_value N;\
  napi_status napiFromArray ## N = napi_get_element(env, A, I, &N);\
  if (napiFromArray ## N != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Could not get element from array at index " #I);\
  }\
  if (napiFromArray ## N != napi_ok)

#define napiGetH3IndexArg(I, O) \
  char O ## Str[17];\
  size_t O ## StrCount;\
  \
  if (napi_get_value_string_utf8(env, argv[I], O ## Str, 17, &O ## StrCount) != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected string h3 index in arg " #I);\
    return NULL;\
  }\
  \
  H3Index O = stringToH3(O ## Str);

#define napiGetH3Index(I, O) \
  char O ## Str[17];\
  size_t O ## StrCount;\
  napi_status napiGetH3Index ## I = napi_get_value_string_utf8(env, I, O ## Str, 17, &O ## StrCount);\
  if (napiGetH3Index ## I != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected string h3 index in arg " #I);\
  }\
  H3Index O = stringToH3(O ## Str);\
  if (napiGetH3Index ## I != napi_ok)

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

#define napiNapiH3Index(V, O) \
  char V ## String[17];\
  h3ToString(V, V ## String, 17);\
  napi_value O;\
  napi_status napiNapiH3Index ## V = napi_create_string_utf8(env, V ## String, 15, &O);\
  if (napiNapiH3Index ## V != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not write H3 string");\
  }\
  if (napiNapiH3Index ## V != napi_ok)

#define napiNapiUnidirectionalEdge(V, O) \
  char V ## String[17];\
  h3ToString(V, V ## String, 17);\
  napi_value O;\
  napi_status napiNapiUnidirectionalEdge ## V = napi_create_string_utf8(env, V ## String, 16, &O);\
  if (napiNapiUnidirectionalEdge ## V != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not write H3 string");\
  }\
  if (napiNapiUnidirectionalEdge ## V != napi_ok)

#define napiNapiValue(V, T, N) \
  napi_value N;\
  if (napi_create_ ## T(env, V, &N) != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create " #N);\
    return NULL;\
  }

#define napiNapiBool(V, N) \
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
  napiGetArg(0, double, double, lat);
  napiGetArg(1, double, double, lng);
  napiGetArg(2, int32, int, res);

  GeoCoord geo = { degsToRads(lat), degsToRads(lng) };
  H3Index h3 = geoToH3(&geo, res);

  napiNapiH3Index(h3, result);

  return result;
}

napiFn(h3ToGeo) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  GeoCoord geo = { 0 };
  h3ToGeo(h3, &geo);

  napiFixedArray(result, 2);
  napiSetValue(result, 0, double, radsToDegs(geo.lat), lat);
  napiSetValue(result, 1, double, radsToDegs(geo.lon), lng);

  return result;
}

napiFn(h3ToGeoBoundary) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

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
  napiGetH3IndexArg(0, h3);

  int res = h3GetResolution(h3);

  napiNapiValue(res, int32, result);

  return result;
}

napiFn(h3GetBaseCell) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  int baseCell = h3GetBaseCell(h3);

  napiNapiValue(baseCell, int32, result);

  return result;
}

napiFn(h3IsValid) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  int isValid = h3IsValid(h3);

  napiNapiBool(isValid, result);

  return result;
}

napiFn(h3IsResClassIII) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  int isResClassIII = h3IsResClassIII(h3);

  napiNapiBool(isResClassIII, result);

  return result;
}

napiFn(h3IsPentagon) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  int isPentagon = h3IsPentagon(h3);

  napiNapiBool(isPentagon, result);

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Traversal Functions                                                       //
///////////////////////////////////////////////////////////////////////////////

napiFn(kRing) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, k);

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
    napiNapiH3Index(h3Num, h3Val) {
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
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, k);

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
    napiNapiH3Index(h3Num, h3Val) {
      free(arrayIndices);
      free(distances);
      free(kRingOut);
      return NULL;
    }
    napiFromArray(result, ring, ringArray) {
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
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, k);

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
    napiNapiH3Index(h3Num, h3Val) {
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
  napiGetH3IndexArg(0, origin);
  napiGetH3IndexArg(1, destination);

  int distance = h3Distance(origin, destination);

  napiNapiValue(distance, int32, result);

  return result;
}

napiFn(experimentalH3ToLocalIj) {
  napiArgs(2);
  napiGetH3IndexArg(0, origin);
  napiGetH3IndexArg(1, destination);

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
  napiGetH3IndexArg(0, origin);
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
  napiNapiH3Index(h3, result);

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Hierarchy Functions                                                       //
///////////////////////////////////////////////////////////////////////////////

napiFn(h3ToParent) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, res);

  H3Index h3Parent = h3ToParent(h3, res);

  napiNapiH3Index(h3Parent, result);

  return result;
}

napiFn(h3ToChildren) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, res);

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
    napiNapiH3Index(child, childObj) {
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
    napiFromArray(h3IndexArrayObj, i, h3IndexObj) {
      free(uncompactedArray);
      free(compactedArray);
      return NULL;
    }
    napiGetH3Index(h3IndexObj, h3Index) {
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
    napiNapiH3Index(compacted, compactedObj) {
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
  napiGetArg(1, int32, int, res);
  uint32_t arrayLen;
  if (napi_get_array_length(env, h3IndexArrayObj, &arrayLen) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected an array of h3 indexes");
    return NULL;
  }
  H3Index* compactedArray = calloc(arrayLen, sizeof(H3Index));
  for (int i = 0; i < arrayLen; i++) {
    napiFromArray(h3IndexArrayObj, i, h3IndexObj) {
      free(compactedArray);
      return NULL;
    }
    napiGetH3Index(h3IndexObj, h3Index) {
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
    napiNapiH3Index(uncompacted, uncompactedObj) {
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

bool polygonArrayToGeofence(napi_env env, Geofence* geofence, napi_value polygonArray, bool isGeoJson) {
  uint32_t arrayLength;
  if (napi_get_array_length(env, polygonArray, &arrayLength) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Could not read length of geofence array");
    // Sorta-useless calloc to make sure error-handling 'free' below always works
    geofence->verts = calloc(1, sizeof(GeoCoord));
    return false;
  }
  geofence->numVerts = arrayLength;
  geofence->verts = calloc(arrayLength, sizeof(GeoCoord));
  for (int i = 0; i < arrayLength; i++) {
    napiFromArray(polygonArray, i, latlngArray) {
      return false;
    }
    napiFromArray(latlngArray, 0, a) {
      return false;
    }
    napiFromArray(latlngArray, 1, b) {
      return false;
    }
    if (isGeoJson) {
      napiToVar(b, double, double, lat) {
        return false;
      }
      napiToVar(a, double, double, lng) {
        return false;
      }
      geofence->verts[i].lat = degsToRads(lat);
      geofence->verts[i].lon = degsToRads(lng);
    } else {
      napiToVar(a, double, double, lat) {
        return false;
      }
      napiToVar(b, double, double, lng) {
        return false;
      }
      geofence->verts[i].lat = degsToRads(lat);
      geofence->verts[i].lon = degsToRads(lng);
    }
  }
  return true;
}

napiFn(polyfill) {
  napiArgsWithOptional(2, 1);
  napiGetNapiArg(0, geofenceArrayObj);
  napiGetArg(1, int32, int, res);
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
  napiFromArray(geofenceArrayObj, 0, firstInnerArray);
  if (napi_get_array_length(env, firstInnerArray, &arrayLength) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Could not read length of the first inner array");
    return NULL;
  }
  if (arrayLength == 0) return result;
  napiFromArray(firstInnerArray, 0, nestingTest);
  if (napi_typeof(env, nestingTest, &typepoke) != napi_ok) {
    napi_throw_error(env, "EINVAL", "No geofence data provided?");
    return NULL;
  }
  GeoPolygon polygon = { 0 };
  if (typepoke == napi_number) {
    // Single polygon shape, no holes
    polygon.numHoles = 0;
    if (!polygonArrayToGeofence(env, &polygon.geofence, geofenceArrayObj, isGeoJson)) {
      free(polygon.geofence.verts);
      return NULL;
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
      napiFromArray(geofenceArrayObj, i, polygonArray) {
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
      if (i == 0) {
        // Main geofence case
        if (!polygonArrayToGeofence(env, &polygon.geofence, polygonArray, isGeoJson)) {
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
      } else {
        // Hole case
        if (!polygonArrayToGeofence(env, &polygon.holes[i - 1], polygonArray, isGeoJson)) {
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
    napiNapiH3Index(h3, h3Obj) {
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
  napiGetH3IndexArg(0, origin);
  napiGetH3IndexArg(1, destination);

  int areNeighbors = h3IndexesAreNeighbors(origin, destination);

  napiNapiBool(areNeighbors, result);

  return result;
}

napiFn(getH3UnidirectionalEdge) {
  napiArgs(2);
  napiGetH3IndexArg(0, origin);
  napiGetH3IndexArg(1, destination);

  H3Index unidirectionalEdge = getH3UnidirectionalEdge(origin, destination);

  napiNapiUnidirectionalEdge(unidirectionalEdge, result);

  return result;
}

napiFn(h3UnidirectionalEdgeIsValid) {
  napiArgs(1);
  napiGetH3IndexArg(0, unidirectionalEdge);

  int isUnidirectionalEdge = h3UnidirectionalEdgeIsValid(unidirectionalEdge);

  napiNapiBool(isUnidirectionalEdge, result);

  return result;
}

napiFn(getOriginH3IndexFromUnidirectionalEdge) {
  napiArgs(1);
  napiGetH3IndexArg(0, unidirectionalEdge);

  H3Index origin = getOriginH3IndexFromUnidirectionalEdge(unidirectionalEdge);

  napiNapiH3Index(origin, result);

  return result;
}

napiFn(getDestinationH3IndexFromUnidirectionalEdge) {
  napiArgs(1);
  napiGetH3IndexArg(0, unidirectionalEdge);

  H3Index destination = getDestinationH3IndexFromUnidirectionalEdge(unidirectionalEdge);

  napiNapiH3Index(destination, result);

  return result;
}

napiFn(getH3IndexesFromUnidirectionalEdge) {
  napiArgs(1);
  napiGetH3IndexArg(0, unidirectionalEdge);

  H3Index originDestination[2];

  getH3IndexesFromUnidirectionalEdge(unidirectionalEdge, originDestination);

  H3Index origin = originDestination[0];
  H3Index destination = originDestination[1];

  napiFixedArray(result, 2);
  napiNapiH3Index(origin, originObj);
  napiNapiH3Index(destination, destinationObj);
  napiSetNapiValue(result, 0, originObj);
  napiSetNapiValue(result, 1, destinationObj);

  return result;
}

napiFn(getH3UnidirectionalEdgesFromHexagon) {
  napiArgs(1);
  napiGetH3IndexArg(0, origin);

  H3Index edges[6];

  getH3UnidirectionalEdgesFromHexagon(origin, edges);

  napiVarArray(result);
  int arrayLength = 0;
  for (int i = 0; i < 6; i++) {
    if (edges[i] == 0) continue;
    H3Index edge = edges[i];
    napiNapiUnidirectionalEdge(edge, unidirectionalEdge);
    napiSetNapiValue(result, arrayLength, unidirectionalEdge);
    arrayLength++;
  }

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
  napiExport(getH3UnidirectionalEdge);
  napiExport(h3UnidirectionalEdgeIsValid);
  napiExport(getOriginH3IndexFromUnidirectionalEdge);
  napiExport(getDestinationH3IndexFromUnidirectionalEdge);
  napiExport(getH3IndexesFromUnidirectionalEdge);
  napiExport(getH3UnidirectionalEdgesFromHexagon);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
