#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <string>
#include <tuple>

using namespace fmt;
using namespace aoc;
using namespace std;

const char* digits = "1234567890";

struct DigitNameToDigit
{
	const char* digitName_;
	char digit_;
};

DigitNameToDigit digitNameToDigitLUT[] =
{
	{ "zero", '0' },
	{ "one", '1' },
	{ "two", '2' },
	{ "three", '3' },
	{ "four", '4' },
	{ "five", '5' },
	{ "six", '6' },
	{ "seven", '7' },
	{ "eight", '8' },
	{ "nine", '9' },
};

tuple<size_t, char> findFirstDigitByChar(const string& line)
{
	const size_t pos = line.find_first_of(digits);

	return (pos != string::npos) ? make_tuple(pos, line[pos])
	                             : make_tuple(string::npos, '\0');
}

tuple<size_t, char> findLastDigitByChar(const string& line)
{
	const size_t pos = line.find_last_of(digits);

	return (pos != string::npos) ? make_tuple(pos, line[pos])
	                             : make_tuple(string::npos, '\0');
}

tuple<size_t, char> findFirstDigitByName(const string& line)
{
	size_t firstDigitNamePos = line.size();
	size_t firstDigitNameIndex = 0;
	bool digitNameFound = false;

	for(size_t i=0; i<AOC_NUM_ELEMENTS(digitNameToDigitLUT); ++i)
	{
		const size_t pos = line.find(digitNameToDigitLUT[i].digitName_);

		if(pos == string::npos)
			continue;

		digitNameFound = true;

		if(pos < firstDigitNamePos)
		{
			firstDigitNamePos = pos;
			firstDigitNameIndex = i;
		}
	}

	if(digitNameFound)
		return make_tuple(firstDigitNamePos,
		                  digitNameToDigitLUT[firstDigitNameIndex].digit_);
	else
		return make_tuple(string::npos, '\0');
}

tuple<size_t, char> findLastDigitByName(const string& line)
{
	size_t lastDigitNamePos = 0;
	size_t lastDigitNameIndex = 0;
	bool digitNameFound = false;

	for(size_t i=0; i<AOC_NUM_ELEMENTS(digitNameToDigitLUT); ++i)
	{
		const size_t pos = line.rfind(digitNameToDigitLUT[i].digitName_);

		if(pos == string::npos)
			continue;

		digitNameFound = true;

		if(pos > lastDigitNamePos)
		{
			lastDigitNamePos = pos;
			lastDigitNameIndex = i;
		}
	}

	if(digitNameFound)
		return make_tuple(lastDigitNamePos,
		                  digitNameToDigitLUT[lastDigitNameIndex].digit_);
	else
		return make_tuple(string::npos, '\0');
}

char findFirstDigit(const string& line)
{
	const auto [firstDigitFoundByCharPos, firstDigitFoundByChar] =
		findFirstDigitByChar(line);

	const auto [firstDigitFoundByNamePos, firstDigitFoundByName] =
		findFirstDigitByName(line);

	if(firstDigitFoundByCharPos == string::npos &&
	   firstDigitFoundByNamePos == string::npos)
		panic(format("at least one digit (either character or name) expected in input line: '{}'", line));
	else if(firstDigitFoundByCharPos != string::npos &&
	        firstDigitFoundByNamePos == string::npos)
		return firstDigitFoundByChar;
	else if(firstDigitFoundByCharPos == string::npos &&
	        firstDigitFoundByNamePos != string::npos)
		return firstDigitFoundByName;
	else
		return (firstDigitFoundByCharPos < firstDigitFoundByNamePos) ? firstDigitFoundByChar
		                                                             : firstDigitFoundByName;

	return '\0';
}

char findLastDigit(const string& line)
{
	const auto [lastDigitFoundByCharPos, lastDigitFoundByChar] =
		findLastDigitByChar(line);

	const auto [lastDigitFoundByNamePos, lastDigitFoundByName] =
		findLastDigitByName(line);

	if(lastDigitFoundByCharPos == string::npos &&
	   lastDigitFoundByNamePos == string::npos)
		panic(format("at least one digit (either character or name) expected in input line: '{}'", line));
	else if(lastDigitFoundByCharPos != string::npos &&
	        lastDigitFoundByNamePos == string::npos)
		return lastDigitFoundByChar;
	else if(lastDigitFoundByCharPos == string::npos &&
	        lastDigitFoundByNamePos != string::npos)
		return lastDigitFoundByName;
	else
		return (lastDigitFoundByCharPos > lastDigitFoundByNamePos) ? lastDigitFoundByChar
		                                                           : lastDigitFoundByName;

	return '\0';
}

int digitToInt(char digit)
{
	if(!isdigit(digit))
		panic(format("'{}' is not a digit", digit));

	return int(digit - '0');
}

