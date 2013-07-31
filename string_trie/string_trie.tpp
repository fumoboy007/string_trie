#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <stack>
#include <utility>


template<typename charT, charT reservedChar>
struct string_trie<charT, reservedChar>::node {
	typedef std::unordered_map<charT, node*> child_map;
	
	
	std::basic_string<charT> string;
	
	bool isLeaf;
	size_t compareIndex;
	child_map children;
	
	
	// Internal node constructor
	node(size_t compareIndex, const std::basic_string<charT>& path) : string(path), isLeaf(false), compareIndex(compareIndex), children() {}
	
	// Leaf node constructor
	node(const std::basic_string<charT>& string) : string(string), isLeaf(true), compareIndex(0), children() {}
	
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
	// Avoid recursion as we may have very many levels
	std::stack<node*> nodes;
	
	node* otherRoot = otherTrie.root_;
	
	if (otherRoot) {
		if (otherRoot->isLeaf) {
			root_ = new node(otherRoot->string);
		} else {
			root_ = new node(otherRoot->compareIndex, otherRoot->string);
			root_->children = otherRoot->children;
			
			nodes.push(root_);
		}
	}
	
	while (!nodes.empty()) {
		node* node = nodes.top();
		nodes.pop();
		
		for (auto& element : node->children) {
			struct node* otherNode = element.second;
			
			struct node* copy;
			
			if (otherNode->isLeaf) {
				copy = new struct node(otherNode->string);
			} else {
				copy = new struct node(otherNode->compareIndex, otherNode->string);
				copy->children = otherNode->children;
				
				nodes.push(copy);
			}
			
			element.second = copy;
		}
	}
}

template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>::string_trie(string_trie&& otherTrie) : string_trie() {
	swap(*this, otherTrie);
}


template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>::~string_trie() {
	// Avoid recursion as we may have very many levels
	std::stack<node*> nodes;
	
	if (root_) nodes.push(root_);
	
	while (!nodes.empty()) {
		node* node = nodes.top();
		nodes.pop();
		
		if (!node->isLeaf) {
			for (const auto& element : node->children) {
				nodes.push(element.second);
			}
		}
		
		delete node;
	}
}


template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>& string_trie<charT, reservedChar>::operator=(string_trie otherTrie) {
	swap(*this, otherTrie);
	
	return *this;
}


template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::insert(std::basic_string<charT> string) {
	normalizeString(string);
	
	
	std::vector<node*> nodes = search(string);
	
	if (!nodes.empty()) {  // if node found
		node* node = nodes.back();
		
		size_t compareIndex = indexOfFirstDifference(string, node->string);
		
		// If internal node key matched up with string, then we should compare the last index
		if (compareIndex == std::basic_string<charT>::npos && !node->isLeaf) compareIndex = string.length() - 1;
		
		if (compareIndex != std::basic_string<charT>::npos) {  // if the strings are not the same
			if (!node->isLeaf && compareIndex == node->string.size() - 1) {  // if node is where we should insert new leaf
				const charT& character = string[compareIndex];
				
				node->children[character] = new struct node(string);
			} else {  // else, create new internal node and insert it at appropriate position along path
				struct node* parentOfInternal;
				struct node* siblingOfInternal = siblingOfNewInternalNode(compareIndex, nodes, &parentOfInternal);
				
				std::basic_string<charT> path = string.substr(0, compareIndex).append(1, reservedChar);
				struct node* internal = new struct node(compareIndex, path);
				
				// Set existing node as child of new internal node
				const charT& existingNodeCharacter = siblingOfInternal->string[compareIndex];
				internal->children[existingNodeCharacter] = siblingOfInternal;
				
				// Set new leaf node as child of new internal node
				const charT& newNodeCharacter = string[compareIndex];
				internal->children[newNodeCharacter] = new struct node(string);
				
				// Set original parent of existing node as parent of internal node
				if (parentOfInternal) {
					const charT& character = siblingOfInternal->string[parentOfInternal->compareIndex];
					
					parentOfInternal->children[character] = internal;
				} else {
					root_ = internal;
				}
			}
		}
	} else {
		root_ = new struct node(string);
	}
}

