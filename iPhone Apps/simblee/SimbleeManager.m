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

#include <objc/message.h>

#import "AppDelegate.h"

#import "Simblee.h"
#import "SimbleeManager.h"

@interface SimbleeManager()
{
    AppDelegate *appDelegate;
    void(^blePowerOnScanBlock)();
    NSMutableArray *blePowerOnBlocks;
    NSTimer *restartTimer;
}
@end

@implementation SimbleeManager

+ (SimbleeManager *)sharedSimbleeManager
{
    static SimbleeManager *simbleeManager;
    if (! simbleeManager) {
        simbleeManager = [[SimbleeManager alloc] init];
    }
    return simbleeManager;
}

- (id)init
{
    DLog();
    
    self = [super init];
    
    if (self) {
        _simblees = [[NSMutableArray alloc] init];
        
        _restartInterval = 60.0;
        
        appDelegate = [[UIApplication sharedApplication] delegate];

        NSDictionary *options = nil;
        if (appDelegate.backgroundMode) {
            options = @{ CBCentralManagerOptionRestoreIdentifierKey :  @"SimbleeManager" };
        }
        
        _central = [[CBCentralManager alloc] initWithDelegate:self queue:nil options:options];
        
        blePowerOnBlocks = [[NSMutableArray alloc] init];
    }
    
    return self;
}

#pragma mark - restart timer

- (void) restartTick:(NSTimer *)timer
{
    // restart scan every 60 seconds to prevent foreground slowdown
    
    [_central stopScan];

    NSDictionary *options = nil;
    if (_scanUpdates) {
        options = @{ CBCentralManagerScanOptionAllowDuplicatesKey : @YES };
    }
    
    [_central scanForPeripheralsWithServices:_scanServices options:options];
}

#pragma mark - misc

- (Simblee *)simbleeForPeripheral:(CBPeripheral *)peripheral
{
    for (Simblee *simblee in _simblees) {
        if ([peripheral isEqual:simblee.peripheral]) {
            return simblee;
        }
    }
    return nil;
}

#pragma mark - CentralManagerDelegate methods

- (void)centralManagerDidUpdateState:(CBCentralManager *)aCentral
{
    DLog(@"%ld", (long)[_central state]);

    switch ([_central state]) {
        case CBCentralManagerStateUnknown:
            break;
            
        case CBCentralManagerStateUnsupported:
            [appDelegate bleUnsupported];
            break;
            
        case CBCentralManagerStateUnauthorized:
            [appDelegate bleUnauthorized];
            break;
            
        case CBCentralManagerStatePoweredOff:
            [appDelegate blePoweredOff];
            break;
            
        case CBCentralManagerStatePoweredOn:
            
            if (blePowerOnScanBlock) {
                DLog(@"ble powered on; executing scan block");
                blePowerOnScanBlock();
                blePowerOnScanBlock = nil;
            }
            
            for (void(^block)() in blePowerOnBlocks) {
                DLog(@"ble powered on; executing block");
                block();
            }
            [blePowerOnBlocks removeAllObjects];
            
            [appDelegate blePoweredOn];
            break;
            
        case CBCentralManagerStateResetting:
            break;
    }
}

- (void)centralManager:(CBCentralManager *)central didRetrieveConnectedPeripherals:(NSArray *)peripherals
{
    DLog();
}

