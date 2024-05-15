build: uuid.c
	mkdir build
	cmake -S . -B build
	cmake --build build
	mv build/libuuid.so ../lua/uuid.so
clean:
	rm -r build
	rm ../lua/uuid.so
