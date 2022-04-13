#include "transaction.h"

/**
 * The transaction constructor
 *
 * @param inputs - The UTXO transaction inputs
 * @param author - The transaction author
 * @param recipient - The transaction recipient
 * @param amount - The amount of coin the recipient should recieve
 */
Transaction::Transaction ( std::vector<TransactionInput> inputs, std::string author, std::string recipient, long amount ) {
	this -> inputs = inputs;
	this -> tx_index = 0;
	this -> create_outputs ( author, recipient, amount );
	this -> set_timestamp ();
	this -> calculate_hash ();
}

/**
 * The transaction constructor
 *
 * @param inputs - The UTXO transaction inputs
 * @param outputs - The UTXO transaction outputs
 */
Transaction::Transaction ( std::vector<TransactionInput> inputs, std::vector<TransactionOutput> outputs ) {
	this -> inputs = inputs;
	this -> outputs = outputs;
	this -> tx_index = 0;
	this -> set_timestamp ();
	this -> calculate_hash ();
}

/**
 * Creates the correct outputs from the given inputs
 *
 * @param recipient - Who should recieve the transaction output
 * @param amount - The quantity fo coin which should be sent in the output
 */
void Transaction::create_outputs ( std::string author, std::string recipient, long amount ) {
	
	// Checks that the input total is enough
	long total = 0;
	for ( auto input : this -> inputs )
		total += input.prev_out.value;

	if ( total < amount )
		throw std::runtime_error ( "Insufficient funds!" );

	// Creates a new output
	this -> outputs.push_back ( TransactionOutput ( false, author, recipient, amount, this -> tx_index ) );

	if ( total - amount > 0 )
		this -> outputs.push_back ( TransactionOutput ( false, author, author, total - amount, this -> tx_index ) );
}

/**
 * Calculates the transaction's hash
 */ 
void Transaction::calculate_hash () {
	this -> hash = crypto::sha256 ( this -> to_string () );
}

/**
 * Verifies the transaction's hash
 *
 * @returns Whether or not the hash is valid
 */
bool Transaction::verify_hash () {
	return this -> hash == crypto::sha256 ( this -> to_string () );
}

/**
 * Verifies whether or not the transaction is valid
 *
 * @param is_coinbase - Whether or not the current transaction is a coinbase
 * @returns Whether or not the transaction is valid
 */ 
bool Transaction::verify ( bool is_coinbase ) {

	// Verifies each transaction input
	long total_input = 0;
	for ( std::vector<TransactionInput>::iterator input = this -> inputs.begin (); input != this -> inputs.end (); input++ )
		if ( !( input -> verify ( is_coinbase ) ) )
			return false;
		else
			total_input += input -> prev_out.value;

	// Verifies each transaction output
	long total_output = 0;
	for ( std::vector<TransactionOutput>::iterator output = this -> outputs.begin (); output != this -> outputs.end (); output++ ) 
		if ( !( output -> verify ( this -> tx_index, is_coinbase ) ) )
			return false;
		else
			total_output += output -> value;	

	// Verifies the transaction value
	if ( total_input != total_output )
		return false;	

	// Verifies the hash
	if ( !( this -> verify_hash () ) ) 
		return false;

	return true;
}

/**
 * Converts the output into a string
 *
 * @returns A string representation of the transaction output
 */
std::string Transaction::to_string () {
	std::ostringstream stream;
	stream << this -> tx_index;
	stream << this -> time.count ();
	for ( auto input : this -> inputs ) {
		stream << input.to_string ();
	}
	for ( auto output : this -> outputs ) {
		stream << output.to_string ( false );
	}
	return stream.str ();
}

/**
 * Updates the transaction index
 *
 * @param tx_index - The new transaction index
 */ 
void Transaction::set_index ( long tx_index ) {
	this -> tx_index = tx_index;
	
	// Updates the outputs
	for ( std::vector<TransactionOutput>::iterator output = this -> outputs.begin (); output != this -> outputs.end (); output++ ) {
		output -> set_index ( tx_index );
	}

	// Recalculates the hash
	this -> calculate_hash ();
}

/**
 * Sets the transaction's timestamp
 */
void Transaction::set_timestamp () {
	this -> time = std::chrono::duration_cast<std::chrono::milliseconds> ( std::chrono::system_clock::now ().time_since_epoch () );
}







void Transaction::print ( bool is_coinbase ) {
	std::cout << "===== TRANSACTION =====" << std::endl;
	std::cout << "Hash: " << this -> hash << std::endl;
	std::cout << "Hash Valid: " << this -> verify_hash () << std::endl;
	std::cout << "Index: " << this -> tx_index << std::endl;
	std::cout << "Timestamp: " << this -> time.count () << std::endl;
	std::cout << "Valid: " << this -> verify ( is_coinbase ) << std::endl;

	// Prints the inputs
	for ( auto input : this -> inputs ) {
		input.print ();
	}

	// Prints the outputs
	for ( auto output : this -> outputs ) {
		output.print ();
	}
} 