#ifndef AOC_H
#define AOC_H

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <set>
#include <source_location>
#include <string>
#include <vector>

#define AOC_FUNCTIONIZE(_a, _b) \
	_a(_b)

#define AOC_STRINGIZE(_a) \
	#_a

#define AOC_TO_STRING(_expression) \
	AOC_FUNCTIONIZE(AOC_STRINGIZE, _expression)

#ifndef NDEBUG

#define AOC_ASSERT(_condition) \
	do \
	{ \
		if(!(_condition)) \
		{ \
			aoc::panic("assertion failed with condition '" AOC_TO_STRING(_condition) "'"); \
		} \
	} while(0)

#define AOC_ASSERT_MSG(_condition, _message) \
	do \
	{ \
		if(!(_condition)) \
		{ \
			aoc::panic(_message); \
		} \
	} while(0)

#else // #ifndef NDEBUG

#define AOC_ASSERT(_condition) \
	do \
	{ \
	} while(0)

#define AOC_ASSERT_MSG(_condition, _message) \
	do \
	{ \
	} while(0)

#endif // #ifndef NDEBUG

#define AOC_ENFORCE(_condition) \
	do \
	{ \
		if(!(_condition)) \
		{ \
			aoc::panic("enforcement failed with condition '" AOC_TO_STRING(_condition) "'"); \
		} \
	} while(0)

#define AOC_ENFORCE_MSG(_condition, _message) \
	do \
	{ \
		if(!(_condition)) \
		{ \
			aoc::panic(_message); \
		} \
	} while(0)

#define AOC_NUM_ELEMENTS(_array) \
	(sizeof(_array) / sizeof(*(_array)))

namespace aoc
{

std::vector<std::string> loadPuzzleInput(const std::string& puzzleInputFilePath);

void panic(const std::string& message,
           const std::source_location sourceLocation=std::source_location::current());

template<class T>
inline bool isEven(const T& value)
{
	AOC_ASSERT(value != T());

	return value % 2 == 0;
}

template<class T>
inline bool isOdd(const T& value)
{
	AOC_ASSERT(value != T());

	return !isEven(value);
}

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

inline Direction turnLeft(Direction currentDirection)
{
	return lhsDirectionFor[static_cast<size_t>(currentDirection)];
}

inline Direction turnRight(Direction currentDirection)
{
	return rhsDirectionFor[static_cast<size_t>(currentDirection)];
}

inline Direction turn(Direction currentDirection, Side side)
{
	AOC_ASSERT(side == Side::Left || side == Side::Right);

	switch(side)
	{
		case Side::Left: return turnLeft(currentDirection);
		case Side::Right: return turnRight(currentDirection);
	}

	return Direction::North;
}

template<class T> struct Vec2;

using Vec2i = Vec2<int>;
using Vec2s = Vec2<size_t>;

using Point2i = Vec2<int>;
using Point2s = Vec2<size_t>;

template<class T>
struct Vec2
{
	friend bool operator==(const Vec2&,
	                       const Vec2&) = default;

	constexpr Vec2();

	template<class X, class Y>
	constexpr Vec2(X x=X(), Y y=Y());

	template<class K>
	constexpr Vec2(const Vec2<K>& vec);

	T x_;
	T y_;
};

template<class T>
constexpr Vec2<T>::Vec2()
	: x_(T())
	, y_(T()) {}

template<class T>
template<class X, class Y>
constexpr Vec2<T>::Vec2(X x, Y y)
	: x_(static_cast<T>(x))
	, y_(static_cast<T>(y)) {}

template<class T>
template<class K>
constexpr Vec2<T>::Vec2(const Vec2<K>& vec)
	: x_(static_cast<T>(vec.x_))
	, y_(static_cast<T>(vec.y_)) {}

template<class T>
inline char toChar(const T& value) { return ' '; }

template<>
inline char toChar<char>(const char& value) { return value; }

template<class T>
inline T fromChar(char ch) { return ' '; }

template<>
inline char fromChar<char>(char ch) { return ch; }

#define DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD() \
	std::string toString() const \
	{ \
		std::string result; \
	\
		const size_t l = length(); \
	\
		for(size_t i=0; i<l; ++i) \
			result += toChar<T>(operator[](i)); \
	\
		return result; \
	}

constexpr Point2i deltaPositionFor[] =
{
	{  0, -1 }, // Direction::North
	{  1,  0 }, // Direction::East
	{  0,  1 }, // Direction::South
	{ -1,  0 }, // Direction::West
};

inline Point2i nextPosition(const Point2i& position, Direction direction)
{
	return Point2i(position.x_ + deltaPositionFor[static_cast<size_t>(direction)].x_,
	               position.y_ + deltaPositionFor[static_cast<size_t>(direction)].y_);
}

template<class T>
class BoundedGrid2D
{
public:
	class Row
	{
		friend class BoundedGrid2D;

