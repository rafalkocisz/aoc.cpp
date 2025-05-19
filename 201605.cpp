#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#else
#define AOC_USE_CINEMATIC_DECRYPTING_ANIMATION
#include <iostream>
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <cctype>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

using namespace fmt;
using namespace aoc;
using namespace std;

constexpr size_t doorPasswordLength = 8;

class MD5
{
public:
	MD5();
	MD5(const string& str);

public:
	void update(const unsigned char* buffer,
	            size_t size);
	void update(const char* buffer,
	            size_t size);
	MD5& finalize();

public:
	static string digest(const string& str);

public:
	string digest() const;
 
private:
	static constexpr size_t blockSize = 64;

	static constexpr auto S11 = 7;
	static constexpr auto S12 = 12;
	static constexpr auto S13 = 17;
	static constexpr auto S14 = 22;
	static constexpr auto S21 = 5;
	static constexpr auto S22 = 9;
	static constexpr auto S23 = 14;
	static constexpr auto S24 = 20;
	static constexpr auto S31 = 4;
	static constexpr auto S32 = 11;
	static constexpr auto S33 = 16;
	static constexpr auto S34 = 23;
	static constexpr auto S41 = 6;
	static constexpr auto S42 = 10;
	static constexpr auto S43 = 15;
	static constexpr auto S44 = 21;

private:
	void initialize();
	void transform(const uint8_t block[blockSize]);

private:
	static void decode(uint32_t output[], const uint8_t input[], size_t size);
	static void encode(uint8_t output[], const uint32_t input[], size_t size);
	static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t rotateLeft(uint32_t x, int n);
	static inline void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);

private:
	bool isFinalized_;
	uint8_t buffer_[blockSize];
	uint32_t count_[2];
	uint32_t state_[4];
	uint8_t digest_[16];
};

inline uint32_t MD5::F(uint32_t x, uint32_t y, uint32_t z)
{
	return x&y | ~x&z;
}
 
inline uint32_t MD5::G(uint32_t x, uint32_t y, uint32_t z)
{
	return x&z | y&~z;
}
 
inline uint32_t MD5::H(uint32_t x, uint32_t y, uint32_t z)
{
	return x^y^z;
}
 
inline uint32_t MD5::I(uint32_t x, uint32_t y, uint32_t z)
{
	return y ^ (x | ~z);
}
 
inline uint32_t MD5::rotateLeft(uint32_t x, int n)
{
	return (x << n) | (x >> (32-n));
}
 
inline void MD5::FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
	a = rotateLeft(a+ F(b, c, d) + x + ac, s) + b;
}
 
inline void MD5::GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
	a = rotateLeft(a + G(b, c, d) + x + ac, s) + b;
}
 
inline void MD5::HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
	a = rotateLeft(a + H(b, c, d) + x + ac, s) + b;
}
 
inline void MD5::II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
	a = rotateLeft(a + I(b, c, d) + x + ac, s) + b;
}

MD5::MD5()
{
	initialize();
}

MD5::MD5(const string& str)
{
	initialize();

	update(str.c_str(), str.length());

	finalize();
}

void MD5::initialize()
{
	isFinalized_ = false;

	count_[0] = 0;
	count_[1] = 0;

	state_[0] = 0x67452301;
	state_[1] = 0xefcdab89;
	state_[2] = 0x98badcfe;
	state_[3] = 0x10325476;
}

void MD5::decode(uint32_t output[], const uint8_t input[], size_t size)
{
	for(size_t i=0,j=0; j<size; ++i,j+=4)
		output[i] = ((uint32_t)input[j]) |
		            (((uint32_t)input[j+1]) << 8) |
		            (((uint32_t)input[j+2]) << 16) |
		            (((uint32_t)input[j+3]) << 24);
}

void MD5::encode(uint8_t output[], const uint32_t input[], size_t size)
{
	for(size_t i=0,j=0; j<size; ++i,j+=4)
	{
		output[j] = input[i] & 0xff;
		output[j+1] = (input[i] >> 8) & 0xff;
		output[j+2] = (input[i] >> 16) & 0xff;
		output[j+3] = (input[i] >> 24) & 0xff;
	}
}

