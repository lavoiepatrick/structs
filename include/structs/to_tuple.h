/*
MIT License

Copyright (c) 2021 Patrick Lavoie

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <boost/preprocessor/repetition/repeat.hpp>

#include <tuple>
#include <type_traits>

#ifndef TO_TUPLE_SIZE_MAX
	#define TO_TUPLE_SIZE_MAX 128
#endif

namespace structs
{
namespace details
{
struct any_type
{
	template< typename T >
	constexpr operator T( );
};

template< typename T, std::size_t... I >
decltype( void( T { ( I, std::declval< any_type >( ) )... } ),
		  std::true_type { } ) test_is_braces_constructible_n( std::index_sequence< I... > );

template< typename, typename... >
std::false_type test_is_braces_constructible_n( ... );

template< typename T, std::size_t N >
using is_braces_constructible_n =
	decltype( test_is_braces_constructible_n< T >( std::make_index_sequence< N > { } ) );

// the returned number of elements will likely be lower than sizeof( T )
// make the right value at least 1 more than sizeof( T )
template< typename T, std::size_t left = 0, std::size_t right = sizeof( T ) + 1 >
constexpr std::size_t to_tuple_element_count( )
{
	constexpr std::size_t mid = ( left + right ) / 2;
	if constexpr( mid == 0 )
		return 0;
	else if constexpr( mid == left )
		return mid;
	else if constexpr( is_braces_constructible_n< T, mid > { } )
		return to_tuple_element_count< T, mid, right >( );
	else
		return to_tuple_element_count< T, left, mid >( );
}

template< typename T >
auto to_tuple_impl( T&&, std::integral_constant< std::size_t, 0 > ) noexcept
{
	return std::make_tuple( );
}

#define TO_TUPLE_PP( Z, N, _ ) , p##N
#define TO_TUPLE_PP_N( Z, N, _ )                                                            \
	template< typename T >                                                                  \
	auto to_tuple_impl( T&& object, std::integral_constant< std::size_t, N + 1 > ) noexcept \
	{                                                                                       \
		auto&& [ p BOOST_PP_REPEAT_##Z( N, TO_TUPLE_PP, nil ) ] = object;                   \
		return std::make_tuple( p BOOST_PP_REPEAT_##Z( N, TO_TUPLE_PP, nil ) );             \
	}
BOOST_PP_REPEAT( TO_TUPLE_SIZE_MAX, TO_TUPLE_PP_N, nil )
#undef TO_TUPLE_PP_N
#undef TO_TUPLE_PP

template< typename T,
		  typename = struct current_value,
		  std::size_t = TO_TUPLE_SIZE_MAX,
		  typename = struct required_value,
		  std::size_t N >
auto to_tuple_impl( T&&, std::integral_constant< std::size_t, N > ) noexcept
{
	static_assert( N <= TO_TUPLE_SIZE_MAX,
				   "Need to increase TO_TUPLE_SIZE_MAX to the number of elements of the struct." );
}
}  // namespace details

template< typename T >
struct to_tuple_size
	: std::integral_constant< std::size_t, details::to_tuple_element_count< T >( ) >
{
};

template< typename T,
		  std::enable_if_t< std::is_aggregate_v< std::remove_reference_t< T > >, bool > = true >
auto to_tuple( T&& object ) noexcept
{
	return details::to_tuple_impl( std::forward< T >( object ),
								   to_tuple_size< std::decay_t< T > > { } );
}

}  // namespace structs