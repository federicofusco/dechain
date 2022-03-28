#include "block.hpp"

/**
 * The block constructor
 *
 * @param id - The block's id
 * @param prev_hash - The hash of the previous block in the blockchain
 * @param difficulty - The mining difficulty for this block
 */ 
Block::Block ( long int id, std::string prev_hash, int difficulty ) {
	this -> id = id;
	this -> prev_hash = prev_hash;
	this -> nonce = 0;
	this -> difficulty = difficulty;
	this -> calculate_hash ();
}

Block::Block () {}

/**
 * Adds a transaction to the current block
 *
 * @param transaction - The transaction which should be added
 */ 
void Block::add_transaction ( Transaction transaction ) {

	// Checks the validity of the transaction
	if ( !transaction.verify () )
		throw std::runtime_error ( "Attempted to add invalid transaction to block!" );

	// Adds the transaction to the block
	this -> transactions.push_back ( transaction );
	this -> calculate_hash ();
}

/**
 * Converts the block to a string
 *
 * @returns The string representation of the block
 */
std::string Block::to_string () {
	std::stringstream stream;

	stream << this -> id << ".";
	stream << this -> nonce << ".";
	stream << this -> difficulty << ".";
	for ( auto transaction : this -> transactions ) {
		stream << transaction.to_string ( true ) << ".";
	}
	stream << this -> prev_hash;

	return stream.str ();
}

/**
 * Converts a given string into a hex encoded string
 *
 * @param input - The string which should be encoded
 * @returns A hex encoded string representing the input
 */
std::string Block::to_hex ( std::string input ) {
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
std::string Block::from_hex ( std::string input ) {
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
 * Calculates the block's hash
 */
void Block::calculate_hash () {
	unsigned char raw_hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init ( &sha256 );
	SHA256_Update ( &sha256, this -> to_string ().c_str (), this -> to_string ().length () );
	SHA256_Final ( raw_hash, &sha256 );

	std::string hex_hash ( reinterpret_cast<char*> ( raw_hash ), SHA256_DIGEST_LENGTH );
	this -> hash = this -> to_hex ( hex_hash );
}

/**
 * Verifies the block's hash
 *
 * @returns Whether or not the block's hash is valid
 */
bool Block::verify_hash () {

	// Calculates the block's hash
	unsigned char raw_hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init ( &sha256 );
	SHA256_Update ( &sha256, this -> to_string ().c_str (), this -> to_string ().length () );
	SHA256_Final ( raw_hash, &sha256 );

	std::string hex_hash (  reinterpret_cast<char*> ( raw_hash ), SHA256_DIGEST_LENGTH );
	return this -> hash == this -> to_hex ( hex_hash );
}

/**
 * Checks if the block has already been mined
 * TODO: **Improve this**
 *
 * @returns Whether or not the block has been mined
 */
bool Block::is_mined () {
	for ( auto c : this -> hash.substr ( 0, this -> difficulty ) ) {
		if ( c != '0' )
			return false;
	}
	return true;
}

/**
 * Mines the current block
 * TODO: **Improve this**
 */
void Block::mine_block () {
	while ( !( this -> is_mined () ) ) {
		this -> nonce++;
		this -> calculate_hash ();
	}
}

/**
 * Verifies the transactions in the block
 *
 * @returns whether or not the transactions are valid
 */
bool Block::verify_transactions () {
	for ( auto transaction : this -> transactions ) {
		if ( !transaction.verify () ) 
			return false;
	}

	return true;
}

/**
 * Verifies that the entire block is valid
 */
bool Block::verify () {
	bool is_hash_valid = this -> verify_hash () ;
	bool are_transactions_valid = this -> verify_transactions ();
	bool is_mined = this -> is_mined ();
	bool has_difficulty = this -> difficulty > 0;
	bool has_previous_hash = !( this -> prev_hash.empty () );
	return is_hash_valid && are_transactions_valid && is_mined && has_difficulty && has_previous_hash;
}

/**
 * Verifies that the entire block is valid
 *
 * @param prev_hash - The previous block hash in the blockchain
 * @param prev_id - The previous block id in the blockchain
 */ 
bool Block::verify ( std::string prev_hash, long int id ) {
	bool is_valid = this -> verify ();
	bool is_prev_hash_valid = this -> prev_hash == prev_hash;
	bool is_id_valid = this -> id == id + 1;
	return is_valid && is_prev_hash_valid && is_id_valid;
}