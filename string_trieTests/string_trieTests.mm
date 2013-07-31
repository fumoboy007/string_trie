#import <XCTest/XCTest.h>

#import "NSString+CPP.h"

#include <iostream>

#include "string_trie.hpp"


@interface string_trieTests : XCTestCase

@property (nonatomic) string_trie<unichar, '\n'>* trie;

@end


@implementation string_trieTests

- (void)setUp {
	[super setUp];
	
	self.trie = new string_trie<unichar, '\n'>();
}

- (void)tearDown {
	delete self.trie;
	self.trie = nullptr;
	
	[super tearDown];
}

- (void)testInvalidString {
	std::basic_string<unichar> string = [@"hello\nworld" cppString];
	
	try {
		self.trie->insert(string);
	} catch (const std::invalid_argument& exception) {
		try {
			self.trie->remove(string);
		} catch (const std::invalid_argument& exception) {
			try {
				self.trie->contains(string);
			} catch (const std::invalid_argument& exception) {
				string = [@"hello world" cppString];
				
				try {
					self.trie->insert(string);
					self.trie->remove(string);
					self.trie->contains(string);
				} catch (const std::invalid_argument& exception) {
					XCTFail(@"Inserting, removing, or searching for a valid string threw an exception.");
				}
				
				
				string = [@"" cppString];
				
				try {
					self.trie->insert(string);
				} catch (const std::invalid_argument& exception) {
					try {
						self.trie->remove(string);
					} catch (const std::invalid_argument& exception) {
						try {
							self.trie->contains(string);
						} catch (const std::invalid_argument& exception) {
							return;
						}
						
						XCTFail(@"Searching for an empty string does not throw an exception.");
					}
					
					XCTFail(@"Removing an empty string does not throw an exception.");
				}
				
				XCTFail(@"Inserting an empty string does not throw an exception.");
			}
			
			XCTFail(@"Searching for a string that contains the reserved character does not throw an exception.");
		}
		
		XCTFail(@"Removing a string that contains the reserved character does not throw an exception.");
	}
	
	XCTFail(@"Inserting a string that contains the reserved character does not throw an exception.");
}

