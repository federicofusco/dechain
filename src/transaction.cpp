#include "transaction.hpp"

/**
 * The transaction contructor
 *
 * @param author - The author's public key
 * @param recipient - The recipient's public key
 * @param amount - The transaction amount
 * @param id - The transaction's id
 */ 
Transaction::Transaction ( std::string author, std::string recipient, float amount, int id ) {
	this -> author = author;
	this -> recipient = recipient;
	this -> amount = amount;
	this -> id = id;
	this -> signature = "";
	this -> calculate_hash (); 
}

/**
 * Converts the transaction into a string
 *
 * @param include_signature - Whether or not the signature should be included in the string
 * @returns The string representation of the transaction
 */
std::string Transaction::to_string ( bool include_signature ) {
	std::ostringstream stream;
	stream << this -> author << "." << this -> recipient << "." << this -> amount << "." << this -> id;
	if ( include_signature && !( this -> signature.empty () ) )
		stream << "." << this -> signature;

	return stream.str ();
}

/**
 * Converts a given string into a hex encoded string
 *
 * @param input - The string which should be encoded
 * @returns A hex encoded string representing the input
 */
std::string Transaction::to_hex ( std::string input ) {
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
std::string Transaction::from_hex ( std::string input ) {
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
 * Calculates the transaction's hash
 */ 
void Transaction::calculate_hash () {
	unsigned char raw_hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init ( &sha256 );
	SHA256_Update ( &sha256, this -> to_string ( true ).c_str (), this -> to_string ( true ).length () );
	SHA256_Final ( raw_hash, &sha256 );

	std::string hex_hash ( reinterpret_cast<char*> ( raw_hash ), SHA256_DIGEST_LENGTH );
	this -> hash = this -> to_hex ( hex_hash );
}

/**
 * Verifies this transaction's hash
 *
 * @returns Whether or not the transaction hash is valid
 */
bool Transaction::verify_hash () {

	// Calculates the transaction's has
	unsigned char raw_hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init ( &sha256 );
	SHA256_Update ( &sha256, this -> to_string ( true ).c_str (), this -> to_string ( true ).length () );
	SHA256_Final ( raw_hash, &sha256 );

	std::string hex_hash ( reinterpret_cast<char*> ( raw_hash ), SHA256_DIGEST_LENGTH );
	return this -> hash == this -> to_hex ( hex_hash );
}

/**
 * Sets the transaction's signature and recalculates the hash
 *
 * @param signature - The transaction's signature
 */
void Transaction::set_signature ( std::string signature ) {
	this -> signature.assign ( this -> to_hex ( signature ) );
	this -> calculate_hash ();
}

/**
 * Gets the ascii encoded signature
 *
 * @returns The transaction's signature
 */
std::string Transaction::get_signature () {
	return this -> from_hex ( this -> signature ); 
}

/**
 * Gets the transaction author as an EVP_PKEY
 *
 * @returns The author's public key
 */ 
EVP_PKEY* Transaction::get_author () {
	
	// Creates a new IO buffer
	BIO *bio = BIO_new_mem_buf ( this -> author.c_str (), this -> author.length () );
	return PEM_read_bio_PUBKEY ( bio, NULL, NULL, NULL );
}

/**
 * Verifies the transaction signature's validity
 *
 * @returns Whether or not the transaction signature is validd
 */
bool Transaction::verify_signature () {

	if ( this -> signature.empty () )
		throw std::runtime_error ( "This transaction hasn't been signed yet!" );

	// Creates the verification context
	EVP_MD_CTX* context = EVP_MD_CTX_create ();
	if ( EVP_DigestVerifyInit ( context, NULL, EVP_sha256 (), NULL, this -> get_author () ) != 1 )
		throw std::runtime_error ( "Failed to verify transaction signature!" );

	if ( EVP_DigestVerifyUpdate ( context, (const unsigned char*) this -> to_string ( false ).c_str (), this -> to_string ( false ).length () ) != 1 )
		throw std::runtime_error ( "Failed to verify transaciton signature!" );

	return EVP_DigestVerifyFinal ( context, (const unsigned char*) this -> get_signature ().c_str (), this -> get_signature ().length () );
}

/**
 * Verifies the transaction's validity
 *
 * @returns Whether or not the transaction is valid
 */ 
bool Transaction::verify () {
	bool is_signature_valid = this -> verify_signature ();
	bool is_hash_valid = this -> verify_hash ();
	bool is_amount_valid = this -> amount > 0;
	bool is_author_valid = !( this -> author.empty () );
	bool is_recipient_valid = !( this -> recipient.empty () ) && this -> author != this -> recipient;
	return is_hash_valid && is_signature_valid && is_amount_valid && is_author_valid && is_recipient_valid; 
}