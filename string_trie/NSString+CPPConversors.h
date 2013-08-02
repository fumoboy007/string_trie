#import <Foundation/Foundation.h>

#include <string>


@interface NSString (CPPConversors)

- (instancetype)initWithCPPString:(const std::basic_string<unichar>&)cppString;
+ (instancetype)stringWithCPPString:(const std::basic_string<unichar>&)cppString;

- (std::basic_string<unichar>)cppString;

@end
