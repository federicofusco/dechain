#include "wallet.h"

Wallet::Wallet ( int key_size = 4096 ) {

	// Generates wallet RSA keypair
	this -> keypair = generate_keypair ( key_size );
	this -> public_key = public_key_to_string ( keypair );
}

Wallet::~Wallet () {

	// Dealloc
	EVP_PKEY_free ( keypair );
}

/**
 * Generates a new RSA keypair
 *
 * @param key_size - An int describing the length of the key (recommended 4096 bits)
 */ 
EVP_PKEY *Wallet::generate_keypair ( int key_size ) {
	
	RSA *rsa = RSA_new ();
	BIGNUM *e = BN_new ();
	EVP_PKEY *keypair = EVP_PKEY_new ();

	// Sets the public exponent
	if ( BN_set_word ( e, RSA_F4 ) != 1 )
		throw std::runtime_error ( "Failed to set the RSA public exponent!" );

	// Generates new RSA keypair
	if ( RSA_generate_key_ex ( rsa, key_size, e, NULL ) != 1 )
		throw std::runtime_error ( "Failed to generate the RSA keypair!" );

	// Assigns the RSA keypair to an EVP_PKEY keypair
	if ( EVP_PKEY_assign_RSA ( keypair, rsa ) != 1 )
		throw std::runtime_error ( "Failed to assign the RSA keypair!" );

	return keypair;

}

/**
 * Converts a given EVP_PKEY to a string with the public key
 *
 * @param key - The EVP_PKEY which should be converted to a string
 */
std::string Wallet::public_key_to_string ( EVP_PKEY *key ) {

	// Writes the RSA public key to memory
	BIO *bio = BIO_new ( BIO_s_mem () );
	if ( PEM_write_bio_PUBKEY ( bio, key ) != 1 ) {
		BIO_free ( bio );
		return NULL;
	}

	char *char_buffer;
	size_t buffer_size = BIO_get_mem_data ( bio, &char_buffer );
	std::string buffer ( char_buffer, buffer_size );
	return buffer;
}

long Wallet::calculate_balance () {

	long balance = 0;
	for ( auto transaction : this -> incoming_transactions )
		for ( auto output : transaction.outputs )
			if ( output.recipient == this -> public_key )
				balance += output.value;

	return balance;
}

Transaction Wallet::create_coinbase ( std::string recipient, long amount ) {

	// Creates a new coinbase input
	TransactionOutput output ( false, "", this -> public_key, amount, 0 );
	TransactionInput input ( output );
	std::vector<TransactionInput> inputs { input };

	// Creates a new coinbase transaction
	Transaction transaction ( inputs, this -> public_key, recipient, amount );
	this -> sign_transaction ( &transaction );
	transaction.calculate_hash ();

	// Adds transaction to wallet
	this -> incoming_transactions.push_back ( transaction );

	return transaction;
}

std::vector<TransactionInput> Wallet::get_tx_inputs ( long amount ) {

	std::vector<TransactionInput> inputs;
	long total = 0;
	for ( auto transaction : this -> incoming_transactions ) {

		// Gets the total output value
		long local_total = 0;
		std::vector<std::vector<TransactionOutput>::iterator> outputs;
		for ( std::vector<TransactionOutput>::iterator output = transaction.outputs.begin (); output != transaction.outputs.end (); output++ ) {
			if ( output -> recipient == this -> public_key ) {
				
				// Adds the input
				inputs.push_back ( TransactionInput ( *output ) );
				local_total += output -> value;
			
				// Deletes the output from the transaction to prevent accidental double spending
				outputs.push_back ( output );
			}
		}

		for ( auto output : outputs ) 
			transaction.outputs.erase ( output );

		total += local_total;

		if ( total >= amount )
			break;
	}

	return inputs;
}







































// std::vector<TransactionInput> Wallet::get_tx_inputs ( long amount ) {
	
// 	std::vector<TransactionInput> inputs;

// 	// Loops through each transaction
// 	long total_value = 0;
// 	for ( auto transaction : this -> incoming_transactions )
// 		for ( auto output : transaction.outputs )
// 			if ( output.recipient == this -> public_key ) {

// 				// Adds the output as an input
// 				inputs.push_back ( TransactionInput ( output ) );
// 				total_value += output.value;

// 				// Checks the current input value
// 				if ( total_value >= amount )
// 					break;
// 			}

// 	return inputs;
// }

Transaction Wallet::create_transaction ( std::string recipient, long amount ) {

	// Checks if the wallet has sufficient funds
	if ( this -> calculate_balance () < amount ) 
		throw std::runtime_error ( "Insufficient funds!" );

	// Creates the necessary transaction inputs
	auto inputs = this -> get_tx_inputs ( amount ); 

	// Creates a new transaction
	Transaction transaction ( inputs, this -> public_key, recipient, amount );
	this -> sign_transaction ( &transaction );
	transaction.calculate_hash ();

	return transaction;
}

void Wallet::sign_transaction ( Transaction *transaction ) {

	// Loops over each output in the transaction
	for ( std::vector<TransactionOutput>::iterator output = transaction -> outputs.begin (); output != transaction -> outputs.end (); output++ ) {


		// Checks that the public key matches the output author
		if ( output -> author != this -> public_key )
			throw std::runtime_error ( "Attemped signing output with different author!" );

		/* Signs the output */

		// Creates the signing context
		EVP_MD_CTX *context = EVP_MD_CTX_create ();
		if ( EVP_DigestSignInit ( context, NULL, EVP_sha256 (), NULL, this -> keypair ) != 1 ) 
			throw std::runtime_error ( "Failed to sign transaction output!" );

		if ( EVP_DigestSignUpdate ( context, (const unsigned char*) output -> to_string ( true ).c_str (), output -> to_string ( true ).length () ) != 1 )
			throw std::runtime_error ( "Failed to sign transaction output!" );

		// Gets the hash size
		size_t x = 8;
		size_t *signature_length = &x;
		if ( EVP_DigestSignFinal ( context, NULL, signature_length ) != 1 ) 
			throw std::runtime_error ( "Failed to sign transaction output!" );

		// Allocates memory for the signature hash
		unsigned char *raw_signature = (unsigned char*) malloc ( *signature_length );
		if ( EVP_DigestSignFinal ( context, raw_signature, signature_length ) != 1 ) 
			throw std::runtime_error ( "Failed to sign transaction output!" );

		std::string signature ( reinterpret_cast<char*> ( raw_signature ), *signature_length );
		output -> signature = crypto::to_hex ( signature );
	}

	// Updates 
}