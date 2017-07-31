

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <complex>

/** these test cases test out the value converters
*/
#include "helics/application_api/ValueConverter.hpp"

BOOST_AUTO_TEST_SUITE(value_converter_tests)

template<class X>
void converterTests(const X &testValue1, const X &testValue2, size_t sz1=0, size_t sz2=0, const std::string &type="")
{
	auto converter = helics::ValueConverter<X>();

	//check the type
	if (!type.empty())
	{
		auto typeString = converter.type();
		BOOST_CHECK_EQUAL(type, typeString);
	}
	
	//check the size
	if (sz1 > 0)
	{
		auto sz = converter.size(testValue1);
		BOOST_CHECK_EQUAL(sz, sz1);
	}
	
	//convert to a data view
	auto dv = converter.convert(testValue1);
	if (sz1 > 0)
	{
		BOOST_CHECK_EQUAL(dv.size(), sz1);
	}
	
	//convert back to a value
	auto res = converter.interpret(dv);
	BOOST_CHECK(res==testValue1);
	//convert back to a value in a different way
	X res2;
	converter.interpret(dv, res2);
	BOOST_CHECK(res2==testValue1);

	helics::data_block db;

	converter.convert(testValue2, db);
	if (sz2 > 0)
	{
		BOOST_CHECK_EQUAL(db.size(), sz2);
	}
	
	auto res3 = converter.interpret(db);
	BOOST_CHECK(res3==testValue2);
}

BOOST_AUTO_TEST_CASE(converter_tests)
{
	converterTests(45.54, 23.7e-7, sizeof(double) + 4, sizeof(double) + 4, "double");
	converterTests<int>(45, -234252, sizeof(int) + 4, sizeof(int) + 4, "int32");
	converterTests<uint64_t>(352, 0x2323427FA, sizeof(uint64_t) + 4, sizeof(uint64_t) + 4, "uint64");

	converterTests('r', 't', 1, 1, "char");

	converterTests(static_cast<unsigned char>(223), static_cast<unsigned char>(46), 1, 1, "uchar");

	using compd = std::complex<double>;
	compd v1{ 1.7,0.9 };
	compd v2{ -34e5,0.345 };
	converterTests(v1, v2, sizeof(compd) + 4, sizeof(compd) + 4, "complex");
	std::string testValue1 = "this is a string test";
	std::string test2 = "";
	converterTests(testValue1, test2, testValue1.size(), test2.size(), "string");
	//test a vector
	using vecd = std::vector<double>;
	vecd vec1 = { 45.4,23.4,-45.2,34.2234234 };
	vecd testv2(234, 0.45);
	converterTests<vecd>(vec1, testv2, vec1.size() * sizeof(double) + 4, testv2.size() * sizeof(double) + 4, "vector_double");

}


/** this one is a bit annoying to use the template so it gets its own case 
we are testing vectors of strings
*/
BOOST_AUTO_TEST_CASE(vector_string_converter_tests)
{
	using vecstr = std::vector<std::string>;
	auto converter = helics::ValueConverter<vecstr>();

	vecstr testValue1 = { "test1", "test45","this is a longer string to test", ""};
	//check the type
	auto type = converter.type();
	BOOST_CHECK_EQUAL(type, "string_vector");
	//check the size
	auto sz = converter.size(testValue1);
	size_t expsize = 2;
	for (auto &str : testValue1)
	{
		expsize += 2 + str.size();
	}
	BOOST_CHECK_EQUAL(sz, expsize);
	//convert to a data view
	auto dv = converter.convert(testValue1);
	BOOST_CHECK_EQUAL(dv.size(), expsize);
	//convert back to a vector
	auto val = converter.interpret(dv);
	BOOST_CHECK(val == testValue1);
	//convert back to a string in a different way
	vecstr val2;
	converter.interpret(dv, val2);
	BOOST_CHECK(val2 == testValue1);

	vecstr test2{ "test string 1", "*SDFSDF*JJ\nSSFSDsdkjflsdjflsdkfjlskdbnowhfoihfoai\0shfoaishfoasifhaofsihaoifhaosifhaosfihaosfihaosfihaohoaihsfiohoh" };
	helics::data_block db;

	converter.convert(test2, db);
	
	auto val3 = converter.interpret(db);
	BOOST_CHECK(val3 == test2);

}

BOOST_AUTO_TEST_CASE(test_block_vectors)
{
	using vecblock = std::vector<helics::data_block>;
	auto converter = helics::ValueConverter<vecblock>();

	vecblock vb(4);
	vb[0] = helics::data_block(437, '<');
	vb[1] = "*SDFSDF*JJ\nSSFSDsdkjflsdjflsdkfjlskdbnowhfoihfoaishfoai\0shfoasifhaofsihaoifhaosifhaosfihaosfihaosfihaohoaihsfiohoh";
	vb[2] = helics::ValueConverter<double>::convert(3.1415);
	vb[3] = helics::ValueConverter<int>::convert(9999);

	auto rb = converter.convert(vb);

	auto res = helics::ValueConverter<std::vector<helics::data_view>>::interpret(rb);

	BOOST_REQUIRE_EQUAL(res.size(), vb.size());
	BOOST_CHECK_EQUAL(res[0].size(), vb[0].size());
	BOOST_CHECK_EQUAL(res[0][5], vb[0][5]);

	BOOST_CHECK(res[1].string() == vb[1].string());

	BOOST_CHECK_EQUAL(3.1415, helics::ValueConverter<double>::interpret(res[2]));
	BOOST_CHECK_EQUAL(9999, helics::ValueConverter<int>::interpret(res[3]));

	auto res2 = helics::ValueConverter<std::vector<helics::data_block>>::interpret(rb);

	BOOST_REQUIRE_EQUAL(res2.size(), vb.size());
	BOOST_CHECK_EQUAL(res2[0].size(), vb[0].size());
	BOOST_CHECK_EQUAL(res2[0][5], vb[0][5]);

	BOOST_CHECK(res2[1].string() == vb[1].string());

	BOOST_CHECK_EQUAL(3.1415, helics::ValueConverter<double>::interpret(res2[2]));
	BOOST_CHECK_EQUAL(9999, helics::ValueConverter<int>::interpret(res2[3]));
}

/** check that the converters do actually throw on invalid sizes*/
BOOST_AUTO_TEST_CASE(test_cnverter_errors)
{
	auto vb1 = helics::ValueConverter<double>::convert(3.1415);
	auto vb2 = helics::ValueConverter<int>::convert(10);

	BOOST_CHECK_THROW(helics::ValueConverter<double>::interpret(vb2), std::invalid_argument);
	BOOST_CHECK_THROW(helics::ValueConverter<int>::interpret(vb1), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
