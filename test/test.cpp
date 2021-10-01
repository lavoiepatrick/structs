#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "streamout.h"

#include "../include/structs/to_struct.h"
#include "../include/structs/to_tuple.h"

#include <iostream>
#include <vector>

struct O
{
	int p1;
	bool p2;
	float* p3;
	std::vector< bool > p4;
};

struct S
{
	int i;
	bool b;
	double d;
	std::optional< std::string > t;
};

struct S2
{
	int i;
	bool b;
	double d;
	std::optional< std::string > t;
	std::vector< int > p4;
	std::optional< std::vector< unsigned > > p5;
};

template<>
struct structs::to_tuple_size< S > : std::integral_constant< std::size_t, 4 >
{
};

template<>
struct structs::to_tuple_size< S2 > : std::integral_constant< std::size_t, 6 >
{
};

template< typename T >
std::string to_string( T&& v )
{
	std::ostringstream os;
	os << streamout( v );
	return os.str( );
}

TEST_CASE( "structs" )
{
	SECTION( "stream out" )
	{
		struct O
		{
			std::string p1;
			const char* p2;
			std::vector< int > p3;
		};

		struct S
		{
			int p1;
			double p2;
			O p3;
			std::vector< O > p4;
		};

		struct T
		{
			int p1;
			double p2;
		};

		auto t = structs::to_tuple( T { 1, 2.0 } );
		static_assert( std::is_same< std::tuple< int, double >, decltype( t ) > { } );
		REQUIRE( to_string( t ) == R"({1, 2})" );

		auto t1 = structs::to_tuple( S { 1,
										 2.0,
										 { "test1", "test2", { 1, 5, 10 } },
										 { { "nested1", "nested2", { 2, 4, 6, 8 } } } } );
		static_assert(
			std::is_same< std::tuple< int, double, O, std::vector< O > >, decltype( t1 ) > { } );
		REQUIRE( to_string( t1 ) ==
				 R"({1, 2, {"test1", test2, [1, 5, 10]}, [{"nested1", nested2, [2, 4, 6, 8]}]})" );

		S s { 1,
			  2.0,
			  { "test1", "test2", { 1, 5, 10 } },
			  { { "nested1", "nested2", { 2, 4, 6, 8 } } } };
		auto t2 = structs::to_tuple( s );
		REQUIRE( to_string( t2 ) ==
				 R"({1, 2, {"test1", test2, [1, 5, 10]}, [{"nested1", nested2, [2, 4, 6, 8]}]})" );

		const S cs { 1,
					 2.0,
					 { "test1", "test2", { 1, 5, 10 } },
					 { { "nested1", "nested2", { 2, 4, 6, 8 } } } };
		auto t3 = structs::to_tuple( cs );
		REQUIRE( to_string( t3 ) ==
				 R"({1, 2, {"test1", test2, [1, 5, 10]}, [{"nested1", nested2, [2, 4, 6, 8]}]})" );
	}

	SECTION( "to_struct" )
	{
		struct S
		{
			int p1;
			double p2;
			std::string p3;
		};

		S s { 1, 2.0, "test" };

		auto t = structs::to_tuple( s );
		static_assert( std::is_same< std::tuple< int, double, std::string >, decltype( t ) > { } );
		REQUIRE( 1 == std::get< 0 >( t ) );
		REQUIRE( 2.0 == std::get< 1 >( t ) );
		REQUIRE( "test" == std::get< 2 >( t ) );

		S s2 = structs::to_struct< S >( t );
		REQUIRE( s.p1 == s2.p1 );
		REQUIRE( s.p2 == s2.p2 );
		REQUIRE( s.p3 == s2.p3 );
	}

	SECTION( "with a vector of string" )
	{
		struct s
		{
			int p1;
			double p2;
			std::string p3;
			std::vector< std::string > p4;
		};

		auto t = structs::to_tuple( s { 1, 2.0, "test", { "a", "b,c", "d" } } );
		static_assert(
			std::is_same< std::tuple< int, double, std::string, std::vector< std::string > >,
						  decltype( t ) > { } );
		REQUIRE( 1 == std::get< 0 >( t ) );
		REQUIRE( 2.0 == std::get< 1 >( t ) );
		REQUIRE( "test" == std::get< 2 >( t ) );
		REQUIRE( 3 == std::get< 3 >( t ).size( ) );
		REQUIRE( "a" == std::get< 3 >( t )[ 0 ] );
		REQUIRE( "b,c" == std::get< 3 >( t )[ 1 ] );
		REQUIRE( "d" == std::get< 3 >( t )[ 2 ] );
		REQUIRE( to_string( t ) == R"({1, 2, "test", [a, b,c, d]})" );
	}

	SECTION( "with a vector of struct" )
	{
		struct S
		{
			int p1;
			double p2;
			std::string p3;
			std::vector< O > p4;
		};

		S s {
			1,
			2.0,
			"test",
			{ { 1, true, nullptr, {} }, { 10, false, nullptr, {} }, { 11, false, nullptr, {} } }
		};
		auto t = structs::to_tuple( s );
		static_assert( std::is_same< std::tuple< int, double, std::string, std::vector< O > >,
									 decltype( t ) > { } );
		REQUIRE( 1 == std::get< 0 >( t ) );
		REQUIRE( 2.0 == std::get< 1 >( t ) );
		REQUIRE( "test" == std::get< 2 >( t ) );
		REQUIRE( 3 == std::get< 3 >( t ).size( ) );
		REQUIRE( 1 == std::get< 3 >( t )[ 0 ].p1 );
		REQUIRE( true == std::get< 3 >( t )[ 0 ].p2 );

		S s2 = structs::to_struct< S >( t );
		REQUIRE( s.p1 == s2.p1 );
		REQUIRE( s.p2 == s2.p2 );
		REQUIRE( s.p3 == s2.p3 );
		REQUIRE( to_string( t ) ==
				 R"({1, 2, "test", [{1, 1, 0, []}, {10, 0, 0, []}, {11, 0, 0, []}]})" );
	}

	SECTION( "with optional" )
	{
		REQUIRE( to_string( structs::to_tuple(
					 S { 12, false, 5.4, std::optional< std::string >( "optional" ) } ) ) ==
				 R"({12, 0, 5.4, "optional"})" );
		REQUIRE( to_string( structs::to_tuple( S2 { 12,
													false,
													5.4,
													std::optional< std::string >( "optional" ),
													{ 1, 2, 3, 4, 5 },
													{ { 5, 4, 3, 2, 1 } } } ) ) ==
				 R"({12, 0, 5.4, "optional", [1, 2, 3, 4, 5], [5, 4, 3, 2, 1]})" );
	}
}
