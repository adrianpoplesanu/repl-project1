#include "token.h"


void test_keywords_map() {
	std::cout << "testing keywords map...\n";
	for(std::map<std::string, TokenType>::iterator it = keywords.begin(); it != keywords.end(); ++it) {
		std::cout << "Key: " << it->first << '\n';
  		std::cout << "Value: " << it->second << '\n';
	}
}

void test_token_type_map() {
	std::cout << "testing token type map...\n";
}

void run_all_tests() {
	test_keywords_map();
	test_token_type_map();
}
