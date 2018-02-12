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
#include "Util.h"

#import <QuartzCore/QuartzCore.h>

#import "UIAlertViewBlock.h"

#import "AppDelegate.h"
#import "OTABootloaderViewController.h"
#import "AppViewController.h"

@interface AppViewController ()
{
    AppDelegate *appDelegate;
    bool lockState;
    UIBarButtonItem *disconnectItem;
    UIBarButtonItem *lockItem;
    UIBarButtonItem *unlockItem;
}
@end

@implementation AppViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        appDelegate = [[UIApplication sharedApplication] delegate];
        
        [appDelegate appViewController:self lockState:&lockState];
        
        self.navigationItem.hidesBackButton = YES;
        
        UIImage *leftImage = [UIImage imageNamed:@"disconnect.png"];
        disconnectItem = [[UIBarButtonItem alloc] initWithImage:leftImage
                                                          style:UIBarButtonItemStylePlain
                                                         target:self
                                                         action:@selector(disconnect:)];
        
        UIImage *lockImage = [UIImage imageNamed:@"lock.png"];
        lockItem = [[UIBarButtonItem alloc] initWithImage:lockImage
                                                    style:UIBarButtonItemStylePlain
                                                   target:self
                                                   action:@selector(unlock:)];
        
        UIImage *unlockImage = [UIImage imageNamed:@"unlock.png"];
        unlockItem = [[UIBarButtonItem alloc] initWithImage:unlockImage
                                                      style:UIBarButtonItemStylePlain
                                                     target:self
                                                     action:@selector(lock:)];
        
        [[self navigationItem] setLeftBarButtonItem:(lockState ? nil : disconnectItem)];
        [[self navigationItem] setTitle:@"Simblee Temperature"];
        [[self navigationItem] setRightBarButtonItem:(lockState ? lockItem : unlockItem)];
        
    }
    return self;
}

- (void)iPhone5PortraitLayout
{
    DLog();
    image.frame = CGRectMake(20,66,108,496);
    label1.frame = CGRectMake(120,225,180,53);
    label2.frame = CGRectMake(120,286,180,53);
}

- (void)iPhone5LandscapeLayout
{
    DLog();
    image.frame = CGRectMake(20,66,108,220);
    label1.frame = CGRectMake(120,105,180,53);
    label2.frame = CGRectMake(120,166,180,53);
}

- (void)iPhone4SPortraitLayout
{
    DLog();
    image.frame = CGRectMake(20,106,108,336);
    label1.frame = CGRectMake(120,205,180,53);
    label2.frame = CGRectMake(120,266,180,53);
}

- (void)iPhone4SLandscapeLayout
{
    DLog();
    image.frame = CGRectMake(20,66,108,216);
    label1.frame = CGRectMake(120,105,180,53);
    label2.frame = CGRectMake(120,166,180,53);
}

- (void)iPadPortraitLayout
{
    DLog();
    image.frame = CGRectMake(20,66,216,992);
    label1.frame = CGRectMake(300,440,180,53);
    label2.frame = CGRectMake(300,500,180,53);
}

- (void)iPadLandscapeLayout
{
    DLog();
    image.frame = CGRectMake(20,166,216,496);
    label1.frame = CGRectMake(300,340,180,53);
    label2.frame = CGRectMake(300,400,180,53);
}

- (bool)isLandscape
{
    UIInterfaceOrientation statusBarOrientation = [UIApplication sharedApplication].statusBarOrientation;
    return (statusBarOrientation == UIInterfaceOrientationLandscapeLeft || statusBarOrientation == UIInterfaceOrientationLandscapeRight);
}

- (void)manualLayout
{
    CGRect rect = [[UIScreen mainScreen] bounds];
    
    if ([self isLandscape]) {
        DLog(@"%f %f", rect.size.width, rect.size.height);
        if (rect.size.width >= 1024) {
            [self iPadLandscapeLayout];
        } else if (rect.size.width >= 568) {
            [self iPhone5LandscapeLayout];
        } else {
            [self iPhone4SLandscapeLayout];
        }
    } else {
        if (rect.size.height >= 1024) {
            [self iPadPortraitLayout];
        } else if (rect.size.height >= 568) {
            [self iPhone5PortraitLayout];
        } else {
            [self iPhone4SPortraitLayout];
        }
    }
}

