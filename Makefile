init: .venv ipc/nanopb middleware/nanopb
	curl https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip --output discord_game_sdk.zip
	unzip discord_game_sdk.zip -x "cpp/*" "csharp/*" "examples/*" "README.md"
	mv c middleware/include
	mv lib middleware/
	rm discord_game_sdk.zip
build: .venv ipc/nanopb middleware/nanopb ipc/build middleware/build
	. .venv/bin/activate
.venv:
	python -m venv .venv
	. .venv/bin/activate && pip install protobuf grpcio-tools && deactivate
ipc/nanopb:
	ln -s "${PWD}/nanopb" "ipc/"
middleware/nanopb:
	ln -s "${PWD}/nanopb" "middleware/"
ipc/build: .venv
	. .venv/bin/activate && cp CMakeLists.txt.dynamic nanopb/CMakeLists.txt && cd ipc && make && deactivate
middleware/build:
	. .venv/bin/activate && cp CMakeLists.txt.static nanopb/CMakeLists.txt && cd middleware && make && deactivate
clean-init:
	rm -r .venv
	rm ipc/nanopb
	rm middleware/nanopb
	rm -r middleware/include
	rm -r middleware/lib
clean-build:
	cd ipc && make clean
	cd middleware && make clean
clean-ipc:
	cd ipc && make clean
clean-middleware:
	cd middleware && make clean

