CC = g++-10 -std=c++20
LDFLAGS = `pkg-config --libs ibsimu-1.0.6dev` -lboost_program_options
CXXFLAGS = -lboost_program_options -Wall -g `pkg-config --cflags ibsimu-1.0.6dev`
SRCDIR = src
OBJDIR = bin/build
#OBJECTS = $(OBJDIR)/%.o
OBJECTS = bin/build/config.o bin/build/config-setup.o bin/build/output.o \
          bin/build/beam.o $(OBJDIR)/output_console.o



all: $(OBJECTS) src/datatype.h
	$(CC) $(CXXFLAGS) -c -o $(OBJDIR)/simulation.o $(SRCDIR)/simulation.cpp
	$(CC) -o bin/simulation $(OBJDIR)/simulation.o $(OBJECTS) $(LDFLAGS)
	$(CC) $(CXXFLAGS) -c -o $(OBJDIR)/analysis.o $(SRCDIR)/analysis.cpp
	$(CC) -o bin/analysis $(OBJDIR)/analysis.o $(OBJECTS) $(LDFLAGS)

# alla fine 

bin/build/config.o: src/config.cpp src/config.h  src/datatype.h src/output.cpp src/output.h
	$(CC) $(CXXFLAGS) -c -o bin/build/config.o src/config.cpp

bin/build/config-setup.o: src/config-setup.cpp src/config-setup.h src/beam.h src/datatype.h
	$(CC) $(CXXFLAGS) -c -o bin/build/config-setup.o src/config-setup.cpp

bin/build/output.o: src/output.cpp src/output.h
	$(CC) $(CXXFLAGS) -c -o bin/build/output.o src/output.cpp

$(OBJDIR)/output_console.o: $(SRCDIR)/output_console.cpp $(SRCDIR)/output_console.h
	$(CC) $(CXXFLAGS) -c -o $(OBJDIR)/output_console.o $(SRCDIR)/output_console.cpp

bin/build/beam.o: src/beam.cpp src/beam.h src/datatype.h
	$(CC) $(CXXFLAGS) -c -o bin/build/beam.o src/beam.cpp

#bin/build/simulation.o: src/simulation.cpp src/datatype.h
#	$(CC) $(CXXFLAGS) -c -o bin/build/simulation.o src/simulation.cpp

#bin/build/analysis.o: src/analysis.cpp src/datatype.h
#	$(CC) $(CXXFLAGS) -c -o bin/build/analysis.o src/analysis.cpp


.PHONY : clean
clean:
	$(RM) bin/analysis bin/simulation $(OBJDIR)/*.o

