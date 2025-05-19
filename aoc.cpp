#include "aoc.h"

#if defined(AOC_TEST)
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#include <fstream>
#include <regex>
#include <sstream>
#include <unordered_set>

using namespace fmt;
using namespace std;

namespace aoc
{

vector<string> loadPuzzleInput(const string& puzzleInputFilePath)
{
	ifstream fileStream(puzzleInputFilePath);

	if(!fileStream.is_open())
		panic(format("unable to open input file: \"{}\"", puzzleInputFilePath));

	vector<string> input;
	string line;

	while(getline(fileStream, line))
		input.push_back(line);

	return input;
}

void panic(const string& message,
           source_location sourceLocation)
{
#if defined(AOC_TEST) || defined(AOC_TEST_SOLUTION)
	throw runtime_error(message);
#else
	constexpr fmt::string_view panicMessageFormat = "`{}` panicked at {}({}:{}) with message: `{}`.";

	println(stderr,
	        panicMessageFormat,
	        sourceLocation.function_name(),
	        sourceLocation.file_name(),
	        sourceLocation.line(),
	        sourceLocation.column(),
	        message);

	exit(EXIT_FAILURE);
#endif
}

bool StringUtils::contains(const string& str,
                           const string& substr)
{
	return str.find(substr) != string::npos;
}

bool StringUtils::contains(const string& str, char ch)
{
	const size_t length = str.size();

	for(size_t i=0; i<length; ++i)
		if(str[i] == ch)
			return true;

	return false;
}

size_t StringUtils::countOverlappingSubstrings(const string& str,
                                               const string& substr)
{
	AOC_ASSERT(!substr.empty());

	size_t pos = 0;
	size_t count = 0;

	while((pos = str.find(substr, pos)) != string::npos)
	{
		++count;

		pos += 1;
	}

	return count;
}

size_t StringUtils::countNonOverlappingSubstrings(const string& str,
                                                  const string& substr)
{
	AOC_ASSERT(!substr.empty());

	size_t pos = 0;
	size_t count = 0;

	while((pos = str.find(substr, pos)) != string::npos)
	{
		++count;

		pos += substr.length();
	}

	return count;
}

string StringUtils::replaceFirst(const string& str,
                                 const string& substr,
                                 const string& substr2)
{
	AOC_ASSERT(!substr.empty());
	AOC_ASSERT(!substr2.empty());

	auto result = str;
	auto pos = result.find(substr);

	if(pos != string::npos)
		result.replace(pos, substr.size(), substr2);

	return result;
}

string StringUtils::replaceLast(const string& str,
                                const string& substr,
                                const string& substr2)
{
	AOC_ASSERT(!substr.empty());
	AOC_ASSERT(!substr2.empty());

	auto result = str;
	auto pos = result.rfind(substr);

	if(pos != string::npos)
		result.replace(pos, substr.size(), substr2);

	return result;
}

string StringUtils::replaceAll(const string& str,
                               const string& substr,
                               const string& substr2)
{
	AOC_ASSERT(!substr.empty());
	AOC_ASSERT(!substr2.empty());

	return regex_replace(str, regex(substr), substr2);
}

string StringUtils::trim(const string& str)
{
	return ltrim(rtrim(str));
}

string StringUtils::ltrim(const string& str)
{
	static const regex lhsWhitespaceRegex{"^[[:space:]]*",
	                                      regex_constants::extended};

	return regex_replace(str, lhsWhitespaceRegex, "");
}

string StringUtils::rtrim(const string& str)
{
	static const regex rhsWhitespaceRegex{"[[:space:]]*$",
	                                      regex_constants::extended};

	return regex_replace(str, rhsWhitespaceRegex, "");
}

string StringUtils::trim(string str, char ch)
{
	return ltrim(rtrim(str, ch), ch);
}

string StringUtils::ltrim(string str, char ch)
{
	str.erase(str.begin(),
	          find_if(str.begin(),
	                  str.end(),
	                  [&ch](char ch2) { return !(ch == ch2); }));

	return str;
}

string StringUtils::rtrim(string str, char ch)
{
	str.erase(find_if(str.rbegin(),
	                  str.rend(),
	                  [&ch](unsigned char ch2) { return !(ch2 == ch); }).base(),
	          str.end());

	return str;
}

vector<string> StringUtils::tokenize(const string& str,
                                     char delimiter,
                                     bool trimWhitespace,
                                     bool skipEmptyTokens)
{
	vector<string> tokens;
	stringstream sin{str};
	string token;

	while(getline(sin, token, delimiter))
		if(!skipEmptyTokens)
			tokens.push_back(trimWhitespace ? trim(token) : token);
		else
			if(!token.empty())
				tokens.push_back(trimWhitespace ? trim(token) : token);

	return tokens;
}

void StringUtils::generateVariations(const set<char>& elements,
                                     size_t k,
                                     vector<string>& variations)
{
	generateVariations(elements, k, 0, "", variations);
}

void StringUtils::generateVariations(const set<char>& elements,
                                     size_t k,
                                     size_t i,
                                     string sequence,
                                     vector<string>& variations)
{
	if(k == i)
	{
		AOC_ASSERT(sequence.size() == k);

		variations.push_back(sequence);

		return;
	}

	for(char element : elements)
	{
		generateVariations(elements, k, i + 1, sequence + element, variations);
	}
}

} // namespace aoc

#if defined(AOC_TEST)

TEST_CASE("isEven")
{
	CHECK(!aoc::isEven(1));
	CHECK( aoc::isEven(2));
}

TEST_CASE("isOdd")
{
	CHECK( aoc::isOdd(1));
	CHECK(!aoc::isOdd(2));
}

