#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <string>
#include <vector>

#define CHECK_KEYPAD(_x, _y, _keyCode) \
	CHECK(keypad.focusPoint() == Point(_x, _y)); \
	CHECK(keypad.keyCodeAtFocusPoint() == _keyCode);

using namespace fmt;
using namespace aoc;
using namespace std;

struct Point
{
	friend bool operator==(const Point&,
	                       const Point&) = default;

	size_t x_;
	size_t y_;
};

class Keypad
{
public:
	Keypad() { setFocusPoint(1, 1); }

public:
	const Point& focusPoint() const { return focusPoint_; }

	char keyCode(size_t x, size_t y) const
	{
		AOC_ASSERT(x < keysWide);
		AOC_ASSERT(y < keysHigh);

		return '1' + x + 3 * y;
	}

	char keyCode(const Point& point) const
	{
		return keyCode(point.x_, point.y_);
	}

	char keyCodeAtFocusPoint() const
	{
		return keyCode(focusPoint());
	}

public:
	void setFocusPoint(size_t x, size_t y)
	{
		AOC_ASSERT(x < keysWide);
		AOC_ASSERT(y < keysHigh);

		focusPoint_.x_ = x;
		focusPoint_.y_ = y;
	}

	void moveFocusPoint(char instruction);
	void moveFocusPoint(const string& instructions);
	string generateBathroomCode(const vector<string>& instructionLines);

private:
	void moveFocusPointHorizontally(int dx);
	void moveFocusPointVertically(int dy);

private:
	static constexpr size_t keysWide = 3;
	static constexpr size_t keysHigh = 3;

private:
	Point focusPoint_;
};

void Keypad::moveFocusPoint(char instruction)
{
	switch(instruction)
	{
		case 'L': moveFocusPointHorizontally(-1); return;
		case 'R': moveFocusPointHorizontally(1); return;
		case 'U': moveFocusPointVertically(-1); return;
		case 'D': moveFocusPointVertically(1); return;
	}

	panic(format("invalid instruction: {}", instruction));
}

void Keypad::moveFocusPoint(const string& instructions)
{
	for(char instruction : instructions)
		moveFocusPoint(instruction);
}

void Keypad::moveFocusPointHorizontally(int dx)
{
	AOC_ASSERT(dx == -1 || dx == 1);

	const int x = focusPoint_.x_ + dx;

	if(x >= 0 && x < keysWide)
		focusPoint_.x_ = x;
}

void Keypad::moveFocusPointVertically(int dy)
{
	AOC_ASSERT(dy == -1 || dy == 1);

	const int y = focusPoint_.y_ + dy;

	if(y >= 0 && y < keysHigh)
		focusPoint_.y_ = y;
}

string Keypad::generateBathroomCode(const vector<string>& instructionLines)
{
	setFocusPoint(1, 1);

	string bathroomCode;

	for(const auto& instructions : instructionLines)
	{
		moveFocusPoint(instructions);

		bathroomCode += keyCodeAtFocusPoint();
	}

	return bathroomCode;
}

class Keypad2
{
public:
	Keypad2() { setFocusPoint(0, 2); }

public:
	const Point& focusPoint() const { return focusPoint_; }

	char keyCode(size_t x, size_t y) const
	{
		AOC_ASSERT(x < keysWide);
		AOC_ASSERT(y < keysHigh);
		AOC_ASSERT(keyCodes[y][x] != ' ');

		return keyCodes[y][x];
	}

	char keyCode(const Point& point) const
	{
		return keyCode(point.x_, point.y_);
	}

	char keyCodeAtFocusPoint() const
	{
		return keyCode(focusPoint());
	}

public:
	void setFocusPoint(size_t x, size_t y)
	{
		AOC_ASSERT(x < keysWide);
		AOC_ASSERT(y < keysHigh);

		focusPoint_.x_ = x;
		focusPoint_.y_ = y;
	}

