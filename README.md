# MERLOS

MERLOS is a program that MERge the LOcal Solutions to a global solution divided by domain decomposition.

Required files includes a global to local element map 'GlobalToLocalMap.dat' and solution files. The format of these files are as follows.

1. File format of GlobalToLocalMap.dat
  * No header
  * First column: Rank of the *m*-th element, where *m* is the *m*-th line of the file
  * Second column: Local ID of the *m*-th element
  * Third column: Global ID of the *m*-th element

2. File format of solution file
  * No header
  * *m*-th line of the file contains the values of the *m*-th element
