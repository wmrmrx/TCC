for file in tests_n\=6/*.txt; do
	./main < $file > /dev/null
done