	void moveFocusPoint(char instruction);
	void moveFocusPoint(const string& instructions);
	string generateBathroomCode(const vector<string>& instructionLines);

private:
	void moveFocusPointHorizontally(int dx);
	void moveFocusPointVertically(int dy);

private:
	static constexpr size_t keysWide = 5;
	static constexpr size_t keysHigh = 5;

	static constexpr char keyCodes[keysHigh][keysWide] =
	{
		{ ' ',' ','1',' ',' ' },
		{ ' ','2','3','4',' ' },
		{ '5','6','7','8','9' },
		{ ' ','A','B','C',' ' },
		{ ' ',' ','D',' ',' ' },
	};

private:
	Point focusPoint_;
};

void Keypad2::moveFocusPoint(char instruction)
{
	switch(instruction)
	{
		case 'L': moveFocusPointHorizontally(-1); return;
		case 'R': moveFocusPointHorizontally(1); return;
		case 'U': moveFocusPointVertically(-1); return;
		case 'D': moveFocusPointVertically(1); return;
	}

	panic(format("invalid instruction: {}", instruction));
}

void Keypad2::moveFocusPoint(const string& instructions)
{
	for(char instruction : instructions)
		moveFocusPoint(instruction);
}

void Keypad2::moveFocusPointHorizontally(int dx)
{
	AOC_ASSERT(dx == -1 || dx == 1);

	const int x = focusPoint_.x_ + dx;

	if(x >= 0 && x < keysWide)
		if(keyCodes[focusPoint_.y_][x] != ' ')
			focusPoint_.x_ = x;
}

void Keypad2::moveFocusPointVertically(int dy)
{
	AOC_ASSERT(dy == -1 || dy == 1);

	const int y = focusPoint_.y_ + dy;

	if(y >= 0 && y < keysHigh)
		if(keyCodes[y][focusPoint_.x_] != ' ')
			focusPoint_.y_ = y;
}

