/*
 * Copyright (C) 2013 by
 * Dent Earl (dearl@soe.ucsc.edu, dentearl@gmail.com)
 * ... and other members of the Reconstruction Team of David Haussler's
 * lab (BME Dept. UCSC).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "CuTest.h"
#include "common.h"
#include "sharedMaf.h"
#include "mafPairCoverageAPI.h"

static void BinContents(BinContainer *bc) {
  fprintf(stderr, "bin contents: ");
  for (int i = 0; i < binContainer_getNumBins(bc); ++i) {
    if ((i != 0) && !(i % 5)) {
      fprintf(stderr, " ");
    }
    fprintf(stderr, "%" PRIu64 " ", binContainer_accessBin(bc, i));
  }
  fprintf(stderr, "\n");
}

static void test_is_wild_0(CuTest *testCase) {
  CuAssertTrue(testCase, is_wild("hg19*"));
  CuAssertTrue(testCase, is_wild("hg19.chr19*"));
  CuAssertTrue(testCase, !is_wild("hg19.chr19"));
  CuAssertTrue(testCase, !is_wild("hg19.chr1*9"));
  CuAssertTrue(testCase, !is_wild("aoeuaoeunstaoeunshtonuts.chrcrhrc.huaoeunsatohunt."));
  CuAssertTrue(testCase, is_wild("aoeuaoeunstaoeunshtonuts.chrcrhrc.huaoeunsatohunt.*"));
}
static void test_searchMatched_0(CuTest *testCase) {
  mafLine_t *ml = maf_newMafLineFromString("s hg19.chr19        123480 13 + 1234870098734 ACGTACGTACGTA", 1);
  CuAssertTrue(testCase, searchMatched(ml, "hg19.chr19"));
  CuAssertTrue(testCase, searchMatched(ml, "hg19*"));
  CuAssertTrue(testCase, searchMatched(ml, "h*"));
  CuAssertTrue(testCase, searchMatched(ml, "*"));
  CuAssertTrue(testCase, !searchMatched(ml, "mm9"));
  maf_destroyMafLineList(ml);
}
static void test_compareLines_0(CuTest *testCase) {
  // just make sure that its counting the number of aligned positions correctly
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      123480 13 + 1234870098734 ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + 1234870098735 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey, stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey, stHash_stringEqualKey, free, free);
  stHash *empty = stHash_construct3(stHash_stringKey, stHash_stringEqualKey, free, free);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_init();
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098735);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions, empty, bc);
  CuAssertTrue(testCase, alignedPositions == 13);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions, empty, bc);
  CuAssertTrue(testCase, alignedPositions == 26);
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(empty);
  binContainer_destruct(bc);

  // test case 1
  alignedPositions = 0;
  ml1 = maf_newMafLineFromString("s hg19.chr19      123480 13 + 1234870098734 ACGTACGTACGTA", 1);
  ml2 = maf_newMafLineFromString("s mm9.chr2        123480  5 + 1234870098735 AC--------GTA", 1);
  seq1Hash = stHash_construct3(stHash_stringKey, stHash_stringEqualKey, free, free);
  seq2Hash = stHash_construct3(stHash_stringKey, stHash_stringEqualKey, free, free);
  empty = stHash_construct3(stHash_stringKey, stHash_stringEqualKey, free, free);
  mcct1 = createMafCoverageCount();
  mcct2 = createMafCoverageCount();
  bc = binContainer_init();
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098735);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr2"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions, empty, bc);
  CuAssertTrue(testCase, alignedPositions == 5);
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(empty);
  binContainer_destruct(bc);
}
static void test_compareLines_1(CuTest *testCase) {
  // make sure that the hash is being correctly populated
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      123480 13 + 1234870098734 ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + 1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey, stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey, stHash_stringEqualKey, free, free);
  stHash *empty = stHash_construct3(stHash_stringKey, stHash_stringEqualKey, free, free);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_init();
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions, empty, bc);
  CuAssertTrue(testCase, stHash_search(seq1Hash, "hg19.chr19") != NULL);
  CuAssertTrue(testCase, stHash_search(seq2Hash, "mm9.chr1") != NULL);
  CuAssertTrue(testCase, stHash_search(seq2Hash, "bannana") == NULL);
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(empty);
  binContainer_destruct(bc);
  // test case 1
}
static void test_intervalCheck_0(CuTest *testCase) {
  // make sure that the hash is being correctly populated
  // test case 0
  stHash *intervalsHash = stHash_construct3(stHash_stringKey, stHash_stringEqualKey, free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  for (int i = 123480; i < 123485; ++i) {
    CuAssertTrue(testCase, inInterval(intervalsHash, "hg19.chr19", i) == true);
  }
  CuAssertTrue(testCase, inInterval(intervalsHash, "hg19.chr19", 123479) == false);
  CuAssertTrue(testCase, inInterval(intervalsHash, "hg19.chr19", 123486) == false);
  CuAssertTrue(testCase, inInterval(intervalsHash, "hg19.chr1", 123482) == false);
  CuAssertTrue(testCase, inInterval(intervalsHash, "hg19", 123482) == false);
  stHash_destruct(intervalsHash);
}
static void displayIntTuple(stIntTuple *t) {
  if (t != NULL) {
    printf("%" PRIi64 " %" PRIi64 "\n", stIntTuple_get(t, 0), stIntTuple_get(t, 1));
  } else {
    printf("NULL\n");
  }
}
static void displaySortedSet(stSortedSet *s) {
  stSortedSetIterator *sit = stSortedSet_getIterator(s);
  stIntTuple *t = NULL;
  while ((t = stSortedSet_getNext(sit)) != NULL) {
    displayIntTuple(t);
  }
}
static void test_compareLines_region_0(CuTest *testCase) {
  // make sure that the hash is being correctly populated
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "123480 13 + 1234870098734 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stIntTuple *q = stIntTuple_construct2(123482, 123484);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_init();
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, stHash_search(seq1Hash, "hg19.chr19") != NULL);
  CuAssertTrue(testCase, stHash_search(seq2Hash, "mm9.chr1") != NULL);
  CuAssertTrue(testCase, stHash_search(seq2Hash, "bannana") == NULL);
  stSortedSet *intSet = stHash_search(intervalsHash, "hg19.chr19");
  CuAssertTrue(testCase, intSet != NULL);
  stIntTuple *u = stSortedSet_search(intSet, q);
  CuAssertTrue(testCase, u == NULL);
  free(q);
  CuAssertTrue(testCase, mafCoverageCount_getInRegion(mcct1) == 4);
  CuAssertTrue(testCase, mafCoverageCount_getOutRegion(mcct1) == 9);
  CuAssertTrue(testCase, mafCoverageCount_getInRegion(mcct2) == 0);
  CuAssertTrue(testCase, mafCoverageCount_getOutRegion(mcct2) == 13);
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
  // test case 1
}
static void test_binning_0(CuTest *testCase) {
  // make sure that the binning is working correctly
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "123480 13 + 1234870098734 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(123480, 123500, 1);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 123480);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 123500);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 21);
  for (int i = 0; i < 13; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 1);
  }
  for (int i = 13; i < binContainer_getNumBins(bc); ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_1(CuTest *testCase) {
  // make sure that the binning is working correctly
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "123480 13 + 1234870098734 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(123480, 123500, 1000);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 123480);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 123500);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 1);
  CuAssertTrue(testCase, binContainer_accessBin(bc, 0) == 13);
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_2(CuTest *testCase) {
  // make sure that the binning is working correctly
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "123480 13 + 1234870098734 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(123480, 123500, 2);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 123480);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 123500);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 11);
  for (int i = 0; i < 5; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 2);
  }
  CuAssertTrue(testCase, binContainer_accessBin(bc, 6) == 1);
  for (int i = 7; i < binContainer_getNumBins(bc); ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_3(CuTest *testCase) {
  // make sure that the binning is working correctly
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "123480 13 + 1234870098734 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(123480, 123500, 3);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 123480);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 123500);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 7);
  for (int i = 0; i < 4; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 3);
  }
  CuAssertTrue(testCase, binContainer_accessBin(bc, 4) == 1);
  for (int i = 5; i < binContainer_getNumBins(bc); ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }

  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_4(CuTest *testCase) {
  // make sure that the binning is working correctly
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "123480 13 + 1234870098734 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(123480, 123500, 4);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 123480);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 123500);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 6);
  for (int i = 0; i < 3; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 4);
  }
  CuAssertTrue(testCase, binContainer_accessBin(bc, 3) == 1);
  for (int i = 4; i < binContainer_getNumBins(bc); ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_5(CuTest *testCase) {
  // make sure that the binning is working correctly
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "123480 13 + 1234870098734 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(123480, 123500, 11);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 123480);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 123500);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 2);
  for (int i = 0; i < 1; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 11);
  }
  CuAssertTrue(testCase, binContainer_accessBin(bc, 1) == 2);
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_6(CuTest *testCase) {
  // make sure that the binning is working correctly
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "123480 13 + 1234870098734 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(123486, 123495, 2);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 123486);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 123495);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 5);
  for (int i = 0; i < 3; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 2);
  }
  CuAssertTrue(testCase, binContainer_accessBin(bc, 3) == 1);
  for (int i = 6; i < binContainer_getNumBins(bc); ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_7(CuTest *testCase) {
  // make sure that the binning is working correctly
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "123480 13 + 1234870098734 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(123476, 123495, 2);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 123476);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 123495);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 10);
  for (int i = 0; i < 2; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  for (int i = 2; i < 8; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 2);
  }
  CuAssertTrue(testCase, binContainer_accessBin(bc, 8) == 1);
  for (int i = 9; i < binContainer_getNumBins(bc); ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_8(CuTest *testCase) {
  // make sure that the binning is working correctly
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "123480 13 + 1234870098734 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(123476, 123485, 2);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 123476);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 123485);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 5);
  for (int i = 0; i < 2; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  for (int i = 2; i < binContainer_getNumBins(bc); ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 2);
  }
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_9(CuTest *testCase) {
  // make sure that the binning is working correctly with negative strand!
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "0 13 - 100 "
                                            "ACGTACGTACGTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACGTACGTACGTA", 1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(80, 100, 1);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 80);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 100);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 21);
  for (int i = 0; i < 7; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  for (int i = 7; i < 20; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 1);
  }
  CuAssertTrue(testCase, binContainer_accessBin(bc, 20) == 0);
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_10(CuTest *testCase) {
  // make sure that the binning is working correctly with gaps negative strand
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "0 13 - 100 "
                                            "ACG---TACGTAC---GTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 ACG---TACGTAC---GTA",
                                            1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(80, 100, 1);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 80);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 100);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 21);
  for (int i = 0; i < 7; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  for (int i = 7; i < 20; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 1);
  }
  CuAssertTrue(testCase, binContainer_accessBin(bc, 20) == 0);
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}

static void test_binning_11(CuTest *testCase) {
  // make sure that the binning is working correctly with gaps negative strand
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "0 13 - 100 "
                                            "ACG---TACGTAC---GTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 "
                                            "ACG------TACGTACGTA",
                                            1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(80, 100, 1);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 80);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 100);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  // remember, this is negative strand
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 21);
  for (int i = 0; i < 7; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  for (int i = 7; i < 14; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 1);
  }
  for (int i = 14; i < 17; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  for (int i = 17; i < 20; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 1);
  }
  CuAssertTrue(testCase, binContainer_accessBin(bc, 20) == 0);
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}
static void test_binning_12(CuTest *testCase) {
  // make sure that the binning is working correctly with gaps positive strand
  // test case 0
  mafLine_t *ml1 = maf_newMafLineFromString("s hg19.chr19      "
                                            "0 13 + 100 "
                                            "ACG---TACGTAC---GTA", 1);
  mafLine_t *ml2 = maf_newMafLineFromString("s mm9.chr1        123480 13 + "
                                            "1234870098734 "
                                            "ACG------TACGTACGTA",
                                            1);
  stHash *seq1Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *seq2Hash = stHash_construct3(stHash_stringKey,
                                       stHash_stringEqualKey, free, free);
  stHash *intervalsHash = stHash_construct3(stHash_stringKey,
                                            stHash_stringEqualKey,
                                            free, (void(*)(void *)) stSortedSet_destruct);
  stSortedSet *intervals = stSortedSet_construct3((int(*)(const void *, const void*)) stIntTuple_cmpFn,
                                                  (void(*)(void *)) stIntTuple_destruct);
  stHash_insert(intervalsHash, stString_copy("hg19.chr19"), intervals);
  stIntTuple *t = stIntTuple_construct2(123480, 123485);
  stSortedSet_insert(intervals, t);
  uint64_t alignedPositions = 0;
  mafCoverageCount_t *mcct1 = createMafCoverageCount();
  mafCoverageCount_t *mcct2 = createMafCoverageCount();
  BinContainer *bc = binContainer_construct(0, 20, 1);
  mafCoverageCount_setSourceLength(mcct1, 1234870098734);
  mafCoverageCount_setSourceLength(mcct2, 1234870098734);
  stHash_insert(seq1Hash, stString_copy("hg19.chr19"), mcct1);
  stHash_insert(seq2Hash, stString_copy("mm9.chr1"), mcct2);
  compareLines(ml1, ml2, seq1Hash, seq2Hash, &alignedPositions,
               intervalsHash, bc);
  CuAssertTrue(testCase, binContainer_getBinStart(bc) == 0);
  CuAssertTrue(testCase, binContainer_getBinEnd(bc) == 20);
  CuAssertTrue(testCase, binContainer_getBins(bc) != NULL);
  // BinContents(bc);
  CuAssertTrue(testCase, binContainer_getNumBins(bc) == 21);
  for (int i = 0; i < 3; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 1);
  }
  for (int i = 3; i < 6; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  for (int i = 6; i < 13; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 1);
  }
  for (int i = 13; i < 21; ++i) {
    CuAssertTrue(testCase, binContainer_accessBin(bc, i) == 0);
  }
  maf_destroyMafLineList(ml1);
  maf_destroyMafLineList(ml2);
  stHash_destruct(seq1Hash);
  stHash_destruct(seq2Hash);
  stHash_destruct(intervalsHash);
  binContainer_destruct(bc);
}

CuSuite* pairCoverage_TestSuite(void) {
  CuSuite* suite = CuSuiteNew();
  (void) BinContents;
  (void) displaySortedSet;
  (void) test_is_wild_0;
  (void) test_searchMatched_0;
  (void) test_compareLines_0;
  (void) test_compareLines_1;
  (void) test_compareLines_region_0;
  (void) test_intervalCheck_0;
  (void) test_binning_0;
  (void) test_binning_1;
  (void) test_binning_2;
  (void) test_binning_3;
  (void) test_binning_4;
  (void) test_binning_5;
  (void) test_binning_6;
  (void) test_binning_7;
  (void) test_binning_8;
  (void) test_binning_9;
  (void) test_binning_10;
  (void) test_binning_11;
  (void) test_binning_12;
  SUITE_ADD_TEST(suite, test_is_wild_0);
  SUITE_ADD_TEST(suite, test_searchMatched_0);
  SUITE_ADD_TEST(suite, test_compareLines_0);
  SUITE_ADD_TEST(suite, test_compareLines_1);
  SUITE_ADD_TEST(suite, test_intervalCheck_0);
  SUITE_ADD_TEST(suite, test_compareLines_region_0);
  SUITE_ADD_TEST(suite, test_binning_0);
  SUITE_ADD_TEST(suite, test_binning_1);
  SUITE_ADD_TEST(suite, test_binning_2);
  SUITE_ADD_TEST(suite, test_binning_3);
  SUITE_ADD_TEST(suite, test_binning_4);
  SUITE_ADD_TEST(suite, test_binning_5);
  SUITE_ADD_TEST(suite, test_binning_6);
  SUITE_ADD_TEST(suite, test_binning_7);
  SUITE_ADD_TEST(suite, test_binning_8);
  SUITE_ADD_TEST(suite, test_binning_9);
  SUITE_ADD_TEST(suite, test_binning_10);
  SUITE_ADD_TEST(suite, test_binning_11);
  SUITE_ADD_TEST(suite, test_binning_12);
  return suite;
}
