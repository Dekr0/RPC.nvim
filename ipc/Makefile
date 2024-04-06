all:
	python nanopb/generator/nanopb_generator.py schema.proto
	mkdir build
	cmake -S . -B build
	cmake --build build
	mv build/libipcwriter.so ../lua/richpresence/ipcwriter.so
clean:
	rm -r build
	rm schema.pb.c schema.pb.h
	rm ../lua/richpresence/ipcwriter.so
