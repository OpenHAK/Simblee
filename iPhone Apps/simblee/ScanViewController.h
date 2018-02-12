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

typedef enum
{
    SignalStyleLine,
    SignalStyleGraph,
} SignalStyle;

@class ScanViewRow;

@class Simblee;
@class SimbleeManager;

@interface ScanViewController : UITableViewController<SimbleeDelegate, UITableViewDelegate>
{
    SimbleeManager *simbleeManager;
}

@property(strong, nonatomic) NSMutableArray *rows;
@property(assign, nonatomic) float updateInterval;

- (void)clearRows;
- (void)clearView;

- (void)deselectRow;

- (void)sortByRSSI;

- (UIView *)subview:(UIView*)view withTag:(NSInteger)tag;
- (void)updateRow:(ScanViewRow*)row signalStyle:(SignalStyle)signalStyle;
- (void)updateRows;

- (void)startScan;
- (void)stopScan;

@end
