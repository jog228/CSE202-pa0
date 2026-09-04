#!/bin/bash

echo -e "\nRunning make clean"
make -f makefile clean
echo -e "\nRunning make prog0"
make
rc=$?
total=0
if [[ $rc == 0 ]]; then
  tests=( "even AAAAAAAA"
          "even 55555555"  
          "left 7f800000"
          "left 00340000"
          "rrotate 7F400000 2"
          "rrotate 7F400000 4"
          "rrotate 7F400000 5"
          "rrotate 7F400000 10"
          "rrotate 7F400000 16"
          "lrotate 7F400000 2"
          "lrotate 7F400000 4"
          "lrotate 7F400000 5"
          "lrotate 7F400000 10"
          "lrotate 7F400000 16"
          "saturate 7fffffff 00000010"
          "saturate C000FFFF 50000000"
          "saturate 000000ff 0000ff00"
          "saturate 7f000000 8fff0000"
          "saturate 84000000 a500ffff"
          "saturate d000ffff cfff0000"
          "twice 3f400000"
          "twice 007fffff"
          "twice 7f000000"
          "twice ff800000"
          "twice ff800777"
          "half 3fc00000"
          "half 00E00000"
          "half ff800000"
          "half 7f800777"
          "add 7fffffff"
          "even 200gffff"
          "rrotate ffff0000 33"
        )

  for test in "${tests[@]}"; do
    total=$((total + 1))
    echo -e "\nTest $total with args: $test"
    ./prog0 $test | tee -a tests.out
  done
paste -d '|' tests.out tests.reference | grep -Po '^(.*)\|+\K\1$' > out
#grep -f tests.out tests.reference > out
count=0
input="./out"
while IFS= read -r line
do
  count=$((count + 1))
done < "$input"

if [[ $count == 80 ]]; then
   echo -e "\nAll tests passed"
   echo -e "Score: 80/80\n"
   exit 0
else
   score=$((count * 2 + count / 2))
   echo -e "\n$count out of 32 passed"
   echo -e "Score: $score/80\n"
   exit 1
fi

fi
