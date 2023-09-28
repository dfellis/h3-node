#include <node_api.h>
#include <stdio.h>
#include "h3/src/h3lib/include/h3api.h"
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
    H3Error O ## err = stringToH3(O ## Str, &O);\
    napiThrowErr(O ## Err, O ## err);\
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
  H3Index O;\
  char O ## Str[17];\
  size_t O ## StrCount;\
  napi_status napiGetH3Index ## I = napi_get_value_string_utf8(env, I, O ## Str, 17, &O ## StrCount);\
  if (napiGetH3Index ## I != napi_ok) {\
    napi_throw_error(env, "EINVAL", "Expected string h3 index in arg " #I);\
  }\
  H3Error O ## err = stringToH3(O ## Str, &O);\
  napiThrowErr(O ## Err, O ## err);\
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

#define napiThrowErr(N, E) \
  if (E) { \
    char N ## code[3]; \
    sprintf(N ## code, "%d", E); \
    napi_throw_error(env, N ## code, ""); \
    return NULL;\
  }

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

napiFn(latLngToCell) {
  napiArgs(3);
  napiGetArg(0, double, double, lat);
  napiGetArg(1, double, double, lng);
  napiGetArg(2, int32, int, res);

  LatLng geo = { degsToRads(lat), degsToRads(lng) };
  H3Index h3;
  H3Error err = latLngToCell(&geo, res, &h3);
  napiThrowErr(latLngToCell, err);

  napiNapiH3Index(h3, result);

  return result;
}

napiFn(cellToLatLng) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  LatLng geo = { 0 };
  cellToLatLng(h3, &geo);

  napiFixedArray(result, 2);
  napiSetValue(result, 0, double, radsToDegs(geo.lat), lat);
  napiSetValue(result, 1, double, radsToDegs(geo.lng), lng);

  return result;
}

napiFn(cellToBoundary) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  CellBoundary geoBoundary = { 0 };
  cellToBoundary(h3, &geoBoundary);

  napiFixedArray(result, geoBoundary.numVerts);
  for (int i = 0; i < geoBoundary.numVerts; i++) {
    napiFixedArray(latlngArray, 2);
    napiSetValue(latlngArray, 0, double, radsToDegs(geoBoundary.verts[i].lat), lat);
    napiSetValue(latlngArray, 1, double, radsToDegs(geoBoundary.verts[i].lng), lng);
    napiSetNapiValue(result, i, latlngArray);
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Inspection Functions                                                      //
///////////////////////////////////////////////////////////////////////////////

napiFn(getResolution) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  int res = getResolution(h3);

  napiNapiValue(res, int32, result);

  return result;
}

napiFn(getBaseCellNumber) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  int baseCell = getBaseCellNumber(h3);

  napiNapiValue(baseCell, int32, result);

  return result;
}

napiFn(isValidCell) {
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
    H3Error err = stringToH3(h3Str, &h3);\
    napiThrowErr(isValidCell, err);\
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

  int isValid = isValidCell(h3);

  napiNapiBool(isValid, result);

  return result;
}

napiFn(isResClassIII) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  int resClassIII = isResClassIII(h3);

  napiNapiBool(resClassIII, result);

  return result;
}

napiFn(isPentagon) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  int pentagon = isPentagon(h3);

  napiNapiBool(pentagon, result);

  return result;
}

napiFn(getIcosahedronFaces) {
  napiArgs(1);
  napiGetH3IndexArg(0, h3);

  int max;
  H3Error err = maxFaceCount(h3, &max);
  napiThrowErr(maxFaceCount, err);

  int* out = calloc(max, sizeof(int));
  getIcosahedronFaces(h3, out);

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

napiFn(gridDisk) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, k);

  int64_t maxSize;
  H3Error err = maxGridDiskSize(k, &maxSize);
  napiThrowErr(gridDisk, err);

  H3Index* gridDiskOut = calloc(maxSize, sizeof(H3Index));
  gridDisk(h3, k, gridDiskOut);

  int arrayIndex = 0;
  napiVarArray(result) {
    free(gridDiskOut);
    return NULL;
  }
  for (int i = 0; i < maxSize; i++) {
    if (gridDiskOut[i] == 0) continue;

    H3Index h3Num = gridDiskOut[i];
    napiNapiH3Index(h3Num, h3Val) {
      free(gridDiskOut);
      return NULL;
    }
    napiSetNapiValue(result, arrayIndex, h3Val) {
      free(gridDiskOut);
      return NULL;
    }

    arrayIndex++;
  }

  free(gridDiskOut);

  return result;
}

