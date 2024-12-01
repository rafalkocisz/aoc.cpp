#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <cctype>
#include <string>
#include <vector>

using namespace fmt;
using namespace aoc;
using namespace std;

class Number
{
	friend bool operator==(const Number&,
	                       const Number&) = default;

public:
	explicit Number(size_t startPos=0,
	                size_t endPos=0,
	                int value=0)
		: startPos_(startPos)
		, endPos_(endPos)
		, value_(value)
	{
	}

public:
	size_t startPos_;
	size_t endPos_;
	int value_;
};

Number parseNumber(const string& line, size_t& pos)
{
	AOC_ASSERT(pos < line.size());
	AOC_ASSERT(isdigit(line[pos]));

	const size_t startPos = pos;

	Number number(startPos);

	++pos;
	while(pos < line.size())
		if(isdigit(line[pos]))
			++pos;
		else
			break;

	const size_t endPos = pos - 1;

	AOC_ASSERT(endPos >= startPos);
	AOC_ASSERT(endPos < line.size());
	AOC_ASSERT(isdigit(line[endPos]));

	number.endPos_ = endPos;

	AOC_ASSERT(pos > startPos);
	AOC_ASSERT((pos >= 1) && (pos <= line.size()));
	AOC_ASSERT(!isdigit(line[pos]));

	number.value_ = stoi(string(line, startPos, pos - startPos));

	return number;
}

class Symbol
{
	friend bool operator==(const Symbol&,
	                       const Symbol&) = default;

public:
	explicit Symbol(size_t pos=0,
	                char value=0)
		: pos_(pos)
		, value_(value)
	{
	}

public:
	size_t pos_;
	char value_;
};

class EngineSchematicRow
{
	friend EngineSchematicRow parseEngineSchematicRow(const string& line);

public:
	size_t numNumbers() const { return numbers_.size(); }
	const Number& number(size_t index) const;
	const vector<Number>& numbers() const { return numbers_; }
	size_t numSymbols() const { return symbols_.size(); }
	const Symbol& symbol(size_t index) const;
	const vector<Symbol>& symbols() const { return symbols_; }

public:
	string toString() const;

private:
	EngineSchematicRow(size_t width) : width_(width) {}

private:
	size_t width_;
	vector<Number> numbers_;
	vector<Symbol> symbols_;
};

const Number& EngineSchematicRow::number(size_t index) const
{
	AOC_ASSERT(index < numNumbers());

	return numbers_[index];
}

const Symbol& EngineSchematicRow::symbol(size_t index) const
{
	AOC_ASSERT(index < numSymbols());

	return symbols_[index];
}

string EngineSchematicRow::toString() const
{
	string result(width_, '.');

	for(const auto& number : numbers_)
	{
		string numberStr(to_string(number.value_));

		for(size_t pos=number.startPos_; pos<=number.endPos_; ++pos)
		{
			AOC_ASSERT(pos < result.size());

			const size_t pos2 = pos-number.startPos_;

			AOC_ASSERT(pos2 < result.size());

			result[pos] = numberStr[pos2];
		}
	}

	for(const auto& symbol : symbols_)
	{
		AOC_ASSERT(symbol.pos_ < result.size());

		result[symbol.pos_] = symbol.value_;
	}

	return result;
}

EngineSchematicRow parseEngineSchematicRow(const string& line)
{
	EngineSchematicRow row(line.size());

	size_t pos = 0;

	while(true)
	{
		AOC_ASSERT(pos < line.size());

		if(line[pos] == '.')
		{
			pos = line.find_first_not_of('.', pos);

			if(pos == string::npos)
				break;
		}
		else if(isdigit(line[pos]))
		{
			row.numbers_.push_back(parseNumber(line, pos));

			if(pos == line.size())
				break;
		}
		else
		{
			row.symbols_.push_back(Symbol{pos, line[pos]});

			++pos;

			if(pos == line.size())
				break;
		}
	}

	return row;
}

