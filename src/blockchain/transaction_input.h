#pragma once
#ifndef TRANSACTION_INPUT_H
#define TRANSACTION_INPUT_H

#include <string>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include "transaction_output.h"
#include "algorithms/crypto.h"

class TransactionInput {
	public:
		std::string hash;
		TransactionOutput prev_out;

		TransactionInput ( TransactionOutput input );

		void calculate_hash ();
		std::string to_string ();
		
		bool verify_hash ();
		bool verify ();
		bool verify ( bool is_coinbase_input );


		void print ();

};

#endif