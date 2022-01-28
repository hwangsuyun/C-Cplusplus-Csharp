ar -rscv kid_lib.a $PWD/o/*.o
gcc -c main.c
gcc -o a.out main.o kid_lib.a
rm -rf main.o
