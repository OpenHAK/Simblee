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
#import "UIAlertViewBlock.h"

#import "Simblee.h"
#import "SimbleeManager.h"

#import "ReconnectViewController.h"
#import "ScanViewRow.h"
#import "ScanViewController.h"
#import "AppViewController.h"
#import "OTABootloaderViewController.h"

#import "AppDelegate.h"

NSString *simbleeService = @"fe84";
NSString *otaBootloaderService = @"00001530-1212-efde-1523-785feabcd123";

@interface AppDelegate()
{
    bool bluetoothLaunch;
    bool restoreStateDone;
    bool startScanning;
    bool active;
}
@end

@implementation AppDelegate

- (void)initializeSettings
{
    DLog(@"");
    
    NSString *mainBundlePath = [[NSBundle mainBundle] bundlePath];
    
    NSString *settingsPropertyListPath = [mainBundlePath
                                          stringByAppendingPathComponent:@"Settings.bundle/Root.plist"];
    
    NSDictionary *settingsPropertyList = [NSDictionary
                                          dictionaryWithContentsOfFile:settingsPropertyListPath];
    
    NSMutableArray *preferenceArray = [settingsPropertyList objectForKey:@"PreferenceSpecifiers"];
    NSMutableDictionary *registerableDictionary = [NSMutableDictionary dictionary];
    
    for (int i = 0; i < [preferenceArray count]; i++)  {
        NSString  *key = [[preferenceArray objectAtIndex:i] objectForKey:@"Key"];
        if (key) {
            id  value = [[preferenceArray objectAtIndex:i] objectForKey:@"DefaultValue"];
            [registerableDictionary setObject:value forKey:key];
        }
    }

    [[NSUserDefaults standardUserDefaults] registerDefaults:registerableDictionary];
    
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    DLog(@"\r\r** app started **\r");
    
    // ios restored app in background to handle a bluetooth event
    if (launchOptions && [launchOptions objectForKey:UIApplicationLaunchOptionsBluetoothCentralsKey]) {
        bluetoothLaunch = true;
    }
    
    // default values are not stored in standardUserDefaults
    // registerDefaults must be called each time to populate the default keys from the Settings.bundle
    [self initializeSettings];

    // get the background mode switch from ios settings
    _backgroundMode = [[NSUserDefaults standardUserDefaults] boolForKey:@"background_mode"];

    DLog(@"bluetooth launch %d, background mode = %d", bluetoothLaunch, _backgroundMode);
    
    _simbleeManager = SimbleeManager.sharedSimbleeManager;
    
    // scan service uuids
    if ([OTABootloaderViewController hexFileURL]) {
        _simbleeManager.scanServices = @[[CBUUID UUIDWithString:simbleeService], [CBUUID UUIDWithString:otaBootloaderService]];
    } else {
        _simbleeManager.scanServices = @[[CBUUID UUIDWithString:simbleeService]];
    }
    
    // scan defaults
    _simbleeManager.scanUpdates = YES;

    // root view controller
    _scanViewController = [[ScanViewController alloc] init];

    // window
    _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    _window.backgroundColor = [UIColor whiteColor];

    // navigation controller
    _navController = [[UINavigationController alloc] initWithRootViewController:_scanViewController];
    _navController.navigationBar.tintColor = [UIColor colorWithRed:0.0 green:0.0 blue:0.0 alpha:1.0];

    // add navigation controller to window
    [_window setRootViewController:_navController];

    // app launched by user
    if (! bluetoothLaunch) {
        NSString *lockIdentifier = [[NSUserDefaults standardUserDefaults] stringForKey:@"lockIdentifier"];
        if (lockIdentifier) {
            ReconnectViewController *reconnectViewController = [[ReconnectViewController alloc] init];
            reconnectViewController.identifier = lockIdentifier;
            [_navController pushViewController:reconnectViewController animated:NO];
        } else {
            startScanning = true;
        }
    }

    [_window makeKeyAndVisible];

    if (_backgroundMode) {
        // prompt user for notification permission
        if ([UIApplication instancesRespondToSelector:@selector(registerUserNotificationSettings:)]) {
            [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:UIUserNotificationTypeAlert categories:nil]];
        }
    }

    return YES;
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    DLog(@"");
    
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.

    // uncomment block to see ios restore app in background to handle a bluetooth event
    if (_backgroundMode) {
        UILocalNotification *notify = [[UILocalNotification alloc] init];
        notify.alertBody = @"Killing App";
        [[UIApplication sharedApplication] presentLocalNotificationNow:notify];

        DLog(@"\r** killing app **\r");
        
        // simulate ios killing the app due to memory pressure
        kill(getpid(), SIGKILL);
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    DLog(@"");

    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.

    active = false;
    
    startScanning = false;
    if (_simbleeManager.isScanning) {
        startScanning = true;

        // stop foreground mode scanning
        [self stopScan];

        // start background mode scanning (conserve battery life)
        if (_backgroundMode) {
            [self startScan];
        }
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    DLog(@"");

    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.

    active = true;
    
    if (startScanning) {
        startScanning = false;
        
        // stop background mode scanning
        if (_simbleeManager.isScanning) {
            [self stopScan];
        }
        
        // start foreground mode scanning
        [self startScan];
    }
}

#pragma warn - misc

- (void)alert:(NSString *)message title:(NSString *)title block:(void (^)(NSInteger buttonIndex))block
{
    UIAlertViewBlock *alert = [[UIAlertViewBlock alloc] initWithTitle:title
                                                              message:message
                                                                block:block
                                                    cancelButtonTitle:@"OK"
                                                    otherButtonTitles:nil];
    [alert show];
}

#pragma mark - general

- (void)bleUnsupported
{
    [self alert:@"BLE Unsupported" title:@"App" block:nil];
}

- (void)bleUnauthorized
{
    [self alert:@"BLE Unauthorized" title:@"App" block:nil];
}

- (void)blePoweredOff
{
    [self alert:@"BLE Powered Off.\n\nOpen Settings, and switch Bluetooth On." title:@"App" block:nil];
}

- (void)blePoweredOn
{
}

#pragma mark - ScanViewController

- (void)stopScan
{
    DLog(@"");
    
    [_scanViewController stopScan];
}

- (void)startScan
{
    DLog(@"");
    
    if (bluetoothLaunch && !restoreStateDone) {
        DLog(@"restoreState not complete");
        return;
    }
    
    if (active) {
        // foreground mode scanning (responsive)
        DLog(@"foreground mode scanning");
        _simbleeManager.scanUpdates = true;
        _simbleeManager.restartInterval = 60.0;
        _scanViewController.updateInterval = 1.0;
    } else {
        // background mode scanning (conserve battery life)
        DLog(@"background mode scanning");
        _simbleeManager.scanUpdates = false;
        _simbleeManager.restartInterval = 0.0;
        _scanViewController.updateInterval = 0.0;
    }
    
    [_scanViewController startScan];
}

- (void)scanViewController:(ScanViewController *)scanViewController viewDidAppear:(BOOL)animated
{
    DLog(@"");

    if (! active) {
        DLog(@"not active");
        return;
    }
    
    if (! _simbleeManager.isScanning) {
        [scanViewController clearRows];
        [self startScan];
    } else {
        DLog(@"scanning already started");
    }
}

- (void)scanViewController:(ScanViewController *)scanViewController viewWillDisappear:(BOOL)animated
{
    NSLog(@"");

    if (_simbleeManager.isScanning) {
        [self stopScan];
    }
}

- (void)scanViewController:(ScanViewController *)scanViewController updateRows:(NSMutableArray *)rows
{
    NSDate *date = [NSDate date];
    
    for (NSUInteger i = 0; i < rows.count; i++) {
        ScanViewRow *row = [rows objectAtIndex:i];
        Simblee *simblee = row.simblee;
        
        // optimization (don't update row unless something changes)
        bool updated = false;
        
        // new row
        if (! row.cell) {
            updated = true;
        }
        
        // set out of range if no advertisement for 3 seconds
        bool outOfRange = (simblee.lastAdvertisement != NULL && [date timeIntervalSinceDate:simblee.lastAdvertisement] >= 3.0);
        if (outOfRange != simblee.outOfRange) {
            simblee.outOfRange = outOfRange;
            //data.hide = outOfRange;
            updated = true;
        }
        
        // update RSSI every 40 samples
        if (simblee.sampleRSSI && simblee.sampleRSSI.count > 40) {
            NSMutableArray *samples = simblee.sampleRSSI;
            
            int current = simblee.RSSI.intValue;
            
            int total = 0;
            int count = (int)[samples count];
            
            for (int j = 0; j < [samples count]; j++) {
                NSNumber *sample = [samples objectAtIndex:j];
                
                int rssi = sample.intValue;
                
                // value used for an invalid/indeterminate rssi sample
                if (rssi == 127) {
                    count--;
                    continue;
                }
                
                total += rssi;
            }
            
            int new = total / count;
            
            // smooth out samples
            if (new < current - 9) {
                new = current - 10;
            } else if (new > current - 9) {
                new = current + 10;
            }
            
            simblee.RSSI = [NSNumber numberWithLong:new];
            
            // reset sampling
            simblee.startRSSI = nil;
            [simblee.sampleRSSI removeAllObjects];
            
            updated = true;
        }

        // set row background color
        // order is important
        UIColor *startColor;
        UIColor *stopColor;
        if ([simblee.connectedService isEqual:[CBUUID UUIDWithString:otaBootloaderService]]) {
            startColor = [UIColor colorWithRed:203/255.0 green:164/255.0 blue:164/255.0 alpha: 1.0];
            stopColor = [UIColor colorWithRed:203/255.0 green:164/255.0 blue:164/255.0 alpha: 0.7];
        } else if (simblee.outOfRange) {
            startColor = [UIColor colorWithRed:160/255.0 green:160/255.0 blue:160/255.0 alpha:1.0];
            stopColor = [UIColor colorWithRed:160/255.0 green:160/255.0 blue:160/255.0 alpha:0.7];
        } else {
            startColor = [UIColor colorWithRed:253/255.0 green:255/255.0 blue:255/255.0 alpha: 1.0];
            stopColor = [UIColor colorWithRed:253/255.0 green:255/255.0 blue:255/255.0 alpha: 0.7];
        }
        if (![startColor isEqual:row.startColor] || ![stopColor isEqual:row.stopColor]) {
            row.startColor = startColor;
            row.stopColor = stopColor;
            row.colorChanged = true;
        }
        
        if (updated) {
            [scanViewController updateRow:row signalStyle:SignalStyleGraph];
        }
    }
}

- (void)scanViewController:(ScanViewController *)scanViewController connectSimblee:(Simblee *)simblee
{
    CBUUID *service = [simblee.advertisementServices objectAtIndex:0];
    
    [self stopScan];
    
    simblee.delegate = scanViewController;
    
    if (_backgroundMode) {
        // background connect last forever
        simblee.connectInterval = 0.0;
    }
    
    if ([service isEqual:[CBUUID UUIDWithString:otaBootloaderService]]) {
        OTABootloaderViewController *viewController = [[OTABootloaderViewController alloc] init];
        viewController.simblee = simblee;
        [_navController pushViewController:viewController animated:true];
    } else {
        [simblee connect:@[[CBUUID UUIDWithString:simbleeService]]];
    }
}

- (void)scanViewController:(ScanViewController *)scanViewController didDiscoverSimblee:(Simblee *)simblee
{
    // DLog(@"simblee = %@", simblee.identifier);
    
    // when scanner restarted, it rediscovers peripherals it has already seen - only notify new discoveries
    if (_backgroundMode && !active && simblee.advertisementPackets == 1) {
        UILocalNotification *notify = [[UILocalNotification alloc] init];
        notify.alertBody = @"Simblee Discovered";
        [[UIApplication sharedApplication] presentLocalNotificationNow:notify];
    }
}

- (void)scanViewController:(ScanViewController *)scanViewController didConnectSimblee:(Simblee *)simblee
{
    AppViewController *viewController = [[AppViewController alloc] init];
    viewController.simblee = simblee;

    // make popViewController animation smooth
    [scanViewController clearView];
    
    [_navController pushViewController:viewController animated:YES];
}

- (void)scanViewController:(ScanViewController *)scanViewController didNotConnectSimblee:(Simblee *)simblee
{
    // the row was selected during connect attempt; deselect the row for visual feedback
    [scanViewController deselectRow];
    
    [self alert:@"Cannot Connect - Please try again."
          title:@"ScanViewController"
          block:^(NSInteger buttonIndex) { [self startScan]; }];
}

- (void)scanViewController:(ScanViewController *)scanViewController didLooseConnectSimblee:(Simblee *)simblee
{
    // not used
}

- (void)scanViewController:(ScanViewController *)scanViewController didDisconnectSimblee:(Simblee *)simblee
{
    // not used
}

#pragma mark - AppViewController

- (void)appViewController:(AppViewController *)appViewController lockState:(bool *)state
{
    *state = ([[NSUserDefaults standardUserDefaults] stringForKey:@"lockIdentifier"] != nil);
}

- (void)appViewController:(AppViewController *)appViewController lockSimblee:(Simblee *)simblee
{
    NSString *lockIdentifier = [simblee.peripheral.identifier UUIDString];
    [[NSUserDefaults standardUserDefaults] setObject:lockIdentifier forKey:@"lockIdentifier"];
}

- (void)appViewController:(AppViewController *)appViewController unlockSimblee:(Simblee *)simblee
{
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:@"lockIdentifier"];
}

