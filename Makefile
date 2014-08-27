SOURCES=Root_draw_hist.cpp STAP_read.cpp
OBJECTS=$(SOURCES:.cpp=.o)
CXXFLAGS=`root-config --cflags` -c -Wall -std=c++11 
LIBS=`root-config --libs` -lMinuit -lboost_program_options
OUT=Root_histogram.out
CC=g++
all : $(SOURCES) $(OUT)

$(OUT) :  $(OBJECTS)
		g++ $(OBJECTS) $(LIBS) -o $@
		
.cpp.o :  $(SOURCES)
		  $(CC) $(CXXFLAGS) $< -o $@
clean :
		rm -f *.o rm *.so* *.out

