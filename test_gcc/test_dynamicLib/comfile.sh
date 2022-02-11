gcc -c mymath.c -fPIC	
gcc -shared -o libmymath.so mymath.o
gcc -o a.out main.c -ldl

# line 1~2 : so 颇老 积己
# line 3   : 角青 颇老 积己   
