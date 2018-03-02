/* LexicalAnalyzerForTrumpScript

Developed by: Austen Rozanski (OUID: 113371188)

Created: 02/27/2018
Last Edited: 03/02/2018
*/

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

/* Global variables found below token class*/

/* token class
	Description: token class to store the value of the token and its associated type. Methods within
	are basic getters/setters and output methods.

	Instance variables:
	int type: Uses an int to refer to what type of token it is.
		0 => empty (Used for end of file)
		1 => keyword
		2 => identifier
		3 => constant
		4 => string
		5 => special symbol
		-1 => identifier error
		-2 => contant error
		-3 => general error
	string value: the value of the token
*/
class token {
protected:
	int type;
	string value;
public:
	token();
	token(int type, string value);
	void setType(int type);
	void setValue(string value);
	void setToken(int type, string value);
	int getType();
	string getValue();
	string output();
};
token::token() {
	type = 0;
	value = "";
}
token::token(int type, string value) {
	this->type = type;
	this->value = value;
}
void token::setType(int type) {
	this->type = type;
}
void token::setValue(string value) {
	this->value = value;
}
void token::setToken(int type, string value) {
	this->value = value;
	this->type = type;
}
int token::getType() {
	return type;
}
string token::getValue() {
	return value;
}
string token::output() {
	string typeOutput = "";
	if (type == 1) {
		typeOutput = "keyword";
	}
	else if (type == 2) {
		typeOutput = "id";
	}
	else if (type == 3) {
		typeOutput = "constant";
	}
	else if (type == 4) {
		typeOutput = "string";
	}
	else if (type == 5) {
		typeOutput = "symbol";
	}
	else if (type == -1) {
		typeOutput = "id error";
	}
	else if (type == -2) {
		typeOutput = "constant error";
	}
	else if (type == -3) {
		typeOutput = "general error";
	}
	return "<" + value + "," + typeOutput + ">";
}


/* Global Variables*/
// open an input stream to the inputFile file
const string inputFile = "theProgram.txt";
std::ifstream ifs(inputFile, std::ifstream::in);

// char c stores the current character being read from ifs.
char c;

// Size of the tokens array used to store all tokens in the file
const int TOKENBUFFER = 1000;

// SYMTAB stores the ids, strings, and constants
token SYMTAB[100];

// symtabIndex stores the next available index in the SYMTAB array
int symtabIndex = 0;


/* ERRORHANDLER Method
	Description: Used for handling errors. The errors are stored as tokens with
	an error code for the type.
	Parameters: string s: the lexical value of the token
				int error: the error code for the token
	Returns a token with the error code.
*/
token ERRORHANDLER(string s, int error) {
	return token(error, s);
}


/* BOOKKEEPER Method
	Description: Used for storing ids, constants, and strings in the SYMTAB array.
	Parameters:	string s: the lexical value of the token
				int type: the type id of token
*/
void BOOKKEEPER(string s, int type) {

	bool tokenExists = false;

	for (int i = 0; i < symtabIndex; i++) {
		if (SYMTAB[i].getValue() == s) {
			tokenExists = true;
		}
	}

	if (!tokenExists) {
		SYMTAB[symtabIndex] = token(type, s);
		symtabIndex++;
	}

}


