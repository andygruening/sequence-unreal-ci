#!/bin/sh

/home/ue4/UnrealEngine/Engine/Build/BatchFiles/RunUAT.sh BuildPlugin \
  -Plugin=/sequence/Plugins/SequencePlugin/SequencePlugin.uplugin \
  -Package=/sequence/Package \
  -CreateSubFolder \
  -nocompile \
  -nocompileuat

cat "/root/Library/Logs/Unreal Engine/LocalBuildLogs/UBA-UnrealEditor-Linux-Development.txt"

touch ./shared/flag