template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::remove(std::basic_string<charT> string) {
	normalizeString(string);
	
	
	std::vector<node*> nodes = search(string);
	
	node* node = nullptr;
	struct node* parent = nullptr;
	struct node* parentOfParent = nullptr;
	
	if (!nodes.empty()) {
		node = nodes.back();
		nodes.pop_back();
	}
	
	if (!nodes.empty()) {
		parent = nodes.back();
		nodes.pop_back();
	}
	
	if (!nodes.empty()) {
		parentOfParent = nodes.back();
		nodes.pop_back();
	}
	
	
	if (node && node->isLeaf && node->string == string) {  // if we found a matching leaf node
		if (parent) {
			// Remove reference to removed node from parent
			const charT& character = node->string[parent->compareIndex];
			
			parent->children.erase(character);
			
			
			assert(parent->children.size() > 0);
			
			if (parent->children.size() == 1) {  // if parent only has one child remaining, then delete parent
				struct node* onlyChild = parent->children.begin()->second;
				
				if (parentOfParent) {  // if parent has a parent, remove the reference to the parent
					const charT& character = parent->string[parentOfParent->compareIndex];
					
					parentOfParent->children[character] = onlyChild;
					
					assert(parentOfParent->children.size() >= 2);
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
	
	
	std::vector<node*> nodes = search(string);
	
	return !nodes.empty() && nodes.back()->string == string;
}

template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::search(const std::basic_string<charT>& string) const -> std::vector<node*> {
	std::vector<node*> nodes;
	
	node* node = root_;
	
	while (node) {
		nodes.push_back(node);
		
		
		if (node->isLeaf) break;  // if at a leaf node
		
		
		if (node->compareIndex > string.length()) break;  // if the index we are looking at is past the end of the string
		
		const charT& character = string[node->compareIndex];
		
		
		auto iter = node->children.find(character);
		
		if (iter == node->children.end()) break;  // if character not found in children map
		
		
		node = iter->second;
	}
	
	return nodes;
}


template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::siblingOfNewInternalNode(size_t compareIndex, const std::vector<struct node*>& nodesInSearchPath, node** parentRef) const -> node* {
	node* parent = nullptr;
	node* node = nullptr;
	
	for (typename std::vector<struct node*>::const_reverse_iterator i = nodesInSearchPath.rbegin(); i != nodesInSearchPath.rend(); i++) {
		node = *i;
		parent = (i == nodesInSearchPath.rend() - 1) ? nullptr : *(i + 1);
		
		assert(!parent || parent->compareIndex != compareIndex);
		
		if (parent && parent->compareIndex < compareIndex) break;
	}
	
	if (parentRef) *parentRef = parent;
	return node;
}


template<typename charT, charT reservedChar>
size_t string_trie<charT, reservedChar>::indexOfFirstDifference(const std::basic_string<charT>& string1, const std::basic_string<charT>& string2) {
	size_t length1 = string1.length();
	size_t length2 = string2.length();
	
	for (size_t i = 0; i < length1 && i < length2; i++) {
		if (string1[i] != string2[i]) return i;  // if characters do not match, return index
	}
	
	return std::basic_string<charT>::npos;  // all characters matched (assumes every string is terminated by reservedChar)
}


template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::normalizeString(std::basic_string<charT>& string) {
	if (string.length() == 0) throw std::invalid_argument("String must not be empty.");  // string cannot be empty
	if (string.find_first_of(reservedChar) != std::basic_string<charT>::npos) throw std::invalid_argument("String must not contain specified reserved character.");  // string cannot contain reserved character
	
	string.append(1, reservedChar);  // append our end-of-string character
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

template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::verifyStructure() const {
	if (root_) {
		assert(root_->string.length() > 0);
		
		if (root_->isLeaf) {
			assert(root_->children.size() == 0);
		} else {
			assert(root_->string.length() == root_->compareIndex + 1);
			assert(root_->children.size() > 1);
			
			auto newPath = root_->string.substr(0, root_->string.length() - 1);
			
			for (const auto& element : root_->children) {
				verifyNode(*element.second, root_->compareIndex, newPath);
			}
		}
	}
}

template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::verifyNode(const node& node, size_t compareIndex, const std::basic_string<charT>& path) const {
	assert(node.string.length() > compareIndex);
	assert(node.string.rfind(path, compareIndex) != std::basic_string<charT>::npos);
	
	if (node.isLeaf) {
		assert(node.children.size() == 0);
	} else {
		assert(node.compareIndex > compareIndex);
		assert(node.string.length() == node.compareIndex + 1);
		assert(node.children.size() > 1);
		
		auto newPath = node.string.substr(0, node.string.length() - 1);
		
		for (const auto& element : node.children) {
			verifyNode(*element.second, node.compareIndex, newPath);
		}
	}
}

#endif