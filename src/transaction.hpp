#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

class Transaction {
	public:
		std::string author;
		std::string recipient;
		float amount;
		long int id;
		std::string hash;
		std::string signature;

		Transaction ( std::string author, std::string recipient, float amount );
		std::string to_string ( bool include_signature );
		void calculate_hash ();
		void set_signature ( std::string signature );
		void set_id ( long int id );
		int verify_signature ();
		EVP_PKEY *get_author ();

	private:
		std::string to_hex ( std::string input );
};

#endif