	public:
		T& operator[](size_t x)
		{
			AOC_ASSERT(x < grid_.width());

			return grid_.cell(x, y_);
		}

		const T& operator[](size_t x) const
		{
			AOC_ASSERT(x < grid_.width());

			return grid_.cell(x, y_);
		}

		size_t length() const { return grid_.width(); }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		Row(BoundedGrid2D& grid, size_t y)
			: grid_(grid)
			, y_(y)
		{
		}

	private:
		BoundedGrid2D& grid_;
		size_t y_;
	};

	class ConstRow
	{
		friend class BoundedGrid2D;

	public:
		const T& operator[](size_t x) const
		{
			AOC_ASSERT(x < grid_.width());

			return grid_.cell(x, y_);
		}

		size_t length() const { return grid_.width(); }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		ConstRow(const BoundedGrid2D& grid, size_t y)
			: grid_(grid)
			, y_(y)
		{
			AOC_ASSERT(y < grid_.height());
		}

	private:
		const BoundedGrid2D& grid_;
		size_t y_;
	};

	class Column
	{
		friend class BoundedGrid2D;

	public:
		T& operator[](size_t y)
		{
			AOC_ASSERT(y < grid_.height());

			return grid_.cell(x_, y);
		}

		const T& operator[](size_t y) const
		{
			AOC_ASSERT(y < grid_.height());

			return grid_.cell(x_, y);
		}

		size_t length() const { return grid_.height(); }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		Column(BoundedGrid2D& grid, size_t x)
			: grid_(grid)
			, x_(x)
		{
			AOC_ASSERT(x < grid_.width());
		}

	private:
		BoundedGrid2D& grid_;
		size_t x_;
	};

	class ConstColumn
	{
		friend class BoundedGrid2D;

	public:
		const T& operator[](size_t y) const
		{
			AOC_ASSERT(y < grid_.height());

			return grid_.cell(x_, y);
		}

		size_t length() const { return grid_.height(); }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		ConstColumn(const BoundedGrid2D& grid, size_t x)
			: grid_(grid)
			, x_(x)
		{
			AOC_ASSERT(x < grid_.width());
		}

	private:
		const BoundedGrid2D& grid_;
		size_t x_;
	};

	//   23456
	// 2 abcde
	// 1 fghij
	// 0 klmno
	//
	// 0 : k
	// 1 : fl
	// 2 : agm
	// 3 : bhn
	// 4 : cio
	// 5 : dj
	// 6 : e
	//
	class TopDownDiagonal
	{
		friend class BoundedGrid2D;

	public:
		T& operator[](size_t i)
		{
			AOC_ASSERT(i < length_);

			return grid_.cell(startX_ + i, startY_ + i);
		}

		const T& operator[](size_t i) const
		{
			AOC_ASSERT(i < length_);

			return grid_.cell(startX_ + i, startY_ + i);
		}

		size_t length() const { return length_; }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		TopDownDiagonal(BoundedGrid2D& grid, size_t diagonalIndex)
			: grid_(grid)
			, startX_(diagonalIndex < grid.height() ? 0 : diagonalIndex - grid.height() + 1)
			, startY_(diagonalIndex < grid.height() ? grid.height() - diagonalIndex - 1 : 0)
			, length_(grid.diagonalLength(diagonalIndex))
		{
			AOC_ASSERT(diagonalIndex < grid.numDiagonals());
			AOC_ASSERT(startX_ < grid_.width() &&
			           startY_ < grid_.height());
		}

	private:
		BoundedGrid2D& grid_;
		size_t startX_;
		size_t startY_;
		size_t length_;
	};

	class ConstTopDownDiagonal
	{
		friend class BoundedGrid2D;

	public:
		const T& operator[](size_t i) const
		{
			AOC_ASSERT(i < length_);

			return grid_.cell(startX_ + i, startY_ + i);
		}

