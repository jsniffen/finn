mkdir -p bin

gcc -g main.c `sdl2-config --cflags --libs` -lSDL2_ttf -o bin/main

bin/main /mnt/chromeos/fonts/roboto/Roboto-Regular.ttf
