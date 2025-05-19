#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <map>
#include <regex>
#include <set>
#include <string>

using namespace fmt;
using namespace aoc;
using namespace std;

class RoomListEntry
{
	friend bool operator==(const RoomListEntry&,
	                       const RoomListEntry&) = default;

public:
	RoomListEntry(const string& encryptedName,
	              int sectorID,
	              const string& checksum)
		: encryptedName_(encryptedName)
		, sectorID_(sectorID)
		, checksum_(checksum)
	{
	}

public:
	const string& encryptedName() const { return encryptedName_; }
	const string decryptedName() const;
	const string& checksum() const { return checksum_; }
	int sectorID() const { return sectorID_; }

public:
	bool isReal() const { return checksum_ == generateChecksum(); }

#ifdef AOC_TEST_SOLUTION
public:
#else
private:
#endif
	string generateChecksum() const;

private:
	string encryptedName_;
	string checksum_;
	int sectorID_;
};

const string RoomListEntry::decryptedName() const
{
	constexpr size_t asciiAlphabetSize = 'z' - 'a' + 1;

	const size_t letterOffset = sectorID_ % asciiAlphabetSize;

	string decryptedName;

	for(char letter : encryptedName_)
	{
		if(letter != '-')
		{
			const size_t letterIndex = letter - 'a';

			decryptedName += (letterIndex + letterOffset < asciiAlphabetSize)
				? letter + letterOffset
				: 'a' + (letterIndex + letterOffset) - asciiAlphabetSize;
		}
		else
		{
			decryptedName += ' ';
		}
	}

	return decryptedName;
}

string RoomListEntry::generateChecksum() const
{
	map<char, int> encryptedNameLetterHistogram;

	for(char letter : encryptedName_)
		if(letter != '-')
			++encryptedNameLetterHistogram[letter];

	map<int, set<char>, greater<int>> numLettersToLetters;

	for(const auto& entry : encryptedNameLetterHistogram)
		numLettersToLetters[entry.second].insert(entry.first);

	string checksum;

	for(const auto& entry : numLettersToLetters)
	{
		for(char letter : entry.second)
		{
			checksum += letter;

			if(checksum.length() == 5)
				return checksum;
		}
	}

	AOC_ENFORCE(checksum.length() >= 5);

	return checksum;
}

RoomListEntry parseRoomListEntry(const string& line)
{
	static const regex roomListEntryRegex(R"(([a-z][a-z\-]+[a-z])-(\d+)\[([a-z]+)\])");

	smatch roomListEntryMatch;

	if(regex_match(line,
	               roomListEntryMatch,
	               roomListEntryRegex))
	{
		constexpr size_t expectedRoomListEntryMatchSize = 4;

		AOC_ENFORCE(roomListEntryMatch.size() == expectedRoomListEntryMatchSize);

		return RoomListEntry(roomListEntryMatch[1],
		                     stoi(roomListEntryMatch[2]),
		                     roomListEntryMatch[3]);
	}
	else
		panic(format("invalid room list entry: {}", line));

	return RoomListEntry("", 0, "");
}

int calculateSumOfSectorIDsOfTheRealRooms(const vector<string>& roomList)
{
	int sumOfSectorIDsOfTheRealRooms = 0;

	for(const auto& line : roomList)
	{
		RoomListEntry entry = parseRoomListEntry(line);

		if(entry.isReal())
			sumOfSectorIDsOfTheRealRooms += entry.sectorID();
	}

	return sumOfSectorIDsOfTheRealRooms;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("parseRoomListEntry")
{
	CHECK(parseRoomListEntry("aaaaa-bbb-z-y-x-123[abxyz]") == RoomListEntry("aaaaa-bbb-z-y-x", 123, "abxyz"));
	CHECK(parseRoomListEntry("a-b-c-d-e-f-g-h-987[abcde]") == RoomListEntry("a-b-c-d-e-f-g-h", 987, "abcde"));
	CHECK(parseRoomListEntry("not-a-real-room-404[oarel]") == RoomListEntry("not-a-real-room", 404, "oarel"));
	CHECK(parseRoomListEntry("totally-real-room-200[decoy]") == RoomListEntry("totally-real-room", 200, "decoy"));
}

TEST_CASE("calculateSumOfSectorIDsOfTheRealRooms")
{
	const vector<string> roomList =
	{
		"aaaaa-bbb-z-y-x-123[abxyz]",
		"a-b-c-d-e-f-g-h-987[abcde]",
		"not-a-real-room-404[oarel]",
		"totally-real-room-200[decoy]"
	};

	CHECK(calculateSumOfSectorIDsOfTheRealRooms(roomList) == 1514);
}

TEST_CASE("RoomListEntry")
{
	SUBCASE("generateChecksum")
	{
		CHECK(parseRoomListEntry("aaaaa-bbb-z-y-x-123[abxyz]").generateChecksum() == "abxyz");
		CHECK(parseRoomListEntry("a-b-c-d-e-f-g-h-987[abcde]").generateChecksum() == "abcde");
		CHECK(parseRoomListEntry("not-a-real-room-404[oarel]").generateChecksum() == "oarel");
		CHECK(parseRoomListEntry("totally-real-room-200[decoy]").generateChecksum() == "loart");
		CHECK(parseRoomListEntry("qzmt-zixmtkozy-ivhz-343[zimth]").generateChecksum() == "zimth");
	}

	SUBCASE("isReal")
	{
		CHECK(parseRoomListEntry("aaaaa-bbb-z-y-x-123[abxyz]").isReal());
		CHECK(parseRoomListEntry("a-b-c-d-e-f-g-h-987[abcde]").isReal());
		CHECK(parseRoomListEntry("not-a-real-room-404[oarel]").isReal());
		CHECK(!parseRoomListEntry("totally-real-room-200[decoy]").isReal());
		CHECK(parseRoomListEntry("qzmt-zixmtkozy-ivhz-343[zimth]").isReal());
	}

	SUBCASE("decryptedName")
	{
		CHECK(parseRoomListEntry("qzmt-zixmtkozy-ivhz-343[zimth]").decryptedName() == "very encrypted name");
	}
}

#else // #ifdef AOC_TEST_SOLUTION

class SecurityThroughObscurity : public PuzzleSolution<int>
{
private:
	int answer1() override;
	int answer2() override;
};

int SecurityThroughObscurity::answer1()
{
	return calculateSumOfSectorIDsOfTheRealRooms(input());
}

int SecurityThroughObscurity::answer2()
{
	for(const auto& line : input())
	{
		RoomListEntry entry = parseRoomListEntry(line);

		if(entry.isReal())
			if(entry.decryptedName() == "northpole object storage")
				return entry.sectorID();
	}

	panic("northpole object storage room not found");

	return 0;
}

int main(int argc, char* argv[])
{
	return SecurityThroughObscurity().run((argc > 1) ? argv[1] : "201604.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
