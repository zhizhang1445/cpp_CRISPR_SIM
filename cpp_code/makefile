
CXX=g++
CXXFLAGS=-std=c++17 -O2 -g
LDFLAGS=-O2 -g
LDLIBS=-lgsl -lcblas -lstdc++fs -fopenmp
all: ../CRISPR_wave_main
test: main_test

OBJFILES=crispr_dyn.o init.o io.o

../CRISPR_wave_main: main.o $(OBJFILES) update_rules.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

CRISPR_wave_main.o: main.cpp parameter.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# ../CRISPR_wave_main_omp: omp_main.o $(OBJFILES) omp_update_rules.o
# 	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

CRISPR_wave_main_omp.o: omp_main.cpp parameter.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

crispr_dyn.o: crispr_dyn.cpp crispr_dyn.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

io.o: io.cpp io.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

update_rules.o: update_rules.cpp update_rules.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# omp_update_rules.o: omp_update_rules.cpp omp_update_rules.h
# 	$(CXX) -c $(CXXFLAGS) -o $@ $<

main_test: main.cpp io.cpp update_rules.cpp init.cpp convolution.cpp 
	g++ -std=c++17 -o main_test main.cpp io.cpp init.cpp crispr_dyn.cpp update_rules.cpp convolution.cpp -lgsl -lcblas -lstdc++fs #test

clean:
	$(RM) *.o ../CRISPR_wave_main ../CRISPR_wave_main_omp

npy_clean:
	$(RM) *.npy

.Phony: all clean