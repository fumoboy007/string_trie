#import "NSString+CPP.h"


@implementation NSString (CPP)

- (std::basic_string<unichar>)cppString {
	NSUInteger length = [self length];
	
	if (length == 0) return std::basic_string<unichar>();
	
	unichar *buffer = new unichar[length];
	[self getCharacters:buffer range:NSMakeRange(0, length)];
	
	std::basic_string<unichar> cppString(buffer, length);
	
	free(buffer);
	buffer = NULL;
	
	return cppString;
}

@end