int digitsToInt(char digit1, char digit2)
{
	return 10 * digitToInt(digit1) + digitToInt(digit2);
}

int extractCalibrationValue1(const string& line)
{
	const auto [firstDigitPos, firstDigit] = findFirstDigitByChar(line);

	if(firstDigitPos == string::npos)
		panic(format("at least one digit character expected in input line: '{}'", line));

	const auto [lastDigitPos, lastDigit] = findLastDigitByChar(line);

	AOC_ASSERT(lastDigitPos != string::npos);

	return digitsToInt(firstDigit, lastDigit);
}

int extractCalibrationValue2(const string& line)
{
	return digitsToInt(findFirstDigit(line), findLastDigit(line));
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("findFirstDigitByChar")
{
	CHECK(findFirstDigitByChar("1abc2") == make_tuple(0, '1'));
	CHECK(findFirstDigitByChar("pqr3stu8vwx") == make_tuple(3, '3'));
	CHECK(findFirstDigitByChar("a1b2c3d4e5f") == make_tuple(1, '1'));
	CHECK(findFirstDigitByChar("treb7uchet") == make_tuple(4, '7'));
}

TEST_CASE("findLastDigitByChar")
{
	CHECK(findLastDigitByChar("1abc2") == make_tuple(4, '2'));
	CHECK(findLastDigitByChar("pqr3stu8vwx") == make_tuple(7, '8'));
	CHECK(findLastDigitByChar("a1b2c3d4e5f") == make_tuple(9, '5'));
	CHECK(findLastDigitByChar("treb7uchet") == make_tuple(4, '7'));
}

TEST_CASE("findFirstDigitByName")
{
	CHECK(findFirstDigitByName("two1nine") == make_tuple(0, '2'));
	CHECK(findFirstDigitByName("eightwothree") == make_tuple(0, '8'));
	CHECK(findFirstDigitByName("abcone2threexyz") == make_tuple(3, '1'));
	CHECK(findFirstDigitByName("xtwone3four") == make_tuple(1, '2'));
	CHECK(findFirstDigitByName("4nineeightseven2") == make_tuple(1, '9'));
	CHECK(findFirstDigitByName("zoneight234") == make_tuple(1, '1'));
	CHECK(findFirstDigitByName("7pqrstsixteen") == make_tuple(6, '6'));
}

TEST_CASE("findLastDigitByName")
{
	CHECK(findLastDigitByName("two1nine") == make_tuple(4, '9'));
	CHECK(findLastDigitByName("eightwothree") == make_tuple(7, '3'));
	CHECK(findLastDigitByName("abcone2threexyz") == make_tuple(7, '3'));
	CHECK(findLastDigitByName("xtwone3four") == make_tuple(7, '4'));
	CHECK(findLastDigitByName("4nineeightseven2") == make_tuple(10, '7'));
	CHECK(findLastDigitByName("zoneight234") == make_tuple(3, '8'));
	CHECK(findLastDigitByName("7pqrstsixteen") == make_tuple(6, '6'));
}

TEST_CASE("findFirstDigit")
{
	CHECK(findFirstDigit("1abc2") == '1');
	CHECK(findFirstDigit("pqr3stu8vwx") == '3');
	CHECK(findFirstDigit("a1b2c3d4e5f") == '1');
	CHECK(findFirstDigit("treb7uchet") == '7');
	CHECK(findFirstDigit("two1nine") == '2');
	CHECK(findFirstDigit("eightwothree") == '8');
	CHECK(findFirstDigit("abcone2threexyz") == '1');
	CHECK(findFirstDigit("xtwone3four") == '2');
	CHECK(findFirstDigit("4nineeightseven2") == '4');
	CHECK(findFirstDigit("zoneight234") == '1');
	CHECK(findFirstDigit("7pqrstsixteen") == '7');
}

TEST_CASE("findLastDigit")
{
	CHECK(findLastDigit("1abc2") == '2');
	CHECK(findLastDigit("pqr3stu8vwx") == '8');
	CHECK(findLastDigit("a1b2c3d4e5f") == '5');
	CHECK(findLastDigit("treb7uchet") == '7');
	CHECK(findLastDigit("two1nine") == '9');
	CHECK(findLastDigit("eightwothree") == '3');
	CHECK(findLastDigit("abcone2threexyz") == '3');
	CHECK(findLastDigit("xtwone3four") == '4');
	CHECK(findLastDigit("4nineeightseven2") == '2');
	CHECK(findLastDigit("zoneight234") == '4');
	CHECK(findLastDigit("7pqrstsixteen") == '6');
}

TEST_CASE("digitToInt")
{
	CHECK(digitToInt('0') == 0);
	CHECK(digitToInt('1') == 1);
	CHECK(digitToInt('2') == 2);
	CHECK(digitToInt('3') == 3);
	CHECK(digitToInt('4') == 4);
	CHECK(digitToInt('5') == 5);
	CHECK(digitToInt('6') == 6);
	CHECK(digitToInt('7') == 7);
	CHECK(digitToInt('8') == 8);
	CHECK(digitToInt('9') == 9);
	CHECK_THROWS_WITH_AS(digitToInt('a'), "'a' is not a digit", runtime_error);
}

TEST_CASE("digitsToInt")
{
	CHECK(digitsToInt('1', '2') == 12);
	CHECK(digitsToInt('3', '8') == 38);
	CHECK(digitsToInt('1', '5') == 15);
	CHECK(digitsToInt('7', '7') == 77);
}

TEST_CASE("extractCalibrationValue1")
{
	CHECK(extractCalibrationValue1("0") == 0);
	CHECK(extractCalibrationValue1("00") == 0);
	CHECK(extractCalibrationValue1("0a0") == 0);
	CHECK(extractCalibrationValue1("1") == 11);
	CHECK(extractCalibrationValue1("11") == 11);
	CHECK(extractCalibrationValue1("1a1") == 11);
	CHECK(extractCalibrationValue1("01") == 1);
	CHECK(extractCalibrationValue1("0a1") == 1);
	CHECK(extractCalibrationValue1("10") == 10);
	CHECK(extractCalibrationValue1("1a0") == 10);
	CHECK(extractCalibrationValue1("1abc2") == 12);
	CHECK(extractCalibrationValue1("pqr3stu8vwx") == 38);
	CHECK(extractCalibrationValue1("a1b2c3d4e5f") == 15);
	CHECK(extractCalibrationValue1("treb7uchet") == 77);
	CHECK_THROWS_WITH_AS(extractCalibrationValue1(""), "at least one digit character expected in input line: ''", runtime_error);
	CHECK_THROWS_WITH_AS(extractCalibrationValue1("a"), "at least one digit character expected in input line: 'a'", runtime_error);
	CHECK_THROWS_WITH_AS(extractCalibrationValue1("abc"), "at least one digit character expected in input line: 'abc'", runtime_error);
}

TEST_CASE("extractCalibrationValue2")
{
	CHECK(extractCalibrationValue2("0") == 0);
	CHECK(extractCalibrationValue2("00") == 0);
	CHECK(extractCalibrationValue2("0a0") == 0);
	CHECK(extractCalibrationValue2("1") == 11);
	CHECK(extractCalibrationValue2("11") == 11);
	CHECK(extractCalibrationValue2("1a1") == 11);
	CHECK(extractCalibrationValue2("01") == 1);
	CHECK(extractCalibrationValue2("0a1") == 1);
	CHECK(extractCalibrationValue2("10") == 10);
	CHECK(extractCalibrationValue2("1a0") == 10);
	CHECK(extractCalibrationValue2("1abc2") == 12);
	CHECK(extractCalibrationValue2("pqr3stu8vwx") == 38);
	CHECK(extractCalibrationValue2("a1b2c3d4e5f") == 15);
	CHECK(extractCalibrationValue2("treb7uchet") == 77);
	CHECK(extractCalibrationValue2("two1nine") == 29);
	CHECK(extractCalibrationValue2("eightwothree") == 83);
	CHECK(extractCalibrationValue2("abcone2threexyz") == 13);
	CHECK(extractCalibrationValue2("xtwone3four") == 24);
	CHECK(extractCalibrationValue2("4nineeightseven2") == 42);
	CHECK(extractCalibrationValue2("zoneight234") == 14);
	CHECK(extractCalibrationValue2("7pqrstsixteen") == 76);
	CHECK_THROWS_WITH_AS(extractCalibrationValue2(""), "at least one digit (either character or name) expected in input line: ''", runtime_error);
	CHECK_THROWS_WITH_AS(extractCalibrationValue2("a"), "at least one digit (either character or name) expected in input line: 'a'", runtime_error);
	CHECK_THROWS_WITH_AS(extractCalibrationValue2("abc"), "at least one digit (either character or name) expected in input line: 'abc'", runtime_error);
}

#else // #ifdef AOC_TEST_SOLUTION

class Trebuchet : public PuzzleSolution<int>
{
private:
	int answer1() override;
	int answer2() override;
};

int Trebuchet::answer1()
{
	int sum = 0;

	for(const auto& line : input())
		sum += extractCalibrationValue1(line);

	return sum;
}

int Trebuchet::answer2()
{
	int sum = 0;

	for(const auto& line : input())
		sum += extractCalibrationValue2(line);

	return sum;
}

int main(int argc, char* argv[])
{
	return Trebuchet().run((argc > 1) ? argv[1] : "202301.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
