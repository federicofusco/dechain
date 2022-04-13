#include "transaction_output.h"

/**
 * The transaction output constructor
 *
 * @param spent - Whether or not the output has been spent
 * @param recipient - The recipient of the output 
 * @param value - The value of the output
 * @param tx_index - The index of the output's transaction
 */
TransactionOutput::TransactionOutput ( bool spent, std::string author, std::string recipient, long value, long tx_index ) {
	this -> spent = spent;
	this -> author = author;
	this -> recipient = recipient;
	this -> value = value;
	this -> tx_index = tx_index;
}

/**
 * The coinbase output constructor
 *
 * @param spent - Whether or not the output has been spent
 * @param recipient - The output recipient
 */
TransactionOutput::TransactionOutput ( bool spent, std::string recipient ) {
	this -> spent = spent;
	this -> recipient = recipient;
	this -> value = 0;
	this -> tx_index = 0; 
}

/**
 * Verifies the output's signature
 *
 * @returns Whether or not the signature is valid
 */
bool TransactionOutput::verify_signature () {

	// Verifies that the transaction has been signed
	if ( this -> signature.empty () )
		throw std::runtime_error ( "This transaction hasn't been signed yet!" );

	// Creates the verification context
	EVP_MD_CTX* context = EVP_MD_CTX_create ();
	if ( EVP_DigestVerifyInit ( context, NULL, EVP_sha256 (), NULL, this -> get_author () ) != 1 )
		throw std::runtime_error ( "Failed to verify transaction signature!" );

	if ( EVP_DigestVerifyUpdate ( context, (const unsigned char*) this -> to_string ( true ).c_str (), this -> to_string ( true ).length () ) != 1 )
		throw std::runtime_error ( "Failed to verify transaction signature!" );

	return EVP_DigestVerifyFinal ( context, (const unsigned char*) crypto::from_hex ( this -> signature ).c_str (), crypto::from_hex ( this -> signature ).length () );
}

/**
 * Verifies whether or not the transaction output is valid
 *
 * @param is_coinbase_output - Whether or not the parent transaction is a coinbase
 * @returns Whether or not the output is valid
 */
bool TransactionOutput::verify ( bool is_coinbase_output ) {

	if ( !is_coinbase_output ) {
		
		// Verifies the signature
		if ( !( this -> verify_signature () ) )
			return false;

		// Verifies that the author is present
		if ( this -> author.empty () ) 
			return false;

		// Verifies that the recipient is present
		if ( this -> recipient.empty () )
			return false;

		return true;
	}

	// Verifies that the signature isn't present
	if ( !( this -> signature.empty () ) ) 
		return false;

	// Verifies that the author isn't present
	if ( !( this -> author.empty () ) )
		return false;

	// Verifies that the recipient is present
	if ( this -> recipient.empty () )
		return false;

	return true;
}

/**
 * Verifies whether or not the transaction output is valid
 *
 * @param tx_index - The index of the output's transaction
 * @param is_coinbase_output - Whether or not the parent transaction is a coinbase
 * @returns Whether or not the output is valid
 */
bool TransactionOutput::verify ( long tx_index, bool is_coinbase_output ) {

	if ( !is_coinbase_output ){
		
		// Verifies the signature
		if ( !( this -> verify_signature () ) )
			return false;

		// Verifies the transaction index
		if ( this -> tx_index != tx_index )
			return false;

		// Verifies that the author is present
		if ( this -> author.empty () ) 
			return false;

		// Verifies that the recipient is present
		if ( this -> recipient.empty () )
			return false;

		return true;
	}

	// Verifies the transaction index
	if ( this -> tx_index != tx_index )
		return false;

	// Verifies that the recipient is present
	if ( this -> recipient.empty () )
		return false;

	return true;
}

/**
 * Converts the output into a string
 * (Should only be used for hashing)
 *
 * @param is_signature - Whether or not to this will be signed
 * @returns A string representation of the transaction output
 */
std::string TransactionOutput::to_string ( bool is_signature ) {
	std::ostringstream stream;
	
	if ( !is_signature ) 
		stream << this -> signature << this -> tx_index;
	
	stream << this -> value;
	stream << this -> spent;
	stream << this -> author;
	stream << this -> recipient;
	return stream.str ();
}

/**
 * Updates the output's transaction index
 *
 * @param tx_index - The new transaction index
 */
void TransactionOutput::set_index ( long tx_index ) {
	this -> tx_index = tx_index;
}

/**
 * Gets the transaction author as an EVP_PKEY
 *
 * @returns The author's public key
 */ 
EVP_PKEY* TransactionOutput::get_author () {
	
	// Creates a new IO buffer
	BIO *bio = BIO_new_mem_buf ( this -> author.c_str (), this -> author.length () );
	return PEM_read_bio_PUBKEY ( bio, NULL, NULL, NULL );
}





void TransactionOutput::print () {
	std::cout << "===== OUTPUT =====" << std::endl;
	std::cout << "Sig: " << this -> signature << std::endl;
	std::cout << "Index: " << this -> tx_index << std::endl;
	std::cout << "Value: " << this -> value << std::endl;
	std::cout << "Spent: " << this -> spent << std::endl;
	std::cout << "Author: " << this -> author << std::endl;
	std::cout << "Recipient: " << this -> recipient << std::endl; 
}
