#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <openssl/sha.h>
#include <vector> 
#include "transaction.hpp"

class Block {
	public:

		Block ( long int id, std::string prev_hash, int difficulty );
		Block ();

		void add_transaction ( Transaction transaction );
		std::string to_string ();
		void calculate_hash ();
		bool verify_hash ();
		void mine_block ();
		bool verify_transactions ();
		bool verify ();
		bool verify ( std::string prev_hash, long int id );

		long int id;
		int nonce;
		int difficulty;
		std::string hash;
		std::string prev_hash;
		std::vector<Transaction> transactions; 

	private:
		std::string to_hex ( std::string input );
		std::string from_hex ( std::string input );
		bool is_mined ();
};

#endif 