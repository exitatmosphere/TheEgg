.PHONY: build clean wasm exec

wasm:
	em++ -O3 --closure 1 -lembind -o immolate.js include/immolate.cpp -s EXPORT_NAME="'Immolate'"

exec:
	mkdir -p bin; g++ -O3 -o bin/tasks-test include/tasks-test.cpp