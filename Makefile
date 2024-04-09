init: 
	curl https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip --output discord_game_sdk.zip
	unzip discord_game_sdk.zip -x "cpp/*" "csharp/*" "examples/*" "README.md"
	mv c sdk/include
	mv lib sdk/
	rm discord_game_sdk.zip
