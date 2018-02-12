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

#import "Simblee.h"

@interface AppViewController : UIViewController<SimbleeDelegate, UITextFieldDelegate>
{
    __weak IBOutlet UIImageView *colorWheel;
    
    __weak IBOutlet UIButton *colorSwatch;
    
    __weak IBOutlet UILabel *rLabel;
    __weak IBOutlet UILabel *gLabel;
    __weak IBOutlet UILabel *bLabel;
    
    __weak IBOutlet UISlider *rSlider;
    __weak IBOutlet UISlider *gSlider;
    __weak IBOutlet UISlider *bSlider;
    
    __weak IBOutlet UITextField *rValue;
    __weak IBOutlet UITextField *gValue;
    __weak IBOutlet UITextField *bValue;
    
    float red;
    float green;
    float blue;
}

@property(strong, nonatomic) Simblee *simblee;

- (void) pickedColor:(UIColor *)color;

- (IBAction)rSliderChanged:(id)sender;
- (IBAction)gSliderChanged:(id)sender;
- (IBAction)bSliderChanged:(id)sender;

- (IBAction)rEditingDidEnd:(id)sender;
- (IBAction)gEditingDidEnd:(id)sender;
- (IBAction)bEditingDidEnd:(id)sender;

@end
