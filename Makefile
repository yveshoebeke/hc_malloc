.PHONY: all clean

all:
	cmake -B build -S .
	cmake --build build

	cmake --install build --component UserFiles
	sudo cmake --install build --component SystemFiles

clean:
	rm -rf build

