proc main.pc
gcc -o a.out -I$ORACLE_HOME/precomp/public -L$ORACLE_HOME/lib -lclntsh main.c
