#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <string>

using namespace fmt;
using namespace aoc;
using namespace std;

inline bool isDigit(char ch)
{
	return ch >= '0' && ch <= '9';
}

inline int digitToInt(char digit)
{
	AOC_ASSERT(isDigit(digit));

	return digit - '0';
}

class MemoryScanner
{
public:
	MemoryScanner(const string& memory)
		: memory_(&memory)
		, start_(0)
		, current_(0)
	{
	}

public:
	void reset(const string& memory)
	{
		memory_ = &memory;
		start_ = 0;
		current_ = 0;
	}

public:
	int scanMuls1();
	int scanMuls2();

private:
	char currentChar() const
	{
		AOC_ASSERT(!isAtEnd());

		return (*memory_)[current_];
	}

	size_t numRemainingChars() const
	{
		return isAtEnd() ? 0 : memory_->length() - current_;
	}

	bool isAtEnd() const
	{
		return current_ >= memory_->length();
	}

	void advance(size_t numChars)
	{
		current_ += numChars;
	}

	void findMul()
	{
		current_ = memory_->find("mul", current_);
	}

	bool findMul2()
	{
		AOC_ASSERT(currentChar() == 'm');

		if(numRemainingChars() >= mulInstructionLength)
		{
			if((*memory_)[current_ + 1] == 'u' &&
			   (*memory_)[current_ + 2] == 'l')
			{
				current_ += mulInstructionLength;

				return true;
			}
		}

		return false;
	}

	int readNumber(char delimiter);
	int scanDo(bool& isMulEnabled);
	int scanDont(bool& isMulEnabled);

private:
	static constexpr size_t mulInstructionLength  = 3;
	static constexpr size_t doInstructionLength   = 4;
	static constexpr size_t dontInstructionLength = 7;

private:
	const string* memory_;
	size_t start_;
	size_t current_;
};

#define TRY_TO_ADVANCE(_numChars) \
	advance(_numChars); \
	if(isAtEnd()) \
		return sumOfProducts;

int MemoryScanner::scanMuls1()
{
	int sumOfProducts = 0;

	while(!isAtEnd())
	{
		findMul();

		if(isAtEnd())
			return sumOfProducts;

		TRY_TO_ADVANCE(mulInstructionLength);

		if(currentChar() != '(')
			continue;

		TRY_TO_ADVANCE(1);

		const int multiplicand = readNumber(',');

		if(multiplicand < 0)
			continue;

		if(currentChar() != ',')
			continue;

		TRY_TO_ADVANCE(1);

		const int multiplier = readNumber(')');

		if(multiplier < 0)
			continue;

		if(currentChar() != ')')
			continue;

		const int product = multiplicand * multiplier;

		sumOfProducts += product;
	}

	start_ = 0;
	current_ = 0;

	return sumOfProducts;
}

int MemoryScanner::scanMuls2()
{
	int sumOfProducts = 0;
	bool isMulEnabled = true;

	while(!isAtEnd())
	{
		const char ch = currentChar();

		switch(ch)
		{
			case 'd':
			{
				if(scanDo(isMulEnabled))
				{
					advance(doInstructionLength);

					continue;
				}
				else if(scanDont(isMulEnabled))
				{
					advance(dontInstructionLength);

					continue;
				}
				else
				{
					TRY_TO_ADVANCE(1);

					continue;
				}

				break;
			}

			case 'm':
			{
				break;
			}

			default:
			{
				TRY_TO_ADVANCE(1);

				continue;
			}
		}

		if(!findMul2())
		{
			TRY_TO_ADVANCE(1);

			continue;
		}

		if(isAtEnd())
			return sumOfProducts;

		if(currentChar() != '(')
			continue;

		TRY_TO_ADVANCE(1);

		const int multiplicand = readNumber(',');

		if(multiplicand < 0)
			continue;

		if(currentChar() != ',')
			continue;

		TRY_TO_ADVANCE(1);

		const int multiplier = readNumber(')');

		if(multiplier < 0)
			continue;

		if(currentChar() != ')')
			continue;

		const int product = multiplicand * multiplier;

		if(isMulEnabled)
			sumOfProducts += product;
	}

	start_ = 0;
	current_ = 0;

	return sumOfProducts;
}

int MemoryScanner::readNumber(char delimiter)
{
	if(!isDigit(currentChar()))
		return -1;

	AOC_ASSERT(isDigit(currentChar()));

	const int num1 = digitToInt(currentChar());

	if(num1 == 0)
	{
		advance(1);

		if(isAtEnd())
			return -1;

		return num1;
	}

	advance(1);

	if(isAtEnd())
		return -1;

	if(currentChar() == delimiter)
		return num1;

	if(!isDigit(currentChar()))
		return -1;

	const int num2 = digitToInt(currentChar());

	advance(1);

	if(isAtEnd())
		return -1;

	if(currentChar() == delimiter)
		return num1 * 10 + num2;

	if(!isDigit(currentChar()))
		return -1;

	const int num3 = digitToInt(currentChar());

	advance(1);

	if(isAtEnd())
		return -1;

	return num1 * 100 + num2 * 10 + num3;
}

