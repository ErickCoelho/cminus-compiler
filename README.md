# Compiler-Project

Results will be stored on results folder

Intermediate codes will be stored on codes folder

### Run C- compiler
```
./semanticanalyser.sh
```


Or run each compiled file separately:
### Generate Flex

```
flex cminus.l
```

### Generate .o files

```
gcc -c lex.yy.c main.c util.c
```

### Create Lexical Analyser:

```
gcc -o cminus *.o -ll
```

### Run test Programs

```
mkdir -p results
./cminus testfiles/mdc.c > results/mdc.txt
./cminus testfiles/sort.c > results/sort.txt
```
