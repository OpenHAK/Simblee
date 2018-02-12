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

#import <QuartzCore/QuartzCore.h>

#include "DLog.h"

#import "ScanViewRow.h"
#import "CustomCellBackground.h"

#import "AppDelegate.h"

#import "Simblee.h"
#import "SimbleeManager.h"

#import "ScanViewController.h"

@interface ScanViewController()
{
    AppDelegate *appDelegate;
    
    NSTimer *updateTimer;
    
    bool clearTableView;
    bool editingRow;
    
    NSArray *signals;
}
@end

@implementation ScanViewController

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        UINavigationItem *navItem = [self navigationItem];
        [navItem setTitle:@"Found Simblees"];
        
        UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithTitle:@"Sort"
                                                                      style:UIBarButtonItemStylePlain
                                                                     target:self
                                                                     action:@selector(sortByRSSI)];
        [navItem setRightBarButtonItem:rightItem];

        appDelegate = [[UIApplication sharedApplication] delegate];
        
        simbleeManager = [SimbleeManager sharedSimbleeManager];
        
        _rows = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
 
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;

    int numberOfLines = 3;
    self.tableView.rowHeight = (44.0 + (numberOfLines - 1) * 19.0);
    
    /*
    UIColor *start = [UIColor colorWithRed:58/255.0 green:108/255.0 blue:183/255.0 alpha:0.15];
    UIColor *stop = [UIColor colorWithRed:58/255.0 green:108/255.0 blue:183/255.0 alpha:0.45];
    
    CAGradientLayer *gradient = [CAGradientLayer layer];
    // gradient.frame = [self.view bounds];
    gradient.frame = CGRectMake(0.0, 0.0, 1024.0, 1024.0);
    gradient.colors = @[(id)start.CGColor, (id)stop.CGColor];
    [self.tableView.layer insertSublayer:gradient atIndex:0];
    */
    
    signals = @[
        [UIImage imageNamed:@"signal0.png"],
        [UIImage imageNamed:@"signal1.png"],
        [UIImage imageNamed:@"signal2.png"],
        [UIImage imageNamed:@"signal3.png"]
    ];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (clearTableView)
        return 0;
    return _rows.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    ScanViewRow *row = [_rows objectAtIndex:[indexPath row]];

    if (row.hide) {
        return 0.0;
    }

    int numberOfLines = 3;
    return (44.0 + (numberOfLines - 1) * 19.0);
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    ScanViewRow *row = [_rows objectAtIndex:[indexPath row]];
    return row.cell;
}

// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    ScanViewRow *row = [_rows objectAtIndex:[indexPath row]];

    // Return NO if you do not want the specified item to be editable.
    return (row.simblee.outOfRange ? YES : NO);
}

- (void)tableView:(UITableView *)tableView willBeginEditingRowAtIndexPath:(NSIndexPath *)indexPath
{
    editingRow = true;
}

- (void)tableView:(UITableView *)tableView didEndEditingRowAtIndexPath:(NSIndexPath *)indexPath
{
    editingRow = false;
}

// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        ScanViewRow *row = [_rows objectAtIndex:[indexPath row]];
        row.hide = true;
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }
    /*
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }
    */
}

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
{
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    ScanViewRow *row = [_rows objectAtIndex:[indexPath row]];
    if (row.simblee.outOfRange) {
        [self.tableView deselectRowAtIndexPath:indexPath animated:YES];
    } else {
        [appDelegate scanViewController:self connectSimblee:row.simblee];
    }
}

#pragma mark - SimbleeManagerDelegate methods

- (void)didDiscoverSimblee:(Simblee *)simblee
{
    [appDelegate scanViewController:self didDiscoverSimblee:simblee];
}

- (void)didConnectSimblee:(Simblee *)simblee
{
    [appDelegate scanViewController:self didConnectSimblee:simblee];
}

- (void)didNotConnectSimblee:(Simblee *)simblee
{
    [appDelegate scanViewController:self didNotConnectSimblee:simblee];
}

