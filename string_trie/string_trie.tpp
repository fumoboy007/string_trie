#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <stdexcept>


template<typename charT, charT reservedChar>
struct string_trie<charT, reservedChar>::node {
	typedef std::unordered_map<charT, node*> child_map;
	
	
	node* parent;
	
	std::basic_string<charT> string;
	
	bool isLeaf;
	size_t compareIndex;
	child_map children;
	
	
	// Internal node constructor
	node(node* parent, size_t compareIndex, const std::basic_string<charT>& path) : parent(parent), string(path), isLeaf(false), compareIndex(compareIndex), children() {}
	
	// Leaf node constructor
	node(node* parent, const std::basic_string<charT>& string) : parent(parent), string(string), isLeaf(true), compareIndex(0), children() {}
	
	~node() {
		for (typename child_map::iterator i = children.begin(); i != children.end(); i++) {
			delete i->second;
		}
	}
	
	
private:
	node(const node& otherNode);
	node(node&& otherNode);
	
	node& operator=(node otherNode);
};


template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>::string_trie() : root_(nullptr) {
}

template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>::string_trie(const string_trie& otherTrie) : root_(nullptr) {
	if (otherTrie.root_) root_ = copyNode(otherTrie.root_, nullptr);
}

template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>::string_trie(string_trie&& otherTrie) : string_trie() {
	swap(*this, otherTrie);
}


template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>::~string_trie() {
	delete root_;
}


template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>& string_trie<charT, reservedChar>::operator=(string_trie otherTrie) {
	swap(*this, otherTrie);
	
	return *this;
}


template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::insert(std::basic_string<charT> string) {
	normalizeString(string);
	
	
	node* node = search(string);
	
	if (node) {  // if node found
		size_t compareIndex = indexOfFirstDifference(node->string, string);
		
		if (compareIndex != std::basic_string<charT>::npos) {  // if the strings are not the same
			if (!node->isLeaf && node->compareIndex == compareIndex) {  // if node is where we should insert new leaf
				const charT& character = string[compareIndex];
				
				node->children[character] = new struct node(node, string);
			} else {  // else, create new internal node and insert it at appropriate position in path
				struct node* siblingOfNewInternal = siblingOfNewInternalNode(node, compareIndex);
				struct node* parentOfNewInternal = siblingOfNewInternal->parent;
				
				
				struct node* internal = new struct node(parentOfNewInternal, compareIndex, string.substr(0, compareIndex).append(1, reservedChar));
				
				// Set new internal node as parent of existing subtree
				const charT& existingNodeCharacter = siblingOfNewInternal->string[compareIndex];
				internal->children[existingNodeCharacter] = siblingOfNewInternal;
				siblingOfNewInternal->parent = internal;
				
				// Set new internal node as parent of new leaf node
				const charT& newNodeCharacter = string[compareIndex];
				internal->children[newNodeCharacter] = new struct node(internal, string);
				
				
				// Set original parent of existing subtree as parent of internal node
				if (parentOfNewInternal) {
					const charT& character = siblingOfNewInternal->string[parentOfNewInternal->compareIndex];
					
					parentOfNewInternal->children[character] = internal;
				} else {
					root_ = internal;
				}
			}
		}
	} else {  // else, root_ == nullptr
		root_ = new struct node(nullptr, string);
	}
}

template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::remove(std::basic_string<charT> string) {
	normalizeString(string);
	
	
	node* node = search(string);
	
	if (node && node->isLeaf && node->string == string) {  // if we found a matching leaf node
		struct node* parent = node->parent;
		
		if (parent) {
			// Remove reference to removed node from parent
			const charT& character = node->string[parent->compareIndex];
			
			parent->children.erase(character);
			
			
			assert(parent->children.size() > 0);
			
			
			if (parent->children.size() == 1) {  // if parent only has one child remaining, then delete parent
				struct node* onlyChild = parent->children.begin()->second;
				
				parent->children.clear();
				
				
				struct node* parentOfParent = parent->parent;
				
				onlyChild->parent = parentOfParent;
				
				if (parentOfParent) {  // if parent has a parent, remove the reference
					const charT& character = parent->string[parentOfParent->compareIndex];
					
					parentOfParent->children[character] = onlyChild;
				} else {
					root_ = onlyChild;
				}
				
				
				delete parent;
			}
		} else {
			root_ = nullptr;
		}
		
		
		delete node;
	}
}


