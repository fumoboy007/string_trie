#ifndef _STRING_TRIE_H_
#define _STRING_TRIE_H_

#include <string>


template<typename charT, charT reservedChar>
class string_trie {
public:
	string_trie();
	string_trie(const string_trie& otherTrie);
	string_trie(string_trie&& otherTrie);
	
	~string_trie();
	
	string_trie& operator=(string_trie otherTrie);
	
	
	/* The following throw std::invalid_argument if string contains reservedChar. */
	
	void insert(std::basic_string<charT> string);
	void remove(std::basic_string<charT> string);
	
	bool contains(std::basic_string<charT> string) const;
	
#ifdef DEBUG
	void printStructure() const;
#endif
	
private:
	struct node;
	
	
	node* root_;
	
	
	node* search(const std::basic_string<charT>& string) const;
	
	static size_t indexOfFirstDifference(const std::basic_string<charT>& string1, const std::basic_string<charT>& string2);
	node* siblingOfNewInternalNode(node* start, size_t compareIndex) const;
	
	static void normalizeString(std::basic_string<charT>& string);
	
	static node* copyNode(const node& node, const struct node* parent);
	static void swap(string_trie& trie1, string_trie& trie2);
	
#ifdef DEBUG
	void printNode(const node& node) const;
#endif
};


#include "string_trie.tpp"

#endif