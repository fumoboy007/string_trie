string_trie
===========
What it does
------------
`string_trie` is a C++ implementation of a Patricia trie that only holds keys (of type `basic_string`). Also included is an Objective-C category called `NSString+CPPConversors` that converts between `NSString` and `basic_string<unichar>`.

There is a test suite written in Objective-C++.

How to use it
-------------
Include the files `string_trie.hpp`/`string_trie.tpp` into your project. `string_trie` takes two template parameters, the first of which is the character type (e.g. `char` in C/C++ or `unichar` in Cocoa). The second template parameter is a character that you are guaranteeing will not be used in any of the strings you pass in (e.g. `'\n'`). (This reserved character is appended to strings so that we don't need an invariant of prefix-free strings.)

To use `NSString+CPPConversors`, include the necessary files into your project and use the `- [NSString cppString]` and `+ [NSString stringWithCPPString:]` methods.

License
-------
Licensed under GPLv3.