build: sdk.c sdk.h
	mkdir build
	cmake -S . -B build
	cmake --build build
	mv build/libsdk.so ../lua/richpresence/sdk.so
clean:
	rm -r build
	rm ../lua/richpresence/sdk.so
