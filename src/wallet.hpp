#ifndef WALLET_HPP
#define WALLET_HPP 

#include <openssl/evp.h>
#include "transaction.hpp"

class Wallet {
	public:
		Wallet ( int key_size );
		~Wallet ();

		std::string public_key; 
		int sign_transaction ( Transaction *transaction );

	private:
		EVP_PKEY *keypair;
		EVP_PKEY *generate_keypair ( int key_size );
		std::string public_key_to_string ( EVP_PKEY *key );
};

#endif