#include "blockchain.hpp"

/**
 * The blockchain constructor
 * 
 * @param difficulty - The default block difficulty
 */
Blockchain::Blockchain ( int difficulty ) {
	this -> difficulty = difficulty;
	this -> create_genesis_block ();
	Block current_block ( 1, this -> genesis_block.hash, this -> difficulty ); 
	this -> current_block = current_block; 
}

/**
 * Creates a genesis block
 */ 
void Blockchain::create_genesis_block () {
	Block genesis_block ( 0, "a", 1 );
	genesis_block.mine_block ();
	this -> add_block ( genesis_block );
	this -> genesis_block = genesis_block;
}

/**
 * Adds a block to the blockchain
 *
 * @param block - The block which should be added
 */
void Blockchain::add_block ( Block block ) {
	this -> blocks.push_back ( block );
}

/**
 * Adds the current block to the blockchain and creates a new block
 */
void Blockchain::insert_current_block () {

	// Checks if the block is valid
	if ( !( this -> current_block.verify () ) )
		throw std::runtime_error ( "Attempted to insert invalid block to blockchain!" );

	// Inserts the current block
	this -> add_block ( this -> current_block );

	// Creates a new block
	Block current_block ( this -> blocks.size (), this -> blocks.back ().hash, this -> difficulty );
	this -> current_block = current_block;
}

/**
 * Adds a transaction to the current block
 *
 * @param transaction - The transaction which should be added
 */
void Blockchain::add_transaction ( Transaction transaction ) {
	current_block.add_transaction ( transaction );
}


/**
 * Mines the current block
 */
void Blockchain::mine_block () {

	// Mines the current block
	this -> current_block.mine_block ();

	// Inserts the block
	this -> insert_current_block ();
}

/**
 * Verifies the integrity of the blockchain
 *
 * @returns Whether or not the blockchain is valid
 */ 
bool Blockchain::verify () {
	bool is_difficulty_valid = this -> difficulty > 0;
	bool are_blocks_valid = true;
	for ( std::vector<Block>::iterator block = this -> blocks.begin () + 1; block != this -> blocks.end (); block++ ) {
		if ( !( block -> verify ( ( block - 1 ) -> hash, ( block - 1 ) -> id ) ) )
			are_blocks_valid = false;
	} 
	return is_difficulty_valid && are_blocks_valid;
}