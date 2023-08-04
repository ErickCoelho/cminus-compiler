bison -d cminus.y &&
flex cminus.l &&
gcc -c *.c &&
gcc -o cminus *.o -ll &&
./deletetests.sh &&
mkdir -p results &&
./cminus testfiles/mdc.c > results/mdc.txt &&
./cminus testfiles/sort.c > results/sort.txt &&
./cminus testfiles/simple.c > results/simple.txt &&
./cminus testfiles/void.c > results/void.txt &&
./cminus testfiles/nomain.c > results/nomain.txt &&
./cminus testfiles/varvoid.c > results/varvoid.txt &&
mkdir -p codes &&
mv testfiles/*.cmm codes &&
rm -f cminus lex.yy.c *.o cminus.tab.*
