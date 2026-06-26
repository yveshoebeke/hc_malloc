.PHONY: all clean

all:
	cmake -B build -S .
	cmake --build build

	sudo cmake --install build

clean:
	rm -rf build

