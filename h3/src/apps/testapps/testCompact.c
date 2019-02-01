/*
 * Copyright 2017-2018 Uber Technologies, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include "constants.h"
#include "h3Index.h"
#include "test.h"

H3Index sunnyvale = 0x89283470c27ffff;

H3Index uncompactable[] = {0x89283470803ffff, 0x8928347081bffff,
                           0x8928347080bffff};

SUITE(compact) {
    TEST(roundtrip) {
        int k = 9;
        int hexCount = H3_EXPORT(maxKringSize)(k);
        int expectedCompactCount = 73;

        // Generate a set of hexagons to compact
        H3Index* sunnyvaleExpanded = calloc(hexCount, sizeof(H3Index));
        H3_EXPORT(kRing)(sunnyvale, k, sunnyvaleExpanded);

        H3Index* compressed = calloc(hexCount, sizeof(H3Index));
        int err = H3_EXPORT(compact)(sunnyvaleExpanded, compressed, hexCount);
        t_assert(err == 0, "no error on compact");

        int count = 0;
        for (int i = 0; i < hexCount; i++) {
            if (compressed[i] != 0) {
                count++;
            }
        }
        t_assert(count == expectedCompactCount, "got expected compacted count");

        H3Index* decompressed = calloc(
            H3_EXPORT(maxUncompactSize)(compressed, count, 9), sizeof(H3Index));
        int err2 =
            H3_EXPORT(uncompact)(compressed, count, decompressed, hexCount, 9);
        t_assert(err2 == 0, "no error on uncompact");

        int count2 = 0;
        for (int i = 0; i < hexCount; i++) {
            if (decompressed[i] != 0) {
                count2++;
            }
        }
        t_assert(count2 == hexCount, "got expected uncompacted count");

        free(compressed);
        free(decompressed);
        free(sunnyvaleExpanded);
    }

    TEST(res0) {
        int hexCount = NUM_BASE_CELLS;

        H3Index* res0Hexes = calloc(hexCount, sizeof(H3Index));
        for (int i = 0; i < hexCount; i++) {
            setH3Index(&res0Hexes[i], 0, i, 0);
        }
        H3Index* compressed = calloc(hexCount, sizeof(H3Index));
        int err = H3_EXPORT(compact)(res0Hexes, compressed, hexCount);
        t_assert(err == 0, "no error on compact");

        for (int i = 0; i < hexCount; i++) {
            // At resolution 0, it will be an exact copy.
            // The test is further optimizing that it will be in order (which
            // isn't guaranteed.)
            t_assert(compressed[i] == res0Hexes[i],
                     "got expected compressed result");
        }

        H3Index* decompressed =
            calloc(H3_EXPORT(maxUncompactSize)(compressed, hexCount, 0),
                   sizeof(H3Index));
        int err2 = H3_EXPORT(uncompact)(compressed, hexCount, decompressed,
                                        hexCount, 0);
        t_assert(err2 == 0, "no error on uncompact");

        int count2 = 0;
        for (int i = 0; i < hexCount; i++) {
            if (decompressed[i] != 0) {
                count2++;
            }
        }
        t_assert(count2 == hexCount, "got expected uncompacted count");

        free(res0Hexes);
        free(compressed);
        free(decompressed);
    }

    TEST(uncompactable) {
        int hexCount = 3;
        int expectedCompactCount = 3;

        H3Index* compressed = calloc(hexCount, sizeof(H3Index));
        int err = H3_EXPORT(compact)(uncompactable, compressed, hexCount);
        t_assert(err == 0, "no error on compact");

        int count = 0;
        for (int i = 0; i < hexCount; i++) {
            if (compressed[i] != 0) {
                count++;
            }
        }
        t_assert(count == expectedCompactCount, "got expected compacted count");

        H3Index* decompressed = calloc(
            H3_EXPORT(maxUncompactSize)(compressed, count, 9), sizeof(H3Index));
        int err2 =
            H3_EXPORT(uncompact)(compressed, count, decompressed, hexCount, 9);
        t_assert(err2 == 0, "no error on uncompact");

        int count2 = 0;
        for (int i = 0; i < hexCount; i++) {
            if (decompressed[i] != 0) {
                count2++;
            }
        }
        t_assert(count2 == hexCount, "got expected uncompacted count");

        free(compressed);
        free(decompressed);
    }

    TEST(compact_duplicate) {
        int numHex = 10;
        H3Index someHexagons[10] = {0};
        for (int i = 0; i < numHex; i++) {
            setH3Index(&someHexagons[i], 5, 0, 2);
        }
        H3Index compressed[10];

        t_assert(H3_EXPORT(compact)(someHexagons, compressed, numHex) != 0,
                 "compact fails on duplicate input");
    }

    TEST(uncompact_wrongRes) {
        int numHex = 3;
        H3Index someHexagons[] = {0, 0, 0};
        for (int i = 0; i < numHex; i++) {
            setH3Index(&someHexagons[i], 5, i, 0);
        }

        int sizeResult = H3_EXPORT(maxUncompactSize)(someHexagons, numHex, 4);
        t_assert(sizeResult < 0,
                 "maxUncompactSize fails when given illogical resolutions");
        sizeResult = H3_EXPORT(maxUncompactSize)(someHexagons, numHex, -1);
        t_assert(sizeResult < 0,
                 "maxUncompactSize fails when given illegal resolutions");

        H3Index uncompressed[] = {0, 0, 0};
        int uncompactResult =
            H3_EXPORT(uncompact)(someHexagons, numHex, uncompressed, numHex, 0);
        t_assert(uncompactResult != 0,
                 "uncompact fails when given illogical resolutions");
        uncompactResult =
            H3_EXPORT(uncompact)(someHexagons, numHex, uncompressed, numHex, 6);
        t_assert(uncompactResult != 0,
                 "uncompact fails when given too little buffer");
        uncompactResult = H3_EXPORT(uncompact)(someHexagons, numHex,
                                               uncompressed, numHex - 1, 5);
        t_assert(
            uncompactResult != 0,
            "uncompact fails when given too little buffer (same resolution)");
    }

    TEST(someHexagon) {
        H3Index origin;
        setH3Index(&origin, 1, 5, 0);

        int childrenSz = H3_EXPORT(maxUncompactSize)(&origin, 1, 2);
        H3Index* children = calloc(childrenSz, sizeof(H3Index));
        int uncompactResult =
            H3_EXPORT(uncompact)(&origin, 1, children, childrenSz, 2);
        t_assert(uncompactResult == 0, "uncompact origin succeeds");

        H3Index* result = calloc(childrenSz, sizeof(H3Index));
        int compactResult = H3_EXPORT(compact)(children, result, childrenSz);
        t_assert(compactResult == 0, "compact origin succeeds");

        int found = 0;
        for (int i = 0; i < childrenSz; i++) {
            if (result[i] != 0) {
                found++;
                t_assert(result[i] == origin, "compacted to correct origin");
            }
        }
        t_assert(found == 1, "compacted to a single hexagon");

        free(children);
        free(result);
    }

    TEST(pentagon) {
        H3Index pentagon;
        setH3Index(&pentagon, 1, 4, 0);

        int childrenSz = H3_EXPORT(maxUncompactSize)(&pentagon, 1, 2);
        H3Index* children = calloc(childrenSz, sizeof(H3Index));
        int uncompactResult =
            H3_EXPORT(uncompact)(&pentagon, 1, children, childrenSz, 2);
        t_assert(uncompactResult == 0, "uncompact pentagon succeeds");

        H3Index* result = calloc(childrenSz, sizeof(H3Index));
        int compactResult = H3_EXPORT(compact)(children, result, childrenSz);
        t_assert(compactResult == 0, "compact pentagon succeeds");

        int found = 0;
        for (int i = 0; i < childrenSz; i++) {
            if (result[i] != 0) {
                found++;
                t_assert(result[i] == pentagon,
                         "compacted to correct pentagon");
            }
        }
        t_assert(found == 1, "compacted to a single pentagon");

        free(children);
        free(result);
    }
}
