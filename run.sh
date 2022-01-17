mkdir -p bin

gcc -g main.c -l SDL2 -l SDL2_ttf -o bin/main

bin/main /mnt/chromeos/fonts/roboto/Roboto-Regular.ttf
