# mafExtractor

14 Feb 2012

## Author

[Dent Earl](https://github.com/dentearl/)

## Description
mafExtractor is a program that will look through a maf file for a particular sequence name and region. If a match is found then the block containing the querry will be printed to standard out. By default blocks are trimmed such that only columns that contain the targeted sequence region are included. Use <code>--soft</code> to include an entire block if any part of the block falls within the targeted region.

__BE AWARE!__ At present mafExtractor doesn't handle maf lines of type <code>e</code>, <code>q</code>, or <code>i</code>. The <code>s</code> lines will be properly processed but these other types of lines will be ignored which could lead to inconsistent data and confusion.

## Installation
1. Download the package.
2. <code>cd</code> into the directory.
3. Type <code>make</code>.

## Use
<code>mafExtractor --seq [sequence name (and possibly chr)] --pos [position to search for] [options] < myFile.maf</code>

### Options
* <code>-h, --help</code>   show this help message and exit.
* <code>-s, --seq</code>   sequence _name.chr_ e.g. `hg18.chr2'.
* <code>--start</code>   start of the region, inclusive. Must be a positive number.
* <code>--stop</code>   end of the region, inclusive. Must be a positive number.
* <code>--soft</code>   include entire block even if it has gaps or over-hangs. default=false.
* <code>-v, --verbose</code>   turns on verbose output.

## Example
    $ ./mafBlockExractor --seq hg19.chr20 --start 500 --stop 1000 < example.maf 
    ##maf version=1 
    
    #a score=0 pctid=99.2
    #s hg19.chr20 0 795 + 73767698 GAT...
    ...