- (Simblee *)simbleeFromPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    // DLog();
    
    // peripheral.name has caching issues
    id localName = [advertisementData objectForKey:CBAdvertisementDataLocalNameKey];
    
    Simblee *simblee = [self simbleeForPeripheral:peripheral];
    if (! simblee) {
        simblee = [[Simblee alloc] init];
        
        simblee.simbleeManager = self;

        simblee.peripheral = peripheral;
        peripheral.delegate = simblee;

        simblee.delegate = (id<SimbleeDelegate>)appDelegate.scanViewController;
        
        simblee.name = (localName ? localName : peripheral.name);
        
        simblee.identifier = peripheral.identifier.UUIDString;
        
        simblee.RSSI = RSSI;
        
        simblee.sampleRSSI = [[NSMutableArray alloc] init];
        
        [_simblees addObject:simblee];
    }
    
    if (localName) {
        simblee.name = localName;
    }
    
    id txPower = [advertisementData objectForKey:CBAdvertisementDataTxPowerLevelKey];
    if (txPower) {
        simblee.txPower = txPower;
    }
    
    /*
     // not available in ios 8
     id advChannel = [advertisementData objectForKey:@"kCBAdvDataChannel"];
     if (advChannel) {
     simblee.advChannel = advChannel;
     }
     */
    
    id advServices = [advertisementData objectForKey:CBAdvertisementDataServiceUUIDsKey];
    if (advServices) {
        simblee.advertisementServices = advServices;  // NSArray of NSUUIDs ([[advService objectAtIndex:0] UUIDString])
    }
    
    simblee.advertisementData = nil;
    
    id manufacturerData = [advertisementData objectForKey:CBAdvertisementDataManufacturerDataKey];
    if (manufacturerData) {
        const uint8_t *bytes = [manufacturerData bytes];
        NSUInteger len = [manufacturerData length];
        // skip manufacturer uuid
        NSData *data = [NSData dataWithBytes:bytes+2 length:len-2];
        simblee.advertisementData = data;
    }
    
    NSDate *date = [NSDate date];
    
    if (advertisementData) {
        simblee.lastAdvertisement = date;
        simblee.advertisementPackets++;
    }
    
    if (RSSI && _scanUpdates) {
        if (! simblee.startRSSI) {
            simblee.startRSSI = date;
        }
        // not indeterminate rssi sample
        if (RSSI.intValue != 127) {
            [simblee.sampleRSSI addObject:RSSI];
        }
    }
    
    return simblee;
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    // DLog();

    Simblee *simblee = [self simbleeFromPeripheral:peripheral advertisementData:advertisementData RSSI:RSSI];
    
    [simblee centralManager:central didDiscoverPeripheral:peripheral advertisementData:advertisementData RSSI:RSSI];
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    DLog();
    
    Simblee *simblee = [self simbleeForPeripheral:peripheral];
    if (simblee) {
        [simblee centralManager:central didConnectPeripheral:peripheral];
    }
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    DLog();

    Simblee *simblee = [self simbleeForPeripheral:peripheral];
    if (simblee) {
        [simblee centralManager:central didFailToConnectPeripheral:peripheral error:error];
    }
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    DLog();

    Simblee *simblee = [self simbleeForPeripheral:peripheral];
    if (simblee) {
        [simblee centralManager:central didDisconnectPeripheral:peripheral error:error];
    }
}

- (void)centralManager:(CBCentralManager *)central willRestoreState:(NSDictionary *)dict
{
    DLog();

    NSArray *peripherals = dict[CBCentralManagerRestoredStatePeripheralsKey];
    for (CBPeripheral *peripheral in peripherals) {
        Simblee *simblee = [self simbleeFromPeripheral:peripheral advertisementData:nil RSSI:nil];
        [simblee restoreState];
    }
    
    NSArray *services = [dict objectForKey:CBCentralManagerRestoredStateScanServicesKey];
    NSDictionary *options = [dict objectForKey:CBCentralManagerRestoredStateScanOptionsKey];
    if (services) {
        _scanServices = services;
        _scanUpdates = false;
        if (options) {
            id obj = [options objectForKey:CBCentralManagerScanOptionAllowDuplicatesKey];
            if (obj) {
                _scanUpdates = (bool)obj;
            }
        }
        // at ble power on, bluetooth launch will automatically resume scanning
        _isScanning = true;
    }

    [appDelegate simbleeManager:self restoredState:dict];
}

#pragma mark - SimbleeManager methods

- (void)addBlePowerOnBlock:(void(^)())block
{
    DLog();
    [blePowerOnBlocks addObject:block];
}

- (void)startScan
{
    DLog();

    _isScanning = true;

    if (_central.state != CBCentralManagerStatePoweredOn) {
        DLog(@"delaying startScan until ble powered on");
        SimbleeManager *this = self;
        blePowerOnScanBlock = ^{ [this startScan]; };
        return;
    }

    NSDictionary *options = nil;
    if (_scanUpdates) {
        options = @{ CBCentralManagerScanOptionAllowDuplicatesKey : @YES };
    }

    [_central scanForPeripheralsWithServices:_scanServices options:options];
    
    if (_restartInterval) {
        restartTimer = [NSTimer scheduledTimerWithTimeInterval:_restartInterval
                                                        target:self
                                                      selector:@selector(restartTick:)
                                                      userInfo:nil
                                                       repeats:YES];
    }
}

- (void)stopScan
{
    DLog();

    _isScanning = false;
    
    if (_central.state != CBCentralManagerStatePoweredOn) {
        blePowerOnScanBlock = nil;
        return;
    }
    
    if (restartTimer) {
        [restartTimer invalidate];
        restartTimer = nil;
    }

    [_central stopScan];
}

@end
