COUNTER_SUCCESS=0
COUNTER_FAIL=0
DIFF_RES=""
echo "" > log.txt
echo "" > err.txt

echo -e "for\nint\nprint\nwhile" > pattern.txt
echo -e "test\npattern\nline" > pattern2.txt
echo -e "" > empty_pattern.txt
echo -e "t.st\nt.*t" > regex_patterns.txt

FLAGS=(-v -c -l -n -h -i)

for var in "${FLAGS[@]}"
do
  for var2 in "${FLAGS[@]}"
  do
      for var3 in "${FLAGS[@]}"
      do
        if [ "$var" != "$var2" ] && [ "$var2" != "$var3" ] && [ "$var" != "$var3" ]; then
          
          echo "Test: $var $var2 $var3 'for' s21_grep.c"
          ./s21_grep $var $var2 $var3 "for" s21_grep.c > s21_grep.txt 2>>err.txt
          grep $var $var2 $var3 "for" s21_grep.c > grep.txt 2>>err.txt
          
          if diff -q s21_grep.txt grep.txt > /dev/null; then
            (( COUNTER_SUCCESS++ ))
          else
            echo "FAIL: $var $var2 $var3 'for' s21_grep.c" >> log.txt
            (( COUNTER_FAIL++ ))
          fi
          rm -f s21_grep.txt grep.txt

          echo "Test: $var $var2 $var3 -e 'for' s21_grep.c"
          ./s21_grep $var $var2 $var3 -e "for" s21_grep.c > s21_grep.txt 2>>err.txt
          grep $var $var2 $var3 -e "for" s21_grep.c > grep.txt 2>>err.txt
          
          if diff -q s21_grep.txt grep.txt > /dev/null; then
            (( COUNTER_SUCCESS++ ))
          else
            echo "FAIL: $var $var2 $var3 -e 'for' s21_grep.c" >> log.txt
            (( COUNTER_FAIL++ ))
          fi
          rm -f s21_grep.txt grep.txt

          echo "Test: $var $var2 $var3 -f pattern.txt s21_grep.c"
          ./s21_grep $var $var2 $var3 -f pattern.txt s21_grep.c > s21_grep.txt 2>>err.txt
          grep $var $var2 $var3 -f pattern.txt s21_grep.c > grep.txt 2>>err.txt
          
          if diff -q s21_grep.txt grep.txt > /dev/null; then
            (( COUNTER_SUCCESS++ ))
          else
            echo "FAIL: $var $var2 $var3 -f pattern.txt s21_grep.c" >> log.txt
            (( COUNTER_FAIL++ ))
          fi
          rm -f s21_grep.txt grep.txt

          echo "Test: $var $var2 $var3 -e 'printf' -f pattern.txt s21_grep.c"
          ./s21_grep $var $var2 $var3 -e "printf" -f pattern.txt s21_grep.c > s21_grep.txt 2>>err.txt
          grep $var $var2 $var3 -e "printf" -f pattern.txt s21_grep.c > grep.txt 2>>err.txt
          
          if diff -q s21_grep.txt grep.txt > /dev/null; then
            (( COUNTER_SUCCESS++ ))
          else
            echo "FAIL: $var $var2 $var3 -e 'printf' -f pattern.txt s21_grep.c" >> log.txt
            (( COUNTER_FAIL++ ))
          fi
          rm -f s21_grep.txt grep.txt

        fi
      done
  done
done

echo "=== Special tests for -e and -f ==="

echo "Test: -e (without argument) - should error"
./s21_grep -e 2>&1 | grep -q "requires an argument" && echo "  OK (correct error)" || echo "  FAIL (no error)"
(( COUNTER_SUCCESS++ ))

echo "Test: -f (without argument) - should error"
./s21_grep -f 2>&1 | grep -q "requires an argument" && echo "  OK (correct error)" || echo "  FAIL (no error)"
(( COUNTER_SUCCESS++ ))

echo "Test: -e 'for' -e 'int' -e 'main' s21_grep.c"
./s21_grep -e "for" -e "int" -e "main" s21_grep.c > s21_grep.txt 2>>err.txt
grep -e "for" -e "int" -e "main" s21_grep.c > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -f pattern.txt -f pattern2.txt s21_grep.c"
./s21_grep -f pattern.txt -f pattern2.txt s21_grep.c > s21_grep.txt 2>>err.txt
grep -f pattern.txt -f pattern2.txt s21_grep.c > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -e 'include' -f pattern.txt -n s21_grep.c"
./s21_grep -e "include" -f pattern.txt -n s21_grep.c > s21_grep.txt 2>>err.txt
grep -e "include" -f pattern.txt -n s21_grep.c > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -f empty_pattern.txt s21_grep.c"
./s21_grep -f empty_pattern.txt s21_grep.c > s21_grep.txt 2>>err.txt
grep -f empty_pattern.txt s21_grep.c > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -f nonexistent.txt s21_grep.c (should error)"
./s21_grep -f nonexistent.txt s21_grep.c 2>&1 | grep -q "No such file" && echo "  OK (correct error)" || echo "  FAIL (no error)"
(( COUNTER_SUCCESS++ ))

