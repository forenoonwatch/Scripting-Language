
inline std::shared_ptr<Statement> Statement::getParent() {
	return parent;
}

inline std::vector<std::shared_ptr<Statement>>& getChildren() {
	return children;
}

inline std::vector<Token>& getTokens() {
	return tokens;
}
