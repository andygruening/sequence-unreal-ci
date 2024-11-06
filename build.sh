#!/bin/sh

/home/ue4/UnrealEngine/Engine/Build/BatchFiles/RunUAT.sh BuildCookRun -project=/SequencePlugin/SequenceUnreal.uproject -nop4 -clientconfig=Development -serverconfig=Development -unrealexe=/home/ue4/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe -utf8output -platform=Linux -targetplatform=Linux -build -cook -map=Default.umap -cookdir=/Packaged/ -skipcookingeditorcontent -unversionedcookedcontent -pak -distribution -compressed -stage -package -stagingdirectory="%~dp0Dist"

