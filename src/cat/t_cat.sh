COUNTER_SUCCESS=0
COUNTER_FAIL=0
LOG_FILE="log_s21_cat.txt"
TEST_FILE1="test_file1.txt"
TEST_FILE2="test_file2.txt"

ALL_OPTIONS=("-b" "-e" "-n" "-s" "-t" "-v" "-T" "--number-nonblank" "--number" "--squeeze-blank")

echo "" > $LOG_FILE

run_test() {
    local test_case="$1"
    
    ./s21_cat $test_case > s21_cat.tmp
    cat $test_case > cat.tmp
    
    DIFF_RES="$(diff -s s21_cat.tmp cat.tmp)"
    
    echo "Тест: $test_case"

    if [ "$DIFF_RES" == "Files s21_cat.tmp and cat.tmp are identical" ]
    then
        (( COUNTER_SUCCESS++ ))
    else
        echo "FAIL: $test_case" >> $LOG_FILE
        (( COUNTER_FAIL++ ))
    fi
    
    rm s21_cat.tmp cat.tmp
}

echo "### 1. Тестирование Одиночных Опций с Одним Файлом ###"
for opt in "${ALL_OPTIONS[@]}"
do
    run_test "$opt $TEST_FILE1"
done

echo -e "\n### 2. Тестирование Одиночных Опций с Двумя Файлами ###"
for opt in "${ALL_OPTIONS[@]}"
do
    run_test "$opt $TEST_FILE1 $TEST_FILE2"
done

echo -e "\n### 3. Тестирование Комбинированных Опций (два флага) ###"
for opt1 in "${ALL_OPTIONS[@]}"
do
    for opt2 in "${ALL_OPTIONS[@]}"
    do

        if [ "$opt1" != "$opt2" ]
        then
            run_test "$opt1 $opt2 $TEST_FILE1"
        fi
    done
done

SHORT_OPTIONS=("-b" "-e" "-n" "-s" "-t" "-v")
echo -e "\n### 4. Тестирование Сгруппированных Коротких Опций (-abc) ###"
for opt1 in "${SHORT_OPTIONS[@]}"
do
    for opt2 in "${SHORT_OPTIONS[@]}"
    do
        if [ "$opt1" != "$opt2" ]
        then
            OPT_GROUP="-${opt1:1}${opt2:1}"
            run_test "$OPT_GROUP $TEST_FILE1"
        fi
    done
done


echo -e "\n### 5. Тестирование Комбинаций (три флага) ###"
for opt1 in "${SHORT_OPTIONS[@]}"
do
    for opt2 in "${SHORT_OPTIONS[@]}"
    do
        for opt3 in "${SHORT_OPTIONS[@]}"
        do
            if [[ "$opt1" != "$opt2" && "$opt2" != "$opt3" && "$opt1" != "$opt3" ]]
            then
                run_test "$opt1 $opt2 $opt3 $TEST_FILE1"
            fi
        done
    done
done


echo "---"
echo "Успешно: $COUNTER_SUCCESS"
echo "Провалено: $COUNTER_FAIL"
if [ "$COUNTER_FAIL" -gt 0 ]; then
    echo "Подробности проваленных тестов в файле: $LOG_FILE"
    exit 1
else
    echo "Все тесты пройдены успешно!"
    exit 0
fi