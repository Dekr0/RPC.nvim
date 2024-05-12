build: pack.c
	mkdir build
	cmake -S . -B build
	cmake --build build
	mv build/libpack.so ../lua/pack.so
clean:
	rm -r build
	rm ../lua/pack.so
