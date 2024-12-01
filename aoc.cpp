#include "aoc.h"

#if defined(AOC_TEST)
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#include <fstream>
#include <regex>
#include <sstream>

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

bool StringUtils::contains(const std::string& str,
                           const std::string& substr)
{
	return str.find(substr) != string::npos;
}

bool StringUtils::contains(const std::string& str, char ch)
{
	const size_t length = str.size();

	for(size_t i=0; i<length; ++i)
		if(str[i] == ch)
			return true;

	return false;
}

string StringUtils::replaceFirst(const string& str,
                                 const string& substr,
                                 const string& substr2)
{
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

} // namespace aoc

#if defined(AOC_TEST)

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

	SUBCASE("trim")
	{
		CHECK(aoc::StringUtils::trim(" test ") == "test");
	}

	SUBCASE("ltrim")
	{
		CHECK(aoc::StringUtils::trim(" test") == "test");
	}

	SUBCASE("rtrim")
	{
		CHECK(aoc::StringUtils::trim("test ") == "test");
	}
}

#endif // #if defined(AOC_TEST)