int MemoryScanner::scanDo(bool& isMulEnabled)
{
	if(numRemainingChars() >= doInstructionLength)
	{
		AOC_ASSERT(currentChar() == 'd');

		if((*memory_)[current_ + 1] == 'o' &&
		   (*memory_)[current_ + 2] == '(' &&
		   (*memory_)[current_ + 3] == ')')
		{
			isMulEnabled = true;

			return true;
		}
	}

	return false;
}

int MemoryScanner::scanDont(bool& isMulEnabled)
{
	if(numRemainingChars() >= dontInstructionLength)
	{
		AOC_ASSERT(currentChar() == 'd');

		if((*memory_)[current_ + 1] == 'o' &&
		   (*memory_)[current_ + 2] == 'n' &&
		   (*memory_)[current_ + 3] == '\'' &&
		   (*memory_)[current_ + 4] == 't' &&
		   (*memory_)[current_ + 5] == '(' &&
		   (*memory_)[current_ + 6] == ')')
		{
			isMulEnabled = false;

			return true;
		}
	}

	return false;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("isDigit")
{
	CHECK( isDigit('0'));
	CHECK( isDigit('1'));
	CHECK( isDigit('2'));
	CHECK( isDigit('3'));
	CHECK( isDigit('4'));
	CHECK( isDigit('5'));
	CHECK( isDigit('6'));
	CHECK( isDigit('7'));
	CHECK( isDigit('8'));
	CHECK( isDigit('9'));
	CHECK(!isDigit('/'));
	CHECK(!isDigit(':'));
	CHECK(!isDigit('a'));
	CHECK(!isDigit('A'));
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
}

#define CHECK_SCAN_MULS1(_memoryStr, _expectedSumOfProducts) \
	{ \
		string memory(_memoryStr); \
		MemoryScanner scanner(memory); \
		CHECK(scanner.scanMuls1() == _expectedSumOfProducts); \
	}

#define CHECK_SCAN_MULS2(_memoryStr, _expectedSumOfProducts) \
	{ \
		string memory(_memoryStr); \
		MemoryScanner scanner(memory); \
		CHECK(scanner.scanMuls2() == _expectedSumOfProducts); \
	}

TEST_CASE("MemoryScanner")
{
	SUBCASE("scanMuls1")
	{
		CHECK_SCAN_MULS1("mul(2,4)", 2*4);
		CHECK_SCAN_MULS1("mul(0,4)", 0);
		CHECK_SCAN_MULS1("mul(2,0)", 0);
		CHECK_SCAN_MULS1("mul(5,5)", 5*5);
		CHECK_SCAN_MULS1("mul(8,5)", 8*5);
		CHECK_SCAN_MULS1("mul(8 ,5)", 0);
		CHECK_SCAN_MULS1("mul(8, 5)", 0);
		CHECK_SCAN_MULS1("mul(73,623)", 73*623);
		CHECK_SCAN_MULS1("mul(390,953)", 390*953);
		CHECK_SCAN_MULS1("xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))", 161);
	}

	SUBCASE("scanMuls2")
	{
		CHECK_SCAN_MULS2("mul(2,4)", 2*4);
		CHECK_SCAN_MULS2("mul(0,4)", 0);
		CHECK_SCAN_MULS2("mul(2,0)", 0);
		CHECK_SCAN_MULS2("mul(5,5)", 5*5);
		CHECK_SCAN_MULS2("mul(8,5)", 8*5);
		CHECK_SCAN_MULS2("mul(8 ,5)", 0);
		CHECK_SCAN_MULS2("mul(8, 5)", 0);
		CHECK_SCAN_MULS2("mul(73,623)", 73*623);
		CHECK_SCAN_MULS2("mul(390,953)", 390*953);
		CHECK_SCAN_MULS2("do()mul(2,4)", 2*4);
		CHECK_SCAN_MULS2("don't()mul(2,4)", 0);
		CHECK_SCAN_MULS2("xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))", 161);
		CHECK_SCAN_MULS2("xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))", 48);
	}
}

#else // #ifdef AOC_TEST_SOLUTION

class MullItOver : public PuzzleSolution<int>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	int answer1() override;
	int answer2() override;

private:
	string memory_;
};

void MullItOver::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	for(const auto& line : input())
		memory_ += line;
}

int MullItOver::answer1()
{
	MemoryScanner scanner(memory_);

	return scanner.scanMuls1();
}

int MullItOver::answer2()
{
	MemoryScanner scanner(memory_);

	return scanner.scanMuls2();
}

int main(int argc, char* argv[])
{
	return MullItOver().run((argc > 1) ? argv[1] : "202403.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