		size_t length() const { return length_; }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		ConstTopDownDiagonal(const BoundedGrid2D& grid, size_t diagonalIndex)
			: grid_(grid)
			, startX_(diagonalIndex < grid.height() ? 0 : diagonalIndex - grid.height() + 1)
			, startY_(diagonalIndex < grid.height() ? grid.height() - diagonalIndex - 1 : 0)
			, length_(grid.diagonalLength(diagonalIndex))
		{
			AOC_ASSERT(diagonalIndex < grid.numDiagonals());
			AOC_ASSERT(startX_ < grid_.width() &&
			           startY_ < grid_.height());
		}

	private:
		const BoundedGrid2D& grid_;
		size_t startX_;
		size_t startY_;
		size_t length_;
	};

	// 0 abcde
	// 1 fghij
	// 2 klmno
	//   23456
	//
	// 0 : k
	// 1 : fb
	// 2 : kgc
	// 3 : lhd
	// 4 : mie
	// 5 : nj
	// 6 : o
	//
	class BottomUpDiagonal
	{
		friend class BoundedGrid2D;

	public:
		T& operator[](size_t i)
		{
			AOC_ASSERT(i < length_);

			return grid_.cell(startX_ + i, startY_ - i);
		}

		const T& operator[](size_t i) const
		{
			AOC_ASSERT(i < length_);

			return grid_.cell(startX_ + i, startY_ - i);
		}

		size_t length() const { return length_; }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		BottomUpDiagonal(BoundedGrid2D& grid, size_t diagonalIndex)
			: grid_(grid)
			, startX_(diagonalIndex < grid.height() ? 0 : diagonalIndex - grid.height() + 1)
			, startY_(diagonalIndex < grid.height() ? diagonalIndex : grid.height() - 1)
			, length_(grid.diagonalLength(diagonalIndex))
		{
			AOC_ASSERT(diagonalIndex < grid.numDiagonals());
			AOC_ASSERT(startX_ < grid_.width() &&
			           startY_ < grid_.height());
		}

	private:
		BoundedGrid2D& grid_;
		size_t startX_;
		size_t startY_;
		size_t length_;
	};

	class ConstBottomUpDiagonal
	{
		friend class BoundedGrid2D;

	public:
		T& operator[](size_t i)
		{
			AOC_ASSERT(i < length_);

			return grid_.cell(startX_ + i, startY_ - i);
		}

		const T& operator[](size_t i) const
		{
			AOC_ASSERT(i < length_);

			return grid_.cell(startX_ + i, startY_ - i);
		}

		size_t length() const { return length_; }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		ConstBottomUpDiagonal(const BoundedGrid2D& grid, size_t diagonalIndex)
			: grid_(grid)
			, startX_(diagonalIndex < grid.height() ? 0 : diagonalIndex - grid.height() + 1)
			, startY_(diagonalIndex < grid.height() ? diagonalIndex : grid.height() - 1)
			, length_(grid.diagonalLength(diagonalIndex))
		{
			AOC_ASSERT(diagonalIndex < grid.numDiagonals());
			AOC_ASSERT(startX_ < grid_.width() &&
			           startY_ < grid_.height());
		}

	private:
		const BoundedGrid2D& grid_;
		size_t startX_;
		size_t startY_;
		size_t length_;
	};

	template<class Sequence>
	class Reverse
	{
	public:
		Reverse(Sequence sequence)
			: sequence_(sequence)
		{
		}

	public:
		T& operator[](size_t index)
		{
			AOC_ASSERT(index < sequence_.length());

			return sequence_[sequence_.length() - index - 1];
		}

		const T& operator[](size_t index) const
		{
			AOC_ASSERT(index < sequence_.length());

			return sequence_[sequence_.length() - index - 1];
		}

		size_t length() const { return sequence_.length(); }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		Sequence sequence_;
	};

	template<class ConstSequence>
	class ConstReverse
	{
	public:
		ConstReverse(ConstSequence sequence)
			: sequence_(sequence)
		{
		}

	public:
		const T& operator[](size_t index) const
		{
			AOC_ASSERT(index < sequence_.length());

			return sequence_[sequence_.length() - index - 1];
		}

		size_t length() const { return sequence_.length(); }