- (void)didLooseConnectSimblee:(Simblee *)simblee
{
    [appDelegate scanViewController:self didLooseConnectSimblee:simblee];
}

- (void)didDisconnectSimblee:(Simblee *)simblee
{
    [appDelegate scanViewController:self didDisconnectSimblee:simblee];
}

#pragma mark - Scan start / stop

- (void)clearRows
{
    [_rows removeAllObjects];
}

- (void)clearView
{
    clearTableView = true;
    [self.tableView reloadData];
    clearTableView = false;
}

- (void)deselectRow
{
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:YES];
}

- (void)sortByRSSI
{
    [_rows sortWithOptions:NSSortStable usingComparator:^NSComparisonResult(ScanViewRow *data1, ScanViewRow *data2) {
        if (data1.simblee.RSSI.intValue > data2.simblee.RSSI.intValue)
            return NSOrderedAscending;
        else if (data1.simblee.RSSI.intValue < data2.simblee.RSSI.intValue)
            return NSOrderedDescending;
        else
            return NSOrderedSame;
    }];
}

- (UIView *)subview:(UIView*)view withTag:(NSInteger)tag
{
    for (UIView *subview in view.subviews) {
        UIView *result = [subview viewWithTag:tag];
        if (result) {
            return result;
        }
    }
    return nil;
}

- (void)updateRow:(ScanViewRow*)row signalStyle:(SignalStyle)signalStyle
{
    UITableViewCell *cell = row.cell;
    
    if (! cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:nil];
        
        cell.textLabel.backgroundColor = [UIColor clearColor];
        cell.detailTextLabel.backgroundColor = [UIColor clearColor];
        
        [cell setIndentationLevel:3];
        
        if (signalStyle == SignalStyleLine) {
            row.rssiProgress = [[UIProgressView alloc]initWithProgressViewStyle:UIProgressViewStyleBar];
            row.rssiProgress.frame = CGRectMake(0.0, 0.0, 150.0, 0.0);
            row.rssiProgress.center = CGPointMake(225,18);
            row.rssiProgress.progressViewStyle = UIProgressViewStyleDefault;
            row.rssiProgress.progressTintColor = [UIColor colorWithRed:0.0 green:0.5 blue:0.0 alpha:1.0];
            row.rssiProgress.tag = 1;
            [cell addSubview:row.rssiProgress];

            if (row.rssiImage) {
                [[self subview:cell withTag:2] removeFromSuperview];
                row.rssiImage = nil;
            }
            
            if (row.rssiLabel) {
                [[self subview:cell withTag:3] removeFromSuperview];
                row.rssiLabel = nil;
                
            }
        } else if (signalStyle == SignalStyleGraph) {
            row.rssiImage = [[UIImageView alloc] initWithFrame:CGRectMake(12, 20, 25, 30)];
            row.rssiImage.tag = 2;
            [cell addSubview:row.rssiImage];
            
            row.rssiLabel = [[UILabel alloc] initWithFrame:CGRectMake(14, 55, 25, 10)];
            [row.rssiLabel setFont:[UIFont boldSystemFontOfSize:12]];
            row.rssiLabel.tag = 3;
            [cell addSubview:row.rssiLabel];

            if (row.rssiProgress) {
                [[self subview:cell withTag:1] removeFromSuperview];
                row.rssiProgress = nil;
            }
        }
        
        row.cell = cell;
    }
    
    Simblee *simblee = row.simblee;

    if (row.colorChanged) {
        // new ccb must be allocated for cell.background to change
        CustomCellBackground *ccb = [[CustomCellBackground alloc] init];
        ccb.startColor = row.startColor;
        ccb.stopColor = row.stopColor;
        cell.backgroundView = ccb;
        row.colorChanged = false;
    }
    
    NSString *text = [[NSString alloc] initWithFormat:@"%@", simblee.name];
    
    NSString *uuid = simblee.identifier;
    
    NSString *advertising = @"";
    if (simblee.advertisementData) {
        advertising = [[NSString alloc] initWithData:simblee.advertisementData encoding:NSUTF8StringEncoding];
    }

    int rssi = simblee.RSSI.intValue;
    
    NSMutableString *detail = [[NSMutableString alloc] init];
    if (signalStyle == SignalStyleLine) {
        [detail appendFormat:@"RSSI: %d dBm", rssi];
        while ([detail length] < 20)
            [detail appendString:@" "];
        [detail appendFormat:@"TX: %c%d dBm", (simblee.txPower.intValue < 0 ? '-' : '+'), simblee.txPower.intValue];
        while ([detail length] < 35)
            [detail appendString:@" "];
        [detail appendFormat:@"Pkts: %ld\n", (long)simblee.advertisementPackets];
        [detail appendFormat:@"Advertising: %@\n", advertising];
        [detail appendFormat:@"%@", uuid];
    } else if (signalStyle == SignalStyleGraph) {
        [detail appendFormat:@"TX: %c%d dBm", (simblee.txPower.intValue < 0 ? '-' : '+'), simblee.txPower.intValue];
        while ([detail length] < 15)
            [detail appendString:@" "];
        [detail appendFormat:@"Pkts: %ld\n", (long)simblee.advertisementPackets];
        [detail appendFormat:@"Advertising: %@\n", advertising];
        [detail appendFormat:@"%@", uuid];
    }
    
    cell.textLabel.text = text;
    cell.detailTextLabel.text = detail;
    cell.detailTextLabel.numberOfLines = 4;
    
    // only interested in graphing the rssi range between -30 and -90
    int min = -30;
    int max = -90;
    
    if (rssi > min) {
        rssi = min;
    }
    if (rssi < max) {
        rssi = max;
    }

    if (signalStyle == SignalStyleLine) {
        float ratio = (float)(rssi - min) / (max - min);
        row.rssiProgress.progress = 1.0 - ratio;
        
        cell.detailTextLabel.hidden = row.hide;
        row.rssiProgress.hidden = row.hide;
    } else if (signalStyle == SignalStyleGraph) {
        if (rssi > -50) {
            [row.rssiImage setImage:[signals objectAtIndex:3]];
        } else if (rssi > -70) {
            [row.rssiImage setImage:[signals objectAtIndex:2]];
        } else if (rssi > -90) {
            [row.rssiImage setImage:[signals objectAtIndex:1]];
        } else {
            [row.rssiImage setImage:[signals objectAtIndex:0]];
        }
        
        [row.rssiLabel setText:[NSString stringWithFormat:@"%d", rssi]];

        cell.detailTextLabel.hidden = row.hide;
        row.rssiImage.hidden = row.hide;
        row.rssiLabel.hidden = row.hide;
    }
}

