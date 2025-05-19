#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#define CHECK_SANTA(_x, _y, _direction) \
	CHECK(santa.x() == _x); \
	CHECK(santa.y() == _y); \
	CHECK(santa.position() == make_tuple(_x, _y)); \
	CHECK(santa.direction() == (_direction));

using namespace fmt;
using namespace aoc;
using namespace std;

enum class Direction
{
	North = 0,
	East,
	South,
	West,
};

enum class Side
{
	Left,
	Right,
};

tuple<Side, size_t> parseInstruction(const string& instruction)
{
	static const regex instructionRegex(R"((L|R)(\d+))");

	smatch instructionMatch;

	if(regex_match(instruction,
	               instructionMatch,
	               instructionRegex))
	{
		constexpr size_t expectedInstructionMatchSize = 3;

		AOC_ENFORCE(instructionMatch.size() == expectedInstructionMatchSize);

		const string& sideTag = instructionMatch[1].str();
		const size_t numBlocks = stoi(instructionMatch[2].str());

		AOC_ENFORCE(sideTag.size() == 1);
		AOC_ENFORCE(sideTag[0] == 'L' || sideTag[0] == 'R');

		switch(sideTag[0])
		{
			case 'L': return make_tuple(Side::Left, numBlocks);
			case 'R': return make_tuple(Side::Right, numBlocks);
		}
	}
	else
		panic(format("invalid instruction: '{}'", instruction));

	return make_tuple(Side::Left, 0);
}

constexpr tuple<int, int> deltaPositionFor[] =
{
	{  0, -1 }, // Direction::North
	{  1,  0 }, // Direction::East
	{  0,  1 }, // Direction::South
	{ -1,  0 }, // Direction::West
};

constexpr Direction lhsDirectionFor[] =
{
	Direction::West , // Direction::North
	Direction::North, // Direction::East
	Direction::East , // Direction::South
	Direction::South, // Direction::West
};

constexpr Direction rhsDirectionFor[] =
{
	Direction::East , // Direction::North
	Direction::South, // Direction::East
	Direction::West , // Direction::South
	Direction::North, // Direction::West
};

class Santa
{
public:
	explicit Santa(int x=0,
	               int y=0,
	               Direction direction=Direction::North)
		: x_(x)
		, y_(y)
		, direction_(direction)
	{
	}

public:
	void reset(int x=0,
	           int y=0,
	           Direction direction=Direction::North)
	{
		x_ = x;
		y_ = y;
		direction_ = direction;
	}

public:
	int x() const { return x_; }
	int y() const { return y_; }

	tuple<int, int> position() 
	{
		return make_tuple(x_, y_);
	}

	Direction direction() const
	{
		return direction_;
	}

	void setDirection(Direction direction)
	{
		AOC_ASSERT(direction >= Direction::North &&
		           direction <= Direction::West);

		direction_ = direction;
	}

public:
	void walk(size_t numBlocks)
	{
		const auto [dx, dy] = deltaPositionFor[static_cast<size_t>(direction_)];

		x_ += numBlocks * dx;
		y_ += numBlocks * dy;
	}

	void turnLeft()
	{
		direction_ = lhsDirectionFor[static_cast<size_t>(direction_)];
	}

	void turnRight()
	{
		direction_ = rhsDirectionFor[static_cast<size_t>(direction_)];
	}

	void turn(Side side)
	{
		switch(side)
		{
			case Side::Left: turnLeft(); break;
			case Side::Right: turnRight(); break;
		}
	}

	void followInstruction(const string& instruction)
	{
		const auto [side, numBlocks] = parseInstruction(instruction);

		turn(side);
		walk(numBlocks);
	}

	void followInstructions(const string& instructionsStr)
	{
		const std::vector<std::string> instructions = StringUtils::tokenize(instructionsStr);

		for(const auto& instruction : instructions)
			followInstruction(instruction);
	}

	bool followInstructionsUntilTheFirstLocationYouVisitTwiceIsFound(const string& instructionsStr)
	{
		positions_.clear();
		positions_.insert(position());

		const std::vector<std::string> instructions = StringUtils::tokenize(instructionsStr);

		for(const auto& instruction : instructions)
			if(followInstructionUntilTheFirstLocationYouVisitTwiceIsFound(instruction))
				return true;

		return false;
	}

private:
	bool followInstructionUntilTheFirstLocationYouVisitTwiceIsFound(const string& instruction)
	{
		const auto [side, numBlocks] = parseInstruction(instruction);

		turn(side);

		for(size_t i=0; i<numBlocks; ++i)
		{
			walk(1);

			if(positions_.contains(position()))
				return true;
			else
				positions_.insert(position());
		}

		return false;
	}

private:
	int x_;
	int y_;
	Direction direction_;
	set<tuple<int, int>> positions_;
};