		std::string toString() const
		{
			std::string result;

			const size_t length = sequence_.length();

			for(size_t i=0; i<length; ++i)
				result += toChar<T>(operator[](i));

			return result;
		}

	private:
		ConstSequence sequence_;
	};

	class AdjacentNeighbors
	{
		friend class BoundedGrid2D;

	public:
		static constexpr size_t maxCount = 4;

	public:
		T& operator[](size_t index)
		{
			AOC_ASSERT(count_ <= maxCount && index < count_);

			return grid_.cell(positions_[index]);
		}

		const T& operator[](size_t index) const
		{
			AOC_ASSERT(count_ <= maxCount && index < count_);

			return grid_.cell(positions_[index]);
		}

		size_t count() const { return count_; }
		size_t length() const { return count_; }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		AdjacentNeighbors(BoundedGrid2D& grid, size_t x, size_t y)
			: grid_(grid)
			, count_(0)
		{
			if(y > 0              ) positions_[count_++] = Point2s(x  , y-1);
			if(x > 0              ) positions_[count_++] = Point2s(x-1, y  );
			if(x < grid.width()-1 ) positions_[count_++] = Point2s(x+1, y  );
			if(y < grid.height()-1) positions_[count_++] = Point2s(x  , y+1);
		}

	private:
		BoundedGrid2D& grid_;
		Point2s positions_[maxCount];
		size_t count_;
	};

	class ConstAdjacentNeighbors
	{
		friend class BoundedGrid2D;

	public:
		static constexpr size_t maxCount = 4;

	public:
		const T& operator[](size_t index) const
		{
			AOC_ASSERT(count_ <= maxCount && index < count_);

			return grid_.cell(positions_[index]);
		}

		size_t count() const { return count_; }
		size_t length() const { return count_; }

		DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD();

	private:
		ConstAdjacentNeighbors(const BoundedGrid2D& grid, size_t x, size_t y)
			: grid_(grid)
			, count_(0)
		{
			if(y > 0              ) positions_[count_++] = Point2s(x  , y-1);
			if(x > 0              ) positions_[count_++] = Point2s(x-1, y  );
			if(x < grid.width()-1 ) positions_[count_++] = Point2s(x+1, y  );
			if(y < grid.height()-1) positions_[count_++] = Point2s(x  , y+1);
		}

	private:
		const BoundedGrid2D& grid_;
		Point2s positions_[maxCount];
		size_t count_;
	};

public:
	typedef      Reverse<Row>                        ReverseRow;
	typedef ConstReverse<ConstRow>              ConstReverseRow;
	typedef      Reverse<Column>                     ReverseColumn;
	typedef ConstReverse<ConstColumn>           ConstReverseColumn;
	typedef      Reverse<TopDownDiagonal>            ReverseTopDownDiagonal;
	typedef ConstReverse<ConstTopDownDiagonal>  ConstReverseTopDownDiagonal;
	typedef      Reverse<BottomUpDiagonal>           ReverseBottomUpDiagonal;
	typedef ConstReverse<ConstBottomUpDiagonal> ConstReverseBottomUpDiagonal;

public:
	static constexpr size_t npos = std::string::npos;

public:
	BoundedGrid2D(size_t width=0, size_t height=0, const T& initCell=T())
		: cells_(height, std::vector<T>(width, initCell))
	{
	}

	BoundedGrid2D(const std::vector<std::string>& lines)
		: cells_(0, std::vector<T>(0))
	{
		reset(lines);
	}

public:
	void reset(size_t width, size_t height, const T& initCell=T())
	{
		cells_.assign(height, std::vector<T>(width, initCell));
	}

	void reset(const std::vector<std::string>& lines)
	{
		AOC_ASSERT(!lines.empty());
		AOC_ASSERT(!(lines[0].empty()));

		reset(lines[0].size(), lines.size());

		setCells(lines);
	}

public:
	T& cell(size_t x, size_t y)
	{
		AOC_ASSERT(x < width() && y < height());

		return cells_[y][x];
	}

	const T& cell(size_t x, size_t y) const
	{
		AOC_ASSERT(x < width() && y < height());

		return cells_[y][x];
	}

	T& cell(const Point2s& pos)
	{
		AOC_ASSERT(pos.x_ < width() && pos.y_ < height());

		return cells_[pos.y_][pos.x_];
	}

