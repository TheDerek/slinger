.PHONY: run
run: build
	./build/slinger

.PHONY: build
build:
	cd ./build && cmake ../ && make -j 6