string Keypad2::generateBathroomCode(const vector<string>& instructionLines)
{
	setFocusPoint(0, 2);

	string bathroomCode;

	for(const auto& instructions : instructionLines)
	{
		moveFocusPoint(instructions);

		bathroomCode += keyCodeAtFocusPoint();
	}

	return bathroomCode;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("Keypad")
{
	Keypad keypad;

	SUBCASE("keyCode")
	{
		CHECK(keypad.keyCode(0, 0) == '1');
		CHECK(keypad.keyCode(1, 0) == '2');
		CHECK(keypad.keyCode(2, 0) == '3');

		CHECK(keypad.keyCode(0, 1) == '4');
		CHECK(keypad.keyCode(1, 1) == '5');
		CHECK(keypad.keyCode(2, 1) == '6');

		CHECK(keypad.keyCode(0, 2) == '7');
		CHECK(keypad.keyCode(1, 2) == '8');
		CHECK(keypad.keyCode(2, 2) == '9');
	}

	SUBCASE("focusPoint")
	{
		CHECK(keypad.focusPoint() == Point(1, 1));
	}

	SUBCASE("keyCodeAtFocusPoint")
	{
		CHECK(keypad.keyCodeAtFocusPoint() == '5');
	}

	SUBCASE("moveFocusPoint(char instruction)")
	{
		keypad.moveFocusPoint('U');
		CHECK_KEYPAD(1, 0, '2');

		keypad.moveFocusPoint('L');
		CHECK_KEYPAD(0, 0, '1');

		keypad.moveFocusPoint('L');
		CHECK_KEYPAD(0, 0, '1');

		keypad.moveFocusPoint('R');
		CHECK_KEYPAD(1, 0, '2');

		keypad.moveFocusPoint('R');
		CHECK_KEYPAD(2, 0, '3');

		keypad.moveFocusPoint('D');
		CHECK_KEYPAD(2, 1, '6');

		keypad.moveFocusPoint('D');
		CHECK_KEYPAD(2, 2, '9');

		keypad.moveFocusPoint('D');
		CHECK_KEYPAD(2, 2, '9');
	}

	SUBCASE("moveFocusPoint(const string& instructions)")
	{
		keypad.moveFocusPoint("ULL");
		CHECK_KEYPAD(0, 0, '1');

		keypad.moveFocusPoint("RRDDD");
		CHECK_KEYPAD(2, 2, '9');

		keypad.moveFocusPoint("LURDL");
		CHECK_KEYPAD(1, 2, '8');

		keypad.moveFocusPoint("UUUUD");
		CHECK_KEYPAD(1, 1, '5');
	}

	SUBCASE("generateBathroomCode")
	{
		const vector<string> instructionLines =
		{
			"ULL",
			"RRDDD",
			"LURDL",
			"UUUUD",
		};

		CHECK(keypad.generateBathroomCode(instructionLines) == "1985");
	}
}

TEST_CASE("Keypad2")
{
	Keypad2 keypad;

	SUBCASE("keyCode")
	{
		CHECK(keypad.keyCode(2, 0) == '1');

		CHECK(keypad.keyCode(1, 1) == '2');
		CHECK(keypad.keyCode(2, 1) == '3');
		CHECK(keypad.keyCode(3, 1) == '4');

		CHECK(keypad.keyCode(0, 2) == '5');
		CHECK(keypad.keyCode(1, 2) == '6');
		CHECK(keypad.keyCode(2, 2) == '7');
		CHECK(keypad.keyCode(3, 2) == '8');
		CHECK(keypad.keyCode(4, 2) == '9');

		CHECK(keypad.keyCode(1, 3) == 'A');
		CHECK(keypad.keyCode(2, 3) == 'B');
		CHECK(keypad.keyCode(3, 3) == 'C');

		CHECK(keypad.keyCode(2, 4) == 'D');
	}

	SUBCASE("focusPoint")
	{
		CHECK(keypad.focusPoint() == Point(0, 2));
	}

	SUBCASE("keyCodeAtFocusPoint")
	{
		CHECK(keypad.keyCodeAtFocusPoint() == '5');
	}

	SUBCASE("moveFocusPoint(char instruction)")
	{
		keypad.moveFocusPoint('U');
		CHECK_KEYPAD(0, 2, '5');

		keypad.moveFocusPoint('L');
		CHECK_KEYPAD(0, 2, '5');

		keypad.moveFocusPoint('L');
		CHECK_KEYPAD(0, 2, '5');

		keypad.moveFocusPoint('R');
		CHECK_KEYPAD(1, 2, '6');

		keypad.moveFocusPoint('R');
		CHECK_KEYPAD(2, 2, '7');

		keypad.moveFocusPoint('D');
		CHECK_KEYPAD(2, 3, 'B');

		keypad.moveFocusPoint('D');
		CHECK_KEYPAD(2, 4, 'D');

		keypad.moveFocusPoint('D');
		CHECK_KEYPAD(2, 4, 'D');
	}

	SUBCASE("moveFocusPoint(const string& instructions)")
	{
		keypad.moveFocusPoint("ULL");
		CHECK_KEYPAD(0, 2, '5');

		keypad.moveFocusPoint("RRDDD");
		CHECK_KEYPAD(2, 4, 'D');

		keypad.moveFocusPoint("LURDL");
		CHECK_KEYPAD(2, 3, 'B');

		keypad.moveFocusPoint("UUUUD");
		CHECK_KEYPAD(2, 1, '3');
	}

	SUBCASE("generateBathroomCode")
	{
		const vector<string> instructionLines =
		{
			"ULL",
			"RRDDD",
			"LURDL",
			"UUUUD",
		};

		CHECK(keypad.generateBathroomCode(instructionLines) == "5DB3");
	}
}

#else // #ifdef AOC_TEST_SOLUTION

class BathroomSecurity : public PuzzleSolution<string>
{
private:
	string answer1() override;
	string answer2() override;
};

string BathroomSecurity::answer1()
{
	Keypad keypad;

	return keypad.generateBathroomCode(input());
}

string BathroomSecurity::answer2()
{
	Keypad2 keypad;

	return keypad.generateBathroomCode(input());
}

int main(int argc, char* argv[])
{
	return BathroomSecurity().run((argc > 1) ? argv[1] : "201602.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