	const T& cell(const Point2s& pos) const
	{
		AOC_ASSERT(pos.x_ < width() && pos.y_ < height());

		return cells_[pos.y_][pos.x_];
	}

	T& cell(const Point2i& pos)
	{
		AOC_ASSERT(pos.x_ >= 0 && pos.x_ < width() &&
		           pos.y_ >= 0 && pos.y_ < height());

		return cells_[pos.y_][pos.x_];
	}

	const T& cell(const Point2i& pos) const
	{
		AOC_ASSERT(pos.x_ >= 0 && pos.x_ < width() &&
		           pos.y_ >= 0 && pos.y_ < height());

		return cells_[pos.y_][pos.x_];
	}

	void setCell(size_t x, size_t y, const T& cell)
	{
		AOC_ASSERT(x < width() && y < height());

		cells_[y][x] = cell;
	}

	void setCell(const Point2s& pos, const T& cell)
	{
		AOC_ASSERT(pos.x_ < width() && pos.y_ < height());

		cells_[pos.y_][pos.x_] = cell;
	}

	void setCell(const Point2i& pos, const T& cell)
	{
		AOC_ASSERT(pos.x_ >= 0 && pos.x_ < width() &&
		           pos.y_ >= 0 && pos.y_ < height());

		cells_[pos.y_][pos.x_] = cell;
	}

	void setCells(const std::vector<std::string>& lines)
	{
		AOC_ASSERT(lines.size() == height());

		size_t y = 0;

		for(const auto& line : lines)
		{
			AOC_ASSERT(line.length() == width());

			size_t x = 0;

			for(char ch : line)
			{
				cells_[y][x] = fromChar<T>(ch);

				++x;
			}

			++y;
		}
	}

	size_t width() const
	{
		return cells_.size() > 0 ? cells_[0].size() : 0;
	}

	size_t height() const
	{
		return cells_.size();
	}

	size_t numCells() const
	{
		return width() * height();
	}

	size_t numDiagonals() const
	{
		return (width() == 0 || height() == 0) ? 0 : width() + height() - 1;
	}

	size_t maxDiagonalLength() const
	{
		return std::min(width(), height());
	}

	size_t diagonalLength(size_t diagonalIndex) const
	{
		AOC_ASSERT(diagonalIndex < numDiagonals());

		size_t maxLength = maxDiagonalLength();

		if(diagonalIndex < maxLength)
			return diagonalIndex + 1;
		else if(diagonalIndex > numDiagonals()-maxLength)
			return numDiagonals() - diagonalIndex;
		else
			return maxLength;
	}

	size_t count(const T& cell) const
	{
		const size_t w = width();
		const size_t h = height();

		size_t counter = 0;

		for(size_t y=0; y<h; ++y)
			for(size_t x=0; x<w; ++x)
				if(cells_[y][x] == cell)
					++counter;

		return counter;
	}

	Point2s findCell(const T& cell) const
	{
		const size_t w = width();
		const size_t h = height();

		for(size_t y=0; y<h; ++y)
			for(size_t x=0; x<w; ++x)
				if(cells_[y][x] == cell)
					return Point2s(x, y);

		return Point2s(npos, npos);
	}

	std::string toString() const
	{
		std::string result;

		const size_t w = width();
		const size_t h = height();

		for(size_t y=0; y<h; ++y)
		{
			for(size_t x=0; x<w; ++x)
			{
				result += toChar<T>(cells_[y][x]);
			}

			result += '\n';
		}

		return result;
	}

public:
	     Row      row(size_t y)       { return      Row(*this, y); }
	ConstRow constRow(size_t y) const { return ConstRow(*this, y); }

	     ReverseRow      reverseRow(size_t y)       { return      ReverseRow(     Row(*this, y)); }
	ConstReverseRow constReverseRow(size_t y) const { return ConstReverseRow(ConstRow(*this, y)); }

	     Column      column(size_t x)       { return      Column(*this, x); }
	ConstColumn constColumn(size_t x) const { return ConstColumn(*this, x); }

	     ReverseColumn      reverseColumn(size_t x)       { return      ReverseColumn(     Column(*this, x)); }
	ConstReverseColumn constReverseColumn(size_t x) const { return ConstReverseColumn(ConstColumn(*this, x)); }

	     TopDownDiagonal      topDownDiagonal(size_t i)       { return      TopDownDiagonal(*this, i); }
	ConstTopDownDiagonal constTopDownDiagonal(size_t i) const { return ConstTopDownDiagonal(*this, i); }