- (void)testAll {
	self.trie->insert([@"hello world" cppString]);
	self.trie->printStructure();
	self.trie->verifyStructure();
	XCTAssert(self.trie->contains([@"hello world" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"hello world", @"hello world");
	XCTAssertFalse(self.trie->contains([@"hello worl" cppString]), @"contains(\"%@\") returned true when it should have been false.", @"hello worl");
	XCTAssertFalse(self.trie->contains([@"hello worldd" cppString]), @"contains(\"%@\") returned true when it should have been false.", @"hello worldd");
	
	self.trie->remove([@"hello world" cppString]);
	self.trie->printStructure();
	self.trie->verifyStructure();
	XCTAssertFalse(self.trie->contains([@"hello world" cppString]), @"contains(\"%@\") returned true even after \"%@\" was removed.", @"hello world", @"hello world");
	
	self.trie->insert([@"hello" cppString]);
	self.trie->insert([@"hello world" cppString]);
	self.trie->printStructure();
	self.trie->verifyStructure();
	XCTAssert(self.trie->contains([@"hello" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"hello", @"hello");
	XCTAssert(self.trie->contains([@"hello world" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"hello world", @"hello world");
	XCTAssertFalse(self.trie->contains([@"hello " cppString]), @"contains(\"%@\") returned true when it should have been false.", @"hello ");
	
	self.trie->insert([@"he said she said" cppString]);
	self.trie->printStructure();
	self.trie->verifyStructure();
	XCTAssert(self.trie->contains([@"he said she said" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"he said she said", @"he said she said");
	XCTAssert(self.trie->contains([@"hello" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"hello", @"hello");
	XCTAssert(self.trie->contains([@"hello world" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"hello world", @"hello world");
	
	self.trie->remove([@"hello " cppString]);
	self.trie->printStructure();
	self.trie->verifyStructure();
	XCTAssert(self.trie->contains([@"he said she said" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"he said she said", @"he said she said");
	XCTAssert(self.trie->contains([@"hello" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"hello", @"hello");
	XCTAssert(self.trie->contains([@"hello world" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"hello world", @"hello world");
	
	self.trie->remove([@"hello" cppString]);
	self.trie->printStructure();
	self.trie->verifyStructure();
	XCTAssertFalse(self.trie->contains([@"hello" cppString]), @"contains(\"%@\") returned true even after \"%@\" was removed.", @"hello", @"hello");
	XCTAssert(self.trie->contains([@"he said she said" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"he said she said", @"he said she said");
	XCTAssert(self.trie->contains([@"hello world" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"hello world", @"hello world");
	
	self.trie->insert([@"what's up" cppString]);
	self.trie->printStructure();
	self.trie->verifyStructure();
	XCTAssert(self.trie->contains([@"what's up" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"what's up", @"what's up");
	XCTAssert(self.trie->contains([@"he said she said" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"he said she said", @"he said she said");
	XCTAssert(self.trie->contains([@"hello world" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"hello world", @"hello world");
	
	self.trie->remove([@"what's up" cppString]);
	self.trie->printStructure();
	self.trie->verifyStructure();
	XCTAssertFalse(self.trie->contains([@"what's up" cppString]), @"contains(\"%@\") returned true even after \"%@\" was removed.", @"what's up", @"what's up");
	XCTAssert(self.trie->contains([@"he said she said" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"he said she said", @"he said she said");
	XCTAssert(self.trie->contains([@"hello world" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"hello world", @"hello world");
	
	self.trie->remove([@"hello world" cppString]);
	self.trie->printStructure();
	self.trie->verifyStructure();
	XCTAssertFalse(self.trie->contains([@"hello world" cppString]), @"contains(\"%@\") returned true even after \"%@\" was removed.", @"hello world", @"hello world");
	XCTAssert(self.trie->contains([@"he said she said" cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", @"he said she said", @"he said she said");
	
	self.trie->remove([@"he said she said" cppString]);
	self.trie->printStructure();
	self.trie->verifyStructure();
	XCTAssertFalse(self.trie->contains([@"he said she said" cppString]), @"contains(\"%@\") returned true even after \"%@\" was removed.", @"he said she said", @"he said she said");
}

- (void)testRealScenario {
	NSString *wordList = [NSString stringWithContentsOfFile:@"/Users/darrenmo/Developer/Open Source/string_trie/string_trieTests/wordList.txt" encoding:NSUTF8StringEncoding error:NULL];
	
	XCTAssert([wordList length] > 0, @"Word list not being loaded.");
	
	NSMutableSet *words = [NSMutableSet set];
	
	[wordList enumerateLinesUsingBlock:^(NSString *word, BOOL *stop) {
		self.trie->insert([word cppString]);
		
		[words addObject:word];
	}];
	
	self.trie->verifyStructure();
	
	for (NSString *word in words) {
		XCTAssert(self.trie->contains([word cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", word, word);
	};
	
	
	NSMutableArray *remainingWords = [NSMutableArray array];
	
	for (NSString *word in words) {
		u_int32_t variate = arc4random_uniform(4);
		
		if (variate == 0) {
			self.trie->remove([word cppString]);
		} else {
			[remainingWords addObject:word];
		}
	}
	
	self.trie->verifyStructure();
	
	for (NSString *word in remainingWords) {
		XCTAssert(self.trie->contains([word cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", word, word);
	}
}

- (void)testCopyAssignmentMove {
	NSString *wordList = [NSString stringWithContentsOfFile:@"/Users/darrenmo/Developer/Open Source/string_trie/string_trieTests/wordList.txt" encoding:NSUTF8StringEncoding error:NULL];
	
	XCTAssert([wordList length] > 0, @"Word list not being loaded.");
	
	NSMutableSet *words = [NSMutableSet set];
	
	[wordList enumerateLinesUsingBlock:^(NSString *word, BOOL *stop) {
		self.trie->insert([word cppString]);
		
		[words addObject:word];
	}];
	
	
	auto copy = new string_trie<unichar, '\n'>(*self.trie);
	
	delete self.trie;
	self.trie = nullptr;
	
	for (NSString *word in words) {
		XCTAssert(copy->contains([word cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", word, word);
	}
	
	
	self.trie = new string_trie<unichar, '\n'>();
	*self.trie = *copy;
	
	delete copy;
	copy = nullptr;
	
	for (NSString *word in words) {
		XCTAssert(self.trie->contains([word cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", word, word);
	}
	
	
	copy = new string_trie<unichar, '\n'>(string_trie<unichar, '\n'>(*self.trie));
	
	delete self.trie;
	self.trie = copy;
	
	for (NSString *word in words) {
		XCTAssert(self.trie->contains([word cppString]), @"contains(\"%@\") returned false even after \"%@\" was inserted.", word, word);
	}
}

- (void)testNSToCPPStringConversion {
	NSString *string = @"hello world";
	
	{
		auto cppString = [string cppString];
		
		XCTAssert(cppString.length() == [string length], @"CPP string is not the same length as Cocoa string.");
		
		for (int i = 0; i < cppString.length(); i++) {
			XCTAssert(cppString[i] == [string characterAtIndex:i], @"Characters at index %d do not match.", i);
		}
	}
	
	{
		auto cppString = [string cppString];
		
		XCTAssert(cppString.length() == [string length], @"CPP string is not the same length as Cocoa string.");
		
		for (int i = 0; i < cppString.length(); i++) {
			XCTAssert(cppString[i] == [string characterAtIndex:i], @"Characters at index %d do not match.", i);
		}
	}
}

@end
