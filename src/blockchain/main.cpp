#include <iostream>
#include "wallet.h"
#include "blockchain.h"
#include "transaction.h"
#include "algorithms/search.h"

int main () {

	// Creates the wallets
	Wallet walletA ( 1024 );
	Wallet walletB ( 1024 );

	// Creates a new coinbase
	Transaction coinbase = walletA.create_coinbase ( walletA.public_key, 69 );


	Transaction test = walletA.create_transaction ( walletB.public_key, 2 );

	// Creates a new blockchain
	Blockchain chain ( 2, 69, coinbase );

	for ( int x = 0; x < 3; x++ ) {
		Transaction y = walletA.create_coinbase ( walletA.public_key, 69 );

		Transaction a = walletA.create_transaction ( walletB.public_key, 1 );
		Transaction b = walletA.create_transaction ( walletB.public_key, 1 );
		Transaction c = walletA.create_transaction ( walletB.public_key, 1 );

		chain.add_transaction ( a );
		chain.add_transaction ( b );
		chain.add_transaction ( c );

		if ( x == 1 ) {
			chain.add_transaction ( test );
			test.set_index ( chain.current_block.index + chain.current_block.transactions.size () );
		}

		chain.mine_block ( y );
	}

	search::binary_search ( chain.blocks, chain.blocks.begin (), chain.blocks.end (), test.tx_index );
}