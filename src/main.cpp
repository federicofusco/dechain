#include <iostream>

#include "wallet.hpp"
#include "transaction.hpp"

int main () {
	Wallet walletA ( 1024 );
	Wallet walletB ( 1024 );

	Transaction transaction ( walletA.public_key, walletB.public_key, 10.0 );
	walletA.sign_transaction ( &transaction );
}