- (void)viewDidLayoutSubviews {
    [self manualLayout];
}

- (void)willAnimateRotationToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
    [self manualLayout];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
 
    _simblee.delegate = self;
    
    /*
    UIColor *start = [UIColor colorWithRed:58/255.0 green:108/255.0 blue:183/255.0 alpha:0.15];
    UIColor *stop = [UIColor colorWithRed:58/255.0 green:108/255.0 blue:183/255.0 alpha:0.45];
    
    CAGradientLayer *gradient = [CAGradientLayer layer];
    gradient.frame = [self.view bounds];
    gradient.colors = [NSArray arrayWithObjects:(id)start.CGColor, (id)stop.CGColor, nil];
    [self.view.layer insertSublayer:gradient atIndex:0];
    */
    
    /*
    // via Simblee advertisementData, services or connectedService
    NSString *data = [[NSString alloc] initWithData:_simblee.advertisementData encoding:NSUTF8StringEncoding];
    
    // update is not available if the ledbtn sketch is used without OTA
    if ([data hasPrefix:@"LBwOTA"]) {
        // only offer upgrade if they are not using a version 2 sketch
        // (version 2, is the LedBtnWithOTABootloader sketch, with a blue led instead of green)
        if ([data characterAtIndex:6] != '2') {
            UIAlertViewBlock *alert = [[UIAlertViewBlock alloc]
                                       initWithTitle:@"Update available"
                                       message:@"Would you like to install the update?"
                                       block:^(NSInteger buttonIndex) {
                                           if (buttonIndex == 1)
                                               [self startOTABootloader];
                                       }
                                       cancelButtonTitle:@"No"
                                       otherButtonTitles:@"Yes", nil];
            [alert show];
        }
    }
    */
}

- (void)startOTABootloader {
    DLog(@"startOTABootloader");
    
    OTABootloaderViewController *viewController = [[OTABootloaderViewController alloc] init];
    viewController.simblee = _simblee;
    [[self navigationController] pushViewController:viewController animated:true];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)disconnect:(id)sender
{
    DLog();
    [appDelegate appViewController:self disconnectSimblee:_simblee];
}

- (IBAction)lock:(id)sender
{
    DLog();
    [appDelegate appViewController:self lockSimblee:_simblee];
    lockState = true;
    [[self navigationItem] setLeftBarButtonItem:(lockState ? nil : disconnectItem)];
    [[self navigationItem] setRightBarButtonItem:(lockState ? lockItem : unlockItem)];
}

- (IBAction)unlock:(id)sender
{
    DLog();
    [appDelegate appViewController:self unlockSimblee:_simblee];
    lockState = false;
    [[self navigationItem] setLeftBarButtonItem:(lockState ? nil : disconnectItem)];
    [[self navigationItem] setRightBarButtonItem:(lockState ? lockItem : unlockItem)];
}

#pragma mark - SimbleeDelegate methods

- (void)didConnectSimblee:(Simblee *)simblee
{
    [appDelegate appViewController:self didConnectSimblee:simblee];
}

- (void)didNotConnectSimblee:(Simblee *)simblee
{
    [appDelegate appViewController:self didNotConnectSimblee:simblee];
}

- (void)didLooseConnectSimblee:(Simblee *)simblee
{
    [appDelegate appViewController:self didLooseConnectSimblee:simblee];
}

- (void)didDisconnectSimblee:(Simblee *)simblee
{
    [appDelegate appViewController:self didDisconnectSimblee:simblee];
}

- (void)didReceive:(NSData *)data
{
    DLog(@"RecievedRX");
    
    float celsius = dataFloat(data);
    float fahrenheit = (celsius * 9 / 5) + 32;
    
    DLog(@"c=%.2f, f=%.2f", celsius, fahrenheit);
    
    NSString* string1 = [NSString stringWithFormat:@"%.2f ºC", celsius];
    NSString* string2 = [NSString stringWithFormat:@"%.2f ºF", fahrenheit];
    
    [label1 setText:string1];
    [label2 setText:string2];
    
    if (appDelegate.backgroundMode) {
        UILocalNotification *notify = [[UILocalNotification alloc] init];
        notify.alertBody = @"Data Received";
        [[UIApplication sharedApplication] presentLocalNotificationNow:notify];
    }
}

@end
