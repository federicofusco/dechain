#include "blockchain.h"

Blockchain::Blockchain ( int difficulty, long reward, Transaction coinbase ) {
	this -> difficulty = difficulty;
	this -> reward = reward;
	this -> create_genesis_block ( coinbase );
	this -> create_block ();
}

/**
 * Mines the current block with a given coinbase
 *
 * @param coinbase - The coinbase transaction
 */
void Blockchain::mine_block ( Transaction coinbase ) {

	// Verifies the validity of the coinbase
	if ( !( this -> verify_coinbase ( coinbase ) ) )
		throw std::runtime_error ( "Invalid coinbase transaction!" );

	this -> current_block.set_coinbase ( coinbase );

	// Mines the block
	this -> current_block.mine_block ();
	this -> insert_block ();
}

void Blockchain::add_transaction ( Transaction transaction ) {
	this -> current_block.add_transaction ( transaction );
}

/**
 * Verifies the validity of a coinbase transaction
 *
 * @param coinbase - The coinbase which should be validated
 * @returns Whether or not the given coinbase is valid
 */
bool Blockchain::verify_coinbase ( Transaction coinbase ) {

	// Verifies the coinbase input total
	long total = 0;
	for ( auto input : coinbase.inputs )
		total += input.prev_out.value;

	if ( this -> reward != total )
		return false;

	// Verifies the coinbase
	return coinbase.verify ( true );
}	

/**
 * Creates a new genesis block
 */
void Blockchain::create_genesis_block ( Transaction coinbase ) {

	// Checks if the blockchain has already been initialized
	if ( !( this -> blocks.empty () ) )
		throw std::runtime_error ( "Genesis block already exists!" );

	// Verifies the coinbase
	if ( !( this -> verify_coinbase ( coinbase ) ) )
		throw std::runtime_error ( "Attempted to create genesis block with invalid coinbase transaction!" ); 

	// Creates a new block
	Block genesis_block ( "genesis", 0, difficulty );
	genesis_block.set_coinbase ( coinbase );
	genesis_block.mine_block ();

	this -> blocks.push_back ( genesis_block );
}

/**
 * Creates a new empty block
 *
 * @param coinbase - The new block's coinbase
 */
void Blockchain::create_block () {

	// Creates a new block
	Block current_block ( this -> blocks.back ().hash, ( this -> blocks.back ().index ) + ( this -> blocks.back ().transactions.size () ), this -> difficulty );
	this -> current_block = current_block;
}

/**
 * Inserts the current block into the blockchain
 */
void Blockchain::insert_block () {

	// Verifies the current block
	if ( !( this -> current_block.verify ( false, this -> reward ) ) )
		throw std::runtime_error ( "Attempted pushing invalid block!" );

	this -> blocks.push_back ( this -> current_block );

	// Creates a new block
	this -> create_block ();
}





















void Blockchain::print () {

	// Prints the unconfirmed block
	this -> current_block.print ( false );

	// Prints the genesis block
	this -> blocks.front ().print ( true );

	// Print the blocks
	for ( std::vector<Block>::iterator block = this -> blocks.begin () + 1; block != this -> blocks.end (); block++ )
		block -> print ( false );

}