- (void)appViewController:(AppViewController *)appViewController disconnectSimblee:(Simblee *)simblee
{
    [simblee disconnect];
    
    [_navController popViewControllerAnimated:YES];
    
    simblee.delegate = _scanViewController;
}

- (void)appViewController:(AppViewController *)appViewController didConnectSimblee:(Simblee *)simblee
{
    // used if trying to reconnect
}

- (void)appViewController:(AppViewController *)appViewController didNotConnectSimblee:(Simblee *)simblee
{
    // used if trying to reconnect
}

- (void)appViewController:(AppViewController *)appViewController didLooseConnectSimblee:(Simblee *)simblee
{
    if (_backgroundMode) {
        UILocalNotification *notify = [[UILocalNotification alloc] init];
        notify.alertBody = @"Lost Connection";
        [[UIApplication sharedApplication] presentLocalNotificationNow:notify];
        
        [_navController popViewControllerAnimated:NO];
        simblee.delegate = _scanViewController;
    }
    
    // report the error or attempt to silently reconnect
    [self alert:@"Lost Connection"
          title:@"AppViewController"
          block:^(NSInteger buttonIndex) {
              [_navController popViewControllerAnimated:YES];
              simblee.delegate = _scanViewController;
          }];
}

- (void)appViewController:(AppViewController *)appViewController didDisconnectSimblee:(Simblee *)simblee
{
    // not used
}

