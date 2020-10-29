.PHONY: run
run: build
	./build/src/slinger

.PHONY: build
build:
	./install-requirements.sh
	mkdir -p build
	cd ./build && cmake ../ && make -j 6

.PHONY: clean
clean:
	rm -rf ./build/
