#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <set>
#include <tuple>

using namespace fmt;
using namespace aoc;
using namespace std;

inline bool isDigit(char height)
{
	return height >= '0' && height <= '9';
}

void followTrailhead1(const BoundedGrid2D<char>& topographicMap,
                      size_t x, size_t y, char height,
                      set<tuple<size_t, size_t>>& topPositions)
{
	AOC_ASSERT(isDigit(height));

	if(height == '9')
	{
		topPositions.insert(make_tuple(x, y));

		return;
	}

	if((y > 0) && (topographicMap.cell(x, y-1) == height+1))
		followTrailhead1(topographicMap, x, y-1, height+1, topPositions);

	if((y < topographicMap.height()-1) && (topographicMap.cell(x, y+1) == height+1))
		followTrailhead1(topographicMap, x, y+1, height+1, topPositions);

	if((x > 0) && (topographicMap.cell(x-1, y) == height+1))
		followTrailhead1(topographicMap, x-1, y, height+1, topPositions);

	if((x < topographicMap.width()-1) && (topographicMap.cell(x+1, y) == height+1))
		followTrailhead1(topographicMap, x+1, y, height+1, topPositions);
}

void followTrailhead2(const BoundedGrid2D<char>& topographicMap,
                      size_t x, size_t y, char height, int& rating)
{
	AOC_ASSERT(isDigit(height));

	if(height == '9')
	{
		rating += 1;

		return;
	}

	if((y > 0) && (topographicMap.cell(x, y-1) == height+1))
		followTrailhead2(topographicMap, x, y-1, height+1, rating);

	if((y < topographicMap.height()-1) && (topographicMap.cell(x, y+1) == height+1))
		followTrailhead2(topographicMap, x, y+1, height+1, rating);

	if((x > 0) && (topographicMap.cell(x-1, y) == height+1))
		followTrailhead2(topographicMap, x-1, y, height+1, rating);

	if((x < topographicMap.width()-1) && (topographicMap.cell(x+1, y) == height+1))
		followTrailhead2(topographicMap, x+1, y, height+1, rating);
}

int topographicMapScore(const BoundedGrid2D<char>& topographicMap)
{
	int score = 0;

	for(size_t x=0; x<topographicMap.width(); ++x)
	{
		for(size_t y=0; y<topographicMap.height(); ++y)
		{
			if(topographicMap.cell(x, y) == '0')
			{
				set<tuple<size_t, size_t>> topPositions;

				followTrailhead1(topographicMap, x, y, '0', topPositions);

				score += topPositions.size();
			}
		}
	}

	return score;
}

int topographicMapRating(const BoundedGrid2D<char>& topographicMap)
{
	int rating = 0;

	for(size_t x=0; x<topographicMap.width(); ++x)
		for(size_t y=0; y<topographicMap.height(); ++y)
			if(topographicMap.cell(x, y) == '0')
				followTrailhead2(topographicMap, x, y, '0', rating);

	return rating;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("topographicMapScore")
{
	const vector<string> lines1 =
	{
		"0123",
		"1234",
		"8765",
		"9876",
	};

	BoundedGrid2D<char> map(lines1);

	CHECK(topographicMapScore(map) == 1);

	const vector<string> lines2 =
	{
		"...0...",
		"...1...",
		"...2...",
		"6543456",
		"7.....7",
		"8.....8",
		"9.....9",
	};

	map.reset(lines2);

	CHECK(topographicMapScore(map) == 2);

	const vector<string> lines3 =
	{
		"..90..9",
		"...1.98",
		"...2..7",
		"6543456",
		"765.987",
		"876....",
		"987....",
	};

	map.reset(lines3);

	CHECK(topographicMapScore(map) == 4);

	const vector<string> lines4 =
	{
		"10..9..",
		"2...8..",
		"3...7..",
		"4567654",
		"...8..3",
		"...9..2",
		".....01",
	};

	map.reset(lines4);

	CHECK(topographicMapScore(map) == 3);

	const vector<string> lines5 =
	{
		"89010123",
		"78121874",
		"87430965",
		"96549874",
		"45678903",
		"32019012",
		"01329801",
		"10456732",
	};

	map.reset(lines5);

	CHECK(topographicMapScore(map) == 36);
}

TEST_CASE("topographicMapRating")
{
	const vector<string> lines1 =
	{
		".....0.",
		"..4321.",
		"..5..2.",
		"..6543.",
		"..7..4.",
		"..8765.",
		"..9....",
	};

	BoundedGrid2D<char> map(lines1);

	CHECK(topographicMapRating(map) == 3);

	const vector<string> lines2 =
	{
		"..90..9",
		"...1.98",
		"...2..7",
		"6543456",
		"765.987",
		"876....",
		"987....",
	};

	map.reset(lines2);

	CHECK(topographicMapRating(map) == 13);

	const vector<string> lines3 =
	{
		"012345",
		"123456",
		"234567",
		"345678",
		"4.6789",
		"56789.",
	};

	map.reset(lines3);

	CHECK(topographicMapRating(map) == 227);

	const vector<string> lines4 =
	{
		"89010123",
		"78121874",
		"87430965",
		"96549874",
		"45678903",
		"32019012",
		"01329801",
		"10456732",
	};

	map.reset(lines4);

	CHECK(topographicMapRating(map) == 81);
}

#else // #ifdef AOC_TEST_SOLUTION

class HoofIt : public PuzzleSolution<int>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	int answer1() override;
	int answer2() override;

private:
	BoundedGrid2D<char> topographicMap_;
};

void HoofIt::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	AOC_ENFORCE(!input().empty());
	AOC_ENFORCE(!(input()[0].empty()));

	topographicMap_.reset(input());
}

int HoofIt::answer1()
{
	return topographicMapScore(topographicMap_);
}

int HoofIt::answer2()
{
	return topographicMapRating(topographicMap_);
}

int main(int argc, char* argv[])
{
	return HoofIt().run((argc > 1) ? argv[1] : "202410.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
