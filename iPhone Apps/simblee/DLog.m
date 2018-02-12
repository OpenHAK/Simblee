/*
 * Copyright (c) 2015 RF Digital Corp. All Rights Reserved.
 *
 * The source code contained in this file and all intellectual property embodied in
 * or covering the source code is the property of RF Digital Corp. or its licensors.
 * Your right to use this source code and intellectual property is non-transferable,
 * non-sub licensable, revocable, and subject to terms and conditions of the
 * SIMBLEE SOFTWARE LICENSE AGREEMENT.
 * http://www.simblee.com/licenses/SimbleeSoftwareLicenseAgreement.txt
 *
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 *
 * This heading must NOT be removed from this file.
 */

#import "DLog.h"

NSMutableString *myLog;

void FLog(NSString *format, const char *function, NSString *args)
{
    NSString *line = [[NSString alloc] initWithFormat:format, function, args];

    if ([args hasPrefix:@"\r"]) {
        NSLog(@"%@", [args substringFromIndex:1]);
    } else {
        NSLog(@"%@", line);
    }
    
    // get file path
    NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains (NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *fileName = [documentsDirectory stringByAppendingPathComponent:@"output.txt"];
    
    // create file if not exist
    if(![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        [[NSFileManager defaultManager] createFileAtPath:fileName contents:nil attributes:nil];
    }

    NSDate *date = [NSDate date];
    NSDateFormatter *dateFormat = [[NSDateFormatter alloc] init];
    [dateFormat setDateFormat:@"yyyy-MM-dd HH:mm:ss.SSS"];
    [dateFormat stringFromDate:date];

    NSString *processName = [[NSProcessInfo processInfo] processName];
    int processId = [[NSProcessInfo processInfo] processIdentifier];
    
    NSString *line2 = [[NSString alloc] initWithFormat:@"%@ %@[%d] %@\r", [dateFormat stringFromDate:date], processName, processId, line];

    if ([args hasPrefix:@"\r"]) {
        line2 = [args substringFromIndex:1];
    }

    // append text to file
    NSFileHandle *file = [NSFileHandle fileHandleForUpdatingAtPath:fileName];
    [file seekToEndOfFile];
    [file writeData:[line2 dataUsingEncoding:NSUTF8StringEncoding]];
    [file closeFile];
}
