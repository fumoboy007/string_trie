#ifndef _STRING_TRIE_H_
#define _STRING_TRIE_H_

#include <string>
#include <vector>


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
	
	void verifyStructure() const;
#endif
	
private:
	struct node;
	
	
	node* root_;
	
	
	std::vector<node*> search(const std::basic_string<charT>& string) const;
	
	node* siblingOfNewInternalNode(size_t compareIndex, const std::vector<node*>& nodesInSearchPath, node** parentRef) const;
	
	static size_t indexOfFirstDifference(const std::basic_string<charT>& string1, const std::basic_string<charT>& string2);
	
	static void normalizeString(std::basic_string<charT>& string);
	
	static void swap(string_trie& trie1, string_trie& trie2);
	
	
#ifdef DEBUG
	void printNode(const node& node) const;
	
	void verifyNode(const node& node, size_t compareIndex, const std::basic_string<charT>& path) const;
#endif
};


#include "string_trie.tpp"

#endif