#pragma once
#ifndef WALLET_H
#define WALLET_H

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include "transaction_input.h"
#include "transaction_output.h"
#include "transaction.h"
#include "algorithms/crypto.h"

class Wallet {
	public:
		Wallet ( int key_size );
		~Wallet ();

		std::string public_key; 

		Transaction create_coinbase ( std::string recipient, long amount );
		Transaction create_transaction ( std::string recipient, long amount );
		void sign_transaction ( Transaction *transaction );

		long calculate_balance ();
		std::vector<TransactionInput> get_tx_inputs ( long amount );

	private:
		EVP_PKEY *keypair;
		std::vector<Transaction> incoming_transactions;

		EVP_PKEY *generate_keypair ( int key_size );
		std::string public_key_to_string ( EVP_PKEY *key );
};

#endif