class EngineSchematic
{
	friend EngineSchematic parseEngineSchematic(const vector<string>& lines);

public:
	EngineSchematic() = default;

public:
	size_t numRows() const { return rows_.size(); }
	const EngineSchematicRow& row(size_t index) const;

public:
	int sumPartNumbers() const;
	int sumGearRatios() const;
	string toString() const;

private:
	EngineSchematic(size_t width) : width_(width) {}

#ifdef AOC_TEST_SOLUTION
public:
#else
private:
#endif
	bool isNumberAdjacentToSymbol(const Number& number,
	                              const Symbol& symbol,
	                              int dy) const;
	bool isPartNumber(size_t rowIndex,
	                  size_t numberIndex) const;
	bool isGear(size_t rowIndex,
	            size_t symbolIndex,
	            int& gearRatio) const;

private:
	size_t width_;
	vector<EngineSchematicRow> rows_;
};

const EngineSchematicRow& EngineSchematic::row(size_t index) const
{
	AOC_ASSERT(index < numRows());

	return rows_[index];
}

int EngineSchematic::sumPartNumbers() const
{
	int sum = 0;

	for(size_t rowIndex=0; rowIndex<numRows(); ++rowIndex)
		for(size_t numberIndex=0; numberIndex<row(rowIndex).numNumbers(); ++numberIndex)
			if(isPartNumber(rowIndex, numberIndex))
				sum += row(rowIndex).number(numberIndex).value_;

	return sum;
}

int EngineSchematic::sumGearRatios() const
{
	int sum = 0;

	for(size_t rowIndex=0; rowIndex<numRows(); ++rowIndex)
	{
		for(size_t symbolIndex=0; symbolIndex<row(rowIndex).numSymbols(); ++symbolIndex)
		{
			int gearRatio = 0;

			if(isGear(rowIndex, symbolIndex, gearRatio))
				sum += gearRatio;
		}
	}

	return sum;
}

string EngineSchematic::toString() const
{
	string result;

	for(size_t i=0; i<rows_.size(); ++i)
	{
		result += rows_[i].toString();

		if(i < rows_.size() - 1)
			result += '\n';
	}

	return result;
}

bool EngineSchematic::isNumberAdjacentToSymbol(const Number& number,
                                               const Symbol& symbol,
                                               int dy) const
{
	const size_t numberRangeStart = (number.startPos_ > 0) ? (number.startPos_ - 1)
	                                                       :  number.startPos_ + 1;
	const size_t numberRangeEnd = number.endPos_ + 1;

	switch(dy)
	{
		case 0:
		{
			return (symbol.pos_ == numberRangeStart) ||
			       (symbol.pos_ == numberRangeEnd);
		}

		case  1:
		case -1:
		{
			return MathUtils::isInRange(symbol.pos_,
			                            numberRangeStart,
			                            numberRangeEnd);
		}
	}

	return false;
}

bool EngineSchematic::isPartNumber(size_t rowIndex,
                                   size_t numberIndex) const
{
	const Number& number = row(rowIndex).number(numberIndex);

	if(rowIndex > 0)
		for(const auto& symbol : row(rowIndex - 1).symbols())
			if(isNumberAdjacentToSymbol(number, symbol, -1))
				return true;

	for(const auto& symbol : row(rowIndex).symbols())
		if(isNumberAdjacentToSymbol(number, symbol, 0))
			return true;

	if(rowIndex < (numRows() - 1))
		for(const auto& symbol : row(rowIndex + 1).symbols())
			if(isNumberAdjacentToSymbol(number, symbol, 1))
				return true;

	return false;
}

bool EngineSchematic::isGear(size_t rowIndex,
                             size_t symbolIndex,
                             int& gearRatio) const
{
	const Symbol& symbol = row(rowIndex).symbol(symbolIndex);

	if(symbol.value_ != '*')
		return false;

	int adjacentNumberCount = 0;
	int product = 1;

	if(rowIndex > 0)
	{
		for(size_t numberIndex=0; numberIndex<row(rowIndex - 1).numNumbers(); ++numberIndex)
		{
			const Number& number = row(rowIndex - 1).number(numberIndex);
			
			if(isPartNumber(rowIndex - 1, numberIndex) &&
			   isNumberAdjacentToSymbol(number, symbol, -1))
			{
				++adjacentNumberCount;

				if(adjacentNumberCount > 2)
					return false;

				product *= number.value_;
			}
		}
	}

	for(size_t numberIndex=0; numberIndex<row(rowIndex).numNumbers(); ++numberIndex)
	{
		const Number& number = row(rowIndex).number(numberIndex);
		
		if(isPartNumber(rowIndex, numberIndex) &&
		   isNumberAdjacentToSymbol(number, symbol, 0))
		{
			++adjacentNumberCount;

			if(adjacentNumberCount > 2)
				return false;

			product *= number.value_;
		}
	}

	if(rowIndex < (numRows() - 1))
	{
		for(size_t numberIndex=0; numberIndex<row(rowIndex + 1).numNumbers(); ++numberIndex)
		{
			const Number& number = row(rowIndex + 1).number(numberIndex);
			
			if(isPartNumber(rowIndex + 1, numberIndex) &&
			   isNumberAdjacentToSymbol(number, symbol, 1))
			{
				++adjacentNumberCount;

				if(adjacentNumberCount > 2)
					return false;

				product *= number.value_;
			}
		}
	}

	if(adjacentNumberCount == 2)
	{
		gearRatio = product;

		return true;
	}

	return false;
}

