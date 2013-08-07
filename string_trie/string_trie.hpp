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


#ifndef _STRING_TRIE_H_
#define _STRING_TRIE_H_

#include <string>
#include <vector>
#include <iterator>


/* Template declarations */

template<typename charT, charT reservedChar> class string_trie_const_iterator;
template<typename charT, charT reservedChar> class string_trie_insert_iterator;

template<typename charT, charT reservedChar>
bool operator==(const string_trie_const_iterator<charT, reservedChar>& iterator1, const string_trie_const_iterator<charT, reservedChar>& iterator2);

template<typename charT, charT reservedChar>
bool operator!=(const string_trie_const_iterator<charT, reservedChar>& iterator1, const string_trie_const_iterator<charT, reservedChar>& iterator2);


/* String trie class */

template<typename charT, charT reservedChar>
class string_trie {
public:
	typedef string_trie_const_iterator<charT, reservedChar> const_iterator;
	typedef string_trie_insert_iterator<charT, reservedChar> insert_iterator;
	
	
	string_trie();
	string_trie(const string_trie& otherTrie);
	string_trie(string_trie&& otherTrie);
	
	~string_trie();
	
	string_trie& operator=(string_trie otherTrie);
	string_trie& operator=(string_trie&& otherTrie);
	
	
	const_iterator cbegin() const;
	const_iterator cend() const;
	insert_iterator inserter();
	
	
	void clear();
	bool empty() const;
	size_t size() const;
	
	
	/* The following throw std::invalid_argument if string contains reservedChar or is empty. */
	
	void insert(std::basic_string<charT> string);
	void remove(std::basic_string<charT> string);
	
	bool contains(std::basic_string<charT> string) const;
	
	const_iterator predecessor(std::basic_string<charT> string) const;
	const_iterator successor(std::basic_string<charT> string) const;
	
	std::pair<const_iterator, const_iterator> prefixedStrings(std::basic_string<charT> prefix) const;
	
	
#ifdef DEBUG
	void printStructure() const;
	
	void verifyStructure() const;
#endif
	
private:
	struct node;
	
	
	node* root_;
	
	size_t size_;
	
	
	std::vector<node*> search(const std::basic_string<charT>& string) const;
	
	const node* leftmostDescendant(const node& root) const;
	const node* rightmostDescendant(const node& root) const;
	
	node* siblingOfNewInternalNode(typename std::basic_string<charT>::size_type compareIndex, const std::vector<node*>& nodesInSearchPath, node** parentRef) const;
	
	static typename std::basic_string<charT>::size_type indexOfFirstDifference(const std::basic_string<charT>& string1, const std::basic_string<charT>& string2);
	
	static void normalizeString(std::basic_string<charT>& string);
	
	static void swap(string_trie& trie1, string_trie& trie2);
	
	
#ifdef DEBUG
	void printNode(const node& node) const;
	
	void verifyNode(const node& node, typename std::basic_string<charT>::size_type compareIndex, const std::basic_string<charT>& path) const;
#endif
};


/* String trie iterators */

template<typename charT, charT reservedChar>
class string_trie_const_iterator : public std::iterator<std::forward_iterator_tag, std::basic_string<charT>> {
	friend bool operator==<>(const typename string_trie<charT, reservedChar>::const_iterator& iterator1, const typename string_trie<charT, reservedChar>::const_iterator& iterator2);
	
	friend bool operator!=<>(const typename string_trie<charT, reservedChar>::const_iterator& iterator1, const typename string_trie<charT, reservedChar>::const_iterator& iterator2);
	
public:
	string_trie_const_iterator();
	string_trie_const_iterator(const string_trie<charT, reservedChar>& trie);
	string_trie_const_iterator(const string_trie<charT, reservedChar>& trie, const std::basic_string<charT>& string);
	
	std::basic_string<charT> operator*();
	string_trie_const_iterator& operator++();
	string_trie_const_iterator operator++(int i);
	
private:
	const string_trie<charT, reservedChar>* trie_;
	std::basic_string<charT> string_;
	
	
	bool pastEnd() const;
};


template<typename charT, charT reservedChar>
class string_trie_insert_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
public:
	string_trie_insert_iterator(string_trie<charT, reservedChar>& trie);
	
	string_trie_insert_iterator& operator=(const std::basic_string<charT>& string);
	string_trie_insert_iterator& operator=(std::basic_string<charT>&& string);
	
	string_trie_insert_iterator& operator*();
	string_trie_insert_iterator& operator++();
	string_trie_insert_iterator operator++(int i);
	
private:
	string_trie<charT, reservedChar>* trie_;
};


#include "string_trie.tpp"

#endif