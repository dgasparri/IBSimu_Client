CC = g++-10 -std=c++20
LDFLAGS = `pkg-config --libs ibsimu-1.0.6dev` -lboost_program_options
CXXFLAGS = -lboost_program_options -Wall -g `pkg-config --cflags ibsimu-1.0.6dev` 


all: bin/build/analysis.o bin/build/simulation.o bin/build/config.o bin/build/config-setup.o bin/build/output.o bin/build/beam.o
	$(CC) -o bin/simulation bin/build/config.o bin/build/config-setup.o bin/build/output.o bin/build/beam.o bin/build/simulation.o $(LDFLAGS)
	$(CC) -o bin/analysis bin/build/config.o bin/build/config-setup.o bin/build/output.o bin/build/analysis.o $(LDFLAGS)

# alla fine 

bin/build/config.o: src/config.cpp src/config.h  src/datatype.h src/output.cpp src/output.h
	$(CC) $(CXXFLAGS) -c -o bin/build/config.o src/config.cpp

bin/build/config-setup.o: src/config-setup.cpp src/config-setup.h src/beam.h src/datatype.h
	$(CC) $(CXXFLAGS) -c -o bin/build/config-setup.o src/config-setup.cpp

bin/build/output.o: src/output.cpp src/output.h
	$(CC) $(CXXFLAGS) -c -o bin/build/output.o src/output.cpp

bin/build/beam.o: src/beam.cpp src/beam.h src/datatype.h
	$(CC) $(CXXFLAGS) -c -o bin/build/beam.o src/beam.cpp

bin/build/simulation.o: src/simulation.cpp src/datatype.h
	$(CC) $(CXXFLAGS) -c -o bin/build/simulation.o src/simulation.cpp

bin/build/analysis.o: src/analysis.cpp src/datatype.h
	$(CC) $(CXXFLAGS) -c -o bin/build/analysis.o src/analysis.cpp


# ecr: ecr.o
# 	$(CC) -o ecr ecr.o $(LDFLAGS)
# ecr.o: ecr.cpp

# analysis: analysis.o
# 	$(CC) -o analysis analysis.o $(LDFLAGS)
# analysis.o: analysis.cpp

# clean:
# 	$(RM) *.o analysis ecr 

