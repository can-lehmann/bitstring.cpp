test: test_bitstring2 test_bitstring4
	./test_bitstring2
	./test_bitstring4

test_%: test_%.cpp bitstring.hpp
	clang++ -o $@ $<
