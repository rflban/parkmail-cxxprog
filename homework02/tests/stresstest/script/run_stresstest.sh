#!/bin/bash
path_to_test_root="tests/stresstest"
path_to_serial_build="$1"
path_to_parallel_build="$2"
path_to_data="$path_to_test_root/data"
token="hi"
rating_size=5

serial_run="$path_to_serial_build/$path_to_test_root/run ./$path_to_data $token $rating_size"
parallel_run="$path_to_parallel_build/$path_to_test_root/run ./$path_to_data $token $rating_size"

rg_output=$(
    sed -E "s/^(\S+) (.+)$/\2: \1/" <(
        head -n $rating_size <(
            sort -r -h <(for file in $(ls ./$path_to_data)
            do
                echo $(rg --count-matches $token ./$path_to_data/$file) $file
            done)
        )
    )
)

test_repeats=10

serial_output=$($serial_run)
serial_time="0"
for i in $(seq $test_repeats)
do
    curr_time=$(/bin/time -f %e $serial_run 2>&1 >/dev/null)
    serial_time=$(echo "$serial_time + $curr_time" | bc)
done
serial_time=$(echo "scale=4; $serial_time / $test_repeats" | bc)
echo -n "Serial implementation took $serial_time seconds "
if [ "$rg_output" = "$serial_output" ]; then
    echo "and it is equal to referece implementation."
else
    echo "and it is not equal to referece implementation."
fi

parallel_output=$($parallel_run)
parallel_time="0"
for i in $(seq $test_repeats)
do
    curr_time=$(/bin/time -f %e $parallel_run 2>&1 >/dev/null)
    parallel_time=$(echo "$parallel_time + $curr_time" | bc)
done
parallel_time=$(echo "scale=4; $parallel_time / $test_repeats" | bc)
echo -n "Parallel implementation took $parallel_time seconds "
if [ "$rg_output" = "$parallel_output" ]; then
    echo "and it is equal to referece implementation."
else
    echo "and it is not equal to referece implementation."
fi
