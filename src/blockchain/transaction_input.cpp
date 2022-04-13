#include "transaction_input.h"

/**
 * The transaction input constructor
 *
 * @param input
 */
TransactionInput::TransactionInput ( TransactionOutput input ): prev_out ( input ) {
	this -> calculate_hash ();
}

/**
 * Calculates the input's hash
 */ 
void TransactionInput::calculate_hash () {
	this -> hash = crypto::sha256 ( this -> prev_out.to_string ( false ) );
}

/**
 * Verifies the input's hash
 *
 * @returns Whether or not the hash is valid
 */
bool TransactionInput::verify_hash () {
	return this -> hash == crypto::sha256 ( this -> prev_out.to_string ( false ) );
}

/**
 * Converts the input into a string
 * (Should only be used for hashing)
 *
 * @returns A string representation of the transaction input
 */
std::string TransactionInput::to_string () {
	std::ostringstream stream;
	stream << this -> hash;
	stream << this -> prev_out.to_string ( false );
	return stream.str ();
}

bool TransactionInput::verify ( bool is_coinbase_input ) {
	
	// Verifies the hash
	if ( !( this -> verify_hash () ) ) 
		return false;	

	// Verifies the previous output
	if ( !( this -> prev_out.verify ( is_coinbase_input ) ) )
		return false;

	return true;
}



void TransactionInput::print () {
	std::cout << "===== INPUT =====" << std::endl;
	std::cout << "Hash: " << this -> hash << std::endl;
	std::cout << "Hash valid: " << this -> verify_hash () << std::endl;
	std::cout << "===== PREV: =====" << std::endl;
	this -> prev_out.print ();
}