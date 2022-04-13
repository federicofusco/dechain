#include "crypto.h"

namespace crypto {

	/**
	* Converts a given string into a hex encoded string
	*
	* @param input - The string which should be encoded
	* @returns A hex encoded string representing the input
	*/
	std::string to_hex ( std::string input ) {
		std::stringstream stream;
		for ( auto c : input ) {
			stream << std::setfill ( '0' ) << std::setw ( 2 ) << std::uppercase << std::hex << ( 0xFF & c );
		}
		return stream.str ();
	}


	/**
	* Converts a given hex string into an ascii string
	*
	* @param input - The string which should be decoded
	* @returns The ascii representation of the hex encoded string
	*/
	std::string from_hex ( std::string input ) {
		std::string output;

		// Calculates the output length
		if ( input.length () % 2 != 0 )
			throw std::runtime_error ( "Invalid hex string conversion!" );

		size_t output_length = input.length () / 2;

		for ( size_t x = 0; x < output_length; x++ ) {
			uint32_t s = 0;
			std::stringstream stream;

			stream << std::hex << input.substr ( x * 2, 2 );
			stream >> s;

			output.push_back ( static_cast<unsigned char> ( s ) );
		}

		return output;
	}	

	/**
	* Hashes a given input string with the sha256 algorithm
	*
	* @param input - The string which should be hashed
	* @returns The hash of the given input
	*/
	std::string sha256 ( std::string input ) {
		unsigned char raw_hash[SHA256_DIGEST_LENGTH];
		SHA256_CTX sha256;
		SHA256_Init ( &sha256 );
		SHA256_Update ( &sha256, input.c_str (), input.length () );
		SHA256_Final ( raw_hash, &sha256 );

		std::string hex_hash ( reinterpret_cast<char*> ( raw_hash ), SHA256_DIGEST_LENGTH );
		return to_hex ( hex_hash );
	}

	/**
	* Calculates the merkel tree of a given vector
	*
	* @param nodes - The vector containing the nodes of the tree
	* @returns The merkel tree
	*/
	std::string merkel_tree ( std::vector<std::string> tree ) {

		std::vector<std::string> nodes = tree;

		// Continues to hash the tree
		while ( nodes.size () > 1 ) {

			std::vector<std::string> tmp_nodes;
			std::string tmp_hash;
			for ( std::vector<std::string>::iterator node = nodes.begin (); node < nodes.end (); node++ ) {

				if ( !( ( nodes.end () - node ) % 2 ) ) 
					tmp_hash.append ( sha256 ( *node ) );
				else {
					tmp_hash.append ( sha256 ( *node ) );
					tmp_nodes.push_back ( tmp_hash );
					tmp_hash = "";
				}
			}

			nodes = tmp_nodes;
		}

		return nodes.front ();
	}
	
}