gcc -o gen-ascii-table gen-ascii-table.c
./gen-ascii-table   | tee ascii8.txt
./gen-ascii-table 2 | tee utf8-80-ff.txt
