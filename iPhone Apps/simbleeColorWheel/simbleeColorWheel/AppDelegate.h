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

@class Simblee;
@class SimbleeManager;

@class ScanViewController;
@class AppViewController;
@class ReconnectViewController;
@class OTABootloaderViewController;

@interface AppDelegate : UIResponder <UIApplicationDelegate>
{
}

@property(strong, nonatomic) SimbleeManager *simbleeManager;
@property(strong, nonatomic) UIWindow *window;
@property(strong, nonatomic) UINavigationController *navController;
@property(strong, nonatomic) ScanViewController *scanViewController;
@property(assign, nonatomic) bool backgroundMode;

- (void)bleUnsupported;
- (void)bleUnauthorized;
- (void)blePoweredOff;
- (void)blePoweredOn;

- (void)scanViewController:(ScanViewController *)scanViewController viewDidAppear:(BOOL)animated;
- (void)scanViewController:(ScanViewController *)scanViewController viewWillDisappear:(BOOL)animated;
- (void)scanViewController:(ScanViewController *)scanViewController updateRows:(NSMutableArray *)rows;
- (void)scanViewController:(ScanViewController *)scanViewController connectSimblee:(Simblee *)simblee;
- (void)scanViewController:(ScanViewController *)scanViewController didDiscoverSimblee:(Simblee *)simblee;
- (void)scanViewController:(ScanViewController *)scanViewController didConnectSimblee:(Simblee *)simblee;
- (void)scanViewController:(ScanViewController *)scanViewController didNotConnectSimblee:(Simblee *)simblee;
- (void)scanViewController:(ScanViewController *)scanViewController didLooseConnectSimblee:(Simblee *)simblee;
- (void)scanViewController:(ScanViewController *)scanViewController didDisconnectSimblee:(Simblee *)simblee;

- (void)appViewController:(AppViewController *)appViewController lockState:(bool *)state;
- (void)appViewController:(AppViewController *)appViewController lockSimblee:(Simblee *)simblee;
- (void)appViewController:(AppViewController *)appViewController unlockSimblee:(Simblee *)simblee;
- (void)appViewController:(AppViewController *)appViewController disconnectSimblee:(Simblee *)simblee;
- (void)appViewController:(AppViewController *)appViewController didConnectSimblee:(Simblee *)simblee;
- (void)appViewController:(AppViewController *)appViewController didNotConnectSimblee:(Simblee *)simblee;
- (void)appViewController:(AppViewController *)appViewController didLooseConnectSimblee:(Simblee *)simblee;
- (void)appViewController:(AppViewController *)appViewController didDisconnectSimblee:(Simblee *)simblee;

- (void)otaBootloaderViewController:(OTABootloaderViewController *)otaBootloaderViewController onSuccessfulFileTransferred:(Simblee *)simblee;

- (void)reconnectViewController:(ReconnectViewController *)reconnectViewController connectSimblee:(Simblee *)simblee;
- (void)reconnectViewController:(ReconnectViewController *)reconnectViewController didConnectSimblee:(Simblee *)simblee;
- (void)reconnectViewController:(ReconnectViewController *)reconnectViewController didNotConnectSimblee:(Simblee *)simblee;
- (void)reconnectViewController:(ReconnectViewController *)reconnectViewController didLooseConnectSimblee:(Simblee *)simblee;
- (void)reconnectViewController:(ReconnectViewController *)reconnectViewController didDisconnectSimblee:(Simblee *)simblee;

- (void)simbleeManager:(SimbleeManager *)simbleeManager restoredState:(NSDictionary *)dict;

@end
