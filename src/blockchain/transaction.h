#pragma once
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "transaction_input.h"
#include "transaction_output.h"
#include "crypto.h"

class Transaction {
	public:
		std::string hash;
		long tx_index;
		std::chrono::milliseconds time;
		std::vector<TransactionInput> inputs;
		std::vector<TransactionOutput> outputs;

		Transaction ( std::vector<TransactionInput> inputs, std::string author, std::string recipient, long amount );
		Transaction ( std::vector<TransactionInput> inputs, std::vector<TransactionOutput> outputs );
		
		void create_outputs ( std::string author, std::string recipient, long amount );
		
		void calculate_hash ();
		bool verify_hash ();
		
		bool verify ( bool is_coinbase );

		std::string to_string ();
		void set_index ( long tx_index );

		void print ( bool is_coinbase );

	private:
		void set_timestamp ();
};

#endif