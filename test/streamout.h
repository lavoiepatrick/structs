#pragma once

#include "../include/structs/to_tuple.h"

#include <iomanip>
#include <optional>
#include <vector>

template< typename T, std::enable_if_t< !std::is_aggregate< T >::value, bool > = true >
std::ostream& _streamout( std::ostream& os, const T& value, bool )
{
	os << value;
	return os;
}

template< typename T >
std::ostream& _streamout( std::ostream& os, const std::vector< T >& value, bool )
{
	os << "[";
	bool first = true;
	for( const T& t: value )
	{
		if( first )
			first = false;
		else
			os << ", ";
		_streamout( os, t, false );
	}
	os << "]";
	return os;
}

std::ostream& _streamout( std::ostream& os, const std::string& value, bool )
{
	os << std::quoted( value );
	return os;
}

std::ostream& _streamout( std::ostream& os, const std::string_view& value, bool )
{
	os << std::quoted( value );
	return os;
}

template< typename T >
std::ostream& _streamout( std::ostream& os, const std::optional< T >& value, bool )
{
	if( value )
		_streamout( os, *value, false );
	else
		os << "null";
	return os;
}

template< typename... T >
std::ostream& _streamout( std::ostream& os, const std::tuple< T... >& tup )
{
	std::apply(
		[ &os ]( const T&... v ) {
			os << '{';
			size_t n { 0 };
			( ( _streamout( os, v, false ), ( os << ( ++n != sizeof...( T ) ? ", " : "" ) ) ),
			  ... );
			os << '}';
		},
		tup );

	return os;
}

template<
	typename T,
	std::enable_if_t< std::is_aggregate< std::remove_reference_t< T > >::value, bool > = true >
std::ostream& _streamout( std::ostream& os, const T& value, bool )
{
	_streamout( os, structs::to_tuple( value ) );
	return os;
}

template< typename T >
class streamout
{
public:
	streamout( const T& value ) : m_value( value )
	{
	}

	template< typename TT >
	friend std::ostream& operator<<( std::ostream& os, const streamout< TT >& so );

private:
	const T& m_value;
};

template< typename T >
std::ostream& operator<<( std::ostream& os, const streamout< T >& so )
{
	_streamout( os, so.m_value );
	return os;
}
