#!/bin/sh

/home/ue4/UnrealEngine/Engine/Build/BatchFiles/RunUAT.sh BuildPlugin \
  -Plugin=/SequencePlugin/Plugins/SequencePlugin/SequencePlugin.uplugin \
  -Package=/SequencePlugin/Package \
  -CreateSubFolder \
  -nocompile \
  -nocompileuat

touch ./shared/flag