call vcvarsall.bat

vcbuild.exe /rebuild /M%NUMBER_OF_PROCESSORS% "..\..\src\TightVNC.sln" "Release|Win32"
vcbuild.exe /rebuild /M%NUMBER_OF_PROCESSORS% "..\..\src\TightVNC.sln" "Release|x64"

