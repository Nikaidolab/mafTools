/* 
 * Copyright (C) 2009-2013 by 
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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "CuTest.h"
#include "comparatorAPI.h"
#include "comparatorRandom.h"

int main(int argc, char **argv) {
    if (argc == 5) {
        st_randomSeed(atoi(argv[4]));
    } else if (argc == 4) {
        st_randomSeed(time(NULL));
    } else {
        fprintf(stderr, "Usage: %s numberOfSamples n p [optional: randomSeed]\n", argv[0]);
        return EXIT_FAILURE;
    }
    uint64_t numSamples = atoi(argv[1]);
    uint64_t n = atoi(argv[2]);
    double p = atof(argv[3]);
    for (uint64_t i = 0; i < numSamples; ++i) {
        printf("%" PRIu64 "\n", rbinom(n, p));
    }
    return EXIT_SUCCESS;
}