- (void)otaBootloaderViewController:(OTABootloaderViewController *)otaBootloaderViewController onSuccessfulFileTransferred:(Simblee *)simblee
{
    simblee.delegate = _scanViewController;
    [_navController popToRootViewControllerAnimated:YES];
}

#pragma mark - ReconnectViewController

- (void)reconnectViewController:(ReconnectViewController *)reconnectViewController connectSimblee:(Simblee *)simblee
{
    [reconnectViewController showControls];
    
    simblee.delegate = reconnectViewController;
    
    [simblee connect:@[[CBUUID UUIDWithString:simbleeService]]];
}

- (void)reconnectViewController:(ReconnectViewController *)reconnectViewController didConnectSimblee:(Simblee *)simblee
{
    [reconnectViewController hideControls];

    AppViewController *appViewController = [[AppViewController alloc] init];
    appViewController.simblee = simblee;
    simblee.delegate = appViewController;

    [_navController popViewControllerAnimated:NO];
    
    [_navController pushViewController:appViewController animated:YES];
}

- (void)reconnectViewController:(ReconnectViewController *)reconnectViewController didNotConnectSimblee:(Simblee *)simblee
{
    [reconnectViewController hideControls];
    
    UIAlertViewBlock *alert = [[UIAlertViewBlock alloc]
                               initWithTitle:@"ReconnectViewController"
                               message:@"Cannot Reconnect."
                               block:^(NSInteger buttonIndex) {
                                   if (buttonIndex == 0) {
                                       [self reconnectViewController:reconnectViewController connectSimblee:simblee];
                                   } else if (buttonIndex == 1) {
                                       [_navController popViewControllerAnimated:YES];
                                       simblee.delegate = _scanViewController;
                                   }
                               }
                               cancelButtonTitle:@"Retry"
                               otherButtonTitles:@"Select"];
    [alert show];
}

- (void)reconnectViewController:(ReconnectViewController *)reconnectViewController didLooseConnectSimblee:(Simblee *)simblee
{
    // not used
}

- (void)reconnectViewController:(ReconnectViewController *)reconnectViewController didDisconnectSimblee:(Simblee *)simblee
{
    // not used
}

#pragma mark - background mode

- (void)simbleeManager:(SimbleeManager *)simbleeManager restoredState:(NSDictionary *)dict
{
    DLog(@"");
    
    for (Simblee *simblee in simbleeManager.simblees) {
        if (simblee.state == SimbleeStateConnected) {
            AppViewController *appViewController = [[AppViewController alloc] init];
            appViewController.simblee = simblee;
            simblee.delegate = appViewController;
            [_navController pushViewController:appViewController animated:NO];
        } else {
            simblee.delegate = _scanViewController;
        }
    }

    restoreStateDone = true;

    // start foreground scanning (when applicationDidBecomeActive:)
    startScanning = simbleeManager.isScanning;

    UILocalNotification *notify = [[UILocalNotification alloc] init];
    notify.alertBody = @"Restored State";
    [[UIApplication sharedApplication] presentLocalNotificationNow:notify];
}

@end