TEST_CASE("BoundedGrid2D<char>")
{
	SUBCASE("smoke")
	{
		aoc::BoundedGrid2D<char> grid(7, 13, ' ');

		CHECK(grid.width() == 7);
		CHECK(grid.height() == 13);

		for(size_t y=0; y<grid.height(); ++y)
			for(size_t x=0; x<grid.width(); ++x)
				CHECK(grid.cell(x, y) == ' ');
	}

	SUBCASE("setCells")
	{
		const vector<string> lines =
		{
			"..X...",
			".SAMX.",
			".A..A.",
			"XMAS.S",
			".X....",
		};

		aoc::BoundedGrid2D<char> grid(6, 5, ' ');

		grid.setCells(lines);

		const char* expectedGridStr = "..X...\n"
		                              ".SAMX.\n"
		                              ".A..A.\n"
		                              "XMAS.S\n"
		                              ".X....\n";

		CHECK(grid.toString() == expectedGridStr);
	}

	SUBCASE("numDiagonals")
	{
		CHECK(aoc::BoundedGrid2D<int>(0, 0).numDiagonals() == 0);
		CHECK(aoc::BoundedGrid2D<int>(1, 1).numDiagonals() == 1);
		CHECK(aoc::BoundedGrid2D<int>(10, 0).numDiagonals() == 0);
		CHECK(aoc::BoundedGrid2D<int>(0, 10).numDiagonals() == 0);
		CHECK(aoc::BoundedGrid2D<int>(10, 1).numDiagonals() == 10);
		CHECK(aoc::BoundedGrid2D<int>(1, 10).numDiagonals() == 10);
		CHECK(aoc::BoundedGrid2D<int>(10, 5).numDiagonals() == 14);
		CHECK(aoc::BoundedGrid2D<int>(5, 10).numDiagonals() == 14);
		CHECK(aoc::BoundedGrid2D<int>(10, 10).numDiagonals() == 19);
		CHECK(aoc::BoundedGrid2D<int>(3, 4).numDiagonals() == 6);
		CHECK(aoc::BoundedGrid2D<int>(4, 3).numDiagonals() == 6);
	}

	SUBCASE("maxDiagonalLength")
	{
		CHECK(aoc::BoundedGrid2D<int>(0, 0).maxDiagonalLength() == 0);
		CHECK(aoc::BoundedGrid2D<int>(1, 1).maxDiagonalLength() == 1);
		CHECK(aoc::BoundedGrid2D<int>(10, 0).maxDiagonalLength() == 0);
		CHECK(aoc::BoundedGrid2D<int>(0, 10).maxDiagonalLength() == 0);
		CHECK(aoc::BoundedGrid2D<int>(10, 1).maxDiagonalLength() == 1);
		CHECK(aoc::BoundedGrid2D<int>(1, 10).maxDiagonalLength() == 1);
		CHECK(aoc::BoundedGrid2D<int>(10, 5).maxDiagonalLength() == 5);
		CHECK(aoc::BoundedGrid2D<int>(5, 10).maxDiagonalLength() == 5);
		CHECK(aoc::BoundedGrid2D<int>(10, 10).maxDiagonalLength() == 10);
		CHECK(aoc::BoundedGrid2D<int>(3, 4).maxDiagonalLength() == 3);
		CHECK(aoc::BoundedGrid2D<int>(4, 3).maxDiagonalLength() == 3);
	}

	SUBCASE("diagonalLength")
	{
		//    345
		// 2 ....
		// 1 ....
		// 0 ....
		//
		// 0 ....
		// 1 ....
		// 2 ....
		//    345
		aoc::BoundedGrid2D<char> grid4x3(4, 3, '.');

		CHECK(grid4x3.diagonalLength(0) == 1);
		CHECK(grid4x3.diagonalLength(1) == 2);
		CHECK(grid4x3.diagonalLength(2) == 3);
		CHECK(grid4x3.diagonalLength(3) == 3);
		CHECK(grid4x3.diagonalLength(4) == 2);
		CHECK(grid4x3.diagonalLength(5) == 1);

		//    45
		// 3 ...
		// 2 ...
		// 1 ...
		// 0 ...
		//
		// 0 ...
		// 1 ...
		// 2 ...
		// 3 ...
		//    45
		aoc::BoundedGrid2D<char> grid3x4(3, 4, '.');

		CHECK(grid3x4.diagonalLength(0) == 1);
		CHECK(grid3x4.diagonalLength(1) == 2);
		CHECK(grid3x4.diagonalLength(2) == 3);
		CHECK(grid3x4.diagonalLength(3) == 3);
		CHECK(grid3x4.diagonalLength(4) == 2);
		CHECK(grid3x4.diagonalLength(5) == 1);

		//    1234
		// 0 .....
		//
		// 0 .....
		//    1234
		aoc::BoundedGrid2D<char> grid5x1(5, 1, '.');

		CHECK(grid5x1.diagonalLength(0) == 1);
		CHECK(grid5x1.diagonalLength(1) == 1);
		CHECK(grid5x1.diagonalLength(2) == 1);
		CHECK(grid5x1.diagonalLength(3) == 1);
		CHECK(grid5x1.diagonalLength(4) == 1);

		// 4 .
		// 3 .
		// 2 .
		// 1 .
		// 0 .
		//
		// 0 .
		// 1 .
		// 2 .
		// 3 .
		// 4 .
		aoc::BoundedGrid2D<char> grid1x5(1, 5, '.');

		CHECK(grid1x5.diagonalLength(0) == 1);
		CHECK(grid1x5.diagonalLength(1) == 1);
		CHECK(grid1x5.diagonalLength(2) == 1);
		CHECK(grid1x5.diagonalLength(3) == 1);
		CHECK(grid1x5.diagonalLength(4) == 1);
	}

	SUBCASE("count")
	{
	}

	SUBCASE("findCell")
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

		aoc::BoundedGrid2D<char> grid(lines);

		CHECK(grid.findCell('^') == aoc::Point2s(4, 6));
	}

	SUBCASE("toString")
	{
		aoc::BoundedGrid2D<char> grid(4, 3, '.');

		grid.setCell(1, 0, '*');
		grid.setCell(2, 1, '@');
		grid.setCell(0, 2, '#');
		grid.setCell(3, 2, '!');

		const char* expectedGridStr = ".*..\n"
		                              "..@.\n"
		                              "#..!\n";

		CHECK(grid.toString() == expectedGridStr);
	}

	SUBCASE("row")
	{
		const vector<string> lines =
		{
			"00000000000000",
			"11111111111111",
			"22222222222222",
		};

		aoc::BoundedGrid2D<char> grid(lines);

		CHECK(grid.row(0).toString() == "00000000000000");
		CHECK(grid.row(1).toString() == "11111111111111");
		CHECK(grid.row(2).toString() == "22222222222222");

		aoc::BoundedGrid2D<char>::Row row1 = grid.row(1);

		for(size_t i=0; i<row1.length(); ++i)
			CHECK(row1[i] == '1');

		CHECK(grid.constRow(0).toString() == "00000000000000");
		CHECK(grid.constRow(1).toString() == "11111111111111");
		CHECK(grid.constRow(2).toString() == "22222222222222");

		aoc::BoundedGrid2D<char>::ConstRow constRow1 = grid.constRow(1);

		for(size_t i=0; i<constRow1.length(); ++i)
			CHECK(constRow1[i] == '1');
	}

	SUBCASE("reverseRow")
	{
		const vector<string> lines =
		{
			"abcdefghijklm",
			"nopqrstuvwxyz",
		};

		aoc::BoundedGrid2D<char> grid(lines);

		CHECK(grid.reverseRow(0).toString() == "mlkjihgfedcba");
		CHECK(grid.reverseRow(1).toString() == "zyxwvutsrqpon");

		aoc::BoundedGrid2D<char>::ReverseRow reverseRow0 = grid.reverseRow(0);

		for(size_t i=0; i<reverseRow0.length(); ++i)
			CHECK(reverseRow0[i] == lines[0][13-i-1]);

		CHECK(grid.constReverseRow(0).toString() == "mlkjihgfedcba");
		CHECK(grid.constReverseRow(1).toString() == "zyxwvutsrqpon");

		aoc::BoundedGrid2D<char>::ConstReverseRow constReverseRow0 = grid.constReverseRow(0);

		for(size_t i=0; i<constReverseRow0.length(); ++i)
			CHECK(constReverseRow0[i] == lines[0][13-i-1]);
	}

	SUBCASE("column")
	{
		const vector<string> lines =
		{
			"0ak",
			"1bl",
			"2cm",
			"3dn",
			"4eo",
			"5fp",
			"6gq",
			"7hr",
			"8is",
			"9jt",
		};

		aoc::BoundedGrid2D<char> grid(lines);

		CHECK(grid.column(0).toString() == "0123456789");
		CHECK(grid.column(1).toString() == "abcdefghij");
		CHECK(grid.column(2).toString() == "klmnopqrst");

		aoc::BoundedGrid2D<char>::Column column1 = grid.column(1);

		for(size_t i=0; i<column1.length(); ++i)
			CHECK(column1[i] == 'a' + i);

		CHECK(grid.constColumn(0).toString() == "0123456789");
		CHECK(grid.constColumn(1).toString() == "abcdefghij");
		CHECK(grid.constColumn(2).toString() == "klmnopqrst");

		aoc::BoundedGrid2D<char>::ConstColumn constColumn1 = grid.constColumn(1);

		for(size_t i=0; i<constColumn1.length(); ++i)
			CHECK(constColumn1[i] == 'a' + i);
	}

	SUBCASE("reverseColumn")
	{
		const vector<string> lines =
		{
			"an",
			"bo",
			"cp",
			"dq",
			"er",
			"fs",
			"gt",
			"hu",
			"iv",
			"jw",
			"kx",
			"ly",
			"mz",
		};

		aoc::BoundedGrid2D<char> grid(lines);

		CHECK(grid.reverseColumn(0).toString() == "mlkjihgfedcba");
		CHECK(grid.reverseColumn(1).toString() == "zyxwvutsrqpon");

		aoc::BoundedGrid2D<char>::ReverseColumn reverseColumn0 = grid.reverseColumn(0);

		for(size_t i=0; i<reverseColumn0.length(); ++i)
			CHECK(reverseColumn0[i] == lines[13-i-1][0]);

		CHECK(grid.constReverseColumn(0).toString() == "mlkjihgfedcba");
		CHECK(grid.constReverseColumn(1).toString() == "zyxwvutsrqpon");

		aoc::BoundedGrid2D<char>::ConstReverseColumn constReverseColumn0 = grid.constReverseColumn(0);

		for(size_t i=0; i<constReverseColumn0.length(); ++i)
			CHECK(constReverseColumn0[i] == lines[13-i-1][0]);
	}

	SUBCASE("topDownDiagonal")
	{
		const vector<string> lines1 =
		{
			"abcde",
			"fghij",
			"klmno",
		};

		aoc::BoundedGrid2D<char> grid(lines1);

		CHECK(grid.topDownDiagonal(0).toString() == "k");
		CHECK(grid.topDownDiagonal(1).toString() == "fl");
		CHECK(grid.topDownDiagonal(2).toString() == "agm");
		CHECK(grid.topDownDiagonal(3).toString() == "bhn");
		CHECK(grid.topDownDiagonal(4).toString() == "cio");
		CHECK(grid.topDownDiagonal(5).toString() == "dj");
		CHECK(grid.topDownDiagonal(6).toString() == "e");

		const vector<string> lines2 =
		{
			"abcd",
			"efgh",
			"ijkl",
			"mnop",
		};

		grid.reset(lines2);

		CHECK(grid.topDownDiagonal(0).toString() == "m");
		CHECK(grid.topDownDiagonal(1).toString() == "in");
		CHECK(grid.topDownDiagonal(2).toString() == "ejo");
		CHECK(grid.topDownDiagonal(3).toString() == "afkp");
		CHECK(grid.topDownDiagonal(4).toString() == "bgl");
		CHECK(grid.topDownDiagonal(5).toString() == "ch");
		CHECK(grid.topDownDiagonal(6).toString() == "d");

		const vector<string> lines3 =
		{
			"abcd",
		};

		grid.reset(lines3);

		CHECK(grid.topDownDiagonal(0).toString() == "a");
		CHECK(grid.topDownDiagonal(1).toString() == "b");
		CHECK(grid.topDownDiagonal(2).toString() == "c");
		CHECK(grid.topDownDiagonal(3).toString() == "d");

		const vector<string> lines4 =
		{
			"abcdefgh",
		};

		grid.reset(lines4);

		CHECK(grid.topDownDiagonal(0).toString() == "a");
		CHECK(grid.topDownDiagonal(1).toString() == "b");
		CHECK(grid.topDownDiagonal(2).toString() == "c");
		CHECK(grid.topDownDiagonal(3).toString() == "d");
		CHECK(grid.topDownDiagonal(4).toString() == "e");
		CHECK(grid.topDownDiagonal(5).toString() == "f");
		CHECK(grid.topDownDiagonal(6).toString() == "g");
		CHECK(grid.topDownDiagonal(7).toString() == "h");
	}

	SUBCASE("constTopDownDiagonal")
	{
		const vector<string> lines1 =
		{
			"abcde",
			"fghij",
			"klmno",
		};

		aoc::BoundedGrid2D<char> grid(lines1);

		CHECK(grid.constTopDownDiagonal(0).toString() == "k");
		CHECK(grid.constTopDownDiagonal(1).toString() == "fl");
		CHECK(grid.constTopDownDiagonal(2).toString() == "agm");
		CHECK(grid.constTopDownDiagonal(3).toString() == "bhn");
		CHECK(grid.constTopDownDiagonal(4).toString() == "cio");
		CHECK(grid.constTopDownDiagonal(5).toString() == "dj");
		CHECK(grid.constTopDownDiagonal(6).toString() == "e");

		const vector<string> lines2 =
		{
			"abcd",
			"efgh",
			"ijkl",
			"mnop",
		};

		grid.reset(lines2);

		CHECK(grid.constTopDownDiagonal(0).toString() == "m");
		CHECK(grid.constTopDownDiagonal(1).toString() == "in");
		CHECK(grid.constTopDownDiagonal(2).toString() == "ejo");
		CHECK(grid.constTopDownDiagonal(3).toString() == "afkp");
		CHECK(grid.constTopDownDiagonal(4).toString() == "bgl");
		CHECK(grid.constTopDownDiagonal(5).toString() == "ch");
		CHECK(grid.constTopDownDiagonal(6).toString() == "d");

		const vector<string> lines3 =
		{
			"abcd",
		};

		grid.reset(lines3);

		CHECK(grid.constTopDownDiagonal(0).toString() == "a");
		CHECK(grid.constTopDownDiagonal(1).toString() == "b");
		CHECK(grid.constTopDownDiagonal(2).toString() == "c");
		CHECK(grid.constTopDownDiagonal(3).toString() == "d");

		const vector<string> lines4 =
		{
			"abcdefgh",
		};

		grid.reset(lines4);

		CHECK(grid.constTopDownDiagonal(0).toString() == "a");
		CHECK(grid.constTopDownDiagonal(1).toString() == "b");
		CHECK(grid.constTopDownDiagonal(2).toString() == "c");
		CHECK(grid.constTopDownDiagonal(3).toString() == "d");
		CHECK(grid.constTopDownDiagonal(4).toString() == "e");
		CHECK(grid.constTopDownDiagonal(5).toString() == "f");
		CHECK(grid.constTopDownDiagonal(6).toString() == "g");
		CHECK(grid.constTopDownDiagonal(7).toString() == "h");
	}

	SUBCASE("reverseTopDownDiagonal")
	{
		const vector<string> lines1 =
		{
			"abcde",
			"fghij",
			"klmno",
		};

		aoc::BoundedGrid2D<char> grid(lines1);

		CHECK(grid.reverseTopDownDiagonal(0).toString() == "k");
		CHECK(grid.reverseTopDownDiagonal(1).toString() == "lf");
		CHECK(grid.reverseTopDownDiagonal(2).toString() == "mga");
		CHECK(grid.reverseTopDownDiagonal(3).toString() == "nhb");
		CHECK(grid.reverseTopDownDiagonal(4).toString() == "oic");
		CHECK(grid.reverseTopDownDiagonal(5).toString() == "jd");
		CHECK(grid.reverseTopDownDiagonal(6).toString() == "e");

		const vector<string> lines2 =
		{
			"abcd",
			"efgh",
			"ijkl",
			"mnop",
		};

		grid.reset(lines2);

		CHECK(grid.reverseTopDownDiagonal(0).toString() == "m");
		CHECK(grid.reverseTopDownDiagonal(1).toString() == "ni");
		CHECK(grid.reverseTopDownDiagonal(2).toString() == "oje");
		CHECK(grid.reverseTopDownDiagonal(3).toString() == "pkfa");
		CHECK(grid.reverseTopDownDiagonal(4).toString() == "lgb");
		CHECK(grid.reverseTopDownDiagonal(5).toString() == "hc");
		CHECK(grid.reverseTopDownDiagonal(6).toString() == "d");

		const vector<string> lines3 =
		{
			"abcd",
		};

		grid.reset(lines3);

		CHECK(grid.reverseTopDownDiagonal(0).toString() == "a");
		CHECK(grid.reverseTopDownDiagonal(1).toString() == "b");
		CHECK(grid.reverseTopDownDiagonal(2).toString() == "c");
		CHECK(grid.reverseTopDownDiagonal(3).toString() == "d");

		const vector<string> lines4 =
		{
			"abcdefgh",
		};

		grid.reset(lines4);

		CHECK(grid.reverseTopDownDiagonal(0).toString() == "a");
		CHECK(grid.reverseTopDownDiagonal(1).toString() == "b");
		CHECK(grid.reverseTopDownDiagonal(2).toString() == "c");
		CHECK(grid.reverseTopDownDiagonal(3).toString() == "d");
		CHECK(grid.reverseTopDownDiagonal(4).toString() == "e");
		CHECK(grid.reverseTopDownDiagonal(5).toString() == "f");
		CHECK(grid.reverseTopDownDiagonal(6).toString() == "g");
		CHECK(grid.reverseTopDownDiagonal(7).toString() == "h");
	}

	SUBCASE("constReverseTopDownDiagonal")
	{
		const vector<string> lines1 =
		{
			"abcde",
			"fghij",
			"klmno",
		};

		aoc::BoundedGrid2D<char> grid(lines1);

		CHECK(grid.constReverseTopDownDiagonal(0).toString() == "k");
		CHECK(grid.constReverseTopDownDiagonal(1).toString() == "lf");
		CHECK(grid.constReverseTopDownDiagonal(2).toString() == "mga");
		CHECK(grid.constReverseTopDownDiagonal(3).toString() == "nhb");
		CHECK(grid.constReverseTopDownDiagonal(4).toString() == "oic");
		CHECK(grid.constReverseTopDownDiagonal(5).toString() == "jd");
		CHECK(grid.constReverseTopDownDiagonal(6).toString() == "e");

		const vector<string> lines2 =
		{
			"abcd",
			"efgh",
			"ijkl",
			"mnop",
		};

		grid.reset(lines2);

		CHECK(grid.constReverseTopDownDiagonal(0).toString() == "m");
		CHECK(grid.constReverseTopDownDiagonal(1).toString() == "ni");
		CHECK(grid.constReverseTopDownDiagonal(2).toString() == "oje");
		CHECK(grid.constReverseTopDownDiagonal(3).toString() == "pkfa");
		CHECK(grid.constReverseTopDownDiagonal(4).toString() == "lgb");
		CHECK(grid.constReverseTopDownDiagonal(5).toString() == "hc");
		CHECK(grid.constReverseTopDownDiagonal(6).toString() == "d");

		const vector<string> lines3 =
		{
			"abcd",
		};

		grid.reset(lines3);

		CHECK(grid.constReverseTopDownDiagonal(0).toString() == "a");
		CHECK(grid.constReverseTopDownDiagonal(1).toString() == "b");
		CHECK(grid.constReverseTopDownDiagonal(2).toString() == "c");
		CHECK(grid.constReverseTopDownDiagonal(3).toString() == "d");

		const vector<string> lines4 =
		{
			"abcdefgh",
		};

		grid.reset(lines4);

		CHECK(grid.constReverseTopDownDiagonal(0).toString() == "a");
		CHECK(grid.constReverseTopDownDiagonal(1).toString() == "b");
		CHECK(grid.constReverseTopDownDiagonal(2).toString() == "c");
		CHECK(grid.constReverseTopDownDiagonal(3).toString() == "d");
		CHECK(grid.constReverseTopDownDiagonal(4).toString() == "e");
		CHECK(grid.constReverseTopDownDiagonal(5).toString() == "f");
		CHECK(grid.constReverseTopDownDiagonal(6).toString() == "g");
		CHECK(grid.constReverseTopDownDiagonal(7).toString() == "h");
	}

	SUBCASE("bottomUpDiagonal")
	{
		const vector<string> lines1 =
		{
			"abcde",
			"fghij",
			"klmno",
		};

		aoc::BoundedGrid2D<char> grid(lines1);

		CHECK(grid.bottomUpDiagonal(0).toString() == "a");
		CHECK(grid.bottomUpDiagonal(1).toString() == "fb");
		CHECK(grid.bottomUpDiagonal(2).toString() == "kgc");
		CHECK(grid.bottomUpDiagonal(3).toString() == "lhd");
		CHECK(grid.bottomUpDiagonal(4).toString() == "mie");
		CHECK(grid.bottomUpDiagonal(5).toString() == "nj");
		CHECK(grid.bottomUpDiagonal(6).toString() == "o");

		const vector<string> lines2 =
		{
			"abcd",
			"efgh",
			"ijkl",
			"mnop",
		};

		grid.reset(lines2);

		CHECK(grid.bottomUpDiagonal(0).toString() == "a");
		CHECK(grid.bottomUpDiagonal(1).toString() == "eb");
		CHECK(grid.bottomUpDiagonal(2).toString() == "ifc");
		CHECK(grid.bottomUpDiagonal(3).toString() == "mjgd");
		CHECK(grid.bottomUpDiagonal(4).toString() == "nkh");
		CHECK(grid.bottomUpDiagonal(5).toString() == "ol");
		CHECK(grid.bottomUpDiagonal(6).toString() == "p");

		const vector<string> lines3 =
		{
			"abcd",
		};

		grid.reset(lines3);

		CHECK(grid.bottomUpDiagonal(0).toString() == "a");
		CHECK(grid.bottomUpDiagonal(1).toString() == "b");
		CHECK(grid.bottomUpDiagonal(2).toString() == "c");
		CHECK(grid.bottomUpDiagonal(3).toString() == "d");

		const vector<string> lines4 =
		{
			"abcdefgh",
		};

		grid.reset(lines4);

		CHECK(grid.bottomUpDiagonal(0).toString() == "a");
		CHECK(grid.bottomUpDiagonal(1).toString() == "b");
		CHECK(grid.bottomUpDiagonal(2).toString() == "c");
		CHECK(grid.bottomUpDiagonal(3).toString() == "d");
		CHECK(grid.bottomUpDiagonal(4).toString() == "e");
		CHECK(grid.bottomUpDiagonal(5).toString() == "f");
		CHECK(grid.bottomUpDiagonal(6).toString() == "g");
		CHECK(grid.bottomUpDiagonal(7).toString() == "h");
	}

	SUBCASE("constBottomUpDiagonal")
	{
		const vector<string> lines1 =
		{
			"abcde",
			"fghij",
			"klmno",
		};

		aoc::BoundedGrid2D<char> grid(lines1);

		CHECK(grid.constBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.constBottomUpDiagonal(1).toString() == "fb");
		CHECK(grid.constBottomUpDiagonal(2).toString() == "kgc");
		CHECK(grid.constBottomUpDiagonal(3).toString() == "lhd");
		CHECK(grid.constBottomUpDiagonal(4).toString() == "mie");
		CHECK(grid.constBottomUpDiagonal(5).toString() == "nj");
		CHECK(grid.constBottomUpDiagonal(6).toString() == "o");

		const vector<string> lines2 =
		{
			"abcd",
			"efgh",
			"ijkl",
			"mnop",
		};

		grid.reset(lines2);

		CHECK(grid.constBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.constBottomUpDiagonal(1).toString() == "eb");
		CHECK(grid.constBottomUpDiagonal(2).toString() == "ifc");
		CHECK(grid.constBottomUpDiagonal(3).toString() == "mjgd");
		CHECK(grid.constBottomUpDiagonal(4).toString() == "nkh");
		CHECK(grid.constBottomUpDiagonal(5).toString() == "ol");
		CHECK(grid.constBottomUpDiagonal(6).toString() == "p");

		const vector<string> lines3 =
		{
			"abcd",
		};

		grid.reset(lines3);

		CHECK(grid.constBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.constBottomUpDiagonal(1).toString() == "b");
		CHECK(grid.constBottomUpDiagonal(2).toString() == "c");
		CHECK(grid.constBottomUpDiagonal(3).toString() == "d");

		const vector<string> lines4 =
		{
			"abcdefgh",
		};

		grid.reset(lines4);

		CHECK(grid.constBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.constBottomUpDiagonal(1).toString() == "b");
		CHECK(grid.constBottomUpDiagonal(2).toString() == "c");
		CHECK(grid.constBottomUpDiagonal(3).toString() == "d");
		CHECK(grid.constBottomUpDiagonal(4).toString() == "e");
		CHECK(grid.constBottomUpDiagonal(5).toString() == "f");
		CHECK(grid.constBottomUpDiagonal(6).toString() == "g");
		CHECK(grid.constBottomUpDiagonal(7).toString() == "h");
	}

	SUBCASE("reverseBottomUpDiagonal")
	{
		const vector<string> lines1 =
		{
			"abcde",
			"fghij",
			"klmno",
		};

		aoc::BoundedGrid2D<char> grid(lines1);

		CHECK(grid.reverseBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.reverseBottomUpDiagonal(1).toString() == "bf");
		CHECK(grid.reverseBottomUpDiagonal(2).toString() == "cgk");
		CHECK(grid.reverseBottomUpDiagonal(3).toString() == "dhl");
		CHECK(grid.reverseBottomUpDiagonal(4).toString() == "eim");
		CHECK(grid.reverseBottomUpDiagonal(5).toString() == "jn");
		CHECK(grid.reverseBottomUpDiagonal(6).toString() == "o");

		const vector<string> lines2 =
		{
			"abcd",
			"efgh",
			"ijkl",
			"mnop",
		};

		grid.reset(lines2);

		CHECK(grid.reverseBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.reverseBottomUpDiagonal(1).toString() == "be");
		CHECK(grid.reverseBottomUpDiagonal(2).toString() == "cfi");
		CHECK(grid.reverseBottomUpDiagonal(3).toString() == "dgjm");
		CHECK(grid.reverseBottomUpDiagonal(4).toString() == "hkn");
		CHECK(grid.reverseBottomUpDiagonal(5).toString() == "lo");
		CHECK(grid.reverseBottomUpDiagonal(6).toString() == "p");

		const vector<string> lines3 =
		{
			"abcd",
		};

		grid.reset(lines3);

		CHECK(grid.reverseBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.reverseBottomUpDiagonal(1).toString() == "b");
		CHECK(grid.reverseBottomUpDiagonal(2).toString() == "c");
		CHECK(grid.reverseBottomUpDiagonal(3).toString() == "d");

		const vector<string> lines4 =
		{
			"abcdefgh",
		};

		grid.reset(lines4);

		CHECK(grid.reverseBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.reverseBottomUpDiagonal(1).toString() == "b");
		CHECK(grid.reverseBottomUpDiagonal(2).toString() == "c");
		CHECK(grid.reverseBottomUpDiagonal(3).toString() == "d");
		CHECK(grid.reverseBottomUpDiagonal(4).toString() == "e");
		CHECK(grid.reverseBottomUpDiagonal(5).toString() == "f");
		CHECK(grid.reverseBottomUpDiagonal(6).toString() == "g");
		CHECK(grid.reverseBottomUpDiagonal(7).toString() == "h");
	}

	SUBCASE("constReverseBottomUpDiagonal")
	{
		const vector<string> lines1 =
		{
			"abcde",
			"fghij",
			"klmno",
		};

		aoc::BoundedGrid2D<char> grid(lines1);

		CHECK(grid.constReverseBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.constReverseBottomUpDiagonal(1).toString() == "bf");
		CHECK(grid.constReverseBottomUpDiagonal(2).toString() == "cgk");
		CHECK(grid.constReverseBottomUpDiagonal(3).toString() == "dhl");
		CHECK(grid.constReverseBottomUpDiagonal(4).toString() == "eim");
		CHECK(grid.constReverseBottomUpDiagonal(5).toString() == "jn");
		CHECK(grid.constReverseBottomUpDiagonal(6).toString() == "o");

		const vector<string> lines2 =
		{
			"abcd",
			"efgh",
			"ijkl",
			"mnop",
		};

		grid.reset(lines2);

		CHECK(grid.constReverseBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.constReverseBottomUpDiagonal(1).toString() == "be");
		CHECK(grid.constReverseBottomUpDiagonal(2).toString() == "cfi");
		CHECK(grid.constReverseBottomUpDiagonal(3).toString() == "dgjm");
		CHECK(grid.constReverseBottomUpDiagonal(4).toString() == "hkn");
		CHECK(grid.constReverseBottomUpDiagonal(5).toString() == "lo");
		CHECK(grid.constReverseBottomUpDiagonal(6).toString() == "p");

		const vector<string> lines3 =
		{
			"abcd",
		};

		grid.reset(lines3);

		CHECK(grid.constReverseBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.constReverseBottomUpDiagonal(1).toString() == "b");
		CHECK(grid.constReverseBottomUpDiagonal(2).toString() == "c");
		CHECK(grid.constReverseBottomUpDiagonal(3).toString() == "d");

		const vector<string> lines4 =
		{
			"abcdefgh",
		};

		grid.reset(lines4);

		CHECK(grid.constReverseBottomUpDiagonal(0).toString() == "a");
		CHECK(grid.constReverseBottomUpDiagonal(1).toString() == "b");
		CHECK(grid.constReverseBottomUpDiagonal(2).toString() == "c");
		CHECK(grid.constReverseBottomUpDiagonal(3).toString() == "d");
		CHECK(grid.constReverseBottomUpDiagonal(4).toString() == "e");
		CHECK(grid.constReverseBottomUpDiagonal(5).toString() == "f");
		CHECK(grid.constReverseBottomUpDiagonal(6).toString() == "g");
		CHECK(grid.constReverseBottomUpDiagonal(7).toString() == "h");
	}

	SUBCASE("adjacentNeighbors")
	{
		const vector<string> lines1 =
		{
			"abcde",
			"fghij",
			"klmno",
		};

		aoc::BoundedGrid2D<char> grid(lines1);

		CHECK(grid.adjacentNeighbors(0, 0).toString() == "bf");
		CHECK(grid.adjacentNeighbors(0, 1).toString() == "agk");
		CHECK(grid.adjacentNeighbors(0, 2).toString() == "fl");

		CHECK(grid.adjacentNeighbors(2, 0).toString() == "bdh");
		CHECK(grid.adjacentNeighbors(2, 1).toString() == "cgim");
		CHECK(grid.adjacentNeighbors(2, 2).toString() == "hln");

		CHECK(grid.adjacentNeighbors(4, 0).toString() == "dj");
		CHECK(grid.adjacentNeighbors(4, 1).toString() == "eio");
		CHECK(grid.adjacentNeighbors(4, 2).toString() == "jn");
	}
}

