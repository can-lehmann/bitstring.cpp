test: test_bitstring2
	./test_bitstring2

test_%: test_%.cpp
	clang++ -o $@ $<
