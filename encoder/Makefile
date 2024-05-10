build: encoder.c encoder.h
	mkdir build
	cmake -S . -B build
	cmake --build build
	mv build/libencoder.so ../lua/RPC/encoder.so
clean:
	rm -r build
	rm ../lua/RPC/encoder.so
