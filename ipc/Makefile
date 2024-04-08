build: ipcwriter.c ipcwriter.h
	mkdir build
	cmake -S . -B build
	cmake --build build
	mv build/libipcwriter.so ../lua/richpresence/ipcwriter.so
clean:
	rm schema.pb.c schema.pb.h
	rm -r build
	rm ../lua/richpresence/ipcwriter.so