/* idSCANNER method
	Description: An extension of the SCANNER method. Used specifically for scanning
	identifiers once the token is confirmed to be one.
	Parameters:	string s: the value of the token thus far before leaving the SCANNER method.
	returns an id token or an error token.
*/
token idSCANNER(string s) {

	// if the size of the input s is 0, check to make sure the first character is alphabetical
	bool isError = false;
	if (s.size() == 0) {
		if (isalpha(c)) {
			s += c;
		}
		else {
			s += c;
			isError = true;
		}
		c = ifs.get();
	}

	// loop through characters in the input stream, adding them to the token until a valid seperator found
	while (!(c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"')) {
		s += c;
		// if the character is not alphanumeric, trigger error flag
		if (!isalnum(c)) {
			isError = true;
		}
		c = ifs.get();
	}

	// If the first letter was not alphabetic or the id contains a non-alphanumeric char, send the token with an error
	if (isError) {
		return ERRORHANDLER(s, -1);
	}

	// If there was no error, send the token to the BOOKKEEPER and return it
	else {
		BOOKKEEPER(s, 2);
		return token(2, s);
	}

}


/* SCANNER METHOD
	Description: called from main loop for every token created.
	and returns the token scanned from the input stream ifs.
*/
token SCANNER() {

	// string that stores the value of the token as it progresses down the tree of conditions.
	string tokenValue = "";

	// End of File - returns empty token if end of file reached
	if (!ifs.good()) {
		return token(0, "");
	}

	// SPACES, NEWLINES & COMMENTS - discards any chars that are spaces, newlines, or in a comment
	if (c == ' ' || c == '\n' || c == '#') {

		// if # symbol found, trigger isComment flag
		bool isComment = false;
		if (c == '#') { isComment = true; }

		// get the next char (and discard old one)
		c = ifs.get();

		// loop until char not in comment and not a whitespace character is found or eof reached
		while (ifs.good() && (isComment == true || c == ' ' || c == '\n' || c == '#')) {
			if (c == '#') { isComment = true; }
			else if (c == '\n') { isComment = false; }
			c = ifs.get();
		}
	}

	// End of File - returns empty token if end of file reached
	if (!ifs.good()) {
		return token(0, "");
	}


	// KEYWORDS & IDENTIFIERS - Huge tree representing a DFA the will check all keywords.
	//							Only the first branch representing the "AGAIN" keyword is commented
	//							due to all branches being structered the same. 

	// if char c is an 'a' or 'A', check all keywords beginning with an 'a'/'A'
	if (c == 'a' || c == 'A') {
		// send c to the tokenValue
		tokenValue += c;
		// get next char from input stream ifs
		c = ifs.get();
		// if end of file reached, no keyword was completed so return the value as an identifier token
		if (!ifs.good()) { return idSCANNER(tokenValue); }

		else if (c == 'g' || c == 'G') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'a' || c == 'A') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'i' || c == 'I') {
					tokenValue += c;
					c = ifs.get();
					if (!ifs.good()) { return idSCANNER(tokenValue); }
					else if (c == 'n' || c == 'N') {
						tokenValue += c;
						c = ifs.get();

						// if end of file reached, the keyword "again" was found, so return the token
						if (!ifs.good()) { return token(1, tokenValue); }

						// if a valid seperator token was found, the keyword again was found so return the token
						else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
							return token(1, tokenValue);
						}

						// if no seperator token was found and eof not reached, there are more characters so the token is an identifier
						else { return idSCANNER(tokenValue); }

						// All else statements after branches are used to return an identifier token if a valid
						// keyword could not be constructed from the characters.
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		}
		else if (c == 'm' || c == 'M') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'e' || c == 'E') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'r' || c == 'R') {
					tokenValue += c;
					c = ifs.get();
					if (!ifs.good()) { return idSCANNER(tokenValue); }
					else if (c == 'i' || c == 'I') {
						tokenValue += c;
						c = ifs.get();
						if (!ifs.good()) { return idSCANNER(tokenValue); }
						else if (c == 'c' || c == 'C') {
							tokenValue += c;
							c = ifs.get();
							if (!ifs.good()) { return idSCANNER(tokenValue); }
							else if (c == 'a' || c == 'A') {
								tokenValue += c;
								c = ifs.get();

								// "AMERICA" keyword token
								if (!ifs.good()) { return token(1, tokenValue); }
								else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
									return token(1, tokenValue);
								} else { return idSCANNER(tokenValue); }
							} else { return idSCANNER(tokenValue); }
						} else { return idSCANNER(tokenValue); }
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		}
		else if (c == 's' || c == 'S') {
			tokenValue += c;
			c = ifs.get();

			// "AS" keyword token
			if (!ifs.good()) { return token(1, tokenValue); }
			else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
				return token(1, tokenValue);
			} else { return idSCANNER(tokenValue); }
		}
		else if (c == 'n' || c == 'N') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'd' || c == 'D') {
				tokenValue += c;
				c = ifs.get();

				// "AND" keyword token
				if (!ifs.good()) { return token(1, tokenValue); }
				else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
					return token(1, tokenValue);
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 'b' || c == 'B') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'o' || c == 'O') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'o' || c == 'O') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'l' || c == 'L') {
					tokenValue += c;
					c = ifs.get();
					if (!ifs.good()) { return idSCANNER(tokenValue); }
					else if (c == 'e' || c == 'E') {
						tokenValue += c;
						c = ifs.get();
						if (!ifs.good()) { return idSCANNER(tokenValue); }
						else if (c == 'a' || c == 'A') {
							tokenValue += c;
							c = ifs.get();
							if (!ifs.good()) { return idSCANNER(tokenValue); }
							else if (c == 'n' || c == 'N') {
								tokenValue += c;
								c = ifs.get();

								// "BOOLEAN" keyword token
								if (!ifs.good()) { return token(1, tokenValue); }
								else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
									return token(1, tokenValue);
								} else { return idSCANNER(tokenValue); }
							} else { return idSCANNER(tokenValue); }
						} else { return idSCANNER(tokenValue); }
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 'e' || c == 'E') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'l' || c == 'L') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 's' || c == 'S') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'e' || c == 'E') {
					tokenValue += c;
					c = ifs.get();

					// "ELSE" keyword token
					if (!ifs.good()) { return token(1, tokenValue); }
					else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
						return token(1, tokenValue);
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 'f' || c == 'F') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'a' || c == 'A') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'c' || c == 'C') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 't' || c == 'T') {
					tokenValue += c;
					c = ifs.get();

					// "FACT" keyword token
					if (!ifs.good()) { return token(1, tokenValue); }
					else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
						return token(1, tokenValue);
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 'g' || c == 'G') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'r' || c == 'R') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'e' || c == 'E') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'a' || c == 'A') {
					tokenValue += c;
					c = ifs.get();
					if (!ifs.good()) { return idSCANNER(tokenValue); }
					else if (c == 't' || c == 'T') {
						tokenValue += c;
						c = ifs.get();

						// "GREAT" keyword token
						if (!ifs.good()) { return token(1, tokenValue); }
						else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
							return token(1, tokenValue);
						} else { return idSCANNER(tokenValue); }
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 'i' || c == 'I') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'f' || c == 'F') {
			tokenValue += c;
			c = ifs.get();

			// "IF" keyword token
			if (!ifs.good()) { return token(1, tokenValue); }
			else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
				return token(1, tokenValue);
			} else { return idSCANNER(tokenValue); }
		}
		else if (c == 's' || c == 'S') {
			tokenValue += c;
			c = ifs.get();

			// "IS" keyword token
			if (!ifs.good()) { return token(1, tokenValue); }
			else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
				return token(1, tokenValue);
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 'l' || c == 'L') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'o' || c == 'O') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'n' || c == 'N') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'g' || c == 'G') {
					tokenValue += c;
					c = ifs.get();

					// "LONG" keyword token
					if (!ifs.good()) { return token(1, tokenValue); }
					else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
						return token(1, tokenValue);
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		}
		else if (c == 'i' || c == 'I') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'e' || c == 'E') {
				tokenValue += c;
				c = ifs.get();

				// "LIE" keyword token
				if (!ifs.good()) { return token(1, tokenValue); }
				else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
					return token(1, tokenValue);
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		}
		else if (c == 'e' || c == 'E') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 's' || c == 'S') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 's' || c == 'S') {
					tokenValue += c;
					c = ifs.get();

					// "LESS" keyword token
					if (!ifs.good()) { return token(1, tokenValue); }
					else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
						return token(1, tokenValue);
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 'm' || c == 'M') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'a' || c == 'A') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'k' || c == 'K') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'e' || c == 'E') {
					tokenValue += c;
					c = ifs.get();

					// "MAKE" keyword token
					if (!ifs.good()) { return token(1, tokenValue); }
					else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
						return token(1, tokenValue);
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		}
		else if (c == 'o' || c == 'O') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'r' || c == 'R') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'e' || c == 'E') {
					tokenValue += c;
					c = ifs.get();

					// "MORE" keyword token
					if (!ifs.good()) { return token(1, tokenValue); }
					else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
						return token(1, tokenValue);
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 'n' || c == 'N') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'o' || c == 'O') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 't' || c == 'T') {
				tokenValue += c;
				c = ifs.get();

				// "NOT" keyword token
				if (!ifs.good()) { return token(1, tokenValue); }
				else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
					return token(1, tokenValue);
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		}
		else if (c == 'u' || c == 'U') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'm' || c == 'M') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'b' || c == 'B') {
					tokenValue += c;
					c = ifs.get();
					if (!ifs.good()) { return idSCANNER(tokenValue); }
					else if (c == 'e' || c == 'E') {
						tokenValue += c;
						c = ifs.get();
						if (!ifs.good()) { return idSCANNER(tokenValue); }
						else if (c == 'r' || c == 'R') {
							tokenValue += c;
							c = ifs.get();

							// "NUMBER" keyword token
							if (!ifs.good()) { return token(1, tokenValue); }
							else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
								return token(1, tokenValue);
							} else { return idSCANNER(tokenValue); }
						} else { return idSCANNER(tokenValue); }
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 'o' || c == 'O') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'r' || c == 'R') {
			tokenValue += c;
			c = ifs.get();

			// "OR" keyword token
			if (!ifs.good()) { return token(1, tokenValue); }
			else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
				return token(1, tokenValue);
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 'p' || c == 'P') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'r' || c == 'R') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'o' || c == 'O') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'g' || c == 'G') {
					tokenValue += c;
					c = ifs.get();
					if (!ifs.good()) { return idSCANNER(tokenValue); }
					else if (c == 'r' || c == 'R') {
						tokenValue += c;
						c = ifs.get();
						if (!ifs.good()) { return idSCANNER(tokenValue); }
						else if (c == 'a' || c == 'A') {
							tokenValue += c;
							c = ifs.get();
							if (!ifs.good()) { return idSCANNER(tokenValue); }
							else if (c == 'm' || c == 'M') {
								tokenValue += c;
								c = ifs.get();
								if (!ifs.good()) { return idSCANNER(tokenValue); }
								else if (c == 'm' || c == 'M') {
									tokenValue += c;
									c = ifs.get();
									if (!ifs.good()) { return idSCANNER(tokenValue); }
									else if (c == 'i' || c == 'I') {
										tokenValue += c;
										c = ifs.get();
										if (!ifs.good()) { return idSCANNER(tokenValue); }
										else if (c == 'n' || c == 'N') {
											tokenValue += c;
											c = ifs.get();
											if (!ifs.good()) { return idSCANNER(tokenValue); }
											else if (c == 'g' || c == 'G') {
												tokenValue += c;
												c = ifs.get();

												// "PROGRAMMING" keyword token
												if (!ifs.good()) { return token(1, tokenValue); }
												else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
													return token(1, tokenValue);
												} else { return idSCANNER(tokenValue); }
											} else { return idSCANNER(tokenValue); }
										} else { return idSCANNER(tokenValue); }
									} else { return idSCANNER(tokenValue); }
								} else { return idSCANNER(tokenValue); }
							} else { return idSCANNER(tokenValue); }
						} else { return idSCANNER(tokenValue); }
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		}
		else if (c == 'l' || c == 'L') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'u' || c == 'U') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 's' || c == 'S') {
					tokenValue += c;
					c = ifs.get();

					// "PLUS" keyword token
					if (!ifs.good()) { return token(1, tokenValue); }
					else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
						return token(1, tokenValue);
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 's' || c == 'S') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'a' || c == 'A') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'y' || c == 'Y') {
				tokenValue += c;
				c = ifs.get();

				// "SAY" keyword token
				if (!ifs.good()) { return token(1, tokenValue); }
				else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
					return token(1, tokenValue);
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}
	else if (c == 't' || c == 'T') {
		tokenValue += c;
		c = ifs.get();
		if (!ifs.good()) { return idSCANNER(tokenValue); }
		else if (c == 'i' || c == 'I') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'm' || c == 'M') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'e' || c == 'E') {
					tokenValue += c;
					c = ifs.get();
					if (!ifs.good()) { return idSCANNER(tokenValue); }
					else if (c == 's' || c == 'S') {
						tokenValue += c;
						c = ifs.get();

						// "TIMES" keyword token
						if (!ifs.good()) { return token(1, tokenValue); }
						else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
							return token(1, tokenValue);
						} else { return idSCANNER(tokenValue); }
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		}
		else if (c == 'e' || c == 'E') {
			tokenValue += c;
			c = ifs.get();
			if (!ifs.good()) { return idSCANNER(tokenValue); }
			else if (c == 'l' || c == 'L') {
				tokenValue += c;
				c = ifs.get();
				if (!ifs.good()) { return idSCANNER(tokenValue); }
				else if (c == 'l' || c == 'L') {
					tokenValue += c;
					c = ifs.get();

					// "TELL" keyword token
					if (!ifs.good()) { return token(1, tokenValue); }
					else if (c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"') {
						return token(1, tokenValue);
					} else { return idSCANNER(tokenValue); }
				} else { return idSCANNER(tokenValue); }
			} else { return idSCANNER(tokenValue); }
		} else { return idSCANNER(tokenValue); }
	}

	// IDENTIFIERS - if the next char is input stream is alphabetic and not accepted in the tree above,
	//				 it must be an identifier or an error. Send to idSCANNER to analyze.
	else if (isalpha(c)) {
		return idSCANNER(tokenValue);
	}

	// CONSTANTS - if the next char is a digit, it must be a constant or an error.
	else if (isdigit(c)) {
		
		// store char in the tokenValue
		tokenValue += c;

		// store values from input stream into a long long variable to be used to analyze if the
		// constant is > 1,000,000
		long long constantValue = c - '0';

		// get next token from input stream ifs
		c = ifs.get();

		// while eof not reached and the chars from input stream are digits, add values to tokenValue and constantValue
		while (ifs.good() && isdigit(c)) {
			constantValue = (10 * constantValue) + (c - '0');
			tokenValue += c;
			c = ifs.get();
		}

		// ID ERROR - if the next chars are not valid seperators and not the eof, send token to ERRORHANDLER as an ID error
		while (ifs.good() && !(c == ' ' || c == '\n' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?' || c == '(' || c == ')' || c == '#' || c == '\"')) {
			tokenValue += c;
			c = ifs.get();
			return ERRORHANDLER(tokenValue, -1);
		}

		// if the constant is greater than 1,000,000 send it to the BOOKKEEPER to store and return the token
		if (constantValue > 1000000) {
			BOOKKEEPER(tokenValue, 3);
			return token(3, tokenValue);
		}
		// CONSTANT ERROR - if the value is less than 1,000,000 send it to ERRORHANDLER as a constant error
		else {
			return ERRORHANDLER(tokenValue, -2);
		}
	}

	// STRINGS - if first char is a ", all contents until next " are stored as a string token and then returned
	//			 The "s are omitted from the string token.
	else if (c == '\"') {

		// get next char (thus omitting the first ")
		c = ifs.get();

		// add all chars to tokenValue until next " or eof reached
		while (ifs.good() && c != '\"') {
			tokenValue += c;
			c = ifs.get();
		}

		// if next char is a ", add string to BOOKKEEPER and then return the token
		if (c == '\"') {
			c = ifs.get();
			BOOKKEEPER(tokenValue, 4);
			return token(4, tokenValue);
		}
		// if eof was reached before another " was found, send to ERRORHANDLER as a general error
		else {
			return ERRORHANDLER(tokenValue, -3);
		}
	}

	// SPECIAL SYMBOLS - check if the char is a special symbol and return it as a token if so.
	else if (c == ',') {
		c = ifs.get();
		return token(5, ",");
	}
	else if (c == ';') {
		c = ifs.get();
		return token(5, ";");
	}
	else if (c == ':') {
		c = ifs.get();
		return token(5, ":");
	}
	else if (c == '!') {
		c = ifs.get();
		return token(5, "!");
	}
	else if (c == '?') {
		c = ifs.get();
		return token(5, "?");
	}
	else if (c == '(') {
		c = ifs.get();
		return token(5, "(");
	}
	else if (c == ')') {
		c = ifs.get();
		return token(5, ")");
	}

	// GENERAL ERROR - if this point reached, an invalid token not in the alphabet of this language was used.
	//				   send char to ERRORHANDLER and get next char.
	else {
		tokenValue += c;
		c = ifs.get();
		return ERRORHANDLER(tokenValue, -3);
	}
}


