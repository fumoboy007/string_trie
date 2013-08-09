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


#import "NSString+CPPConversors.h"


@implementation NSString (CPPConversors)

- (instancetype)initWithCPPString:(const std::basic_string<unichar>&)cppString {
	return [self initWithCharacters:cppString.c_str() length:cppString.length()];
}

+ (instancetype)stringWithCPPString:(const std::basic_string<unichar>&)cppString {
	return [[[self class] alloc] initWithCPPString:cppString];
}


- (std::basic_string<unichar>)cppString {
	NSUInteger length = [self length];
	
	if (length == 0) return std::basic_string<unichar>();
	
	unichar buffer[length];
	[self getCharacters:buffer range:NSMakeRange(0, length)];
	
	std::basic_string<unichar> cppString(buffer, length);
	
	return cppString;
}

@end
