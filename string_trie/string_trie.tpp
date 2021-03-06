// string_trie: A C++ Patricia trie implementation for strings.
// Copyright (C) 2013 Darren Mo
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include <map>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <stack>
#include <utility>


/* string_trie_const_iterator */

template<typename charT, charT reservedChar>
string_trie_const_iterator<charT, reservedChar>::string_trie_const_iterator() : trie_(nullptr), string_() {
}

template<typename charT, charT reservedChar>
string_trie_const_iterator<charT, reservedChar>::string_trie_const_iterator(const string_trie<charT, reservedChar>& trie) : string_trie_const_iterator(trie, std::basic_string<charT>()) {
}

template<typename charT, charT reservedChar>
string_trie_const_iterator<charT, reservedChar>::string_trie_const_iterator(const string_trie<charT, reservedChar>& trie, const std::basic_string<charT>& string) : trie_(&trie), string_(string) {
	typename std::basic_string<charT>::size_type position = string_.rfind(reservedChar);
	
	if (position != std::basic_string<charT>::npos) string_.erase(position, 1);
	
	assert(string_.rfind(reservedChar) == std::basic_string<charT>::npos);
}


template<typename charT, charT reservedChar>
std::basic_string<charT> string_trie_const_iterator<charT, reservedChar>::operator*() {
	return string_;
}

template<typename charT, charT reservedChar>
string_trie_const_iterator<charT, reservedChar>& string_trie_const_iterator<charT, reservedChar>::operator++() {
	if (trie_ && !pastEnd()) *this = trie_->successor(string_);
	
	return *this;
}

template<typename charT, charT reservedChar>
string_trie_const_iterator<charT, reservedChar> string_trie_const_iterator<charT, reservedChar>::operator++(int i) {
	string_trie_const_iterator tmp = *this;
	
	if (trie_ && !pastEnd()) *this = trie_->successor(string_);
	
	return tmp;
}


template<typename charT, charT reservedChar>
bool string_trie_const_iterator<charT, reservedChar>::pastEnd() const {
	return string_.length() == 0;
}


template<typename charT, charT reservedChar>
bool operator==(const string_trie_const_iterator<charT, reservedChar>& iterator1, const string_trie_const_iterator<charT, reservedChar>& iterator2) {
	return iterator1.trie_ == iterator2.trie_ && iterator1.string_ == iterator2.string_;
}

template<typename charT, charT reservedChar>
bool operator!=(const string_trie_const_iterator<charT, reservedChar>& iterator1, const string_trie_const_iterator<charT, reservedChar>& iterator2) {
	return !(iterator1 == iterator2);
}


/* string_trie_insert_iterator */

template<typename charT, charT reservedChar>
string_trie_insert_iterator<charT, reservedChar>::string_trie_insert_iterator(string_trie<charT, reservedChar>& trie) : trie_(&trie) {
}


template<typename charT, charT reservedChar>
string_trie_insert_iterator<charT, reservedChar>& string_trie_insert_iterator<charT, reservedChar>::operator=(const std::basic_string<charT>& string) {
	trie_->insert(string);
	
	return *this;
}

template<typename charT, charT reservedChar>
string_trie_insert_iterator<charT, reservedChar>& string_trie_insert_iterator<charT, reservedChar>::operator=(std::basic_string<charT>&& string) {
	trie_->insert(string);
	
	return *this;
}


template<typename charT, charT reservedChar>
string_trie_insert_iterator<charT, reservedChar>& string_trie_insert_iterator<charT, reservedChar>::operator*() {
	return *this;
}

template<typename charT, charT reservedChar>
string_trie_insert_iterator<charT, reservedChar>& string_trie_insert_iterator<charT, reservedChar>::operator++() {
	return *this;
}

template<typename charT, charT reservedChar>
string_trie_insert_iterator<charT, reservedChar> string_trie_insert_iterator<charT, reservedChar>::operator++(int i) {
	return *this;
}


/* string_trie */

template<typename charT, charT reservedChar>
struct string_trie<charT, reservedChar>::node {
	typedef std::map<charT, node*> child_map;
	
	
	string_trie* trie_;
	
	std::basic_string<charT> string;
	
	bool isLeaf;
	typename std::basic_string<charT>::size_type compareIndex;
	child_map children;
	
	
	// Internal node constructor
	node(typename std::basic_string<charT>::size_type compareIndex, const std::basic_string<charT>& path) : trie_(nullptr), string(path), isLeaf(false), compareIndex(compareIndex), children() {}
	
	// Leaf node constructor
	node(string_trie& trie, const std::basic_string<charT>& string) : trie_(&trie), string(string), isLeaf(true), compareIndex(0), children() {
		trie_->size_++;
	}
	
