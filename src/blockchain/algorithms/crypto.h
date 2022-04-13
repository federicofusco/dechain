#pragma once
#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

namespace crypto {
	std::string to_hex ( std::string input );
	std::string from_hex ( std::string input );

	std::string sha256 ( std::string input );

	std::string merkel_tree ( std::vector<std::string> nodes );
}

#endif