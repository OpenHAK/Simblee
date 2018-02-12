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
#import <AudioToolbox/AudioToolbox.h>

#import "Simblee.h"

@interface AppViewController : UIViewController<SimbleeDelegate, UITextFieldDelegate>
{
    __weak IBOutlet UILabel *OutputLabel;
    __weak IBOutlet UISegmentedControl *Output;
    
    __weak IBOutlet UILabel *PositionLabel;
    
    __weak IBOutlet UILabel *ValueLabel;
    __weak IBOutlet UITextField *Value;
    
    __weak IBOutlet UIStepper *Stepper;
    __weak IBOutlet UISlider *Slider;
    __weak IBOutlet UISegmentedControl *Segment;

    int output;
    int value[4];
}

@property(strong, nonatomic) Simblee *simblee;

- (IBAction)outputSegmentValueChanged:(id)sender;
- (IBAction)textFieldEditingDidEnd:(id)sender;
- (IBAction)stepperValueChanged:(id)sender;
- (IBAction)sliderTouchUpInside:(id)sender;
- (IBAction)segmentValueChanged:(id)sender;

@end
