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

#include "DLog.h"

#import "AppDelegate.h"

#import "Simblee.h"
#import "SimbleeManager.h"

#import "ReconnectViewController.h"

@interface ReconnectViewController ()
{
    AppDelegate *appDelegate;
}
@end

@implementation ReconnectViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    [self.navigationItem setTitle:@"Simblee"];
    self.navigationItem.hidesBackButton = YES;
    
    appDelegate = [[UIApplication sharedApplication] delegate];
    
    simbleeManager = [SimbleeManager sharedSimbleeManager];

    NSUUID *uuid = [[NSUUID UUID] initWithUUIDString:_identifier];
    NSArray *peripherals = [simbleeManager.central retrievePeripheralsWithIdentifiers:@[uuid]];
    if (peripherals.count > 0) {
        DLog(@"Reconnecting");
        
        CBPeripheral *peripheral = [peripherals objectAtIndex:0];
        
        Simblee *simblee = [simbleeManager simbleeFromPeripheral:peripheral advertisementData:nil RSSI:0];
        
        [appDelegate reconnectViewController:self connectSimblee:simblee];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)showControls
{
    label.hidden = NO;
    activityIndicator.hidden = NO;
}

- (void)hideControls
{
    label.hidden = YES;
    activityIndicator.hidden = YES;
}

#pragma mark - SimbleeManagerDelegate methods

- (void)didConnectSimblee:(Simblee *)simblee
{
    [appDelegate reconnectViewController:self didConnectSimblee:simblee];
}

- (void)didNotConnectSimblee:(Simblee *)simblee
{
    [appDelegate reconnectViewController:self didNotConnectSimblee:simblee];
}

- (void)didLooseConnectSimblee:(Simblee *)simblee
{
    [appDelegate reconnectViewController:self didLooseConnectSimblee:simblee];
}

- (void)didDisconnectSimblee:(Simblee *)simblee
{
    [appDelegate reconnectViewController:self didDisconnectSimblee:simblee];
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
