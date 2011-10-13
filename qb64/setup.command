cd "$(dirname "$0")"
echo "QB64 Setup"
echo ""
echo "Before installing QB64, you must manually install 'Xcode' from Apple's website."
echo "Use this link:"
echo "http://developer.apple.com/technologies/tools/xcode.html"
echo ""
read -p "Press ENTER to continue..."
echo ""
echo "Checking for existence of 'g++', Xcode's C++ compiler"
echo "****************************************"
g++
echo "****************************************"
echo "The above line should contain 'g++: no input files' or something similar. If it doesn't then you need to install Xcode before continuing."
echo ""
read -p "Press ENTER to continue..."
echo ""
echo "Identifying compliant FreeType library..."
echo "(Note: Several FreeType library builds may be tested for compliance, any errors from this test process should be ignored)"
cd ./internal/c
g++ -F../../common -I./i ./SDLMain.m freetype_buildA_detect.cpp -framework Cocoa -framework SDL -framework SDL_ttf ../../common/freetype/buildA/libfreetype.dylib -o ../../freetype_buildA_detect
cd ../..
./freetype_buildA_detect
rm freetype_buildA_detect
cd ./internal/c
g++ -F../../common -I./i ./SDLMain.m freetype_buildB_detect.cpp -framework Cocoa -framework SDL -framework SDL_ttf ../../common/freetype/buildB/libfreetype.dylib -o ../../freetype_buildB_detect
cd ../..
./freetype_buildB_detect
rm freetype_buildB_detect
echo ""
echo "Creating 'qb64'..."
echo ""
echo "(Notes: Warnings can be ignored. This process could take 3-10 minutes.)"
cp ./internal/source/* ./internal/temp/
cd ./internal/c
g++ -c -w -Wall libqbx.cpp -I./i -o libqbx_lnx.o
g++ -F../../common -I./i ./SDLMain.m libqbx_lnx.o  qbx.cpp -framework Cocoa -framework SDL -framework SDL_mixer -framework mikmod -framework smpeg -framework SDL_net -framework SDL_ttf -framework SDL_image ../../common/freetype/libfreetype.dylib -o ../../qb64
cd ../..
echo ""
echo "Setup complete!"
echo ""
echo "Launching 'qb64'..."
./qb64
echo ""
echo "Note: 'qb64' is located in same folder as this setup file."
echo "      To start QB64 directly, double click on 'qb64_start.command', also"
echo "      located in this folder. From a terminal, QB64 can be started by"
echo "      typing './qb64'"
echo ""
read -p "Press ENTER to exit..."