napiFn(gridDiskDistances) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, k);

  int64_t maxSize;
  H3Error err = maxGridDiskSize(k, &maxSize);
  napiThrowErr(gridDisk, err);

  H3Index* gridDiskOut = calloc(maxSize, sizeof(H3Index));
  int* distances = calloc(maxSize, sizeof(int));
  gridDiskDistances(h3, k, gridDiskOut, distances);

  napiVarArray(result) {
    free(distances);
    free(gridDiskOut);
    return NULL;
  }
  for (int i = 0; i < k + 1; i++) {
    napiVarArray(ring) {
      free(distances);
      free(gridDiskOut);
      return NULL;
    }
    napiSetNapiValue(result, i, ring) {
      free(distances);
      free(gridDiskOut);
      return NULL;
    }
  }

  int* arrayIndices = calloc(k + 1, sizeof(int));

  for (int i = 0; i < maxSize; i++) {
    if (gridDiskOut[i] == 0) continue;

    H3Index h3Num = gridDiskOut[i];
    int ring = distances[i];
    napiNapiH3Index(h3Num, h3Val) {
      free(arrayIndices);
      free(distances);
      free(gridDiskOut);
      return NULL;
    }
    napiFromArray(result, ring, ringArray) {
      free(arrayIndices);
      free(distances);
      free(gridDiskOut);
      return NULL;
    }
    napiSetNapiValue(ringArray, arrayIndices[ring], h3Val) {
      free(arrayIndices);
      free(distances);
      free(gridDiskOut);
      return NULL;
    }

    arrayIndices[ring]++;
  }

  free(arrayIndices);
  free(distances);
  free(gridDiskOut);

  return result;
}

napiFn(gridRingUnsafe) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, k);

  int maxSize = k == 0 ? 1 : 6 * k;
  H3Index* gridRingUnsafeOut = calloc(maxSize, sizeof(H3Index));
  if (gridRingUnsafe(h3, k, gridRingUnsafeOut) != 0) {
    napi_throw_error(env, "EINVAL", "Pentagon encountered in range of ring");
  }

  int arrayIndex = 0;
  napiVarArray(result) {
    free(gridRingUnsafeOut);
    return NULL;
  }
  for (int i = 0; i < maxSize; i++) {
    if (gridRingUnsafeOut[i] == 0) continue;

    H3Index h3Num = gridRingUnsafeOut[i];
    napiNapiH3Index(h3Num, h3Val) {
      free(gridRingUnsafeOut);
      return NULL;
    }
    napiSetNapiValue(result, arrayIndex, h3Val) {
      free(gridRingUnsafeOut);
      return NULL;
    }

    arrayIndex++;
  }

  free(gridRingUnsafeOut);

  return result;
}

napiFn(gridDistance) {
  napiArgs(2);
  napiGetH3IndexArg(0, origin);
  napiGetH3IndexArg(1, destination);

  int64_t distance;
  H3Error err = gridDistance(origin, destination, &distance);
  napiThrowErr(gridDistance, err);
  // TODO: This throws away the upper bits and would cause problems with fine resolutions and large
  // distance, but not doing this deviates from what h3-js does.
  int32_t distance2 = (int32_t)distance;

  napiNapiValue(distance2, int32, result);

  return result;
}

