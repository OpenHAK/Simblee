/*
 * Copyright (c) 2016 RF Digital Corp. All Rights Reserved.
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

 /*
  * Table of values is drawn and updated on Simblee
  */

#include <SimbleeForMobile.h>

//data
int data[] = {25,24,121,5,46}; //random data
uint8_t val[5];

void setup(){
  SimbleeForMobile.begin();
}

void loop(){
  if(SimbleeForMobile.updatable){
    for(int x = 0; x < 5; x++){
    SimbleeForMobile.updateValue(val[x],data[x]);
    }
  }
  SimbleeForMobile.process();
}

int i = 52; //spacing value
int16_t value = 0;

void ui(){
  SimbleeForMobile.beginScreen();
  SimbleeForMobile.drawText(100,40,"TABLE",BLACK,40);
  
  //draw table outline
  SimbleeForMobile.drawRect(1,100,1,100,BLACK);
  SimbleeForMobile.drawRect(1+i,100,1,100,BLACK);
  SimbleeForMobile.drawRect(1+(2*i),100,1,100,BLACK);
  SimbleeForMobile.drawRect(1+(3*i),100,1,100,BLACK);
  SimbleeForMobile.drawRect(1+(4*i),100,1,100,BLACK);
  SimbleeForMobile.drawRect(1+(5*i),100,1,100,BLACK);
  SimbleeForMobile.drawRect(319,100,1,100,BLACK);
  
  SimbleeForMobile.drawRect(1,100,319,1,BLACK);
  SimbleeForMobile.drawRect(1,152,319,1,BLACK);
  SimbleeForMobile.drawRect(1,204,319,1,BLACK);


  //labels
  SimbleeForMobile.drawText(65,110,"[0]",BLACK,24);
  SimbleeForMobile.drawText(65+i,110,"[1]",BLACK,24);
  SimbleeForMobile.drawText(65+(2*i),110,"[2]",BLACK,24);
  SimbleeForMobile.drawText(65+(3*i),110,"[3]",BLACK,24);
  SimbleeForMobile.drawText(65+(4*i),110,"[4]",BLACK,24);

  SimbleeForMobile.drawText(3,162,"Value",BLACK,20);
  
  
  //values
  val[0] = SimbleeForMobile.drawText(58,166,value,BLACK);
  val[1] = SimbleeForMobile.drawText(58+i,166,value,BLACK);
  val[2] = SimbleeForMobile.drawText(58+(i*2),166,value,BLACK);
  val[3] = SimbleeForMobile.drawText(58+(i*3),166,value,BLACK);
  val[4] = SimbleeForMobile.drawText(58+(i*4),166,value,BLACK);
  
  SimbleeForMobile.endScreen();
}

