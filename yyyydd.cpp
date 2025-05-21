#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

using namespace fmt;
using namespace aoc;
using namespace std;

int findAnswer1()
{
	return 1;
}

int findAnswer2()
{
	return 2;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("findAnswer1")
{
	CHECK(findAnswer1() == 1);
}

TEST_CASE("findAnswer2")
{
	CHECK(findAnswer2() == 2);
}

#else // #ifdef AOC_TEST_SOLUTION

class PuzzleName : public PuzzleSolution<int>
{
private:
	int answer1() override;
	int answer2() override;
};

int PuzzleName::answer1()
{
	return findAnswer1();
}

int PuzzleName::answer2()
{
	return findAnswer2();
}

int main(int argc, char* argv[])
{
	return PuzzleName().run((argc > 1) ? argv[1] : "yyyydd.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
