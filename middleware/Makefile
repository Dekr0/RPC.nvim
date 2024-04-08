build: schema.proto middleware.c middleware.h include/discord_game_sdk.h lib/x86_64/discord_game_sdk.so 
	mkdir build 
	cmake -S . -B build 
	cmake --build build 
	cmake --install build
clean:
	rm schema.pb.c schema.pb.h schema.proto
	rm -r build
	rm -r ${HOME}/.local/share/nvim/richpresence.nvim

