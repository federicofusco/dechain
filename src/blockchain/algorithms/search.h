#pragma once
#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include <iostream>
#include <math.h>
#include "block.h"
#include "transaction.h"

namespace search {
	Transaction binary_search ( std::vector<Block> blocks, std::vector<Block>::iterator begin, std::vector<Block>::iterator end, long tx_index );
}

#endif