include inc/common.mk

python_version_full := $(wordlist 2,4,$(subst ., ,$(shell python --version 2>&1)))
python_version_major := $(word 1,${python_version_full})
python_version_minor := $(word 2,${python_version_full})
##############################
# These modules are dependent and are
# only included if their depedencies exist!
ifeq ($(wildcard ../sonLib/Makefile),)
	Comparator =
	TransitiveClosure =
	Stats =
	ToFasta =
	PairCoverage =
	Coverage =
$(warning Because dependency ../sonLib is missing mafComparator, mafTransitiveClosure, mafStats, mafToFastaStitcher, mafPairCoverage, mafCoverage will not be built / tested / cleaned. See README.md for information about dependencies.)
else
	Comparator = mafComparator
	Stats = mafStats
	ToFasta = mafToFastaStitcher
	PairCoverage = mafPairCoverage
	Coverage = mafCoverage
ifeq ($(wildcard ../sonLib/lib/stPinchesAndCacti.a),)
	TransitiveClosure =
$(warning Because dependency ../pinchesAndCacti is missing mafTransitiveClosure will not be built / tested / cleaned. See README.md for information about dependencies.)
else
	TransitiveClosure = mafTransitiveClosure
endif # sonlib
endif # pinches
##############################
dependentModules= ${Comparator} ${TransitiveClosure} ${Stats} ${ToFasta} ${PairCoverage} ${Coverage}

modules = lib ${dependentModules} mafValidator mafPositionFinder mafExtractor mafSorter mafDuplicateFilter mafFilter mafStrander mafRowOrderer

.PHONY: all %.all clean %.clean test %.test
.SECONDARY:

all: ${modules:%=%.all}

%.all:
	cd $* && make all

# python :
# 	@echo ${python_version_full}
# 	@echo ${python_version_major}
# 	@echo ${python_version_minor}

clean: ${modules:%=%.clean}

%.clean:
	cd $* && make clean

test: ${modules:%=%.test} ${Warnings:%=%.warn}
	@echo 'mafTools tests complete.'

%.test:
	cd $* && make test
