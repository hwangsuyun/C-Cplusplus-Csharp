gcc -shared -o kid_lib.so $PWD/o/*.o
gcc -c -fPIC main.c
gcc -o a.out main.o kid_lib.so
rm -rf main.o