	     ReverseTopDownDiagonal      reverseTopDownDiagonal(size_t i)       { return      ReverseTopDownDiagonal(     TopDownDiagonal(*this, i)); }
	ConstReverseTopDownDiagonal constReverseTopDownDiagonal(size_t i) const { return ConstReverseTopDownDiagonal(ConstTopDownDiagonal(*this, i)); }

	     BottomUpDiagonal      bottomUpDiagonal(size_t i)       { return      BottomUpDiagonal(*this, i); }
	ConstBottomUpDiagonal constBottomUpDiagonal(size_t i) const { return ConstBottomUpDiagonal(*this, i); }

	     ReverseBottomUpDiagonal      reverseBottomUpDiagonal(size_t i)       { return      ReverseBottomUpDiagonal(     BottomUpDiagonal(*this, i)); }
	ConstReverseBottomUpDiagonal constReverseBottomUpDiagonal(size_t i) const { return ConstReverseBottomUpDiagonal(ConstBottomUpDiagonal(*this, i)); }

	     AdjacentNeighbors      adjacentNeighbors(size_t x, size_t y)       { return      AdjacentNeighbors(*this, x, y); }
	ConstAdjacentNeighbors constAdjacentNeighbors(size_t x, size_t y) const { return ConstAdjacentNeighbors(*this, x, y); }

private:
	std::vector<std::vector<T>> cells_;
};

#undef DEFINE_BOUNDED_GRID2D_SEQUENCE_TO_STRING_METHOD

template<class T=int>
class PuzzleSolution
{
public:
	virtual ~PuzzleSolution() {}

public:
	int run(const std::string& puzzleInputFilePath);

protected:
	const std::vector<std::string>& input() const { return input_; }

protected:
	virtual void processInput(const std::string& puzzleInputFilePath);

private:
	virtual T answer1() = 0;
	virtual T answer2() = 0;

private:
	std::vector<std::string> input_;
};

template<class T>
int PuzzleSolution<T>::run(const std::string& puzzleInputFilePath)
{
	processInput(puzzleInputFilePath);

	fmt::println("{}", answer1());
	fmt::println("{}", answer2());

	return EXIT_SUCCESS;
}

template<class T>
void PuzzleSolution<T>::processInput(const std::string& puzzleInputFilePath)
{
	input_ = loadPuzzleInput(puzzleInputFilePath);
}

class MathUtils
{
public:
	template<class T>
	static bool isInRange(const T& value, const T& rangeStart, const T& rangeEnd)
	{
		return (value >= rangeStart) && (value <= rangeEnd);
	}

	static int manhattanDistance(int x1, int y1, int x2, int y2)
	{
		return abs(x2 - x1) + abs(y2 - y1);
	}
};

class StringUtils
{
public:
	static bool contains(const std::string& str,
	                     const std::string& substr);
	static bool contains(const std::string& str, char ch);

	static size_t countOverlappingSubstrings(const std::string& str,
	                                         const std::string& substr);
	static size_t countNonOverlappingSubstrings(const std::string& str,
	                                            const std::string& substr);

	static std::string replaceFirst(const std::string& str,
	                                const std::string& substr,
	                                const std::string& substr2);
	static std::string replaceLast(const std::string& str,
	                               const std::string& substr,
	                               const std::string& substr2);
	static std::string replaceAll(const std::string& str,
	                              const std::string& substr,
	                              const std::string& substr2);

	static std::vector<std::string> tokenize(const std::string& str,
	                                         char delimiter=',',
	                                         bool trimWhitespace=true,
	                                         bool skipEmptyTokens=false);

	static std::string trim(const std::string& str);
	static std::string ltrim(const std::string& str);
	static std::string rtrim(const std::string& str);

	static std::string trim(std::string str, char ch);
	static std::string ltrim(std::string str, char ch);
	static std::string rtrim(std::string str, char ch);

	static void generateVariations(const std::set<char>& elements,
	                               size_t k,
	                               std::vector<std::string>& variations);

private:
	static void generateVariations(const std::set<char>& elements,
	                               size_t k,
	                               size_t i,
	                               std::string sequence,
	                               std::vector<std::string>& variations);
};

} // namespace aoc

#endif // #ifndef AOC_H
