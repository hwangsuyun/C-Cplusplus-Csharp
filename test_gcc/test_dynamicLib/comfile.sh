gcc -c mymath.c -fPIC	
gcc -shared -o libmymath.so mymath.o
gcc -o a.out main.c -ldl

# line 1~2 : so ���� ����
# line 3   : ���� ���� ����   
