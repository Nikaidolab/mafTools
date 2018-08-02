# mafTools

**mafTools** is a collection of tools that operate on Multiple Alignment Format ([maf](http://genome.ucsc.edu/FAQ/FAQformat.html#format5)) files.

## Authors
[Dent Earl](https://github.com/dentearl/), [Benedict Paten](https://github.com/benedictpaten/), [Mark Diekhans](https://github.com/diekhans)

## Dependencies
With the exception of the python dependencies, when a component is missing a dependency it will not be built, tested or cleaned by the Makefile. If the python dependencies are missing then some of the modules will fail to function and all of the modules' tests will fail. The <code>sonLib</code> and <code>pinchesAndCacti</code> dependencies should be built and placed in the same parent directory as <code>mafTools</code>.
* [python 2.7](http://www.python.org/): all modules.
   * [scipy](http://www.scipy.org/)
   * [numpy](http://numpy.scipy.org/)
* [sonLib](https://github.com/benedictpaten/sonLib/): mafComparator, mafStats, mafTransitiveClosure, mafToFastaStitcher, mafPairCoverage.
* [pinchesAndCacti](https://github.com/benedictpaten/pinchesAndCacti): mafTransitiveClosure.

## Installation
0. Install dependencies.
1. Download or clone the <code>mafTools</code> package. Consider making it a sibling directory to <code>sonLib/</code> and <code>pinchesAndCacti</code>.
2. <code>cd</code> into <code>mafTools</code> directory.
3. Type <code>make</code>.

## Components
* **mafComparator** A program to compare two maf files by sampling. Useful when testing predicted alignments against known true alignments.
* **mafCoverage** A program to calculate the amount of alignment coverage between a target sequence and all other sequences in a maf file.
* **mafDuplicateFilter** A program to filter alignment blocks to remove duplicate species. One sequence per species is allowed to remain, chosen by comparing the sequence to the consensus for the block and computing a similarity bit score between the IUPAC formatted consensus and the sequence. The highest scoring duplicate stays, or in the case of ties, the sequence closest to the start of the file stays.
* **mafExtractor** A program to extract all alignment blocks that contain a region in a particular sequence. Useful for isolating regions of interest in large maf files.
* **mafFilter** A program to filter a maf based on sequence names. Can be used to include or exclude sequence names. Useful for removing extraneous sequences from maf files.
* **mafPairCoverage** A program to compare the number of aligned positions between any pair of sequences within a maf file. Can use the * wildcard character to specify a species name. Can use a BED file to limit region of inspection to just intervals specified in the bed. Outputs total lengths of sequencs, number of aligned positions, percent coverage and in the case where a bed file was specified the number of bases within and outside of the region.
* **mafPositionFinder** A program to search for a position in a particular sequence. Useful for determining where in maf a particular part of the alignment resides.
* **mafRowOrderer** A program to order maf lines within blocks. Useful for moving a reference species to the top of all blocks. Species not specified in the ordering are automatically trimmed from the results.
* **mafSorter** A program to sort all of the blocks in a MAF based on the (absolute) start position of one of the sequences. Blocks without the sequence are placed at the start of the output in their original order.
* **mafStats** A program to read a maf file and report back summary statistics about the file contents.
* **mafStrander** A program to enforce, when possible, a particular strandedness for blocks for a given species and strand orientation.
* **mafToFastaStitcher** A program to convert a reference-based MAF file to a multiple sequence fasta. Requires both a .maf and a fasta containing complete sequences for all entries in the maf.
* **mafTransitiveClosure** A program to perform the transitive closure on an alignment. That is it checks every column of the alignment and looks for situations where a position A is aligned to B in one part of a file and B is aligned to C in another part of the file. The transitive closure of this relationship would be a single column with A, B and C all present. Useful for when you have pairwise alignments and you wish to turn them into something more resembling a multiple alignment.
* **mafValidator** A program to assess whether or not a given maf file's formatting is valid.

## External tools
* mafTools internal tests use Asim Jalis' [CuTest](http://cutest.sourceforge.net/) C unit testing framework (included in <code>external/</code>). The license for CuTest is spelled out in external/license.txt.
* mafTools internal tests will use [valgrind](http://www.valgrind.org/) __if__ installed on your system.

## How to Cite:
Genome Res. 2014 Dec;24(12):2077-89. doi: 10.1101/gr.174920.114. Epub 2014 Oct 1.
Alignathon: a competitive assessment of whole-genome alignment methods.
