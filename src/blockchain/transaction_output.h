#pragma once
#ifndef TRANSACTION_OUTPUT_H
#define TRANSACTION_OUTPUT_H

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include "algorithms/crypto.h"

class TransactionOutput {
	public:
		std::string signature;
		long tx_index;
		long value;
		bool spent;
		std::string author;
		std::string recipient;

		TransactionOutput ( bool spent, std::string author, std::string recipient, long value, long tx_index );
		TransactionOutput ( bool spent, std::string recipient );

		bool verify_signature ();
		bool verify ( bool is_coinbase_output );
		bool verify ( long tx_index, bool is_coinbase_output );

		std::string to_string ( bool is_signature );
		void set_index ( long tx_index );
		EVP_PKEY *get_author ();

		void print ();

};

#endif