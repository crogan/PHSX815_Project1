# C++ compiler
CXX = g++ 

# necessary compiler flags for using ROOT (root.cern.ch) - remove these if you're not using root
ROOTCFLAGS    = $(shell root-config --cflags)
ROOTGLIBS     = $(shell root-config --glibs)

# Path to the folder with the Python shared library (for matplotliib-cpp)
GLIBS =  -L/usr/local/Cellar/python\@3.9/3.9.1_6/Frameworks/Python.framework/Versions/3.9/lib/
# Flag to let the compiler know which shared library to use (for matplotlib-cpp)
GLIBS += -lpython3.9
# ROOT shared library flags
GLIBS += $(filter-out -stdlib=libc++ -pthread , $(ROOTGLIBS))

# some compiler flags
CXXFLAGS = -std=c++11
# ROOT flags
CXXFLAGS += -fPIC $(filter-out -stdlib=libc++ -pthread , $(ROOTCFLAGS))

# location of source code
SRCDIR = ./src/

#location of header files
INCLUDEDIR = ./include/

CXXFLAGS += -I$(INCLUDEDIR)

# location of object files (from compiled library files)
OUTOBJ = ./obj/

CC_FILES := $(wildcard src/*.cc)
HH_FILES := $(wildcard include/*.hh)
OBJ_FILES := $(addprefix $(OUTOBJ),$(notdir $(CC_FILES:.cc=.o)))

# targets to make
all: CookieTimer.x CookieAnalysis.x CookieHypoTest.x

# recipe for building CookieTimer.x
CookieTimer.x:  $(SRCDIR)CookieTimer.C $(OBJ_FILES) $(HH_FILES)
	$(CXX) $(CXXFLAGS) -o CookieTimer.x $(OUTOBJ)/*.o $(GLIBS) $ $<
	touch CookieTimer.x

# recipe for building CookieAnalysis.x
CookieAnalysis.x:  $(SRCDIR)CookieAnalysis.C $(OBJ_FILES) $(HH_FILES)
	$(CXX) $(CXXFLAGS) -o CookieAnalysis.x $(OUTOBJ)/*.o $(GLIBS) $ $<
	touch CookieAnalysis.x

# recipe for building CookieHypoTest.x
CookieHypoTest.x:  $(SRCDIR)CookieHypoTest.C $(OBJ_FILES) $(HH_FILES)
	$(CXX) $(CXXFLAGS) -o CookieHypoTest.x $(OUTOBJ)/*.o $(GLIBS) $ $<
	touch CookieHypoTest.x

$(OUTOBJ)%.o: src/%.cc include/%.hh
	$(CXX) $(CXXFLAGS) -c $< -o $@

# clean-up target (make clean)
clean:
	rm -f *.x
	rm -rf *.dSYM
	rm -f $(OUTOBJ)*.o