EngineSchematic parseEngineSchematic(const vector<string>& lines)
{
	EngineSchematic engineSchematic(lines.size());

	for(const auto& line : lines)
		engineSchematic.rows_.push_back(parseEngineSchematicRow(line));

	return engineSchematic;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("parseNumber")
{
	string line1("467..114..");
	size_t pos = 0;

	CHECK(parseNumber(line1, pos) == Number(0, 2, 467));
	CHECK(pos == 3);

	pos = 5;

	CHECK(parseNumber(line1, pos) == Number(5, 7, 114));
	CHECK(pos == 8);

	string line2("*.........982");
	pos = 10;

	CHECK(parseNumber(line2, pos) == Number(10, 12, 982));
	CHECK(pos == 13);

	string line3("617*......");
	pos = 0;

	CHECK(parseNumber(line3, pos) == Number(0, 2, 617));
	CHECK(pos == 3);

	string line4(".........1");
	pos = 9;

	CHECK(parseNumber(line4, pos) == Number(9, 9, 1));
	CHECK(pos == 10);
}

TEST_CASE("parseEngineSchematicRow")
{
	CHECK(parseEngineSchematicRow("..........").toString() == "..........");
	CHECK(parseEngineSchematicRow("1.........").toString() == "1.........");
	CHECK(parseEngineSchematicRow(".........2").toString() == ".........2");
	CHECK(parseEngineSchematicRow("(.........").toString() == "(.........");
	CHECK(parseEngineSchematicRow(".........)").toString() == ".........)");
	CHECK(parseEngineSchematicRow("1........&").toString() == "1........&");
	CHECK(parseEngineSchematicRow("!........2").toString() == "!........2");
	CHECK(parseEngineSchematicRow("1234567890").toString() == "1234567890");
	CHECK(parseEngineSchematicRow("!@#$%^&*()").toString() == "!@#$%^&*()");
	CHECK(parseEngineSchematicRow("....*.....").toString() == "....*.....");
	CHECK(parseEngineSchematicRow("..35..633.").toString() == "..35..633.");
	CHECK(parseEngineSchematicRow("......#...").toString() == "......#...");
	CHECK(parseEngineSchematicRow("617*......").toString() == "617*......");
	CHECK(parseEngineSchematicRow(".....+.58.").toString() == ".....+.58.");
	CHECK(parseEngineSchematicRow("..592.....").toString() == "..592.....");
	CHECK(parseEngineSchematicRow("......755.").toString() == "......755.");
	CHECK(parseEngineSchematicRow("...$.*....").toString() == "...$.*....");
	CHECK(parseEngineSchematicRow(".664.598..").toString() == ".664.598..");
}

TEST_CASE("parseEngineSchematic")
{
	const vector<string> lines1;
	const char* engineSchematicStr1 = "";

	CHECK(parseEngineSchematic(lines1).toString() == string(engineSchematicStr1));

	const vector<string> lines2 = { "617*......" };
	const char* engineSchematicStr2 = "617*......";

	CHECK(parseEngineSchematic(lines2).toString() == string(engineSchematicStr2));

	const vector<string> lines3 =
	{
		"617*......",
		".....+.58."
	};

	const char* engineSchematicStr3 =
		"617*......\n"
		".....+.58.";

	CHECK(parseEngineSchematic(lines3).toString() == string(engineSchematicStr3));

	const vector<string> lines4 =
	{
		"467..114..",
		"...*......",
		"..35..633.",
		"......#...",
		"617*......",
		".....+.58.",
		"..592.....",
		"......755.",
		"...$.*....",
		".664.598.."
	};

	const char* engineSchematicStr4 =
		"467..114..\n"
		"...*......\n"
		"..35..633.\n"
		"......#...\n"
		"617*......\n"
		".....+.58.\n"
		"..592.....\n"
		"......755.\n"
		"...$.*....\n"
		".664.598..";

	CHECK(parseEngineSchematic(lines4).toString() == string(engineSchematicStr4));
}

TEST_CASE("EngineSchematic")
{
	const vector<string> lines =
	{
	//   0123456789
		"467..114..", //  0
		"...*......", //  1
		"..35..633.", //  2
		"......#...", //  3
		"617*......", //  4
		".....+.58.", //  5
		"..592.....", //  6
		"......755.", //  7
		"...$.*....", //  8
		".664.598..", //  9
	};

	EngineSchematic engineSchematic = parseEngineSchematic(lines);

	SUBCASE("isNumberAdjacentToSymbol")
	{
		const Number& number1 = engineSchematic.row(0).number(0);

		CHECK(number1.startPos_ == 0);
		CHECK(number1.endPos_ == 2);
		CHECK(number1.value_ == 467);

		const Number& number2 = engineSchematic.row(0).number(1);

		CHECK(number2.startPos_ == 5);
		CHECK(number2.endPos_ == 7);
		CHECK(number2.value_ == 114);

		const Number& number3 = engineSchematic.row(2).number(0);

		CHECK(number3.startPos_ == 2);
		CHECK(number3.endPos_ == 3);
		CHECK(number3.value_ == 35);

		const Symbol& symbol1 = engineSchematic.row(1).symbol(0);

		CHECK(symbol1.pos_ == 3);
		CHECK(symbol1.value_ == '*');

		CHECK(engineSchematic.isNumberAdjacentToSymbol(number1, symbol1, -1) == true);
		CHECK(engineSchematic.isNumberAdjacentToSymbol(number2, symbol1, -1) == false);
		CHECK(engineSchematic.isNumberAdjacentToSymbol(number3, symbol1,  1) == true);
	}

	SUBCASE("isPartNumber")
	{
		CHECK(engineSchematic.isPartNumber(0, 0) == true);
		CHECK(engineSchematic.isPartNumber(0, 1) == false);
		CHECK(engineSchematic.isPartNumber(2, 0) == true);
		CHECK(engineSchematic.isPartNumber(5, 0) == false);
	}


	SUBCASE("isGear")
	{
		int gearRatio = 0;

		CHECK(engineSchematic.isGear(1, 0, gearRatio) == true);
		CHECK(gearRatio == 16345);

		gearRatio = 0;

		CHECK(engineSchematic.isGear(4, 0, gearRatio) == false);
		CHECK(gearRatio == 0);

		gearRatio = 0;

		CHECK(engineSchematic.isGear(5, 0, gearRatio) == false);
		CHECK(gearRatio == 0);

		gearRatio = 0;

		CHECK(engineSchematic.isGear(8, 1, gearRatio) == true);
		CHECK(gearRatio == 451490);
	}

	SUBCASE("sumPartNumbers")
	{
		CHECK(engineSchematic.sumPartNumbers() == 4361);
	}

	SUBCASE("sumGearRatios")
	{
		CHECK(engineSchematic.sumGearRatios() == 467835);
	}
}

#else // #ifdef AOC_TEST_SOLUTION

class GearRatios : public PuzzleSolution<int>
{
private:
	void processInput(const std::string& puzzleInputFilePath) override;
	int answer1() override;
	int answer2() override;

private:
	EngineSchematic engineSchematic_;
};

void GearRatios::processInput(const std::string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	engineSchematic_ = parseEngineSchematic(input());
}

int GearRatios::answer1()
{
	return engineSchematic_.sumPartNumbers();
}

int GearRatios::answer2()
{
	return engineSchematic_.sumGearRatios();
}

int main(int argc, char* argv[])
{
	return GearRatios().run((argc > 1) ? argv[1] : "202303.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
