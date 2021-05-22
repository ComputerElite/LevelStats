# Builds a .qmod file for loading with QP
& $PSScriptRoot/build.ps1

Compress-Archive -Path "./libs/arm64-v8a/libLevelStats.so", ".\mod.json" -DestinationPath "./LevelStats.zip" -Update
Remove-Item "./LevelStats.qmod"
Rename-Item "./LevelStats.zip" "./LevelStats.qmod"