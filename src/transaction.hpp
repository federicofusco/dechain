#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <string>

class Transaction {
	public: 
		std::string author;
		std::string recipient;
		float amount;
		int id;
		std::string hash;
		std::string signature;

		/* Constructors */
		Transaction ( std::string author, std::string recipient, float amount, int id );

		/* Getters / Setters */
		EVP_PKEY *get_author ();
		void set_signature ( std::string signature );
		std::string get_signature ();
		std::string to_string ( bool include_signature );

		/* Crypto */
		void calculate_hash ();
		bool verify_signature ();
		bool verify_hash ();
		bool verify ();

	private:
		std::string to_hex ( std::string input );
		std::string from_hex ( std::string input );
};

#endif