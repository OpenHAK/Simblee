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

#include <QuartzCore/QuartzCore.h>

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
    UIImage *originalColorWheelImage;
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
        [[self navigationItem] setTitle:@"Simblee ColorWheel"];
        [[self navigationItem] setRightBarButtonItem:(lockState ? lockItem : unlockItem)];
        
        originalColorWheelImage = [UIImage imageNamed:@"colorwheel.png"];
    }
    return self;
}

- (void)scaleColorWheelImage
{
    float scaleWidth = colorWheel.frame.size.width / originalColorWheelImage.size.width;
    float scaleHeight = colorWheel.frame.size.height / originalColorWheelImage.size.height;
    
    float scale = MIN(scaleWidth, scaleHeight);
    
    float width = originalColorWheelImage.size.width * scale;
    float height = originalColorWheelImage.size.height * scale;

    UIGraphicsBeginImageContext(CGSizeMake(width, height));
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    CGContextTranslateCTM(context, 0.0, height);
    CGContextScaleCTM(context, 1.0, -1.0);
    
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), originalColorWheelImage.CGImage);
    
    colorWheel.image = UIGraphicsGetImageFromCurrentImageContext();
    
    UIGraphicsEndImageContext();
}

- (void)iPhone5PortraitLayout
{
    DLog();
    
    colorWheel.frame = CGRectMake(12,77,300,290);
    [self scaleColorWheelImage];
    
    colorSwatch.frame = CGRectMake(20,394,280,16);
    
    rLabel.frame = CGRectMake(20,440,21,21);
    gLabel.frame = CGRectMake(20,478,21,21);
    bLabel.frame = CGRectMake(20,516,21,21);
    
    rSlider.frame = CGRectMake(47,441,202,23);
    gSlider.frame = CGRectMake(47,479,202,23);
    bSlider.frame = CGRectMake(47,517,202,23);
    
    rValue.frame = CGRectMake(255,437,45,30);
    gValue.frame = CGRectMake(255,475,45,30);
    bValue.frame = CGRectMake(255,513,45,30);
}

- (void)iPhone5LandscapeLayout
{
    DLog();
    
    colorWheel.frame = CGRectMake(7,47,254,250);
    [self scaleColorWheelImage];
    
    colorSwatch.frame = CGRectMake(277,90,280,16);
    
    rLabel.frame = CGRectMake(277,139,21,21);
    gLabel.frame = CGRectMake(277,177,21,21);
    bLabel.frame = CGRectMake(277,215,21,21);
    
    rSlider.frame = CGRectMake(304,140,202,23);
    gSlider.frame = CGRectMake(304,178,202,23);
    bSlider.frame = CGRectMake(304,216,202,23);
    
    rValue.frame = CGRectMake(512,136,45,30);
    gValue.frame = CGRectMake(512,174,45,30);
    bValue.frame = CGRectMake(512,212,45,30);
}

- (void)iPhone4SPortraitLayout
{
    DLog();
    
    colorWheel.frame = CGRectMake(45,73,225,225);
    [self scaleColorWheelImage];
    
    colorSwatch.frame = CGRectMake(20,314,280,16);
    
    rLabel.frame = CGRectMake(20,350,21,21);
    gLabel.frame = CGRectMake(20,388,21,21);
    bLabel.frame = CGRectMake(20,426,21,21);
    
    rSlider.frame = CGRectMake(47,351,202,23);
    gSlider.frame = CGRectMake(47,389,202,23);
    bSlider.frame = CGRectMake(47,427,202,23);
    
    rValue.frame = CGRectMake(255,347,45,30);
    gValue.frame = CGRectMake(255,385,45,30);
    bValue.frame = CGRectMake(255,423,45,30);
}

- (void)iPhone4SLandscapeLayout
{
    DLog();
    
    colorWheel.frame = CGRectMake(10,60,225,225);
    [self scaleColorWheelImage];
    
    colorSwatch.frame = CGRectMake(260,100,213,16);
    
    rLabel.frame = CGRectMake(260,129,21,21);
    gLabel.frame = CGRectMake(260,167,21,21);
    bLabel.frame = CGRectMake(260,205,21,21);
    
    rSlider.frame = CGRectMake(287,130,135,23);
    gSlider.frame = CGRectMake(287,168,135,23);
    bSlider.frame = CGRectMake(287,206,135,23);
    
    rValue.frame = CGRectMake(428,126,45,30);
    gValue.frame = CGRectMake(428,164,45,30);
    bValue.frame = CGRectMake(428,202,45,30);
}

- (void)iPadPortraitLayout
{
    DLog();
    
    colorWheel.frame = CGRectMake(80,100,600,600);
    [self scaleColorWheelImage];
    
    colorSwatch.frame = CGRectMake(40,754,685,16);
    
    rLabel.frame = CGRectMake(40,790,21,21);
    gLabel.frame = CGRectMake(40,838,21,21);
    bLabel.frame = CGRectMake(40,886,21,21);
    
    rSlider.frame = CGRectMake(67,791,602,23);
    gSlider.frame = CGRectMake(67,839,602,23);
    bSlider.frame = CGRectMake(67,887,602,23);
    
    rValue.frame = CGRectMake(680,787,45,30);
    gValue.frame = CGRectMake(680,835,45,30);
    bValue.frame = CGRectMake(680,883,45,30);
}

