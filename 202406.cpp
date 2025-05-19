#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <memory>

using namespace fmt;
using namespace aoc;
using namespace std;

constexpr string directionChars("^>v<");

bool isDirectionChar(char ch)
{
	for(char directionChar : directionChars)
		if(directionChar == ch)
			return true;

	return false;
}

Direction directionCharToDirection(char directionChar)
{
	switch(directionChar)
	{
		case '^': return Direction::North;
		case '>': return Direction::East;
		case 'v': return Direction::South;
		case '<': return Direction::West;
	}

	panic(format("invalid direction character: '{}'", directionChar));

	return Direction::North;
}

class Lab
{
public:
	static constexpr char obstacleChar = '#';
	static constexpr char guardPathChar = 'X';

public:
	Lab(const std::vector<std::string>& lines)
		: map_(lines)
	{
	}

public:
	BoundedGrid2D<char>& map() { return map_; }
	const BoundedGrid2D<char>& map() const { return map_; }

public:
	Point2s findGuardStartPosition() const;

private:
	BoundedGrid2D<char> map_;
};

Point2s Lab::findGuardStartPosition() const
{
	Point2s guardStartPosition;

	for(char directionChar : directionChars)
	{
		guardStartPosition = map_.findCell(directionChar);

		if(guardStartPosition.x_ != BoundedGrid2D<char>::npos &&
		   guardStartPosition.y_ != BoundedGrid2D<char>::npos)
			break;
	}

	AOC_ENFORCE(guardStartPosition.x_ != BoundedGrid2D<char>::npos &&
	            guardStartPosition.y_ != BoundedGrid2D<char>::npos);

	return guardStartPosition;
}

class Guard
{
public:
	Guard(Lab& lab)
		: lab_(lab)
		, direction_(Direction::North)
	{
		startPosition_ = lab.findGuardStartPosition();
		currentPosition_ = startPosition_;
		direction_ = directionCharToDirection(lab.map().cell(startPosition_));
	}

public:
	bool isOutOfLab(const Point2i& position) const
	{
		return position.x_ < 0 ||
		       position.y_ < 0 ||
		       position.x_ >= lab_.map().width() ||
		       position.y_ >= lab_.map().height();
	}

public:
	bool predictPath(vector<Point2i>* guardPath=nullptr);

#ifdef AOC_TEST_SOLUTION
public:
#else
private:
#endif
	Lab& lab_;
	Point2i startPosition_;
	Point2i currentPosition_;
	Direction direction_;
};

