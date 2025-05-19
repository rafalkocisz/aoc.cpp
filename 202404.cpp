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

size_t countWord(const BoundedGrid2D<char>& wordSearch,
                 const string& word)
{
	size_t wordCounter = 0;

	for(size_t y=0; y<wordSearch.height(); ++y)
	{
		wordCounter += StringUtils::countNonOverlappingSubstrings(wordSearch.constRow(y).toString(), word);
		wordCounter += StringUtils::countNonOverlappingSubstrings(wordSearch.constReverseRow(y).toString(), word);
	}

	for(size_t x=0; x<wordSearch.width(); ++x)
	{
		wordCounter += StringUtils::countNonOverlappingSubstrings(wordSearch.constColumn(x).toString(), word);
		wordCounter += StringUtils::countNonOverlappingSubstrings(wordSearch.constReverseColumn(x).toString(), word);
	}

	for(size_t i=0; i<wordSearch.numDiagonals(); ++i)
	{
		wordCounter += StringUtils::countNonOverlappingSubstrings(wordSearch.constTopDownDiagonal(i).toString(), word);
		wordCounter += StringUtils::countNonOverlappingSubstrings(wordSearch.constReverseTopDownDiagonal(i).toString(), word);
		wordCounter += StringUtils::countNonOverlappingSubstrings(wordSearch.constBottomUpDiagonal(i).toString(), word);
		wordCounter += StringUtils::countNonOverlappingSubstrings(wordSearch.constReverseBottomUpDiagonal(i).toString(), word);
	}

	return wordCounter;
}

size_t countXMasPattern(const BoundedGrid2D<char>& wordSearch)
{
	AOC_ENFORCE(wordSearch.width() >= 3 &&
	            wordSearch.height() >= 3);

	BoundedGrid2D<char> subGrid(3, 3);

	size_t xMasPatternCounter = 0;
	
	for(size_t x=0; x<wordSearch.width()-2; ++x)
	{
		for(size_t y=0; y<wordSearch.height()-2; ++y)
		{
			for(size_t x1=0; x1<3; ++x1)
				for(size_t y1=0; y1<3; ++y1)
					subGrid.setCell(x1, y1, wordSearch.cell(x+x1, y+y1));

			if((subGrid.constTopDownDiagonal(2).toString() == "MAS" ||
			    subGrid.constReverseTopDownDiagonal(2).toString() == "MAS") &&
			   (subGrid.constBottomUpDiagonal(2).toString() == "MAS" ||
			    subGrid.constReverseBottomUpDiagonal(2).toString() == "MAS"))
			{
				++xMasPatternCounter;
			}
		}
	}

	return xMasPatternCounter;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("countWord")
{
	const vector<string> lines1 =
	{
		"..X...",
		".SAMX.",
		".A..A.",
		"XMAS.S",
		".X....",
	};

	BoundedGrid2D<char> grid(lines1);

	CHECK(countWord(grid, "XMAS") == 4);

	const vector<string> lines2 =
	{
		"MMMSXXMASM",
		"MSAMXMSMSA",
		"AMXSXMAAMM",
		"MSAMASMSMX",
		"XMASAMXAMM",
		"XXAMMXXAMA",
		"SMSMSASXSS",
		"SAXAMASAAA",
		"MAMMMXMMMM",
		"MXMXAXMASX",
	};

	grid.reset(lines2);

	CHECK(countWord(grid, "XMAS") == 18);
}

TEST_CASE("countXMasPattern")
{
	const vector<string> lines =
	{
		"MMMSXXMASM",
		"MSAMXMSMSA",
		"AMXSXMAAMM",
		"MSAMASMSMX",
		"XMASAMXAMM",
		"XXAMMXXAMA",
		"SMSMSASXSS",
		"SAXAMASAAA",
		"MAMMMXMMMM",
		"MXMXAXMASX",
	};

	BoundedGrid2D<char> grid(lines);

	CHECK(countXMasPattern(grid) == 9);
}

#else // #ifdef AOC_TEST_SOLUTION

class CeresSearch : public PuzzleSolution<int>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	int answer1() override;
	int answer2() override;

private:
	BoundedGrid2D<char> wordSearch_;
};

void CeresSearch::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	AOC_ENFORCE(!input().empty());
	AOC_ENFORCE(!(input()[0].empty()));

	wordSearch_.reset(input());
}

int CeresSearch::answer1()
{
	return countWord(wordSearch_, "XMAS");
}

int CeresSearch::answer2()
{
	return countXMasPattern(wordSearch_);
}

int main(int argc, char* argv[])
{
	return CeresSearch().run((argc > 1) ? argv[1] : "202404.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
