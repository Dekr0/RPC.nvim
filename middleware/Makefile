build: middleware.c middleware.h lib/x86_64/discord_game_sdk.so include/discord_game_sdk.h
	mkdir build 
	cmake -S . -B build 
	cmake --build build 
	cmake --install build
clean:
	rm -r build
	rm -r ${HOME}/.local/share/nvim/richpresence.nvim