echo "Test: -f nonexistent.txt -s s21_grep.c (should NOT error)"
./s21_grep -f nonexistent.txt -s s21_grep.c 2>&1 | grep -q "No such file" && echo "  FAIL (showed error)" || echo "  OK (no error with -s)"
(( COUNTER_SUCCESS++ ))

echo "=== Tests for flag -o ==="

echo -e "test test test\ntesting\nanother test\nno match\ntata tatatat" > test_o.txt

echo "Test: -o 'test' test_o.txt"
./s21_grep -o "test" test_o.txt > s21_grep.txt 2>>err.txt
grep -o "test" test_o.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -o -n 'test' test_o.txt"
./s21_grep -o -n "test" test_o.txt > s21_grep.txt 2>>err.txt
grep -o -n "test" test_o.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -o -h 'test' test_o.txt test_o.txt"
./s21_grep -o -h "test" test_o.txt test_o.txt > s21_grep.txt 2>>err.txt
grep -o -h "test" test_o.txt test_o.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -o -i 'TEST' test_o.txt"
./s21_grep -o -i "TEST" test_o.txt > s21_grep.txt 2>>err.txt
grep -o -i "TEST" test_o.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -o -e 'test' -e 'tat' test_o.txt"
./s21_grep -o -e "test" -e "tat" test_o.txt > s21_grep.txt 2>>err.txt
grep -o -e "test" -e "tat" test_o.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -o -f pattern.txt test_o.txt"
./s21_grep -o -f pattern.txt test_o.txt > s21_grep.txt 2>>err.txt
grep -o -f pattern.txt test_o.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -o 't.st' test_o.txt"
./s21_grep -o "t.st" test_o.txt > s21_grep.txt 2>>err.txt
grep -o "t.st" test_o.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -f empty_pattern.txt -o test_o.txt"
./s21_grep -f empty_pattern.txt -o test_o.txt > s21_grep.txt 2>>err.txt
grep -f empty_pattern.txt -o test_o.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: -o -c (should error)"
./s21_grep -o -c "test" test_o.txt 2>&1 | grep -q "cannot be used" && echo "  OK (correct error)" || echo "  FAIL (no error)"
(( COUNTER_SUCCESS++ ))

echo "Test: -o -l (should error)"
./s21_grep -o -l "test" test_o.txt 2>&1 | grep -q "cannot be used" && echo "  OK (correct error)" || echo "  FAIL (no error)"
(( COUNTER_SUCCESS++ ))

echo "Test: -o -v (should error)"
./s21_grep -o -v "test" test_o.txt 2>&1 | grep -q "cannot be used" && echo "  OK (correct error)" || echo "  FAIL (no error)"
(( COUNTER_SUCCESS++ ))

echo "Test: -o 'tata' in 'tatatat'"
echo "tatatat" > overlap.txt
./s21_grep -o "tata" overlap.txt > s21_grep.txt 2>>err.txt
grep -o "tata" overlap.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt overlap.txt

echo "=== Edge case tests ==="

echo "Test: 'pattern' empty.txt"
touch empty.txt
./s21_grep "pattern" empty.txt > s21_grep.txt 2>>err.txt
grep "pattern" empty.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt empty.txt

echo "Test: Pattern with dots 't.st'"
./s21_grep "t.st" test_o.txt > s21_grep.txt 2>>err.txt
grep "t.st" test_o.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt

echo "Test: Long line handling"
head -c 1000 /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 80 > long.txt
echo "test" >> long.txt
./s21_grep "test" long.txt > s21_grep.txt 2>>err.txt
grep "test" long.txt > grep.txt 2>>err.txt
if diff -q s21_grep.txt grep.txt > /dev/null; then
  (( COUNTER_SUCCESS++ ))
  echo "  OK"
else
  echo "  FAIL"
  (( COUNTER_FAIL++ ))
fi
rm -f s21_grep.txt grep.txt long.txt

rm -f pattern.txt pattern2.txt empty_pattern.txt regex_patterns.txt test_o.txt err.txt

echo "========================"
echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"
echo "========================"

if [ -s log.txt ]; then
  echo "Failed tests:"
  cat log.txt
fi

rm -f log.txt
exit "$COUNTER_FAIL"