- (void)updateRows
{
    NSArray *simblees = [simbleeManager simblees];
    
    for (NSUInteger i = _rows.count; i < simblees.count; i++) {
        ScanViewRow *data = [[ScanViewRow alloc] init];
        
        data.simblee = [simblees objectAtIndex:i];
        data.hide = false;
        
        [_rows addObject:data];
    }
    
    [appDelegate scanViewController:self updateRows:_rows];
}

- (void)updateTick:(NSTimer *)timer
{
    // dont update the ui if editing a row
    if (editingRow) {
        return;
    }

    // update rows
    [self updateRows];

    // update ui
    [self.tableView reloadData];
}

- (void)startScan
{
    if (_updateInterval) {
        updateTimer = [NSTimer scheduledTimerWithTimeInterval:_updateInterval
                                                       target:self
                                                     selector:@selector(updateTick:)
                                                     userInfo:nil
                                                      repeats:YES];
    }
    
    [simbleeManager startScan];
}

- (void)stopScan
{
    if (updateTimer) {
        [updateTimer invalidate];
        updateTimer = nil;
    }
    
    [simbleeManager stopScan];
}

- (void)viewDidAppear:(BOOL)animated
{
    DLog();
    [super viewDidAppear:animated];

    [appDelegate scanViewController:self viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    DLog();
    [super viewWillDisappear:animated];

    [appDelegate scanViewController:self viewWillDisappear:animated];
}

@end
