#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include <vector>
#include <iostream>
#include "block.hpp"
#include "transaction.hpp"

class Blockchain {
	public:
		Block genesis_block;
		Block current_block;
		std::vector<Block> blocks;
		int difficulty;

		Blockchain ( int difficulty );
		void add_transaction ( Transaction transaction );
		void mine_block ();
		bool verify ();

	private:
		void create_genesis_block ();
		void insert_current_block ();
		void add_block ( Block block );

};

#endif