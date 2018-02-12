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

#import <UIKit/UIKit.h>

#import "SimbleeDelegate.h"

@class Simblee;
@class SimbleeManager;

@interface ReconnectViewController : UIViewController<SimbleeDelegate>
{
    SimbleeManager *simbleeManager;
    
    __weak IBOutlet UILabel *label;
    __weak IBOutlet UIActivityIndicatorView *activityIndicator;
}

@property(strong, nonatomic) NSString *identifier;

- (void)showControls;
- (void)hideControls;

@end
