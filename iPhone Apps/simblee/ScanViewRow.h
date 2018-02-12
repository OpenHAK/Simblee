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

@class CustomCellBackground;

@interface ScanViewRow : NSObject
{
}

@property(strong, nonatomic) UITableViewCell *cell;
@property(strong, nonatomic) UIColor *startColor;
@property(strong, nonatomic) UIColor *stopColor;
@property(assign, nonatomic) bool colorChanged;
@property(strong, nonatomic) UIProgressView *rssiProgress;
@property(strong, nonatomic) UIImageView *rssiImage;
@property(strong, nonatomic) UILabel *rssiLabel;

@property(weak, nonatomic) Simblee *simblee;

@property(assign, nonatomic) bool hide;

@end
