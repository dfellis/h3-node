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

#define napiGetStringArg(I, L, N) \
  char N[L];\
  size_t N ## Count;\
  \
  if (napi_get_value_string_utf8(env, argv[I], N, L, &N ## Count) != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected unit argument");\
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

#define napiArrayLen(A, N) \
  uint32_t N;\
  napi_status napiArrayLen ## A ## N = napi_get_array_length(env, A, &N);\
  if (napiArrayLen ## A ## N != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Did not receive a valid array");\
  }\
  if (napiArrayLen ## A ## N != napi_ok)

#define napiGetH3IndexArg(I, O) \
  H3Index O;\
  /*For string form input*/\
  char O ## Str[17];\
  size_t O ## StrCount;\
  /*For typedarray form input*/\
  napi_typedarray_type O ## Type;\
  size_t O ## Length;\
  void* O ## Data;\
  /*For array form input*/\
  napiGetNapiArg(I, O ## Arr);\
  uint32_t O ## ArrSz;\
  \
  if (napi_get_value_string_utf8(env, argv[I], O ## Str, 17, &O ## StrCount) == napi_ok) {\
    O = stringToH3(O ## Str);\
  } else if (napi_get_typedarray_info(env, O ## Arr, &O ## Type, &O ## Length, &O ## Data, NULL, NULL) == napi_ok) {\
    if (O ## Type != napi_uint32_array || O ## Length != 2) {\
      napi_throw_error(env, "EINVAL", "Invalid Uint32Array H3 index in arg " #I);\
      return NULL;\
    }\
    O = ((H3Index)*(((uint32_t*)O ## Data) + 1) << 32) | *((uint32_t*)O ## Data);\
  } else if (napi_get_array_length(env, O ## Arr, &O ## ArrSz) == napi_ok) {\
    if (O ## ArrSz != 2) {\
      napi_throw_error(env, "EINVAL", "Invalid length array H3 index in arg " #I);\
      return NULL;\
    }\
    napi_value O ## Val0, O ## Val1;\
    if (napi_get_element(env, O ## Arr, 0, &O ## Val0) != napi_ok || napi_get_element(env, O ## Arr, 1, &O ## Val1) != napi_ok) {\
      napi_throw_error(env, "EINVAL", "Invalid array H3 index in arg " #I);\
      return NULL;\
    }\
    uint32_t O ## Part0, O ## Part1;\
    if (napi_get_value_uint32(env, O ## Val0, &O ## Part0) != napi_ok || napi_get_value_uint32(env, O ## Val1, &O ## Part1) != napi_ok) {\
      napi_throw_error(env, "EINVAL", "Invalid integer array H3 index in arg " #I);\
      return NULL;\
    }\
    O = ((H3Index)(O ## Part1) << 32) | (O ## Part0);\
  } else {\
    napi_throw_error(env, "EINVAL", "Expected string or array H3 index in arg " #I);\
    return NULL;\
  }

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
  napi_status napiFixedArray ## V = napi_create_array_with_length(env, L, &V);\
  if (napiFixedArray ## V != napi_ok) {\
    napi_throw_error(env, "ENOSPC", "Could not create fixed length array");\
    return NULL;\
  }\
  if (napiFixedArray ## V != napi_ok)

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
  napiSetNapiValue(A, I, N)

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
  // inlined napiGetH3IndexArg in order to return false instead of throwing
  H3Index h3;
  /*For string form input*/
  char h3Str[17];
  size_t h3StrCount;
  /*For typedarray form input*/
  napi_typedarray_type h3Type;
  size_t h3Length;
  void* h3Data;
  /*For array form input*/
  napiGetNapiArg(0, h3Arr);
  uint32_t h3ArrSz;

  if (napi_get_value_string_utf8(env, argv[0], h3Str, 17, &h3StrCount) == napi_ok) {
    h3 = stringToH3(h3Str);
  } else if (napi_get_typedarray_info(env, h3Arr, &h3Type, &h3Length, &h3Data, NULL, NULL) == napi_ok) {
    if (h3Type != napi_uint32_array || h3Length != 2) {
      napiNapiBool(false, result);
      return result;
    }
    h3 = ((H3Index)*(((uint32_t*)h3Data) + 1) << 32) | *((uint32_t*)h3Data);
  } else if (napi_get_array_length(env, h3Arr, &h3ArrSz) == napi_ok) {
    if (h3ArrSz != 2) {
      napiNapiBool(false, result);
      return result;
    }
    napi_value h3Val0, h3Val1;
    if (napi_get_element(env, h3Arr, 0, &h3Val0) != napi_ok || napi_get_element(env, h3Arr, 1, &h3Val1) != napi_ok) {
      napiNapiBool(false, result);
      return result;
    }
    uint32_t h3Part0, h3Part1;
    if (napi_get_value_uint32(env, h3Val0, &h3Part0) != napi_ok || napi_get_value_uint32(env, h3Val1, &h3Part1) != napi_ok) {
      napiNapiBool(false, result);
      return result;
    }
    h3 = ((H3Index)(h3Part1) << 32) | (h3Part0);
  } else {
    napiNapiBool(false, result);
    return result;
  }

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

napiFn(h3GetFaces) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  int max = maxFaceCount(h3);
  int* out = calloc(max, sizeof(int));
  h3GetFaces(h3, out);

  napiFixedArray(result, max);
  for (int i = 0; i < max; i++) {
    if (out[i] >= 0) {
      napiSetValue(result, i, int32, out[i], temp);
    }
  }
  free(out);

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

napiFn(h3Line) {
  napiArgs(2);
  napiGetH3IndexArg(0, origin);
  napiGetH3IndexArg(1, destination);

  int numHexes = h3LineSize(origin, destination);
  if (numHexes <= 0) {
    napi_throw_error(env, "EINVAL", "Line cannot be calculated");
    return NULL;
  }

  H3Index* line = calloc(numHexes, sizeof(H3Index));
  int errorCode = h3Line(origin, destination, line);
  if (errorCode != 0) {
    napi_throw_error(env, "EINVAL", "Line cannot be calculated");
    free(line);
    return NULL;
  }

  int arrayIndex = 0;
  napiVarArray(result) {
    free(line);
    return NULL;
  }
  for (int i = 0; i < numHexes; i++) {
    if (line[i] == 0) continue;

    H3Index h3Num = line[i];
    napiNapiH3Index(h3Num, h3Val) {
      free(line);
      return NULL;
    }
    napiSetNapiValue(result, arrayIndex, h3Val) {
      free(line);
      return NULL;
    }

    arrayIndex++;
  }

  free(line);
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

napiFn(h3ToCenterChild) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, res);

  H3Index h3CenterChild = h3ToCenterChild(h3, res);

  napiNapiH3Index(h3CenterChild, result);

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

napiFn(h3SetToMultiPolygon) {
  napiArgsWithOptional(1, 1);
  napiGetNapiArg(0, h3SetArrayObj);
  napi_value formatAsGeoJsonObj;
  if (napi_coerce_to_bool(env, argv[1], &formatAsGeoJsonObj) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Somehow cannot coerce a boolean?");
    return NULL;
  }
  bool formatAsGeoJson;
  if (napi_get_value_bool(env, formatAsGeoJsonObj, &formatAsGeoJson) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Somehow cannot extract boolean from JS object");
    return NULL;
  }

  napiArrayLen(h3SetArrayObj, h3SetLen);
  H3Index* h3Set = calloc(h3SetLen, sizeof(H3Index));
  for (int i = 0; i < h3SetLen; i++) {
    napiFromArray(h3SetArrayObj, i, h3Obj) {
      free(h3Set);
      return NULL;
    }
    napiGetH3Index(h3Obj, h3) {
      free(h3Set);
      return NULL;
    }
    h3Set[i] = h3;
  }
  
  LinkedGeoPolygon* lgp = calloc(1, sizeof(LinkedGeoPolygon));
  LinkedGeoPolygon* original = lgp; // For later deallocation needs
  h3SetToLinkedGeo(h3Set, h3SetLen, lgp);
 
  // Create the output outer array
  napiVarArray(out) {
    destroyLinkedPolygon(original);
    free(original);
    free(h3Set);
  }
  int outLen = 0;
  // For each polygon defined
  while (lgp) {
    // Create an inner array for that polygon and insert it into the output array
    napiVarArray(loops) {
      destroyLinkedPolygon(original);
      free(original);
      free(h3Set);
    }
    int loopsLen = 0;
    napiSetNapiValue(out, outLen, loops) {
      destroyLinkedPolygon(original);
      free(original);
      free(h3Set);
    }
    outLen++;
    LinkedGeoLoop* loop = lgp->first;
    // For each coordinate loop in that polygon
    while (loop) {
      // Create an inner coordinates array for that loop and insert into the polygon array
      napiVarArray(coords) {
        destroyLinkedPolygon(original);
        free(original);
        free(h3Set);
      }
      int coordsLen = 0;
      napiSetNapiValue(loops, loopsLen, coords) {
        destroyLinkedPolygon(original);
        free(original);
        free(h3Set);
      }
      loopsLen++;
      LinkedGeoCoord* coord = loop->first;
      // For each coordinate in that loop
      while (coord) {
        // Create a fixed coordinates array and insert into the coordinates loop array
        napiFixedArray(coordObj, 2) {
          destroyLinkedPolygon(original);
          free(original);
          free(h3Set);
        }
        napiSetNapiValue(coords, coordsLen, coordObj) {
          destroyLinkedPolygon(original);
          free(original);
          free(h3Set);
        }
        coordsLen++;
        // Then actually get the coordinates and insert them in the specified order
        double lat = coord->vertex.lat;
        double lng = coord->vertex.lon;
        if (formatAsGeoJson) {
          napiSetValue(coordObj, 0, double, lng, lngObj)  {
            destroyLinkedPolygon(original);
            free(original);
            free(h3Set);
          }
          napiSetValue(coordObj, 1, double, lat, latObj) {
            destroyLinkedPolygon(original);
            free(original);
            free(h3Set);
          }
        } else {
          napiSetValue(coordObj, 0, double, lat, latObj) {
            destroyLinkedPolygon(original);
            free(original);
            free(h3Set);
          }
          napiSetValue(coordObj, 1, double, lng, lngObj) {
            destroyLinkedPolygon(original);
            free(original);
            free(h3Set);
          }
        }
        // And repeat until all of the coordinates are done
        coord = coord->next;
      }
      // GeoJSON arrays must be closed
      if (formatAsGeoJson) {
        napiFromArray(coords, 0, firstCoord) {
          destroyLinkedPolygon(original);
          free(original);
          free(h3Set);
        }
        napiSetNapiValue(coords, coordsLen, firstCoord) {
          destroyLinkedPolygon(original);
          free(original);
          free(h3Set);
        }
      }
      // And repeat until all polygon loops are done
      loop = loop->next;
    }
    // And repeat until all polygons are done
    lgp = lgp->next;
  }

  destroyLinkedPolygon(original);
  free(original);
  free(h3Set);

  return out;
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

napiFn(getH3UnidirectionalEdgeBoundary) {
  napiArgs(1);
  napiGetH3IndexArg(0, unidirectionalEdge);

  GeoBoundary geoBoundary;

  getH3UnidirectionalEdgeBoundary(unidirectionalEdge, &geoBoundary);

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
// Miscellaneous Functions                                                   //
///////////////////////////////////////////////////////////////////////////////

napiFn(degsToRads) {
  napiArgs(1);
  napiGetArg(0, double, double, degs);

  double rads = degsToRads(degs);

  napiNapiValue(rads, double, result);

  return result;
}

napiFn(radsToDegs) {
  napiArgs(1);
  napiGetArg(0, double, double, rads);

  double degs = radsToDegs(rads);

  napiNapiValue(degs, double, result);

  return result;
}

napiFn(numHexagons) {
  napiArgs(1);
  napiGetArg(0, int32, int, res);

  int64_t count = numHexagons(res);
  double approximateCount = count;

  napiNapiValue(res, double, result);

  return result;
}

napiFn(edgeLength) {
  napiArgs(2);
  napiGetArg(0, int32, int, res);
  napiGetStringArg(1, 3, unit);

  if (unit[0] == 'm') {
    double edgeLength = edgeLengthM(res);

    napiNapiValue(edgeLength, double, result);

    return result;
  } else if (unit[0] == 'k' && unit[1] == 'm') {
    double edgeLength = edgeLengthKm(res);

    napiNapiValue(edgeLength, double, result);

    return result;
  } else {
    napi_throw_error(env, "EINVAL", "Unknown unit provided");
    return NULL;
  }
}

napiFn(exactEdgeLength) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetStringArg(1, 5, unit);

  if (unit[0] == 'm') {
    double len = exactEdgeLengthM(h3);

    napiNapiValue(len, double, result);

    return result;
  } else if (unit[0] == 'k' && unit[1] == 'm') {
    double len = exactEdgeLengthKm(h3);

    napiNapiValue(len, double, result);

    return result;
  } else if (unit[0] == 'r' && unit[1] == 'a' && unit[2] == 'd' && unit[3] == 's') {
    double len = exactEdgeLengthRads(h3);

    napiNapiValue(len, double, result);

    return result;
  } else {
    napi_throw_error(env, "EINVAL", "Unknown unit provided");
    return NULL;
  }
}

napiFn(hexArea) {
  napiArgs(2);
  napiGetArg(0, int32, int, res);
  napiGetStringArg(1, 4, unit);

  if (unit[0] == 'm' && unit[1] == '2') {
    double area = hexAreaM2(res);

    napiNapiValue(area, double, result);

    return result;
  } else if (unit[0] == 'k' && unit[1] == 'm' && unit[2] == '2') {
    double area = hexAreaKm2(res);

    napiNapiValue(area, double, result);

    return result;
  } else {
    napi_throw_error(env, "EINVAL", "Unknown unit provided");
    return NULL;
  }
}

napiFn(cellArea) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetStringArg(1, 6, unit);

  if (unit[0] == 'm' && unit[1] == '2') {
    double area = cellAreaM2(h3);

    napiNapiValue(area, double, result);

    return result;
  } else if (unit[0] == 'k' && unit[1] == 'm' && unit[2] == '2') {
    double area = cellAreaKm2(h3);

    napiNapiValue(area, double, result);

    return result;
  } else if (unit[0] == 'r' && unit[1] == 'a' && unit[2] == 'd' && unit[3] == 's' && unit[4] == '2') {
    double area = cellAreaRads2(h3);

    napiNapiValue(area, double, result);

    return result;
  } else {
    napi_throw_error(env, "EINVAL", "Unknown unit provided");
    return NULL;
  }
}

napiFn(pointDist) {
  napiArgs(3);
  napiGetNapiArg(0, origArr);
  napiGetNapiArg(1, destArr);
  napiGetStringArg(2, 5, unit);

  napiFromArray(origArr, 0, origLatObj) {
    napi_throw_error(env, "EINVAL", "No origin latitude");
    return NULL;
  }
  napiFromArray(origArr, 1, origLngObj) {
    napi_throw_error(env, "EINVAL", "No origin longitude");
    return NULL;
  }
  napiFromArray(destArr, 0, destLatObj) {
    napi_throw_error(env, "EINVAL", "No destination latitude");
    return NULL;
  }
  napiFromArray(destArr, 1, destLngObj) {
    napi_throw_error(env, "EINVAL", "No destination longitude");
    return NULL;
  }
  napiToVar(origLatObj, double, double, origLat) {
    napi_throw_error(env, "EINVAL", "Origin latitude is not a number");
    return NULL;
  }
  napiToVar(origLngObj, double, double, origLng) {
    napi_throw_error(env, "EINVAL", "Origin longitude is not a number");
    return NULL;
  }
  napiToVar(destLatObj, double, double, destLat) {
    napi_throw_error(env, "EINVAL", "Destination latitude is not a number");
    return NULL;
  }
  napiToVar(destLngObj, double, double, destLng) {
    napi_throw_error(env, "EINVAL", "Destination longitude is not a number");
    return NULL;
  }

  GeoCoord orig = { origLat, origLng };
  GeoCoord dest = { destLat, destLng };


  if (unit[0] == 'm') {
    double dist = pointDistM(&orig, &dest);

    napiNapiValue(dist, double, result);

    return result;
  } else if (unit[0] == 'k' && unit[1] == 'm') {
    double dist = pointDistKm(&orig, &dest);

    napiNapiValue(dist, double, result);

    return result;
  } else if (unit[0] == 'r' && unit[1] == 'a' && unit[2] == 'd' && unit[3] == 's') {
    double dist = pointDistRads(&orig, &dest);

    napiNapiValue(dist, double, result);

    return result;
  } else {
    napi_throw_error(env, "EINVAL", "Unknown unit provided");
    return NULL;
  }
}

napiFn(getRes0Indexes) {
  int numHexes = res0IndexCount();
  H3Index* res0Indexes = calloc(numHexes, sizeof(H3Index));
  getRes0Indexes(res0Indexes);

  napiVarArray(result) {
    free(res0Indexes);
    return NULL;
  }
  for (int i = 0; i < numHexes; i++) {
    H3Index h3Num = res0Indexes[i];
    napiNapiH3Index(h3Num, h3Val) {
      free(res0Indexes);
      return NULL;
    }
    napiSetNapiValue(result, i, h3Val) {
      free(res0Indexes);
      return NULL;
    }
  }

  free(res0Indexes);
  return result;
}

napiFn(getPentagonIndexes) {
  napiArgs(1);
  napiGetArg(0, int32, int, res);

  H3Index* pentagonIndexes = calloc(12, sizeof(H3Index));
  getPentagonIndexes(res, pentagonIndexes);

  napiVarArray(result) {
    free(pentagonIndexes);
    return NULL;
  }
  for (int i = 0; i < 12; i++) {
    H3Index h3Num = pentagonIndexes[i];
    napiNapiH3Index(h3Num, h3Val) {
      free(pentagonIndexes);
      return NULL;
    }
    napiSetNapiValue(result, i, h3Val) {
      free(pentagonIndexes);
      return NULL;
    }
  }

  free(pentagonIndexes);
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
  napiExport(h3GetFaces);

  // Traversal Functions
  napiExport(kRing);
  napiExport(kRingDistances);
  napiExport(hexRing);
  napiExport(h3Distance);
  napiExport(experimentalH3ToLocalIj);
  napiExport(experimentalLocalIjToH3);
  napiExport(h3Line);

  // Hierarchy Functions
  napiExport(h3ToParent);
  napiExport(h3ToChildren);
  napiExport(h3ToCenterChild);
  napiExport(compact);
  napiExport(uncompact);

  // Region Functions
  napiExport(polyfill);
  napiExport(h3SetToMultiPolygon);

  // Unidirectional Edge Functions
  napiExport(h3IndexesAreNeighbors);
  napiExport(getH3UnidirectionalEdge);
  napiExport(h3UnidirectionalEdgeIsValid);
  napiExport(getOriginH3IndexFromUnidirectionalEdge);
  napiExport(getDestinationH3IndexFromUnidirectionalEdge);
  napiExport(getH3IndexesFromUnidirectionalEdge);
  napiExport(getH3UnidirectionalEdgesFromHexagon);
  napiExport(getH3UnidirectionalEdgeBoundary);

  // Miscellaneous Functions
  napiExport(degsToRads);
  napiExport(radsToDegs);
  napiExport(numHexagons);
  napiExport(edgeLength);
  napiExport(exactEdgeLength);
  napiExport(hexArea);
  napiExport(cellArea);
  napiExport(pointDist);
  napiExport(getRes0Indexes);
  napiExport(getPentagonIndexes);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
