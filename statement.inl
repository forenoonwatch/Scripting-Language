
inline Statement* Statement::getParent() {
	return parent;
}

inline std::vector<Statement*>& Statement::getChildren() {
	return children;
}

inline std::vector<Token>& Statement::getTokens() {
	return tokens;
}
