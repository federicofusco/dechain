#include "search.h"

namespace search {

	/**
	 * An implementation of binary search to find transactions in the blockchain based on their index
	 *
	 * @param blocks - A vector containing all the blocks
	 * @param begin - The begin iterator of the vector
	 * @param end - The end iterator of the vector
	 * @param tx_index - The transaction index
	 * @returns The transaction
	 */
	Transaction binary_search ( std::vector<Block> blocks, std::vector<Block>::iterator begin, std::vector<Block>::iterator end, long tx_index ) {

		// Gets the middle block
		Block middle = *( begin + (int)( ( end - begin ) / 2 ) );

		// Checks if the transaction index is less than the block's index
		if ( tx_index < middle.index ) {

			// Searches the left hash of the binary tree
			return binary_search ( blocks, begin, begin + (int)( ( end - begin ) / 2 ), tx_index );
		}

		if ( tx_index > middle.index ) {

			// Checks if the index is in the block's range 
			if ( (long) middle.transactions.size () > tx_index - middle.index ) { 

				// The transaction is in the block
				return middle.transactions.at ( tx_index - middle.index );
			}

			// Searches the right half of the tree
			return binary_search ( blocks, begin + (int)( ( end - begin ) / 2 ), end, tx_index );
		}

		return middle.transactions.front ();

	}

}