CXX=g++
LD=ld
CXXFLAGS=-std=c++17 -c -g
LDFLAGS=-macosx_version_min 10.13 -lLimeSuite -lc -lc++

all: lime_juice

src/%.o: src/%.cc src/lime_juice.h
	$(CXX) $(CXXFLAGS) -o $@ $<

lime_juice: src/main.o src/lime_juice_impl.o
	$(LD) src/main.o src/lime_juice_impl.o -o $@ $(LDFLAGS)

clean:
	rm -f lime_juice src/*.o
