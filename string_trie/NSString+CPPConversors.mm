#import "NSString+CPPConversors.h"


@implementation NSString (CPPConversors)

- (std::basic_string<unichar>)cppString {
	NSUInteger length = [self length];
	
	if (length == 0) return std::basic_string<unichar>();
	
	unichar* buffer = new unichar[length];
	[self getCharacters:buffer range:NSMakeRange(0, length)];
	
	std::basic_string<unichar> cppString(buffer, length);
	
	delete[] buffer;
	buffer = NULL;
	
	return cppString;
}

@end
