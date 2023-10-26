TARGET = dbscratch
BUILD_TYPE ?= Debug # Debug or Release
BUILD_DIR ?= cmake-build-debug

build:
	echo "start building...($(BUILD_TYPE))"
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)
	cd $(PWD)/$(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(PWD)
	cmake --build $(PWD)/$(BUILD_DIR)

run: build
	./bin/$(TARGET)
