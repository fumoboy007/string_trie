#import <Foundation/Foundation.h>

#include <string>


@interface NSString (CPP)

- (std::basic_string<unichar>)cppString;

@end