#ifdef AOC_TEST_SOLUTION

TEST_CASE("parseInstruction")
{
	CHECK(parseInstruction("L1") == make_tuple(Side::Left, 1));
	CHECK(parseInstruction("R0") == make_tuple(Side::Right, 0));
	CHECK(parseInstruction("R1") == make_tuple(Side::Right, 1));
	CHECK(parseInstruction("L11") == make_tuple(Side::Left, 11));
	CHECK(parseInstruction("L22") == make_tuple(Side::Left, 22));
	CHECK(parseInstruction("R33") == make_tuple(Side::Right, 33));
	CHECK(parseInstruction("R44") == make_tuple(Side::Right, 44));
	CHECK_THROWS_WITH_AS(parseInstruction("L"), "invalid instruction: 'L'", runtime_error);
	CHECK_THROWS_WITH_AS(parseInstruction("L-1"), "invalid instruction: 'L-1'", runtime_error);
	CHECK_THROWS_WITH_AS(parseInstruction("R"), "invalid instruction: 'R'", runtime_error);
	CHECK_THROWS_WITH_AS(parseInstruction("Rxy"), "invalid instruction: 'Rxy'", runtime_error);
}

TEST_CASE("Santa")
{
	Santa santa;
	CHECK_SANTA(0, 0, Direction::North);

	SUBCASE("walk")
	{
		santa.walk(3);
		CHECK_SANTA(0, -3, Direction::North);
	}

	SUBCASE("turnLeft")
	{
		santa.turnLeft();
		CHECK_SANTA(0, 0, Direction::West);

		santa.turnLeft();
		CHECK_SANTA(0, 0, Direction::South);

		santa.turnLeft();
		CHECK_SANTA(0, 0, Direction::East);

		santa.turnLeft();
		CHECK_SANTA(0, 0, Direction::North);
	}

	SUBCASE("turnRight")
	{
		santa.turnRight();
		CHECK_SANTA(0, 0, Direction::East);

		santa.turnRight();
		CHECK_SANTA(0, 0, Direction::South);

		santa.turnRight();
		CHECK_SANTA(0, 0, Direction::West);

		santa.turnRight();
		CHECK_SANTA(0, 0, Direction::North);
	}

	SUBCASE("turn")
	{
		santa.setDirection(Direction::South);

		santa.turn(Side::Right);
		CHECK_SANTA(0, 0, Direction::West);

		santa.turn(Side::Left);
		CHECK_SANTA(0, 0, Direction::South);

		santa.turn(Side::Left);
		CHECK_SANTA(0, 0, Direction::East);

		santa.turn(Side::Left);
		CHECK_SANTA(0, 0, Direction::North);

		santa.turn(Side::Right);
		CHECK_SANTA(0, 0, Direction::East);

		santa.turn(Side::Right);
		CHECK_SANTA(0, 0, Direction::South);
	}

	SUBCASE("followInstruction")
	{
		santa.followInstruction("R2");
		CHECK_SANTA(2, 0, Direction::East);

		santa.followInstruction("L5");
		CHECK_SANTA(2, -5, Direction::North);

		santa.followInstruction("L4");
		CHECK_SANTA(-2, -5, Direction::West);

		santa.followInstruction("L9");
		CHECK_SANTA(-2, 4, Direction::South);

		santa.followInstruction("L192");
		CHECK_SANTA(190, 4, Direction::East);
	}

	SUBCASE("followInstructions")
	{
		santa.followInstructions("R2, L3");
		CHECK_SANTA(2, -3, Direction::North);
	}
}

#else // #ifdef AOC_TEST_SOLUTION

class NoTimeForATaxicab : public PuzzleSolution<int>
{
private:
	int answer1() override;
	int answer2() override;
};

int NoTimeForATaxicab::answer1()
{
	AOC_ENFORCE_MSG(input().size() == 1, "invalid size of the input: one line expected");

	const string& instructionsStr = input()[0];

	Santa santa;

	santa.followInstructions(instructionsStr);

	return MathUtils::manhattanDistance(0, 0, santa.x(), santa.y());
}

int NoTimeForATaxicab::answer2()
{
	AOC_ENFORCE_MSG(input().size() == 1, "invalid size of the input: one line expected");

	const string& instructionsStr = input()[0];

	Santa santa;

	const bool isFound = santa.followInstructionsUntilTheFirstLocationYouVisitTwiceIsFound(instructionsStr);

	if(!isFound)
		panic("could not find the location you visited twice");

	return MathUtils::manhattanDistance(0, 0, santa.x(), santa.y());
}

int main(int argc, char* argv[])
{
	return NoTimeForATaxicab().run((argc > 1) ? argv[1] : "201601.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
