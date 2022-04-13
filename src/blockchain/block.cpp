#include "block.h"

/**
 * The block constructor
 *
 * @param prev_block - The hash of the previous block
 * @param index - The block's index
 * @param coinbase - The coinbase transaction which contains the miner reward
 * @param difficulty - The block's mining difficulty
 */
Block::Block ( std::string prev_block, long index, Transaction coinbase, int difficulty ) {
	this -> prev_block = prev_block;
	this -> index = index;
	this -> nonce = 0;
	this -> difficulty = difficulty;
	this -> transactions.push_back ( coinbase );
	this -> set_timestamp ();
	this -> calculate_merkel_tree ();
	this -> calculate_hash ();
}

/**
 * The block constructor
 *
 * @param prev_block - The hash of the previous block
 * @param index - The block's index
 * @param difficulty - The block's mining difficulty
 */
Block::Block ( std::string prev_block, long index, int difficulty ) {
	this -> prev_block = prev_block;
	this -> index = index;
	this -> nonce = 0;
	this -> difficulty = difficulty;
	this -> set_timestamp ();
	this -> calculate_merkel_tree ();
	this -> calculate_hash ();
}

Block::Block () {}

/**
 * Adds a transaction to the block
 *
 * @param transaction - The transaction which should be added to the block
 */ 
void Block::add_transaction ( Transaction transaction ) {

	// Verifies the transaction
	if ( !( transaction.verify ( false ) ) )
		throw std::runtime_error ( "Attempted adding an invalid transaction to the block!" );

	// Pushes the transaction
	transaction.set_index ( this -> index + this -> transactions.size () + 1 );
	this -> transactions.push_back ( transaction );
	this -> calculate_merkel_tree ();
	this -> calculate_hash ();
}

/**
 * Sets the block's coinbase
 *
 * @param coinbase - The block's new coinbase
 */
void Block::set_coinbase ( Transaction coinbase ) {

	// Verifies the coinbase
	if ( !coinbase.verify ( true ) )
		throw std::runtime_error ( "Attempted to set coinbase to invalid transaction!" );

	// Adds the coinbase 
	coinbase.set_index ( this -> index );
	this -> transactions.insert ( this -> transactions.begin (), coinbase );
	this -> calculate_merkel_tree ();
	this -> calculate_hash ();
}

/**
 * Calculates the block's hash
 */
void Block::calculate_hash () {
	this -> hash = crypto::sha256 ( this -> to_string ( true ) );
}

/**
 * Calculates the block transaction's merkel tree
 */
void Block::calculate_merkel_tree () {
	if ( this -> transactions.size () > 0 ) {

		// Calculates the merkel tree
		std::vector<std::string> tree;
		for ( std::vector<Transaction>::iterator transaction = this -> transactions.begin (); transaction < this -> transactions.end (); transaction += 2 ) {

			std::string raw ( transaction -> to_string () );

			if ( ( transaction + 1 ) != this -> transactions.end () )
				raw.append ( ( transaction + 1 ) -> to_string () );

			tree.push_back ( crypto::sha256 ( raw ) ); 
		}

		this -> merkel_tree = crypto::merkel_tree ( tree ); 
	}
}

/**
 * Verifies the block's hash
 *
 * @returns Whether or not the block's hash is valid
 */
bool Block::verify_hash () {
	return this -> hash == crypto::sha256 ( this -> to_string ( true ) );
}

/**
 * Verifies the block's merkel tree
 *
 * @returns Whether or not the block's merkel tree is valid
 */
bool Block::verify_merkel_tree () {
	if ( this -> transactions.size () == 0 )
		return false;
	
	// Loops through the transactions
	std::vector<std::string> tree;
	for ( std::vector<Transaction>::iterator transaction = this -> transactions.begin (); transaction < this -> transactions.end (); transaction += 2 ) {

		std::string raw ( transaction -> to_string () );

		if ( ( transaction + 1 ) != this -> transactions.end () )
			raw.append ( ( transaction + 1 ) -> to_string () );

		tree.push_back ( crypto::sha256 ( raw ) ); 
	}
	
	return this -> merkel_tree == crypto::merkel_tree ( tree );
}

/**
 * Verifies each transaction in the block (including the coinbase)
 *
 * @returns Whether or not every trasaction in the block is valid
 */
bool Block::verify_transactions () {

	if ( this -> transactions.size () == 0 )
		return false;

	// Loops through each transaction
	if ( this -> transactions.size () > 1 )
		for ( std::vector<Transaction>::iterator transaction = this -> transactions.begin () + 1; transaction < this -> transactions.end (); transaction++ ) {

			// Verifies the integrity of the transaction
			if ( !( transaction -> verify ( false ) ) )
				return false;

			// Verifies the transaction's index
			if ( transaction -> tx_index != (long int)( this -> index + ( transaction - this -> transactions.begin () ) ) )
				return false;
			

		}

	return true;
}

