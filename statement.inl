
inline std::shared_ptr<Statement> Statement::getParent() {
	return parent;
}

inline std::vector<std::shared_ptr<Statement>>& Statement::getChildren() {
	return children;
}

inline std::vector<Token>& Statement::getTokens() {
	return tokens;
}