/* main method
	Description: loops until the end of the ifs stream. then outputs the contents of file, tokens, and the SYMTAB.
*/
int main() {

	// Create an array to store the tokens
	token tokens[TOKENBUFFER];

	// get the next char in the ifs stream
	c = ifs.get();

	// i used as next available index in the tokens array
	int i = 0;

	// loop until end of ifs stream reached, calling SCANNER for each token and putting
	// them into the tokens array.
	while (ifs.good()) {
		tokens[i] = SCANNER();
		if (tokens[i].getType() != 0) {
			i++;
		}
	}

	// close the ifs stream
	ifs.close();

	// OUTPUT

	// output file - create another stream to output contents of the file. Works same as above
	// without creating tokens
	std::ifstream ifs2(inputFile, std::ifstream::in);
	char c2 = ifs2.get();
	while (ifs2.good()) {
		std::cout << c2;
		c2 = ifs2.get();
	}
	ifs2.close();

	// output each token
	cout << endl << endl << "TOKENS:" << endl;
	for (int j = 0; j < i; j++) {
		cout << tokens[j].output() + " ";
	}

	// output symtab
	cout << endl << endl << "SYMTAB:" << endl;
	for (int j = 0; j < symtabIndex; j++) {
		cout << SYMTAB[j].output();
	}

	// pause the system to see output (WINDOWS ONLY!)
	system("pause");

	return 0;
}