void MD5::transform(const uint8_t block[blockSize])
{
	uint32_t a = state_[0];
	uint32_t b = state_[1];
	uint32_t c = state_[2];
	uint32_t d = state_[3];
	uint32_t x[16];

	decode(x, block, blockSize);

	// Round 1
	FF(a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	// Round 2
	GG(a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	// Round 3
	HH(a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH(a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	// Round 4
	II(a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	state_[0] += a;
	state_[1] += b;
	state_[2] += c;
	state_[3] += d;

	memset(x, 0, sizeof(x));
}

void MD5::update(const unsigned char input[], size_t size)
{
	size_t index = count_[0] / 8 % blockSize;

	if((count_[0] += (size << 3)) < (size << 3))
		++count_[1];

	count_[1] += (size >> 29);

	const size_t firstPart = 64 - index;

	size_t i = 0;

	if(size >= firstPart)
	{
		memcpy(&buffer_[index], input, firstPart);
		transform(buffer_);
		
		for(i=firstPart; i+blockSize<=size; i+=blockSize)
			transform(&input[i]);

		index = 0;
	}
	else
		i = 0;

	memcpy(&buffer_[index], &input[i], size-i);
}

void MD5::update(const char input[], size_t size)
{
	update((const unsigned char*)input, size);
}

MD5& MD5::finalize()
{
	AOC_ENFORCE(!isFinalized_);

	static const unsigned char padding[64] =
	{
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	unsigned char bits[8];

	encode(bits, count_, 8);
 
	const size_t index = count_[0] / 8 % 64;
	const size_t paddingLength = (index < 56) ? (56 - index) : (120 - index);

	update(padding, paddingLength);
	update(bits, 8);
	encode(digest_, state_, 16);
	memset(buffer_, 0, sizeof(buffer_));
	memset(count_, 0, sizeof(count_));

	isFinalized_ = true;

	return *this;
}

string MD5::digest(const string& str)
{
	MD5 md5(str);

	return md5.digest();
}

string MD5::digest() const
{
	AOC_ENFORCE(isFinalized_);

	char buffer[33];

	for(int i=0; i<16; ++i)
		sprintf(buffer + i * 2, "%02x", digest_[i]);

	buffer[32]=0;

	return string(buffer);
}

string generateDoorPassword(const string& doorID)
{
	string doorPassword;

#ifdef AOC_USE_CINEMATIC_DECRYPTING_ANIMATION
	cout << "________";
#endif

	for(unsigned long long i=0; i<=numeric_limits<unsigned long long>::max(); ++i)
	{
		const string hash = MD5::digest(doorID + to_string(i));

		AOC_ASSERT(hash.size() == 32);

		if(hash.starts_with("00000"))
		{
			doorPassword += hash[5];

#ifdef AOC_USE_CINEMATIC_DECRYPTING_ANIMATION
			cout << format("\r{}{}",
			               doorPassword,
			               string(doorPasswordLength - doorPassword.size(), '_'));
#endif
			if(doorPassword.size() == doorPasswordLength)
			{
#ifdef AOC_USE_CINEMATIC_DECRYPTING_ANIMATION
				cout << "\r";
#endif
				return doorPassword;
			}
		}
	}

	panic("failed to generate door password");

	return "";
}

inline size_t digitToPosition(char digit)
{
	AOC_ASSERT(isdigit(digit));

	return digit - '0';
}

string generateDoorPassword2(const string& doorID)
{
	string doorPassword(doorPasswordLength, '_');

#ifdef AOC_USE_CINEMATIC_DECRYPTING_ANIMATION
	cout << doorPassword;
#endif
	for(unsigned long long i=0; i<=numeric_limits<unsigned long long>::max(); ++i)
	{
		const string hash = MD5::digest(doorID + to_string(i));

		AOC_ASSERT(hash.size() == 32);

		if(hash.starts_with("00000"))
		{
			if(isdigit(hash[5]))
			{
				const size_t position = digitToPosition(hash[5]);

				if(position < doorPasswordLength)
				{
					if(doorPassword[position] == '_')
					{
						doorPassword[position] = hash[6];

#ifdef AOC_USE_CINEMATIC_DECRYPTING_ANIMATION
						cout << "\r" << doorPassword;
#endif
						if(!StringUtils::contains(doorPassword, '_'))
						{
#ifdef AOC_USE_CINEMATIC_DECRYPTING_ANIMATION
							cout << "\r";
#endif
							return doorPassword;
						}
					}
				}
			}
		}
	}

	panic("failed to generate door password");

	return "";
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("MD5")
{
	CHECK(MD5::digest("have") == "b42dad5453b2128a32f6612b13ea5d9f");
	CHECK(MD5::digest("linotype") == "ceb714da473ff329e66e3447bf39c698");
	CHECK(MD5::digest("bits") == "cc411e6c13670e52124629b8ac83f7d0");
	CHECK(MD5::digest("distributions") == "67c9924929e6e71642d037d6fefe4a68");
	CHECK(MD5::digest("young") == "f6182f0359f72aae12fb90d305ccf9eb");
	CHECK(MD5::digest("gambol") == "7cce80fb6a1d8895ddfdbfaabba49ed4");
	CHECK(MD5::digest("suppress") == "674408fa67da8075268336c018ec6fc1");
	CHECK(MD5::digest("sauerkraut") == "52381cef39ef6cf8b58cf1f710f05270");
	CHECK(MD5::digest("gsxr750") == "ec1ffa08791e0ae7811f924ff86f1600");
	CHECK(MD5::digest("luminous") == "3b34aae43a0c2f0a58d9fb6faa984bab");
	CHECK(MD5::digest("doitnow") == "66e5dcdf55aa868b3bfdce7845978535");
	CHECK(MD5::digest("evaded") == "4b636015da21a66943b43217559e0131");
	CHECK(MD5::digest("committing") == "724ba0730054fa08f5ce69caa4361dd7");
	CHECK(MD5::digest("telethon") == "9c998372cc089401eded28f3d50e3d83");
	CHECK(MD5::digest("narcissus") == "2edd9e1242a8c7e54701689a5dbf6bad");
	CHECK(MD5::digest("coziness") == "1aa05cfe064cae2ca7648efe5133cce6");
	CHECK(MD5::digest("because") == "cc70865f1ecd933ec055851deb5bd69a");
	CHECK(MD5::digest("mitigative") == "3b8314b159f6e756209d26b21cf93f92");
	CHECK(MD5::digest("sequel") == "66124aaa9b39896c92d058e985281e72");
	CHECK(MD5::digest("midnight") == "1506aeeb8c3a699b1e3c87db03156428");
	CHECK(MD5::digest("dynamited") == "7c081b90fbe1cead93ccc91d8a7c1262");
	CHECK(MD5::digest("vanes") == "5fb8f7656e91d02703ca8389529afb83");
	CHECK(MD5::digest("adjusts") == "cba8462cf982e1a02ff3ee229fdb4468");
	CHECK(MD5::digest("harry") == "3b87c97d15e8eb11e51aa25e9a5770e9");
	CHECK(MD5::digest("crusty") == "2c7c461c7fb173557c8c56478c21ec29");
	CHECK(MD5::digest("partly") == "d05332da9294920b4cc9f31595b58e7f");
	CHECK(MD5::digest("slow") == "4fe9dfe784a2be25dd8aec3b403c3654");
	CHECK(MD5::digest("quartile") == "ea694530c3e4967afe81b0e0ef67d54e");
	CHECK(MD5::digest("speechlessness") == "86cec50d48d4f74079bc5b41a10d49b4");
	CHECK(MD5::digest("percussion") == "bc0170ec03f46929a82db213582a0bbe");
	CHECK(MD5::digest("centenary") == "ff62f69fd61cd51db7c2761d89c62d1f");
	CHECK(MD5::digest("upturns") == "c6d1aaa162998e126d3d4bd191f9a23d");
	CHECK(MD5::digest("moorish") == "7f328536fad334de99af56e40a379603");
	CHECK(MD5::digest("grantor") == "9ac0fd27b4f974c996fe1b4a1be97c3f");
	CHECK(MD5::digest("fahey") == "f7d2242829b7191398d8026a158313c8");
	CHECK(MD5::digest("chemisorb") == "fe763169bec57a97aae602ac7bf8bffe");
	CHECK(MD5::digest("leer") == "3f60eb7939d4fdcb9244c0999b175f09");
	CHECK(MD5::digest("kirchner") == "b33e01101cf162b27b2bca000bc0a16d");
	CHECK(MD5::digest("collocation") == "e7d1889b24c5095eba0bd08e47d454dc");
	CHECK(MD5::digest("depress") == "ff1bcdbe49f5804d5df4c2268a42d3d1");
	CHECK(MD5::digest("waft") == "e384d87d5de535612145b8e6f5e38076");
	CHECK(MD5::digest("amort") == "aa2bd19b0d5d103cd31b022e45fa7d41");
	CHECK(MD5::digest("strolling") == "6eadcf66e8c789b90450090d36dc3b12");
	CHECK(MD5::digest("effectors") == "599cd2b7c932d44cd628dc40d0cb9b47");
	CHECK(MD5::digest("booster") == "449a6bfbd6a81186eba0c3b996662b49");
	CHECK(MD5::digest("stub") == "e8f65fd8d973f9985dc7ea3cf1614ae1");
	CHECK(MD5::digest("proscribe") == "0a20f93a8da57b664df1a55bc9e13326");
	CHECK(MD5::digest("formate") == "8dc7ffb0ac0bf9510c441cfe802c8253");
	CHECK(MD5::digest("taxpaying") == "a49580ec8b28086fa97ef6c3786515b1");
	CHECK(MD5::digest("coagulable") == "11800cfc8592b5bea4513139a7f03174");
	CHECK(MD5::digest("adverse") == "8abdad3ba6bcf4a7125fa5e6f029ae69");
	CHECK(MD5::digest("dictionarys") == "5af878227534762d6edd5fca315b886f");
	CHECK(MD5::digest("simplifications") == "57d224397c23c116ca7a07a890e716b8");
	CHECK(MD5::digest("puritan") == "cd914ed57dc2d4157a7e1d43d7853778");
	CHECK(MD5::digest("annulled") == "6487853b1acaba95e5982ede7b1d3227");
	CHECK(MD5::digest("infamous") == "efe1fadc9a0ca29a1f376cdd3c83e298");
	CHECK(MD5::digest("scientists") == "0d40887127bee8f7731f5c4f95eb5331");
	CHECK(MD5::digest("paulson") == "fe5963abdee2744878a3470b3ddbea73");
	CHECK(MD5::digest("splinter") == "1f74df58d1c0f7b55b5caa01e6c7f806");
	CHECK(MD5::digest("devilman") == "e2f4b11ba4591d2a84e8462623b45d60");
	CHECK(MD5::digest("canoes") == "8f0c946fa197dffeebcab2213f7393cd");
	CHECK(MD5::digest("campaigner") == "2537d33515ab01e3f4de8a9c21eba48a");
	CHECK(MD5::digest("intimating") == "2f4f3a7128cb202392f6ce849d7a49ac");
	CHECK(MD5::digest("rube") == "6b872687df9aba1933ac59abdab6302f");
	CHECK(MD5::digest("seller") == "64c9ac2bb5fe46c3ac32844bb97be6bc");
	CHECK(MD5::digest("maximums") == "32a47a075026654ec2cc11e7eb54223e");
	CHECK(MD5::digest("mickey") == "4d5257e5acc7fcac2f5dcd66c4e78f9a");
	CHECK(MD5::digest("underlying") == "4ab57483724829010c32e288b605d5e5");
	CHECK(MD5::digest("gould") == "16b65bd37aa7f2eb751ff12755d49730");
	CHECK(MD5::digest("alexandra") == "9db3c8f773c85206e661d7538de50fe7");
	CHECK(MD5::digest("enveloper") == "6ff4d3fb60214892c9634ec143443bc6");
	CHECK(MD5::digest("overload") == "1cf9f8be06cdce86bfde06213124c08b");
	CHECK(MD5::digest("isentropic") == "e9bef3d7f0bb29fb53c7ae00d5ee6745");
	CHECK(MD5::digest("reputable") == "175eb4b69d4ff04e56f81927059a636f");
	CHECK(MD5::digest("cortland") == "39959bb750e3a2f32429a487a9862397");
	CHECK(MD5::digest("bearings") == "6fc2bdbafc4589d65f9b49dfc00e88fb");
	CHECK(MD5::digest("thank") == "f5ab96926a6a561aab7ecb4ba7c915d4");
	CHECK(MD5::digest("scranton") == "7eaa20d9293ce488d920c703e9006af9");
	CHECK(MD5::digest("candidly") == "c7c432e687cdbb698582502416bf76ad");
	CHECK(MD5::digest("hug") == "af231c77af76002cdaec813c29a6db09");
	CHECK(MD5::digest("depositors") == "03d5181bcf7e81a6c409574a409a4995");
	CHECK(MD5::digest("sigmachi") == "c2a19a20002c4b99964c14f23f99981a");
	CHECK(MD5::digest("expeditious") == "fb1880bd907dedbf8a7c628f7c380c05");
	CHECK(MD5::digest("arrogated") == "3c73e632a45fdedec5dbc24d2a15594f");
	CHECK(MD5::digest("beauties") == "11d9d4907d53028875120807aa296b12");
	CHECK(MD5::digest("enforcement") == "42f349ce49f76cbbaeec781f7c4c6bd4");
	CHECK(MD5::digest("apatite") == "158626882b06fcd06e77bf328d8f2194");
	CHECK(MD5::digest("smoldered") == "cb6a40a965d805f37e0c720fc4cd54a0");
	CHECK(MD5::digest("masks") == "ce38fdcaab276279ceda3f9c29a0d153");
	CHECK(MD5::digest("travertine") == "48daa0b0242422dfe63b26c9f0abca62");
	CHECK(MD5::digest("transmit") == "f477de506a932b4c64020556d2d4b40b");
	CHECK(MD5::digest("olympic") == "133564794a1bfbfad19be33f1b1e341c");
	CHECK(MD5::digest("wolff") == "7158513bcdad9fbdf2409e8ef89b878a");
	CHECK(MD5::digest("qwertyui") == "22d7fe8c185003c98f97e5d6ced420c7");
	CHECK(MD5::digest("ceases") == "9e7268ca8e53a2f185b86a7d30cf9afb");
	CHECK(MD5::digest("worldwide") == "663c92cbb27d188f32533fc2ec4c1652");
	CHECK(MD5::digest("mcdermott") == "7bdb4d5721d0e4edb43f54f7f65bcdf1");
	CHECK(MD5::digest("inventions") == "ab4e67b11a87f6acc34c0e52973230c6");
	CHECK(MD5::digest("ullman") == "e0e3baf6a07496f9d1a1486ee9412100");
	CHECK(MD5::digest("watercourse") == "f806d529f7dbcf24f96b94961b81bccb");
}

TEST_CASE("generateDoorPassword")
{
	CHECK(generateDoorPassword("abc") == "18f47a30");
}

TEST_CASE("digitToPosition")
{
	CHECK(digitToPosition('0') == 0);
	CHECK(digitToPosition('1') == 1);
	CHECK(digitToPosition('2') == 2);
	CHECK(digitToPosition('3') == 3);
	CHECK(digitToPosition('4') == 4);
	CHECK(digitToPosition('5') == 5);
	CHECK(digitToPosition('6') == 6);
	CHECK(digitToPosition('7') == 7);
	CHECK(digitToPosition('8') == 8);
	CHECK(digitToPosition('9') == 9);
}

TEST_CASE("generateDoorPassword2")
{
	CHECK(generateDoorPassword2("abc") == "05ace8e3");
}

#else // #ifdef AOC_TEST_SOLUTION

class HowAboutANiceGameOfChess : public PuzzleSolution<string>
{
private:
	string answer1() override;
	string answer2() override;
};

string HowAboutANiceGameOfChess::answer1()
{
	AOC_ENFORCE(input().size() == 1);

	const string& doorId = input()[0];

	return generateDoorPassword(doorId);
}

string HowAboutANiceGameOfChess::answer2()
{
	AOC_ENFORCE(input().size() == 1);

	const string& doorId = input()[0];

	return generateDoorPassword2(doorId);
}

int main(int argc, char* argv[])
{
	return HowAboutANiceGameOfChess().run((argc > 1) ? argv[1] : "201605.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
