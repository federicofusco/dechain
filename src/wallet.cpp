#include <iostream>
#include <stdlib.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>

#include "wallet.hpp"

Wallet::Wallet ( int key_size = 4096 ) {

	// Generates wallet RSA keypair
	keypair = generate_keypair ( key_size );
	public_key = public_key_to_string ( keypair );
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

/**
 * Signs a transaction with the wallet's private key
 *
 * @param transaction - A pointer to the transaction which should be signed
 */
int Wallet::sign_transaction ( Transaction *transaction ) {

	// Checks that the public key matches the transaction author
	if ( transaction -> author != public_key )
		throw std::runtime_error ( "Attempted signing transaction with different author!" );

	// Creates the signing context
	EVP_MD_CTX *context = EVP_MD_CTX_create ();
	if ( EVP_DigestSignInit ( context, NULL, EVP_sha256 (), NULL, keypair ) != 1 )
		throw std::runtime_error ( "Failed tot sign transaction!" );

	if ( EVP_DigestSignUpdate ( context, (const unsigned char*) transaction -> to_string ( false ).c_str (), transaction -> to_string ( false ).length () ) != 1 )
		throw std::runtime_error ( "Failed to sign transaction!" );

	// Gets the hash size
	size_t x = 8;
	size_t *signature_length = &x;
	if ( EVP_DigestSignFinal ( context, NULL, signature_length ) != 1 )
		throw std::runtime_error ( "Failedd to sign transaction!" );

	// Allocates memory for the signature hash
	unsigned char *signature = (unsigned char*) malloc ( *signature_length );
	if ( EVP_DigestSignFinal ( context, signature, signature_length ) != 1 )
		throw std::runtime_error ( "Failed to sign transaction!" );

	std::string output ( reinterpret_cast<char*>(signature), *signature_length); 
	transaction -> set_signature ( output );

	return 1;
}