cd "$(dirname "$0")"
echo "Recompiling..."
cd ../c
g++ -F../../common -I./i ./SDLMain.m libqbx_lnx.o  qbx.cpp -framework Cocoa -framework SDL -framework SDL_mixer -framework mikmod -framework smpeg -framework SDL_net -framework SDL_ttf -framework SDL_image ../../common/freetype/libfreetype.dylib -o '../../PERPLEX'
read -p "Press ENTER to exit..."
