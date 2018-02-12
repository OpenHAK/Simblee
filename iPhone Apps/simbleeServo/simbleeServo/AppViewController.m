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
        [[self navigationItem] setTitle:@"Simblee Servo"];
        [[self navigationItem] setRightBarButtonItem:(lockState ? lockItem : unlockItem)];
    }
    return self;
}

- (void)iPhone5PortraitLayout
{
    OutputLabel.frame = CGRectMake(104,160,113,21);
    Output.frame = CGRectMake(20,199,280,44);
    
    PositionLabel.frame = CGRectMake(40,276,241,21);
    
    ValueLabel.frame = CGRectMake(25,315,58,21);
    Value.frame = CGRectMake(97,311,83,30);
    
    Stepper.frame = CGRectMake(206,312,94,27);
    Slider.frame = CGRectMake(18,352,284,23);
    Segment.frame = CGRectMake(20,388,280,30);
}

- (void)iPhone5LandscapeLayout
{
    OutputLabel.frame = CGRectMake(230,60,113,21);
    Output.frame = CGRectMake(146,84,280,44);
    
    PositionLabel.frame = CGRectMake(167,154,241,21);
    
    ValueLabel.frame = CGRectMake(147,183,58,21);
    Value.frame = CGRectMake(213,180,83,30);
    
    Stepper.frame = CGRectMake(323,180,94,27);
    Slider.frame = CGRectMake(145,218,284,23);
    Segment.frame = CGRectMake(146,248,280,30);
}

- (void)iPhone4SPortraitLayout
{
    OutputLabel.frame = CGRectMake(104,125,113,21);
    Output.frame = CGRectMake(20,159,280,44);

    PositionLabel.frame = CGRectMake(40,236,241,21);

    ValueLabel.frame = CGRectMake(25,275,58,21);
    Value.frame = CGRectMake(97,271,83,30);

    Stepper.frame = CGRectMake(206,272,94,27);
    Slider.frame = CGRectMake(18,312,284,23);
    Segment.frame = CGRectMake(20,348,280,30);
}

- (void)iPhone4SLandscapeLayout
{
    OutputLabel.frame = CGRectMake(190,60,113,21);
    Output.frame = CGRectMake(106,84,280,44);
    
    PositionLabel.frame = CGRectMake(127,154,241,21);
    
    ValueLabel.frame = CGRectMake(107,183,58,21);
    Value.frame = CGRectMake(173,180,83,30);
    
    Stepper.frame = CGRectMake(293,180,94,27);
    Slider.frame = CGRectMake(105,218,284,23);
    Segment.frame = CGRectMake(106,248,280,30);
}

- (void)iPadPortraitLayout
{
    OutputLabel.frame = CGRectMake(328,383,113,21);
    Output.frame = CGRectMake(244,423,280,44);
    
    PositionLabel.frame = CGRectMake(264,499,241,21);
    
    ValueLabel.frame = CGRectMake(249,538,58,21);
    Value.frame = CGRectMake(321,534,83,30);
    
    Stepper.frame = CGRectMake(430,535,94,27);
    Slider.frame = CGRectMake(242,576,284,23);
    Segment.frame = CGRectMake(242,612,280,30);
}

- (void)iPadLandscapeLayout
{
    OutputLabel.frame = CGRectMake(456,255,113,21);
    Output.frame = CGRectMake(372,295,280,44);
    
    PositionLabel.frame = CGRectMake(392,371,241,21);
    
    ValueLabel.frame = CGRectMake(377,410,58,21);
    Value.frame = CGRectMake(449,406,83,30);
    
    Stepper.frame = CGRectMake(558,407,94,27);
    Slider.frame = CGRectMake(370,448,284,23);
    Segment.frame = CGRectMake(372,484,280,30);
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

    for (int i = 0; i < 4; i++)
      value[i] = 90;
    
    Value.delegate = self;

    [_simblee setDelegate:self];
    
    /*
    UIColor *start = [UIColor colorWithRed:58/255.0 green:108/255.0 blue:183/255.0 alpha:0.15];
    UIColor *stop = [UIColor colorWithRed:58/255.0 green:108/255.0 blue:183/255.0 alpha:0.45];
    
    CAGradientLayer *gradient = [CAGradientLayer layer];
    //gradient.frame = [self.view bounds];
    gradient.frame = CGRectMake(0, 0, 1024, 1024);
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
    
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]
                                   initWithTarget:self
                                   action:@selector(dismissKeyboard)];
    
    [tap setCancelsTouchesInView:NO];
    [self.view addGestureRecognizer:tap];
    
    output = 0;
    [self updateUI];
}

- (void)startOTABootloader {
    DLog(@"startOTABootloader");
    
    OTABootloaderViewController *viewController = [[OTABootloaderViewController alloc] init];
    viewController.simblee = _simblee;
    [[self navigationController] pushViewController:viewController animated:true];
}

- (void)dismissKeyboard {
    [Value endEditing:YES];
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

- (void)updateUI
{
    int val = value[output];
    
    DLog(@"output=%d, value=%d", output, val);

    Output.selectedSegmentIndex = output;
    
    Value.text = [[NSString alloc] initWithFormat:@"%d", val];
    
    Stepper.value = val;

    Slider.value = val;
    //[Slider setValue:val animated:YES];
    
    if (val == 0) {
        Segment.selectedSegmentIndex = 0;
    } else if (val == 90) {
        Segment.selectedSegmentIndex = 1;
    } else if (val == 180) {
        Segment.selectedSegmentIndex = 2;
    } else {
        Segment.selectedSegmentIndex = -1;
    }
}

- (void)updateServo
{
    // the sketch allows multiple servos to be updated at the same time
    int servo = 1 << (1 + output);
    
    int val = value[output];
    
    if (val == 180) {
        val = 179;
    }
    
    DLog(@"output=%d, servo=%d, value=%d", output, servo, val);

    uint8_t bytes[] = { servo, val };
    
    NSData* data = [NSData dataWithBytes:(void*)&bytes length:2];
    [_simblee send:data];
}

- (IBAction)outputSegmentValueChanged:(id)sender {
    output = (int)Output.selectedSegmentIndex;
    
    [self updateUI];
}

- (IBAction)textFieldEditingDidEnd:(id)sender {
    int val = Value.text.intValue;
    
    if (val < 0) {
        val = 0;
    } else if (val > 180) {
        val = 180;
    }
    
    value[output] = val;
    
    [self updateUI];
    [self updateServo];
}

- (IBAction)stepperValueChanged:(id)sender {
    value[output] = Stepper.value;
    
    [self updateUI];
    [self updateServo];
}

- (IBAction)sliderTouchUpInside:(id)sender {
    value[output] = Slider.value;
    
    [self updateUI];
    [self updateServo];
}

- (IBAction)segmentValueChanged:(id)sender {
    int val = value[output];
    
    if (Segment.selectedSegmentIndex == 0) {
        val = 0;
    } else if (Segment.selectedSegmentIndex == 1) {
        val = 90;
    } else if (Segment.selectedSegmentIndex == 2) {
        val = 180;
    }
    
    value[output] = val;
    
    [self updateUI];
    [self updateServo];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    DLog(@"textFieldShouldReturn");
    [textField resignFirstResponder];
    return YES;
}

@end