TEST_CASE("MathUtils")
{
	SUBCASE("isInRange")
	{
		CHECK(aoc::MathUtils::isInRange(1, 1, 10) == true);
		CHECK(aoc::MathUtils::isInRange(5, 1, 10) == true);
		CHECK(aoc::MathUtils::isInRange(10, 1, 10) == true);
		CHECK(aoc::MathUtils::isInRange(0, 1, 10) == false);
		CHECK(aoc::MathUtils::isInRange(11, 1, 10) == false);
		CHECK(aoc::MathUtils::isInRange(-10, 1, 10) == false);
		CHECK(aoc::MathUtils::isInRange(20, 1, 10) == false);
		CHECK(aoc::MathUtils::isInRange(3, -1, 3) == true);
	}

	SUBCASE("manhattanDistance")
	{
		CHECK(aoc::MathUtils::manhattanDistance(0, 0, 0, 0) == 0);
		CHECK(aoc::MathUtils::manhattanDistance(0, 0, 1, 0) == 1);
		CHECK(aoc::MathUtils::manhattanDistance(0, 0, 0, 1) == 1);
		CHECK(aoc::MathUtils::manhattanDistance(0, 0, 1, 1) == 2);
		CHECK(aoc::MathUtils::manhattanDistance(1, 6, -1, 5) == 3);
		CHECK(aoc::MathUtils::manhattanDistance(3, 5, -1, 5) == 4);
		CHECK(aoc::MathUtils::manhattanDistance(2, 3, -1, 5) == 5);
		CHECK(aoc::MathUtils::manhattanDistance(-6, 0, 0, 0) == 6);
		CHECK(aoc::MathUtils::manhattanDistance(1, 1, 4, 5) == 7);
	}
}