bool Guard::predictPath(vector<Point2i>* guardPath)
{
	AOC_ASSERT(!isOutOfLab(currentPosition_));

	if(guardPath)
		guardPath->clear();

	lab_.map().setCell(currentPosition_, Lab::guardPathChar);
	if(guardPath)
		if(find(guardPath->begin(), guardPath->end(), currentPosition_) == guardPath->end())
			guardPath->push_back(currentPosition_);

	const size_t maxSteps = lab_.map().numCells();

	size_t stepCounter = 1;

	while(true)
	{
		Point2i nextPosition = aoc::nextPosition(currentPosition_, direction_);

		if(isOutOfLab(nextPosition))
			break;

		if(lab_.map().cell(nextPosition) == Lab::obstacleChar)
		{
			direction_ = turnRight(direction_);
		}
		else
		{
			currentPosition_ = nextPosition;

			lab_.map().setCell(currentPosition_, Lab::guardPathChar);
			if(guardPath)
				if(find(guardPath->begin(), guardPath->end(), currentPosition_) == guardPath->end())
					guardPath->push_back(currentPosition_);

			++stepCounter;

			if(stepCounter >= maxSteps)
				return false;
		}
	}

	return true;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("isDirectionChar")
{
	CHECK( isDirectionChar('^'));
	CHECK( isDirectionChar('>'));
	CHECK( isDirectionChar('v'));
	CHECK( isDirectionChar('<'));
	CHECK(!isDirectionChar('.'));
	CHECK(!isDirectionChar('#'));
	CHECK(!isDirectionChar('X'));
}

TEST_CASE("directionCharToDirection")
{
	CHECK(directionCharToDirection('^') == Direction::North);
	CHECK(directionCharToDirection('>') == Direction::East);
	CHECK(directionCharToDirection('v') == Direction::South);
	CHECK(directionCharToDirection('<') == Direction::West);

	CHECK_THROWS_WITH_AS(directionCharToDirection('a'), "invalid direction character: 'a'", runtime_error);
}

TEST_CASE("Lab")
{
	const vector<string> lines =
	{
		"....#.....",
		".........#",
		"..........",
		"..#.......",
		".......#..",
		"..........",
		".#..^.....",
		"........#.",
		"#.........",
		"......#...",
	};

	Lab lab(lines);

	CHECK(lab.map().toString() == "....#.....\n"
	                              ".........#\n"
	                              "..........\n"
	                              "..#.......\n"
	                              ".......#..\n"
	                              "..........\n"
	                              ".#..^.....\n"
	                              "........#.\n"
	                              "#.........\n"
	                              "......#...\n");

	SUBCASE("findGuardStartPosition")
	{
		CHECK(lab.findGuardStartPosition() == Point2s(4, 6));
	}
}

TEST_CASE("Guard")
{
	const vector<string> lines =
	{
		"....#.....",
		".........#",
		"..........",
		"..#.......",
		".......#..",
		"..........",
		".#..^.....",
		"........#.",
		"#.........",
		"......#...",
	};

	Lab lab(lines);
	Guard guard(lab);

	CHECK(guard.startPosition_ == Point2s(4, 6));
	CHECK(guard.currentPosition_ == Point2s(4, 6));
	CHECK(guard.direction_ == Direction::North);

	SUBCASE("predictPath")
	{
		const bool succeed = guard.predictPath();

		CHECK(succeed);

		CHECK(lab.map().toString() == "....#.....\n"
		                              "....XXXXX#\n"
		                              "....X...X.\n"
		                              "..#.X...X.\n"
		                              "..XXXXX#X.\n"
		                              "..X.X.X.X.\n"
		                              ".#XXXXXXX.\n"
		                              ".XXXXXXX#.\n"
		                              "#XXXXXXX..\n"
		                              "......#X..\n");

		CHECK(lab.map().count(Lab::guardPathChar) == 41);
	}

	SUBCASE("predictPath2")
	{
		lab.map().setCell(3, 6, Lab::obstacleChar);

		CHECK(lab.map().toString() == "....#.....\n"
		                              ".........#\n"
		                              "..........\n"
		                              "..#.......\n"
		                              ".......#..\n"
		                              "..........\n"
		                              ".#.#^.....\n"
		                              "........#.\n"
		                              "#.........\n"
		                              "......#...\n");

		const bool succeed = guard.predictPath();

		CHECK(lab.map().toString() == "....#.....\n"
		                              "....XXXXX#\n"
		                              "....X...X.\n"
		                              "..#.X...X.\n"
		                              "....X..#X.\n"
		                              "....X...X.\n"
		                              ".#.#XXXXX.\n"
		                              "........#.\n"
		                              "#.........\n"
		                              "......#...\n");

		CHECK(!succeed);
	}
}

#else // #ifdef AOC_TEST_SOLUTION

class GuardGallivant : public PuzzleSolution<int>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	int answer1() override;
	int answer2() override;

private:
	unique_ptr<Lab> lab_;
	vector<Point2i> guardPath_;
};

void GuardGallivant::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	lab_ = make_unique<Lab>(input());
}

int GuardGallivant::answer1()
{
	Guard guard(*lab_);

	const bool succeed = guard.predictPath(&guardPath_);

	AOC_ENFORCE(succeed);

	return lab_->map().count(Lab::guardPathChar);
}

int GuardGallivant::answer2()
{
	int loopCausingObstaclePositionCounter = 0;

	for(const auto& position : guardPath_)
	{
		Lab lab(input());

		if(!isDirectionChar(lab.map().cell(position)))
		{
			lab.map().setCell(position, Lab::obstacleChar);

			Guard guard(lab);

			if(!guard.predictPath())
			{
				++loopCausingObstaclePositionCounter;
			}
		}
	}

	return loopCausingObstaclePositionCounter;
}

int main(int argc, char* argv[])
{
	return GuardGallivant().run((argc > 1) ? argv[1] : "202406.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