template<typename charT, charT reservedChar>
bool string_trie<charT, reservedChar>::contains(std::basic_string<charT> string) const {
	normalizeString(string);
	
	
	node* node = search(string);
	
	return node && node->string == string;
}

template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::search(const std::basic_string<charT>& string) const -> node* {
	node* node = root_;
	
	while (node) {
		if (node->isLeaf) break;  // if at a leaf node
		
		
		if (node->compareIndex > string.length()) break;  // if the index we are looking at is past the end of the string
		
		const charT& character = string[node->compareIndex];
		
		
		auto iter = node->children.find(character);
		
		if (iter == node->children.end()) break;  // if character not found in children map
		
		
		node = iter->second;
	}
	
	return node;
}


template<typename charT, charT reservedChar>
size_t string_trie<charT, reservedChar>::indexOfFirstDifference(const std::basic_string<charT>& string1, const std::basic_string<charT>& string2) {
	size_t length1 = string1.length();
	size_t length2 = string2.length();
	
	for (size_t i = 0; i < length1 && i < length2; i++) {
		if (string1[i] != string2[i]) return i;  // if characters do not match, return index
	}
	
	return std::basic_string<charT>::npos;  // all characters matched
}

template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::siblingOfNewInternalNode(node* start, size_t compareIndex) const -> node* {
	struct node* sibling = start;
	
	while (start) {
		if (!start->isLeaf && (!start->parent || start->parent->compareIndex <= compareIndex)) {
			sibling = start;
			
			break;
		}
		
		start = start->parent;
	}
	
	return sibling;
}


template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::normalizeString(std::basic_string<charT>& string) {
	if (string.length() == 0) throw std::invalid_argument("String must not be empty.");  // string cannot be empty
	if (string.find_first_of(reservedChar) != std::basic_string<charT>::npos) throw std::invalid_argument("String must not contain specified reserved character.");  // string cannot contain reserved character
	
	string.append(1, reservedChar);  // append our end-of-string character
}


template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::copyNode(const node& node, const struct node* parent) -> struct node* {
	struct node* copy = node.isLeaf ? new struct node(parent, node->string) : new struct node(parent, node.compareIndex, node->string);
	
	
	copy->children = node->children;
	
	for (auto& element : copy->children) {
		element->second = copyNode(*element->second, copy);
	}
	
	
	return copy;
}

template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::swap(string_trie<charT, reservedChar>& trie1, string_trie<charT, reservedChar>& trie2) {
	using std::swap;
	
	swap(trie1.root_, trie2.root_);
}


#ifdef DEBUG

#include <iostream>

template<typename charT>
std::ostream& operator<<(std::ostream& out, std::basic_string<charT> string) {
	string.pop_back();
	
	for (const auto& character : string) {
		out << (char)character;
	}
	
	return out;
}

template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::printStructure() const {
	std::cout << "begin structure" << std::endl;
	
	if (root_) printNode(*root_);
	
	std::cout << "end structure" << std::endl << std::endl;
}

template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::printNode(const node& node) const {
	if (node.isLeaf) {
		std::cout << "Leaf Node" << std::endl << "---------" << std::endl;
		std::cout << "String: " << node.string << std::endl << std::endl;
	} else {
		std::cout << "Internal Node" << std::endl << "-------------" << std::endl;
		std::cout << "Compare index: " << node.compareIndex << std::endl;
		std::cout << "Path: " << node.string << std::endl;
		std::cout << "Children:";
		
		for (const auto& element : node.children) {
			std::cout << " (";
			
			struct node* child = element.second;
			if (child->isLeaf) {
				std::cout << child->string;
			} else {
				std::cout << child->compareIndex;
			}
			
			std::cout << ")";
		}
		
		std::cout << std::endl << std::endl;
		
		
		for (const auto& element : node.children) {
			printNode(*element.second);
		}
	}
}

#endif