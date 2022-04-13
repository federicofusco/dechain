#pragma once 
#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <iostream>
#include "block.h"
#include "transaction.h"

class Blockchain {
	public: 
		Block current_block;
		std::vector<Block> blocks;
		long reward;
		int difficulty;

		Blockchain ( int difficulty, long reward, Transaction coinbase );

		void mine_block ( Transaction coinbase );

		void add_transaction ( Transaction transaction );

		void print ();

	private:
		bool verify_coinbase ( Transaction coinbase );

		void create_genesis_block ( Transaction coinbase );
		void create_block ();

		void insert_block ();

};

#endif