	~node() {
		if (isLeaf) {
			assert(trie_);
			
			trie_->size_--;
		}
	}
	
private:
	node(const node& otherNode);
	node(node&& otherNode);
	
	node& operator=(node otherNode);
};


template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>::string_trie() : root_(nullptr), size_(0) {
}

template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>::string_trie(const string_trie& otherTrie) : string_trie() {
	// Avoid recursion as we may have very many levels
	std::stack<node*> nodes;
	
	node* otherRoot = otherTrie.root_;
	
	if (otherRoot) {
		if (otherRoot->isLeaf) {
			root_ = new node(*this, otherRoot->string);
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
				copy = new struct node(*this, otherNode->string);
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
	clear();
}


template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>& string_trie<charT, reservedChar>::operator=(string_trie otherTrie) {
	swap(*this, otherTrie);
	
	return *this;
}

template<typename charT, charT reservedChar>
string_trie<charT, reservedChar>& string_trie<charT, reservedChar>::operator=(string_trie&& otherTrie) {
	swap(*this, otherTrie);
	
	return *this;
}


template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::cbegin() const -> const_iterator {
	if (root_) {
		const node* node = leftmostDescendant(*root_);
		
		// Remove last character (reserved character)
		auto string = node->string.substr(0, node->string.length() - 1);
		
		return const_iterator(*this, string);
	} else {
		return cend();
	}
}

template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::cend() const -> const_iterator {
	return const_iterator(*this);
}

template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::inserter() -> insert_iterator {
	return insert_iterator(*this);
}


template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::clear() {
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
bool string_trie<charT, reservedChar>::empty() const {
	return size_ == 0;
}

template<typename charT, charT reservedChar>
size_t string_trie<charT, reservedChar>::size() const {
	return size_;
}


template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::insert(std::basic_string<charT> string) {
	normalizeString(string);
	
	
	std::vector<node*> nodes = searchPath(string);
	
	if (!nodes.empty()) {  // if node found
		node* node = nodes.back();
		
		typename std::basic_string<charT>::size_type compareIndex = indexOfFirstDifference(string, node->string);
		
		// If internal node key matched up with string, then we should compare the last index
		if (compareIndex == std::basic_string<charT>::npos && !node->isLeaf) compareIndex = string.length() - 1;
		
		if (compareIndex != std::basic_string<charT>::npos) {  // if the strings are not the same
			if (!node->isLeaf && compareIndex == node->string.size() - 1) {  // if node is where we should insert new leaf
				const charT& character = string[compareIndex];
				
				node->children[character] = new struct node(*this, string);
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
				internal->children[newNodeCharacter] = new struct node(*this, string);
				
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
		root_ = new struct node(*this, string);
	}
}

template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::remove(std::basic_string<charT> string) {
	normalizeString(string);
	
	
	std::vector<node*> nodes = searchPath(string);
	
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
	
	
	node* node = search(string);
	
	return node && node->string == string;
}


template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::predecessor(std::basic_string<charT> string) const -> const_iterator {
	normalizeString(string);
	
	
	std::vector<node*> nodes = searchPath(string);
	
	if (nodes.empty()) return const_iterator(*this);
	
	
	node* last = nodes.back();
	nodes.pop_back();
	
	// If node's string is less than our string, then its rightmost descendant is what we want
	if (string.compare(last->string) > 0) return const_iterator(*this, rightmostDescendant(*last)->string);
	
	
	node* previousNode = last;
	node* desiredNode = nullptr;
	
	for (auto i = nodes.rbegin(); i != nodes.rend(); i++) {
		node* node = *i;
		assert(!node->isLeaf);
		
		
		// Get iterator pointing to edge going down to previous node
		const charT& character = previousNode->string[node->compareIndex];
		auto iter = node->children.find(character);
		
		assert(iter != node->children.end());
		
		
		// Get the preceding node if present
		if (iter != node->children.begin()) {
			iter--;
			
			if (string.compare(iter->second->string) > 0) {
				desiredNode = iter->second;
				
				break;
			}
		}
		
		previousNode = node;
	}
	
	if (!desiredNode) return const_iterator(*this);
	
	return const_iterator(*this, rightmostDescendant(*desiredNode)->string);
}

template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::successor(std::basic_string<charT> string) const -> const_iterator {
	normalizeString(string);
	
	
	std::vector<node*> nodes = searchPath(string);
	
	if (nodes.empty()) return const_iterator(*this);
	
	
	node* last = nodes.back();
	nodes.pop_back();
	
	// If node's string is greater than our string, then its leftmost descendant is what we want
	if (string.compare(last->string) < 0) return const_iterator(*this, leftmostDescendant(*last)->string);
	
	
	node* previousNode = last;
	node* desiredNode = nullptr;
	
	for (auto i = nodes.rbegin(); i != nodes.rend(); i++) {
		node* node = *i;
		assert(!node->isLeaf);
		
		
		// Get iterator pointing to edge following edge going down to previous node
		const charT& character = previousNode->string[node->compareIndex];
		auto iter = node->children.upper_bound(character);
		
		
		if (iter != node->children.end() && string.compare(iter->second->string) < 0) {
			desiredNode = iter->second;
			
			break;
		}
		
		previousNode = node;
	}
	
	if (!desiredNode) return const_iterator(*this);
	
	return const_iterator(*this, leftmostDescendant(*desiredNode)->string);
}


template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::prefixedStrings(std::basic_string<charT> prefix) const -> std::pair<const_iterator, const_iterator> {
	normalizeString(prefix);
	
	
	node* node = search(prefix);
	
	if (!node) return std::pair<const_iterator, const_iterator>(cend(), cend());
	
	
	// If found node has the specified prefix
	if (node->string.rfind(prefix, prefix.length() - 2) == std::basic_string<charT>::npos) return std::pair<const_iterator, const_iterator>(cend(), cend());
	
	
	const struct node* leftmost = leftmostDescendant(*node);
	const struct node* rightmost = rightmostDescendant(*node);
	
	
	const_iterator begin = const_iterator(*this, leftmost->string);
	
	
	// Get node after rightmost node
	std::basic_string<charT> lastString = rightmost->string.substr(0, rightmost->string.length() - 1);
	
	const_iterator end = successor(lastString);
	
	
	return std::pair<const_iterator, const_iterator>(begin, end);
}


template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::search(const std::basic_string<charT>& string) const -> node* {
	typename std::basic_string<charT>::size_type length = string.length();
	
	
	node* node = root_;
	
	while (node) {
		if (node->isLeaf) break;  // if at a leaf node
		
		
		typename std::basic_string<charT>::size_type compareIndex = node->compareIndex;
		
		if (compareIndex > length) break;  // if the index we are looking at is past the end of the string
		
		const charT& character = string[compareIndex];
		
		
		auto iter = node->children.find(character);
		
		if (iter == node->children.end()) break;  // if character not found in children map
		
		
		node = iter->second;
	}
	
	return node;
}


template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::searchPath(const std::basic_string<charT>& string) const -> std::vector<node*> {
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
auto string_trie<charT, reservedChar>::leftmostDescendant(const node& root) const -> const node* {
	const node* node = &root;
	
	while (!node->isLeaf) {
		node = node->children.begin()->second;
	}
	
	return node;
}

template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::rightmostDescendant(const node& root) const -> const node* {
	const node* node = &root;
	
	while (!node->isLeaf) {
		node = node->children.rbegin()->second;
	}
	
	return node;
}


template<typename charT, charT reservedChar>
auto string_trie<charT, reservedChar>::siblingOfNewInternalNode(typename std::basic_string<charT>::size_type compareIndex, const std::vector<struct node*>& nodesInSearchPath, node** parentRef) const -> node* {
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
typename std::basic_string<charT>::size_type string_trie<charT, reservedChar>::indexOfFirstDifference(const std::basic_string<charT>& string1, const std::basic_string<charT>& string2) {
	typename std::basic_string<charT>::size_type length1 = string1.length();
	typename std::basic_string<charT>::size_type length2 = string2.length();
	
	for (typename std::basic_string<charT>::size_type i = 0; i < length1 && i < length2; i++) {
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
	swap(trie1.size_, trie2.size_);
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
	std::cerr << "begin structure" << std::endl;
	
	if (root_) printNode(*root_);
	
	std::cerr << "end structure" << std::endl << std::endl;
}

template<typename charT, charT reservedChar>
void string_trie<charT, reservedChar>::printNode(const node& node) const {
	if (node.isLeaf) {
		std::cerr << "Leaf Node" << std::endl << "---------" << std::endl;
		std::cerr << "String: " << node.string << std::endl << std::endl;
	} else {
		std::cerr << "Internal Node" << std::endl << "-------------" << std::endl;
		std::cerr << "Compare index: " << node.compareIndex << std::endl;
		std::cerr << "Path: " << node.string << std::endl;
		std::cerr << "Children:";
		
		for (const auto& element : node.children) {
			std::cerr << " (";
			
			struct node* child = element.second;
			if (child->isLeaf) {
				std::cerr << child->string;
			} else {
				std::cerr << child->compareIndex;
			}
			
			std::cerr << ")";
		}
		
		std::cerr << std::endl << std::endl;
		
		
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
void string_trie<charT, reservedChar>::verifyNode(const node& node, typename std::basic_string<charT>::size_type compareIndex, const std::basic_string<charT>& path) const {
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