napiFn(cellToLocalIj) {
  napiArgs(2);
  napiGetH3IndexArg(0, origin);
  napiGetH3IndexArg(1, destination);

  CoordIJ ij = { 0 };
  int errorCode = cellToLocalIj(origin, destination, 0, &ij);
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

napiFn(localIjToCell) {
  napiArgs(2);
  napiGetH3IndexArg(0, origin);
  napiGetNapiArg(1, coords);

  CoordIJ ij = { 0 };
  H3Index h3;
  napiGetFromObject(coords, i, int32, &ij.i);
  napiGetFromObject(coords, j, int32, &ij.j);
  int errorCode = localIjToCell(origin, &ij, 0, &h3);
  if (errorCode != 0) {
    napi_throw_error(env, "EINVAL", "Index not defined for this origin and IJ coordinate pair. "
      "IJ coordinates may be too far from origin, or a pentagon distortion was encountered.");
    return NULL;
  }
  napiNapiH3Index(h3, result);

  return result;
}

napiFn(gridPathCells) {
  napiArgs(2);
  napiGetH3IndexArg(0, origin);
  napiGetH3IndexArg(1, destination);

  int64_t numHexes;
  H3Error err = gridPathCellsSize(origin, destination, &numHexes);
  napiThrowErr(gridPathCells, err);

  if (numHexes <= 0) {
    napi_throw_error(env, "EINVAL", "Line cannot be calculated");
    return NULL;
  }

  H3Index* line = calloc(numHexes, sizeof(H3Index));
  int errorCode = gridPathCells(origin, destination, line);
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

napiFn(cellToParent) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, res);

  H3Index h3Parent;
  H3Error err = cellToParent(h3, res, &h3Parent);
  napiThrowErr(cellToParent, err);

  napiNapiH3Index(h3Parent, result);

  return result;
}

napiFn(cellToChildren) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, res);

  int64_t maxSize;
  H3Error err = cellToChildrenSize(h3, res, &maxSize);
  napiThrowErr(cellToChildren, err);

  H3Index* children = calloc(maxSize, sizeof(H3Index));
  cellToChildren(h3, res, children);

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

napiFn(cellToCenterChild) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetArg(1, int32, int, res);

  H3Index h3CenterChild;
  H3Error err = cellToCenterChild(h3, res, &h3CenterChild);
  napiThrowErr(cellToCenterChild, err);

  napiNapiH3Index(h3CenterChild, result);

  return result;
}