TEST_CASE("StringUtils")
{
	SUBCASE("contains(const string& str, const string& substr)")
	{
		CHECK(aoc::StringUtils::contains("abc", "a"));
		CHECK(aoc::StringUtils::contains("abc", "b"));
		CHECK(aoc::StringUtils::contains("abc", "c"));
		CHECK(aoc::StringUtils::contains("abc", "ab"));
		CHECK(aoc::StringUtils::contains("abc", "bc"));
		CHECK(aoc::StringUtils::contains("abc", "abc"));
		CHECK(!aoc::StringUtils::contains("abc", "x"));
		CHECK(!aoc::StringUtils::contains("abc", "ac"));
		CHECK(!aoc::StringUtils::contains("abc", "acb"));
	}

	SUBCASE("contains(const string& str, char ch)")
	{
		CHECK(aoc::StringUtils::contains("abc", 'a'));
		CHECK(aoc::StringUtils::contains("abc", 'b'));
		CHECK(aoc::StringUtils::contains("abc", 'c'));
		CHECK(!aoc::StringUtils::contains("abc", 'x'));
	}

	SUBCASE("countOverlappingSubstrings")
	{
		CHECK(aoc::StringUtils::countOverlappingSubstrings("", "a") == 0);
		CHECK(aoc::StringUtils::countOverlappingSubstrings("a", "a") == 1);
		CHECK(aoc::StringUtils::countOverlappingSubstrings("aa", "a") == 2);
		CHECK(aoc::StringUtils::countOverlappingSubstrings("aaa", "a") == 3);
		CHECK(aoc::StringUtils::countOverlappingSubstrings("a", "aa") == 0);

		CHECK(aoc::StringUtils::countOverlappingSubstrings("", "bb") == 0);
		CHECK(aoc::StringUtils::countOverlappingSubstrings("bb", "bb") == 1);
		CHECK(aoc::StringUtils::countOverlappingSubstrings("bbb", "bb") == 2);
		CHECK(aoc::StringUtils::countOverlappingSubstrings("bbbb", "bb") == 3);
		CHECK(aoc::StringUtils::countOverlappingSubstrings("bb", "bbb") == 0);

		CHECK(aoc::StringUtils::countOverlappingSubstrings("aaabbaaa", "aa") == 4);
	}

	SUBCASE("countNonOverlappingSubstrings")
	{
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("", "a") == 0);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("a", "a") == 1);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("aa", "a") == 2);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("aaa", "a") == 3);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("a", "aa") == 0);

		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("", "bb") == 0);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("bb", "bb") == 1);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("bbb", "bb") == 1);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("bbbb", "bb") == 2);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("bb", "bbb") == 0);

		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("aaabbaaa", "aa") == 2);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("aaaaaaa", "aa") == 3);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("aaaaaaaa", "aa") == 4);
		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("aaaaaaaaa", "aa") == 4);

		CHECK(aoc::StringUtils::countNonOverlappingSubstrings("MMMSXXMASM", "XMAS") == 1);
	}

	SUBCASE("replaceAll")
	{
		CHECK(aoc::StringUtils::replaceAll("one", "one", "1") == "1");
		CHECK(aoc::StringUtils::replaceAll("twotwo", "two", "2") == "22");
		CHECK(aoc::StringUtils::replaceAll("twoonetwo", "two", "2") == "2one2");
		CHECK(aoc::StringUtils::replaceAll("one two three three", "three", "3") == "one two 3 3");
		CHECK(aoc::StringUtils::replaceAll("one twoone three one one threeoneone", "one", "1") == "1 two1 three 1 1 three11");
		CHECK(aoc::StringUtils::replaceAll("two1nine", "two", "2") == "21nine");
	}

	SUBCASE("tokenize")
	{
		CHECK(aoc::StringUtils::tokenize("15 red, 1 green, 3 blue") == vector<string>({ "15 red", "1 green", "3 blue"}));
		CHECK(aoc::StringUtils::tokenize("7  15   30", ' ', true, true) == vector<string>({ "7", "15", "30"}));
	}

	SUBCASE("trim(str)")
	{
		CHECK(aoc::StringUtils::trim(" test ") == "test");
	}

	SUBCASE("ltrim(str)")
	{
		CHECK(aoc::StringUtils::ltrim(" test") == "test");
	}

	SUBCASE("rtrim(str)")
	{
		CHECK(aoc::StringUtils::rtrim("test ") == "test");
	}

	SUBCASE("trim(str, char)")
	{
		CHECK(aoc::StringUtils::trim("=test=", '=') == "test");
	}

	SUBCASE("ltrim(str, char)")
	{
		CHECK(aoc::StringUtils::ltrim("_test", '_') == "test");
	}

	SUBCASE("rtrim(str, char)")
	{
		CHECK(aoc::StringUtils::rtrim("test#", '#') == "test");
	}

	SUBCASE("generateVariations")
	{
		vector<string> variations;

		aoc::StringUtils::generateVariations(set<char>({ 'a', 'b', 'c' }),
		                                     2,
		                                     variations);

		CHECK(variations == vector<string>({ "aa", "ab", "ac", "ba", "bb", "bc", "ca", "cb", "cc" }));
	}
}

#endif // #if defined(AOC_TEST)
