#import <Foundation/Foundation.h>

#include <string>


@interface NSString (CPPConversors)

- (std::basic_string<unichar>)cppString;

@end
