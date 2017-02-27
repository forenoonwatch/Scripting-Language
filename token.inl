
inline std::string Token::getContent() const {
	return content;
}

inline Token::TokenType Token::getTokenType() const {
	return tokenType;
}

inline Statement* Token::getLink() {
	return link;
}