/**
 * Verifies the block's coinbase transactions
 *
 * @param reward - The block's mining reward
 * @returns Whether or not the coinbase is valid
 */
bool Block::verify_coinbase ( long reward ) {

	// Calculates the coinbase's input total
	long total = 0;
	for ( auto input : this -> transactions.begin () -> inputs )
		total += input.prev_out.value;

	if ( reward != total )
		return false;

	if ( !( this -> transactions.begin () -> verify ( true ) ) )
		return false;

	return true;
}

/**
 * Verifies the block
 *
 * @param is_genesis - If the block is a genesis block
 * @returns Whether or not the block is valid
 */
bool Block::verify ( bool is_genesis ) {

	// Verifies the block's hash
	if ( !( this -> verify_hash () ) )
		return false;

	// Verifies the tree's merkel tree
	if ( !( this -> verify_merkel_tree () ) ) 
		return false;

	// Verifies the transactions in the block
	if ( !( this -> verify_transactions () ) )
		return false;

	if ( this -> difficulty <= 0 )
		return false;

	if ( !is_genesis && this -> prev_block.empty () )
		return false;

	if ( !is_genesis && this -> index == 0 )
		return false;

	if ( !( this -> is_mined () ) )
		return false;


	return true;
}

/**
 * Verifies the block
 *
 * @param is_genesis - If the block is a genesis block
 * @param reward - The block's mining reward
 * @returns Whether or not the block is valid
 */
bool Block::verify ( bool is_genesis, long reward ) {

	// Verifies the block's hash
	if ( !( this -> verify_hash () ) )
		return false;

	// Verifies the tree's merkel tree
	if ( !( this -> verify_merkel_tree () ) ) 
		return false;

	// Verifies the transactions in the block
	if ( !( this -> verify_transactions () ) )
		return false;

	// Verifies the coinbase
	if ( !( this -> verify_coinbase ( reward ) ) ) 
		return false;

	if ( this -> difficulty <= 0 )
		return false;

	if ( !is_genesis && this -> prev_block.empty () )
		return false;

	if ( !is_genesis && this -> index == 0 )
		return false;

	if ( !( this -> is_mined () ) )
		return false;

	return true;
}

/**
 * Converts the block to a string
 *
 * @param is_hash - Whether or not this should include the block's current hash
 */
std::string Block::to_string ( bool is_hash ) {
	std::ostringstream stream;

	if ( !is_hash )
		stream << this -> hash;
	
	stream << this -> prev_block;
	stream << this -> merkel_tree;
	stream << this -> time.count ();
	stream << this -> nonce;
	stream << this -> index;
	return stream.str ();
}

/**
 * Mines the current block
 */
void Block::mine_block () {
	while ( !( this -> is_mined () ) ) {
		nonce++;
		this -> calculate_hash ();
	}
}

/**
 * Sets the block's timestamp
 */
void Block::set_timestamp () {
	this -> time = std::chrono::duration_cast<std::chrono::milliseconds> ( std::chrono::system_clock::now ().time_since_epoch () );
}

/**
 * Checks if the block is mined
 *
 * @returns Whether or not the block has been mined
 */
bool Block::is_mined () {
	for ( auto c : this -> hash.substr ( 0, this -> difficulty ) )
		if ( c != '0' )
			return false;

	return true;
}





















void Block::print ( bool is_genesis ) {
	std::cout << "===== BLOCK =====" << std::endl;
	std::cout << "Index: " << this -> index << std::endl;
	std::cout << "Timestamp: " << this -> time.count () << std::endl;
	std::cout << "Hash: " << this -> hash << std::endl;
	std::cout << "Merkl Root: " << this -> merkel_tree << std::endl;
	std::cout << "Prev Block: " << this -> prev_block << std::endl;
	std::cout << "Nonce: " << this -> nonce << std::endl;
	std::cout << "Mined: " << this -> is_mined () << std::endl;
	std::cout << "Valid: " << this -> verify ( is_genesis ) << std::endl;

	// Prints the coinbase
	if ( this -> transactions.size () > 0 ) 
		this -> transactions.front ().print ( true );

	// Prints the transactions
	if ( this -> transactions.size () > 1 )
		for ( std::vector<Transaction>::iterator transaction = this -> transactions.begin () + 1; transaction != this -> transactions.end (); transaction++ )
			transaction -> print ( false );
}