- (void)iPadLandscapeLayout
{
    DLog();
    
    colorWheel.frame = CGRectMake(20,120,600,600);
    [self scaleColorWheelImage];
    
    colorSwatch.frame = CGRectMake(667,310,330,16);
    
    rLabel.frame = CGRectMake(667,349,21,21);
    gLabel.frame = CGRectMake(667,397,21,21);
    bLabel.frame = CGRectMake(667,445,21,21);
    
    rSlider.frame = CGRectMake(694,350,242,23);
    gSlider.frame = CGRectMake(694,398,242,23);
    bSlider.frame = CGRectMake(694,446,242,23);
    
    rValue.frame = CGRectMake(950,346,45,30);
    gValue.frame = CGRectMake(950,394,45,30);
    bValue.frame = CGRectMake(950,442,45,30);
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

    _simblee.delegate = self;
    
    // delegate needed so UITextField textFieldShouldReturn can dismiss the keyboard when the return key is pressed
    rValue.delegate = self;
    gValue.delegate = self;
    bValue.delegate = self;

    /*
    UIColor *start = [UIColor colorWithRed:102/255.0 green:102/255.0 blue:102/255.0 alpha:1.0];
    UIColor *stop = [UIColor colorWithRed:0.0 green:0.0 blue:0.0 alpha:1.0];

    CAGradientLayer *gradient = [CAGradientLayer layer];
    //gradient.frame = [self.view bounds];
    gradient.frame = CGRectMake(0, 0, 1024, 1024);
    gradient.colors = @[(id)start.CGColor, (id)stop.CGColor];
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

- (UIColor*)colorAt:(CGPoint)point
{
    // image must be 1:1
    
    CGImageRef img = colorWheel.image.CGImage;
    
    int width = (int)CGImageGetWidth(img);
    int height = (int)CGImageGetHeight(img);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    uint8_t *bytes = (uint8_t *)calloc(height * width * 4, sizeof(uint8_t));
    int bytesPerPixel = 4;
    int bytesPerRow = bytesPerPixel * width;
    int bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(bytes, width, height,
                                                 bitsPerComponent, bytesPerRow, colorSpace,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);
    
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), img);
    CGContextRelease(context);
    
    // raw data in RGBA8888 pixel format
    int offset = (bytesPerRow * (int)point.y) + (int)point.x * bytesPerPixel;
    
    uint8_t r = bytes[offset];
    uint8_t g = bytes[offset + 1];
    uint8_t b = bytes[offset + 2];
    uint8_t a = bytes[offset + 3];
    
    free(bytes);
    
    return [UIColor colorWithRed:r/255.0f green:g/255.0f blue:b/255.0f alpha:a/255.0f];
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	UITouch* touch = [touches anyObject];
	CGPoint point = [touch locationInView:colorWheel];
    if (CGRectContainsPoint(colorWheel.bounds, point)) {
        UIColor *color = [self colorAt:point];
        [self pickedColor:color];
    }
}

- (void)setColor
{
    UIColor *color = [UIColor colorWithRed:red green:green blue:blue alpha:1.0];
    
    colorSwatch.backgroundColor = color;
    
    uint8_t tx[3] = { red * 255, green * 255, blue * 255 };
    NSData *data = [NSData dataWithBytes:(void*)&tx length:3];

    [_simblee send:data];
}

- (void)pickedColor:(UIColor *)color
{
    DLog();
    
    const CGFloat *components = CGColorGetComponents([color CGColor]);
    
    red = components[0];
    green = components[1];
    blue = components[2];
    
    rSlider.value = red;
    gSlider.value = green;
    bSlider.value = blue;
    
    rValue.text = [[NSString alloc] initWithFormat:@"%d", (int)(red * 255)];
    gValue.text = [[NSString alloc] initWithFormat:@"%d", (int)(green * 255)];
    bValue.text = [[NSString alloc] initWithFormat:@"%d", (int)(blue * 255)];
    
    [self setColor];
}

- (IBAction)rSliderChanged:(id)sender
{
    DLog();
    red = [rSlider value];
    rValue.text = [[NSString alloc] initWithFormat:@"%d", (int)(red * 255)];
    [self setColor];
}

- (IBAction)gSliderChanged:(id)sender
{
    DLog();
    green = [gSlider value];
    gValue.text = [[NSString alloc] initWithFormat:@"%d", (int)(green * 255)];
    [self setColor];
}

- (IBAction)bSliderChanged:(id)sender
{
    DLog();
    blue = [bSlider value];
    bValue.text = [[NSString alloc] initWithFormat:@"%d", (int)(blue * 255)];
    [self setColor];
}

- (IBAction)rEditingDidEnd:(id)sender
{
    DLog();
    red = rValue.text.intValue / 255.0;
    rSlider.value = red;
    [self setColor];
}

- (IBAction)gEditingDidEnd:(id)sender
{
    DLog();
    green = gValue.text.intValue / 255.0;
    gSlider.value = green;
    [self setColor];
}

- (IBAction)bEditingDidEnd:(id)sender
{
    DLog();
    blue = bValue.text.intValue / 255.0;
    bSlider.value = blue;
    [self setColor];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    DLog();
    [textField resignFirstResponder];
    return YES;
}

@end
