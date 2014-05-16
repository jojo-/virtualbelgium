# -------------------------------------
# Makefile for building vbelgium
#
# Authors: J.Barthelemy and L. Hollaert
# Date   : 17 july 2012  
# -------------------------------------

export CXX            = mpicxx 
export CXXFLAGSDEBUG  = -Wall -O0 -ggdb -pg -std=c++0x -D DEBUGVB -Wall
export CXXFLAGS       = -Wall -O2 -DNDEBUG -march='native' -std=c++0x
export CXXFLAGSUCL    = -Wall -O2 -DNDEBUG -march='native' 
export EXEC_NAME      = vbel

SRC_DIR   = ./src/
BIN_DIR   = ./bin/
OUT_DIR   = ./output/

all :
	@(cd $(SRC_DIR) && $(MAKE))

debug : CXXFLAGS = $(CXXFLAGSDEBUG)
debug :
	@(cd $(SRC_DIR) && $(MAKE) debug)

ucl : CXXFLAGS = $(CXXFLAGSUCL)
ucl :
	@(cd $(SRC_DIR) && $(MAKE) ucl)

clean :
	@rm $(SRC_DIR)*.o $(BIN_DIR)$(EXEC_NAME)

clean_output :
	@rm $(OUT_DIR)* 

#NETCDF_INCLUDE=/soft/apps/netcdf-3.6.2/include/