napiFn(compactCells) {
  napiArgs(1);
  napiGetNapiArg(0, h3IndexArrayObj);
  uint32_t arrayLen;
  if (napi_get_array_length(env, h3IndexArrayObj, &arrayLen) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected an array of h3 indexes");
    return NULL;
  }
  H3Index* uncompactedArray = calloc(arrayLen, sizeof(H3Index));
  H3Index* compactedArray = calloc(arrayLen, sizeof(H3Index));
  for (uint32_t i = 0; i < arrayLen; i++) {
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
  int errorCode = compactCells(uncompactedArray, compactedArray, arrayLen);
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
  for (uint32_t i = 0; i < arrayLen; i++) {
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

napiFn(uncompactCells) {
  napiArgs(2);
  napiGetNapiArg(0, h3IndexArrayObj);
  napiGetArg(1, int32, int, res);
  uint32_t arrayLen;
  if (napi_get_array_length(env, h3IndexArrayObj, &arrayLen) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Expected an array of h3 indexes");
    return NULL;
  }
  H3Index* compactedArray = calloc(arrayLen, sizeof(H3Index));
  for (uint32_t i = 0; i < arrayLen; i++) {
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
  int64_t maxSize;
  H3Error err = uncompactCellsSize(compactedArray, arrayLen, res, &maxSize);
  napiThrowErr(uncompactCells, err);

  H3Index* uncompactedArray = calloc(maxSize, sizeof(H3Index));
  int errorCode = uncompactCells(compactedArray, arrayLen, uncompactedArray, maxSize, res);
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

bool polygonArrayToGeofence(napi_env env, GeoLoop* geofence, napi_value polygonArray, bool isGeoJson) {
  uint32_t arrayLength;
  if (napi_get_array_length(env, polygonArray, &arrayLength) != napi_ok) {
    napi_throw_error(env, "EINVAL", "Could not read length of geofence array");
    // Sorta-useless calloc to make sure error-handling 'free' below always works
    geofence->verts = calloc(1, sizeof(LatLng));
    return false;
  }
  geofence->numVerts = arrayLength;
  geofence->verts = calloc(arrayLength, sizeof(LatLng));
  for (uint32_t i = 0; i < arrayLength; i++) {
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
      geofence->verts[i].lng = degsToRads(lng);
    } else {
      napiToVar(a, double, double, lat) {
        return false;
      }
      napiToVar(b, double, double, lng) {
        return false;
      }
      geofence->verts[i].lat = degsToRads(lat);
      geofence->verts[i].lng = degsToRads(lng);
    }
  }
  return true;
}

napiFn(polygonToCells) {
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
    if (!polygonArrayToGeofence(env, &polygon.geoloop, geofenceArrayObj, isGeoJson)) {
      free(polygon.geoloop.verts);
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
      polygon.holes = calloc(polygon.numHoles, sizeof(GeoLoop));
    }
    for (uint32_t i = 0; i < arrayLength; i++) {
      napiFromArray(geofenceArrayObj, i, polygonArray) {
        if (polygon.geoloop.numVerts > 0) {
          free(polygon.geoloop.verts);
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
        if (!polygonArrayToGeofence(env, &polygon.geoloop, polygonArray, isGeoJson)) {
          if (polygon.geoloop.numVerts > 0) {
            free(polygon.geoloop.verts);
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
          if (polygon.geoloop.numVerts > 0) {
            free(polygon.geoloop.verts);
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

  int64_t maxSize;
  H3Error err = maxPolygonToCellsSize(&polygon, res, 0, &maxSize);
  napiThrowErr(maxPolygonToCellsSize, err);

  H3Index* fill = calloc(maxSize, sizeof(H3Index));
  err = polygonToCells(&polygon, res, 0, fill);
  napiThrowErr(polygonToCells, err);

  int arrayIndex = 0;
  for (int i = 0; i < maxSize; i++) {
    H3Index h3 = fill[i];
    if (h3 == 0) continue;
    napiNapiH3Index(h3, h3Obj) {
      if (polygon.geoloop.numVerts > 0) {
        free(polygon.geoloop.verts);
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
      if (polygon.geoloop.numVerts > 0) {
        free(polygon.geoloop.verts);
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

  if (polygon.geoloop.numVerts > 0) {
    free(polygon.geoloop.verts);
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

napiFn(cellsToMultiPolygon) {
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
  for (uint32_t i = 0; i < h3SetLen; i++) {
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
  cellsToLinkedMultiPolygon(h3Set, h3SetLen, lgp);
 
  // Create the output outer array
  napiVarArray(out) {
    destroyLinkedMultiPolygon(original);
    free(original);
    free(h3Set);
  }
  int outLen = 0;
  // For each polygon defined
  while (lgp) {
    // Create an inner array for that polygon and insert it into the output array
    napiVarArray(loops) {
      destroyLinkedMultiPolygon(original);
      free(original);
      free(h3Set);
    }
    int loopsLen = 0;
    napiSetNapiValue(out, outLen, loops) {
      destroyLinkedMultiPolygon(original);
      free(original);
      free(h3Set);
    }
    outLen++;
    LinkedGeoLoop* loop = lgp->first;
    // For each coordinate loop in that polygon
    while (loop) {
      // Create an inner coordinates array for that loop and insert into the polygon array
      napiVarArray(coords) {
        destroyLinkedMultiPolygon(original);
        free(original);
        free(h3Set);
      }
      int coordsLen = 0;
      napiSetNapiValue(loops, loopsLen, coords) {
        destroyLinkedMultiPolygon(original);
        free(original);
        free(h3Set);
      }
      loopsLen++;
      LinkedLatLng* coord = loop->first;
      // For each coordinate in that loop
      while (coord) {
        // Create a fixed coordinates array and insert into the coordinates loop array
        napiFixedArray(coordObj, 2) {
          destroyLinkedMultiPolygon(original);
          free(original);
          free(h3Set);
        }
        napiSetNapiValue(coords, coordsLen, coordObj) {
          destroyLinkedMultiPolygon(original);
          free(original);
          free(h3Set);
        }
        coordsLen++;
        // Then actually get the coordinates and insert them in the specified order
        double lat = coord->vertex.lat;
        double lng = coord->vertex.lng;
        if (formatAsGeoJson) {
          napiSetValue(coordObj, 0, double, lng, lngObj)  {
            destroyLinkedMultiPolygon(original);
            free(original);
            free(h3Set);
          }
          napiSetValue(coordObj, 1, double, lat, latObj) {
            destroyLinkedMultiPolygon(original);
            free(original);
            free(h3Set);
          }
        } else {
          napiSetValue(coordObj, 0, double, lat, latObj) {
            destroyLinkedMultiPolygon(original);
            free(original);
            free(h3Set);
          }
          napiSetValue(coordObj, 1, double, lng, lngObj) {
            destroyLinkedMultiPolygon(original);
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
          destroyLinkedMultiPolygon(original);
          free(original);
          free(h3Set);
        }
        napiSetNapiValue(coords, coordsLen, firstCoord) {
          destroyLinkedMultiPolygon(original);
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

  destroyLinkedMultiPolygon(original);
  free(original);
  free(h3Set);

  return out;
}

///////////////////////////////////////////////////////////////////////////////
// Unidirectional Edge Functions                                             //
///////////////////////////////////////////////////////////////////////////////

napiFn(areNeighborCells) {
  napiArgs(2);
  napiGetH3IndexArg(0, origin);
  napiGetH3IndexArg(1, destination);

  int areNeighbors;
  H3Error err = areNeighborCells(origin, destination, &areNeighbors);
  napiThrowErr(areNeighborCells, err);

  napiNapiBool(areNeighbors, result);

  return result;
}

napiFn(cellsToDirectedEdge) {
  napiArgs(2);
  napiGetH3IndexArg(0, origin);
  napiGetH3IndexArg(1, destination);

  H3Index directedEdge;
  H3Error err = cellsToDirectedEdge(origin, destination, &directedEdge);
  napiThrowErr(cellsToDirectedEdge, err);

  napiNapiUnidirectionalEdge(directedEdge, result);

  return result;
}

napiFn(isValidDirectedEdge) {
  napiArgs(1);
  napiGetH3IndexArg(0, unidirectionalEdge);

  int isUnidirectionalEdge = isValidDirectedEdge(unidirectionalEdge);

  napiNapiBool(isUnidirectionalEdge, result);

  return result;
}

napiFn(getDirectedEdgeOrigin) {
  napiArgs(1);
  napiGetH3IndexArg(0, unidirectionalEdge);

  H3Index origin;
  H3Error err = getDirectedEdgeOrigin(unidirectionalEdge, &origin);
  napiThrowErr(getDirectedEdgeOrigin, err);

  napiNapiH3Index(origin, result);

  return result;
}

napiFn(getDirectedEdgeDestination) {
  napiArgs(1);
  napiGetH3IndexArg(0, unidirectionalEdge);

  H3Index destination;
  H3Error err = getDirectedEdgeDestination(unidirectionalEdge, &destination);
  napiThrowErr(getDirectedEdgeDestination, err);
    
  napiNapiH3Index(destination, result);

  return result;
}

napiFn(directedEdgeToCells) {
  napiArgs(1);
  napiGetH3IndexArg(0, unidirectionalEdge);

  H3Index originDestination[2];

  directedEdgeToCells(unidirectionalEdge, originDestination);

  H3Index origin = originDestination[0];
  H3Index destination = originDestination[1];

  napiFixedArray(result, 2);
  napiNapiH3Index(origin, originObj);
  napiNapiH3Index(destination, destinationObj);
  napiSetNapiValue(result, 0, originObj);
  napiSetNapiValue(result, 1, destinationObj);

  return result;
}

napiFn(originToDirectedEdges) {
  napiArgs(1);
  napiGetH3IndexArg(0, origin);

  H3Index edges[6];

  originToDirectedEdges(origin, edges);

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

napiFn(directedEdgeToBoundary) {
  napiArgs(1);
  napiGetH3IndexArg(0, unidirectionalEdge);

  CellBoundary geoBoundary;

  directedEdgeToBoundary(unidirectionalEdge, &geoBoundary);

  napiFixedArray(result, geoBoundary.numVerts);
  for (int i = 0; i < geoBoundary.numVerts; i++) {
    napiFixedArray(latlngArray, 2);
    napiSetValue(latlngArray, 0, double, radsToDegs(geoBoundary.verts[i].lat), lat);
    napiSetValue(latlngArray, 1, double, radsToDegs(geoBoundary.verts[i].lng), lng);
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

napiFn(getNumCells) {
  napiArgs(1);
  napiGetArg(0, int32, int, res);

  int64_t count;
  H3Error err = getNumCells(res, &count);
  napiThrowErr(getNumCells, err);

  napiNapiValue(res, double, result);

  return result;
}

napiFn(getHexagonEdgeLengthAvg) {
  napiArgs(2);
  napiGetArg(0, int32, int, res);
  napiGetStringArg(1, 3, unit);

  if (unit[0] == 'm') {
    double edgeLength;
    H3Error err = getHexagonEdgeLengthAvgM(res, &edgeLength);
    napiThrowErr(edgeLengthM, err);

    napiNapiValue(edgeLength, double, result);

    return result;
  } else if (unit[0] == 'k' && unit[1] == 'm') {
    double edgeLength;
    H3Error err = getHexagonEdgeLengthAvgKm(res, &edgeLength);
    napiThrowErr(edgeLengthKm, err);

    napiNapiValue(edgeLength, double, result);

    return result;
  } else {
    napi_throw_error(env, "EINVAL", "Unknown unit provided");
    return NULL;
  }
}

napiFn(edgeLength) {
  napiArgs(2);
  napiGetH3IndexArg(0, h3);
  napiGetStringArg(1, 5, unit);

  if (unit[0] == 'm') {
    double len;
    H3Error err = edgeLengthM(h3, &len);
    napiThrowErr(edgeLengthM, err);

    napiNapiValue(len, double, result);

    return result;
  } else if (unit[0] == 'k' && unit[1] == 'm') {
    double len;
    H3Error err = edgeLengthKm(h3, &len);
    napiThrowErr(edgeLengthKm, err);

    napiNapiValue(len, double, result);

    return result;
  } else if (unit[0] == 'r' && unit[1] == 'a' && unit[2] == 'd' && unit[3] == 's') {
    double len;
    H3Error err = edgeLengthRads(h3, &len);
    napiThrowErr(edgeLengthRads, err);

    napiNapiValue(len, double, result);

    return result;
  } else {
    napi_throw_error(env, "EINVAL", "Unknown unit provided");
    return NULL;
  }
}

napiFn(getHexagonAreaAvg) {
  napiArgs(2);
  napiGetArg(0, int32, int, res);
  napiGetStringArg(1, 4, unit);

  if (unit[0] == 'm' && unit[1] == '2') {
    double area;
    H3Error err = getHexagonAreaAvgM2(res, &area);
    napiThrowErr(areaM2, err);

    napiNapiValue(area, double, result);

    return result;
  } else if (unit[0] == 'k' && unit[1] == 'm' && unit[2] == '2') {
    double area;
    H3Error err = getHexagonAreaAvgKm2(res, &area);
    napiThrowErr(areaKm2, err);

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
    double area;
    H3Error err = cellAreaM2(h3, &area);
    napiThrowErr(areaM2, err);

    napiNapiValue(area, double, result);

    return result;
  } else if (unit[0] == 'k' && unit[1] == 'm' && unit[2] == '2') {
    double area;
    H3Error err = cellAreaKm2(h3, &area);
    napiThrowErr(areaKm2, err);

    napiNapiValue(area, double, result);

    return result;
  } else if (unit[0] == 'r' && unit[1] == 'a' && unit[2] == 'd' && unit[3] == 's' && unit[4] == '2') {
    double area;
    H3Error err = cellAreaRads2(h3, &area);
    napiThrowErr(areaRads2, err);

    napiNapiValue(area, double, result);

    return result;
  } else {
    napi_throw_error(env, "EINVAL", "Unknown unit provided");
    return NULL;
  }
}

napiFn(greatCircleDistance) {
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

  LatLng orig = { origLat, origLng };
  LatLng dest = { destLat, destLng };


  if (unit[0] == 'm') {
    double dist = greatCircleDistanceM(&orig, &dest);

    napiNapiValue(dist, double, result);

    return result;
  } else if (unit[0] == 'k' && unit[1] == 'm') {
    double dist = greatCircleDistanceKm(&orig, &dest);

    napiNapiValue(dist, double, result);

    return result;
  } else if (unit[0] == 'r' && unit[1] == 'a' && unit[2] == 'd' && unit[3] == 's') {
    double dist = greatCircleDistanceRads(&orig, &dest);

    napiNapiValue(dist, double, result);

    return result;
  } else {
    napi_throw_error(env, "EINVAL", "Unknown unit provided");
    return NULL;
  }
}

napiFn(getRes0Cells) {
  int numHexes = res0CellCount();
  H3Index* res0Indexes = calloc(numHexes, sizeof(H3Index));
  getRes0Cells(res0Indexes);

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

napiFn(getPentagons) {
  napiArgs(1);
  napiGetArg(0, int32, int, res);

  H3Index* pentagonIndexes = calloc(12, sizeof(H3Index));
  getPentagons(res, pentagonIndexes);

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
  napiExport(latLngToCell);
  napiExport(cellToLatLng);
  napiExport(cellToBoundary);

  // Inspection Functions
  napiExport(getResolution);
  napiExport(getBaseCellNumber);
  napiExport(isValidCell);
  napiExport(isValidDirectedEdge);
  // napiExport(isValidVertex); // TODO: Implement this
  napiExport(isResClassIII);
  napiExport(isPentagon);
  napiExport(getIcosahedronFaces);

  // Traversal Functions
  napiExport(gridDisk);
  napiExport(gridDiskDistances);
  napiExport(gridRingUnsafe);
  napiExport(gridDistance);
  napiExport(cellToLocalIj);
  napiExport(localIjToCell);
  napiExport(gridPathCells);

  // Hierarchy Functions
  napiExport(cellToParent);
  napiExport(cellToChildren);
  napiExport(cellToCenterChild);
  napiExport(compactCells);
  napiExport(uncompactCells);

  // Region Functions
  napiExport(polygonToCells);
  napiExport(cellsToMultiPolygon);

  // Unidirectional Edge Functions
  napiExport(areNeighborCells);
  napiExport(cellsToDirectedEdge);
  napiExport(getDirectedEdgeOrigin);
  napiExport(getDirectedEdgeDestination);
  napiExport(directedEdgeToCells);
  napiExport(originToDirectedEdges);
  napiExport(directedEdgeToBoundary);

  // Miscellaneous Functions
  napiExport(degsToRads);
  napiExport(radsToDegs);
  napiExport(getNumCells);
  napiExport(getHexagonEdgeLengthAvg);
  napiExport(edgeLength);
  napiExport(getHexagonAreaAvg);
  napiExport(cellArea);
  napiExport(greatCircleDistance);
  napiExport(getRes0Cells);
  napiExport(getPentagons);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all)
