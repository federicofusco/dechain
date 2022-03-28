#include <iostream>

#include "wallet.hpp"
#include "transaction.hpp"
#include "blockchain.hpp"

int main () {

	// Creates the wallets
	Wallet walletA ( 1024 );
	Wallet walletB ( 1024 );

	// Creates a new blockchain
	Blockchain chain ( 2 );

	// Creates a new transaction
	Transaction transactionA ( walletA.public_key, walletB.public_key, 10.0, chain.current_block.transactions.size () );
	walletA.sign_transaction ( &transactionA );
	chain.add_transaction ( transactionA );
	chain.mine_block ();

	// Creates a new transaction
	Transaction transactionB ( walletB.public_key, walletA.public_key, 5.0, chain.current_block.transactions.size () );
	walletB.sign_transaction ( &transactionB );
	chain.add_transaction ( transactionB );
	chain.mine_block ();

	std::cout << "Validity: " << chain.verify () <<std::endl;
}