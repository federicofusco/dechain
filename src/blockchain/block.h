#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include "transaction.h"
#include "algorithms/crypto.h"

class Block {
	public:
		int difficulty;
		std::string hash;
		std::string prev_block;
		std::string merkel_tree;
		std::chrono::milliseconds time;
		long long nonce;
		long index;
		std::vector<Transaction> transactions;

		Block ( std::string prev_block, long index, Transaction coinbase, int difficulty );
		Block ( std::string prev_block, long index, int difficulty );
		Block ();

		void add_transaction ( Transaction transaction );
		void set_coinbase ( Transaction coinbase );

		void calculate_hash ();
		void calculate_merkel_tree ();

		bool verify_hash ();
		bool verify_merkel_tree ();
		bool verify_transactions ();
		bool verify_coinbase ( long reward );
		bool verify ( bool is_genesis );
		bool verify ( bool is_genesis, long reward );

		std::string to_string ( bool is_hash );

		void mine_block ();


		void print ( bool is_genesis );

	private:
		void set_timestamp ();
		bool is_mined ();
};

#endif