#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

using namespace fmt;
using namespace aoc;
using namespace std;

int floorOffset(char instruction)
{
	switch(instruction)
	{
		case '(': return  1;
		case ')': return -1;

		default: panic(format("invalid instruction: '{}'", instruction));
	}

	return 0;
}

int findFloor(const string& instructions)
{
	int floor = 0;

	for(char instruction : instructions)
		floor += floorOffset(instruction);

	return floor;
}

int findEnterTheBasementInstructionPosition(const string& instructions)
{
	int floor = 0;

	for(size_t i=0; i<instructions.size(); ++i)
	{
		floor += floorOffset(instructions[i]);

		if(floor == -1)
			return i + 1;
	}

	panic(format("'enter the basement' instruction not found"));

	return -1;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("floorOffset")
{
	CHECK(floorOffset('(') == 1);
	CHECK(floorOffset(')') == -1);
	CHECK_THROWS_WITH_AS(floorOffset('a'), "invalid instruction: 'a'", runtime_error);
}

TEST_CASE("findFloor")
{
	CHECK(findFloor("") == 0);
	CHECK(findFloor("(())") == 0);
	CHECK(findFloor("()()") == 0);
	CHECK(findFloor("(((") == 3);
	CHECK(findFloor("(()(()(") == 3);
	CHECK(findFloor("))(((((") == 3);
	CHECK(findFloor("())") == -1);
	CHECK(findFloor("))(") == -1);
	CHECK(findFloor(")))") == -3);
	CHECK(findFloor(")())())") == -3);
	CHECK_THROWS_WITH_AS(findFloor("a()"), "invalid instruction: 'a'", runtime_error);
	CHECK_THROWS_WITH_AS(findFloor("(b)"), "invalid instruction: 'b'", runtime_error);
	CHECK_THROWS_WITH_AS(findFloor("()c"), "invalid instruction: 'c'", runtime_error);
}

TEST_CASE("findEnterTheBasementInstructionPosition")
{
	CHECK(findEnterTheBasementInstructionPosition(")") == 1);
	CHECK(findEnterTheBasementInstructionPosition("()())") == 5);
	CHECK_THROWS_WITH_AS(findEnterTheBasementInstructionPosition("a()())"), "invalid instruction: 'a'", runtime_error);
	CHECK_THROWS_WITH_AS(findEnterTheBasementInstructionPosition("()b())"), "invalid instruction: 'b'", runtime_error);
	CHECK_THROWS_WITH_AS(findEnterTheBasementInstructionPosition("(((c"), "invalid instruction: 'c'", runtime_error);
	CHECK_THROWS_WITH_AS(findEnterTheBasementInstructionPosition(""), "'enter the basement' instruction not found", runtime_error);
	CHECK_THROWS_WITH_AS(findEnterTheBasementInstructionPosition("("), "'enter the basement' instruction not found", runtime_error);
	CHECK_THROWS_WITH_AS(findEnterTheBasementInstructionPosition("(((())))"), "'enter the basement' instruction not found", runtime_error);
}

#else // #ifdef AOC_TEST_SOLUTION

class NotQuiteLisp : public PuzzleSolution<int>
{
private:
	int answer1() override;
	int answer2() override;
};

int NotQuiteLisp::answer1()
{
	AOC_ENFORCE_MSG(input().size() == 1, "invalid size of the input: one line expected");

	return findFloor(input()[0]);
}

int NotQuiteLisp::answer2()
{
	AOC_ENFORCE_MSG(input().size() == 1, "invalid size of the input: one line expected");

	return findEnterTheBasementInstructionPosition(input()[0]);
}

int main(int argc, char* argv[])
{
	return NotQuiteLisp().run((argc > 1) ? argv[1] : "201501.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
