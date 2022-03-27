#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

#include "transaction.hpp"

Transaction::Transaction ( std::string author, std::string recipient, float amount ) {
	this -> author = author;
	this -> recipient = recipient;
	this -> amount = amount;
	this -> id = 0;
	this -> signature = "";
	this -> calculate_hash (); 
}

/**
 * Converts the transaction into a string
 *
 * @param include_signature - Whether or not the signature should be included in the string
 */
std::string Transaction::to_string ( bool include_signature ) {
	std::ostringstream stream;
	
	stream << author << "." << recipient << "." << amount << "." << id;
	if ( include_signature && !signature.empty () )
		stream << "." << signature;

	return stream.str ();
}

/**
 * Converts a given string into a hex encoded string
 *
 * @param input - The string which should be encoded
 */
std::string Transaction::to_hex ( std::string input ) {
	std::stringstream stream;
	for ( auto c : input ) {
		stream << std::setfill ( '0' ) << std::setw ( 2 ) << std::uppercase << std::hex << ( 0xFF & c );
	}
	return stream.str ();
}

/**
 * Calculates the transaction's hash
 */ 
void Transaction::calculate_hash () {

	unsigned char raw_hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init ( &sha256 );
	SHA256_Update ( &sha256, to_string ( true ).c_str (), to_string ( true ).length () );
	SHA256_Final ( raw_hash, &sha256 );

	std::string hex_hash ( reinterpret_cast<char*> ( raw_hash ), SHA256_DIGEST_LENGTH );
	this -> hash = this -> to_hex ( hex_hash );
}

/**
 * Sets the transaction's signature and recalculates the hash
 */
void Transaction::set_signature ( std::string signature ) {
	this -> signature.assign ( signature );
	this -> calculate_hash ();
}

void Transaction::set_id ( long int id ) {
	this -> id = id;
	this -> calculate_hash ();
}

/**
 * Gets the transaction author as an EVP_PKEY
 */ 
EVP_PKEY* Transaction::get_author () {
	
	// Creates a new IO buffer
	BIO *bio = BIO_new_mem_buf ( author.c_str (), author.length () );
	return PEM_read_bio_PUBKEY ( bio, NULL, NULL, NULL );
}

/**
 * Verifies the trransaction's signature's validity
 */
int Transaction::verify_signature () {

	if ( this -> signature.empty () ) {
		std::cout << "This transaction hasn't been signed yet!" << std::endl;
		return -1;
	}

	// Creates the verification context
	EVP_MD_CTX* context = EVP_MD_CTX_create ();
	if ( EVP_DigestVerifyInit ( context, NULL, EVP_sha256 (), NULL, this -> get_author () ) != 1 ) {
		std::cout << "Failed to verify transaction signature!" << std::endl;
		return -1;
	}

	if ( EVP_DigestVerifyUpdate ( context, (const unsigned char*) this -> to_string ( false ).c_str (), this -> to_string ( false ).length () ) != 1 ) {
		std::cout << "Failed to verify transaction signature!" << std::endl;
		return -1;
	} 

	return EVP_DigestVerifyFinal ( context, (const unsigned char*) this -> signature.c_str (), this -> signature.length () );
}