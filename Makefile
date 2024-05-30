all:
	cd pack && make
	cd uuid4 && make
clean:
	rm lua/pack.so
	rm lua/uuid.so
