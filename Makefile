.PHONY: run
run: build
	./build/slinger

.PHONY: build
build:
	cd ./build && cmake ../ && make -j 6

.PHONY: vcpkg
vcpkg:
	cmake -B ./build/ -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
	cmake --build ./build -j 10


.PHONY: clean
clean:
	rm -rf ./build/
