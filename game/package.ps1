Set-Location ..

$buildDir = "dist"
$mingwDir = "C:\DEV\mingw64\bin"

New-Item -ItemType Directory -Path "$buildDir" -Force

# Copiar DLLs
Copy-Item "build\*.dll" -Destination $buildDir
Copy-Item "build\*.exe" -Destination $buildDir
Copy-Item "$mingwDir\libwinpthread-1.dll" -Destination $buildDir
Copy-Item "$mingwDir\libgcc_s_seh-1.dll" -Destination $buildDir
Copy-Item "$mingwDir\libstdc++-6.dll" -Destination $buildDir

# Copiar assets
New-Item -ItemType Directory -Path "$buildDir\assets" -Force
Copy-Item "assets" -Destination "$buildDir" -Recurse -Force