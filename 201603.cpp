#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

using namespace fmt;
using namespace aoc;
using namespace std;

bool isValidTriangle(int a, int b, int c)
{
	if(a + b <= c || a + c <= b || b + c <= a)
		return false;
	else
		return true;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("isValidTriangle")
{
	CHECK(isValidTriangle(5, 5, 5));
	CHECK(!isValidTriangle(5, 10, 15));
}

#else // #ifdef AOC_TEST_SOLUTION

class SquaresWithThreeSides : public PuzzleSolution<int>
{
private:
	int answer1() override;
	int answer2() override;
};

int SquaresWithThreeSides::answer1()
{
	size_t numValidTriangles = 0;

	for(const auto& line : input())
	{
		const auto tokens = StringUtils::tokenize(line, ' ', true, true);

		AOC_ENFORCE(tokens.size() == 3);

		if(isValidTriangle(stoi(tokens[0]),
		                   stoi(tokens[1]),
		                   stoi(tokens[2])))
			++numValidTriangles;
	}

	return numValidTriangles;
}

int SquaresWithThreeSides::answer2()
{
	size_t numValidTriangles = 0;

	AOC_ENFORCE((input().size() % 3) == 0);

	const size_t numSquares = input().size() / 3;

	for(size_t i=0; i<numSquares; ++i)
	{
		const auto& line0 = input()[i * 3 + 0];
		const auto& line1 = input()[i * 3 + 1];
		const auto& line2 = input()[i * 3 + 2];

		const auto tokens0 = StringUtils::tokenize(line0, ' ', true, true);
		const auto tokens1 = StringUtils::tokenize(line1, ' ', true, true);
		const auto tokens2 = StringUtils::tokenize(line2, ' ', true, true);

		if(isValidTriangle(stoi(tokens0[0]),
		                   stoi(tokens1[0]),
		                   stoi(tokens2[0])))
			++numValidTriangles;

		if(isValidTriangle(stoi(tokens0[1]),
		                   stoi(tokens1[1]),
		                   stoi(tokens2[1])))
			++numValidTriangles;

		if(isValidTriangle(stoi(tokens0[2]),
		                   stoi(tokens1[2]),
		                   stoi(tokens2[2])))
			++numValidTriangles;
	}

	return numValidTriangles;
}

int main(int argc, char* argv[])
{
	return SquaresWithThreeSides().run((argc > 1) ? argv[1] : "201603.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
