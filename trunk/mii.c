/**
 * libmii
 * A library for accessing miis for use on wii homebrew
 *
 * Created by mdbrim (http://wiibrew.org/wiki/User:Mdbrim)
 *
 * Based on the work started by Matthew Bauer (http://wiibrew.org/wiki/User:Mjbauer95)
 *
 * Copyright © 2010 mdbrim
 * =======================
 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <isfs/isfs.h>
#include <string.h>

#include "mii.h"

#define FACELIB_Wii "isfs://shared2/menu/FaceLib/RFL_DB.dat"
int NoOfMiis;													// variable for use in your program. Defines how many Miis are on the system.

char *readmii(char *path){										// ***reads in miidata and stores as buffer***
	long Size;
	FILE *BGM;
	char *buffer;
	BGM = fopen(path, "rb");
	if (BGM==NULL) {
		printf("%s does not exist\n", path);
		return NULL;
	}
	Size = MII_MAX * MII_SIZE + MII_HEADER;
	buffer = (char*) malloc (sizeof(char)*Size);
	fread(buffer, 1, Size, BGM);
	fclose(BGM);
	return buffer;
}

Mii *loadMiis_Wii(){											// ***loadMiis_Wii***
	if (ISFS_Initialize() == 0) {								// Initialize ISFS  (ISFS_SU(); Works for some people but not others ?)
		ISFS_Mount();											// Mounts ISFS
		char *wiidata;											//
		wiidata = readmii(FACELIB_Wii);							// sends Miidata file to function above for reading and stores resulting buffer as wiidata
		ISFS_Unmount();											// Unmounts ISFS
		if(wiidata) return loadMiis(wiidata);					// Sends resulting buffer to the loadMiis function at bottom (not to be confused with loadMii function)
	}
	return NULL;
}

Mii loadMii(int start, char *data) {							// read each block of individual mii data
	Mii mii;
	int c,d;
	char tempChar;
	
	//	start turning the data into the variables we'll need to decipher and build our miis!
	//  see the wiki (wiibrew.org/wiki/Libmii) for a more detailed explanation
	
	mii.invalid = (data[start+0x00] & 0b10000000) >> 7;
	mii.female = (data[start+0x00] & 0b01000000) >> 6;
	mii.month = (data[start+0x00] & 0b00111100) >> 2;
	mii.day = ((data[start+0x00] & 0b00000011) << 3) | ((data[start+0x01] & 0b11100000) >> 5);
	mii.favColor = (data[start+0x01] & 0b00011110) >> 1;
	mii.favorite = (data[start+0x01] & 0b00000001);
	for (c=0, d=0;d<MII_NAME_LENGTH;c++, d++) {
        tempChar = data[start + 0x02 + d * 2 + 1];
        if(tempChar < 0x80)
            mii.name[c] = tempChar;
        else {
            mii.name[c] = (((tempChar) >>  6) & 0x1F) | 0xC0;
            c++;
            mii.name[c] = ((tempChar) & 0x3F) | 0x80;
        }
	}
	mii.name[MII_NAME_LENGTH] = '\0';
	mii.height = data[start + 0x16];
	mii.weight = data[start + 0x17];
	mii.miiID1 = data[start + 0x18];
	mii.miiID2 = data[start + 0x19];
	mii.miiID3 = data[start + 0x1A];
	mii.miiID4 = data[start + 0x1B];
	mii.systemID0 = data[start + 0x1C];
	mii.systemID1 = data[start + 0x1D];
	mii.systemID2 = data[start + 0x1E];
	mii.systemID3 = data[start + 0x1F];
	mii.faceShape = (data[start+0x20] & 0b11100000) >> 5;
	mii.skinColor = (data[start+0x20] & 0b00011100) >> 2;
	mii.facialFeature = ((data[start+0x20] & 0b00000011) << 2) | ((data[start+0x21] & 0b11000000) >> 6);
//	mii.unknown1 = (data[start+0x21] & 0b00111000) << 3;
	mii.mingleOff = (data[start+0x21] & 0b00000100) << 2;
//	mii.unknown2 = (data[start+0x21] & 0b00000010) << 1;
	mii.downloaded = (data[start+0x21] & 0b00000001);
	mii.hairType = (data[start+0x22] & 0b11111110) >> 1;
	mii.hairColor = ((data[start+0x22] & 0b00000001) << 2) | ((data[start+0x23] & 0b11000000) >> 6);
	mii.hairPart = (data[start+0x23] & 0b00100000) >> 5;
//	mii.unknown3 = (data[start+0x23] & 0b00011111);
	mii.eyebrowType = (data[start+0x24] & 0b11111000) >> 3;
//	mii.unknown4 = (data[start+0x24] & 0b00000100) >> 2;
	mii.eyebrowRotation = ((data[start+0x24] & 0b00000011) << 2) | ((data[start+0x25] & 0b11000000) >> 6);
//	mii.unknown5 = (data[start+0x25] & 0b00111111);
	mii.eyebrowColor = (data[start+0x26] & 0b11100000) >> 5;
	mii.eyebrowSize = (data[start+0x26] & 0b00011110) >> 1;
	mii.eyebrowVertPos = ((data[start+0x26] & 0b00000001) << 4) | ((data[start+0x27] & 0b11110000) >> 4);
	mii.eyebrowHorizSpacing = (data[start+0x27] & 0b00001111);
	mii.eyeType = (data[start+0x28] & 0b11111100) >> 2;
//	mii.unknown6 = (data[start+0x28] & 0b00000011);
	mii.eyeRotation = (data[start+0x29] & 0b11100000) >> 5;
	mii.eyeVertPos = (data[start+0x29] & 0b00011111);
	mii.eyeColor = (data[start+0x2A] & 0b11100000) >> 5;
//	mii.unknown7 = (data[start+0x2A] & 0b00010000) >> 4;
	mii.eyeSize = (data[start+0x2A] & 0b00001110) >> 1;
	mii.eyeHorizSpacing = ((data[start+0x2A] & 0b00000001) << 3) | ((data[start+0x2B] & 0b11100000) >> 5);
//	mii.unknown8 = (data[start+0x2B] & 0b00011111);
	mii.noseType = (data[start+0x2C] & 0b11110000) >> 4;
	mii.noseSize = (data[start+0x2C] & 0b00001111);
	mii.noseVertPos = (data[start+0x2D] & 0b11111000) >> 3;
//	mii.unknown9 = (data[start+0x2D] & 0b00000111);
	mii.lipType = (data[start+0x2E] & 0b11111000) >> 3;
	mii.lipColor = (data[start+0x2E] & 0b00000110) >> 1;
	mii.lipSize = ((data[start+0x2E] & 0b00000001) << 3) | ((data[start+0x2F] & 0b11100000) >> 5);
	mii.lipVertPos = (data[start+0x2F] & 0b00011111);
	mii.glassesType = (data[start+0x30] & 0b11110000) >> 4;
	mii.glassesColor = (data[start+0x30] & 0b00001110) >> 1;
//	mii.unknown10 = (data[start+0x30] & 0b00000001);
	mii.glassesSize = (data[start+0x31] & 0b11100000) >> 5;
	mii.glassesVertPos = (data[start+0x31] & 0b00011111);
	mii.mustacheType = (data[start+0x32] & 0b11000000) >> 6;
	mii.beardType = (data[start+0x32] & 0b00110000) >> 4;
	mii.facialHairColor = (data[start+0x32] & 0b00001110) >> 1;
	mii.mustacheSize = ((data[start+0x32] & 0b00000001) << 3) | ((data[start+0x33] & 0b11100000) >> 5);
	mii.mustacheVertPos = (data[start+0x33] & 0b00011111);
	mii.mole = (data[start+0x34] & 0b10000000) >> 7;
	mii.moleSize = (data[start+0x34] & 0b01111000) >> 3;
	mii.moleVertPos = ((data[start+0x34] & 0b00000111) << 2) | ((data[start+0x35] & 0b11000000) >> 6);
	mii.moleHorizPos = (data[start+0x35] & 0b00111110) >> 1;
//	mii.unknown11 = (data[start+0x35] & 0b00000001);
	for (c=0, d=0;d<MII_NAME_LENGTH;c++, d++) {
        tempChar = data[start + 0x36 + d * 2 + 1];
        if(tempChar < 0x80)
            mii.creator[c] = tempChar;
        else {
            mii.creator[c] = (((tempChar) >>  6) & 0x1F) | 0xC0;
            c++;
            mii.creator[c] = ((tempChar) & 0x3F) | 0x80;
        }
	}
	mii.creator[MII_NAME_LENGTH] = '\0';
	
	
	if(mii.eyeType==0 && mii.eyeColor==0 && mii.eyeSize==0 && mii.eyebrowType==0 && mii.eyebrowRotation==0) mii.exists=0;	//as each mii is processed, check to see if these key variables are all 0s.
																																//If they are, then this mii doesn't exist, and is just a block of 0s.
																																//These variables were choosen because it is unlikely that someone would
																																//create an actual mii with all these parameters being 0. Therefore: Not a Mii.
	else mii.exists=1;

	return mii;
}
//		Mii data is stored in a "block of data" big enough to hold 100 miis.  If you have 100 miis, all blocks will be full of valid data.
//		Miis are stored in the order in which they were created. If you have less than 100 miis, then you will have x blocks of valid data,
//		followed by 100-x blocks of 0s.  Because of this, the mii buffer is always the same size, it just may have 0 value miis at the end.
//		Likewise, if you had 100 miis and then erased the 54th mii you made... there would be a "hole" in the data at the 54th slot full of 0s.
//		Because of this, the mii buffer is always saying 100miis, but there could be slots that have 0s indicating that no mii exists there.
//		The below code checks each block, and only grabs the non zero blocks, sends them up to the loadMii function above for processing, and stores
//		the result in the array "miis".  Therefore the array miis will only have valid mii data in it and will contain the actuall number of miis on
//      the wii enabling us to use this in our program to manipulate the valid miis. 


Mii *loadMiis(char *data){								// ***takes resulting miidata buffer from top***
	static Mii miis[MII_MAX];							// creates array of 100 elements (the most miis the wii can hold)

	if (data[0] == 'R' && data[1] == 'N' && data[2] == 'O' && data[3] == 'D'){
		int start;										// Miis are stored in the buffer as a continuous block of data.
		int n = 0;										// Each mii takes up "mii_size" worth of space and occurs after
		int cur = 0;									//      the initial 4 blocks of header data.
		Mii mii;										//
		for (n=0;n < 100;n++){							// starting with n=0 and going to the end (n=100)
			start = n * MII_SIZE + 4;					// set the start point at the beginning of each mii data block
			mii = loadMii(start, data);					// send each individual block of mii data up to the loadMii function for processing and store the result as mii
			if(mii.exists==1) {						// if the mii passes the "exist test" up in loadMii...
				miis[cur] = mii;						// then store that miidata block in the miis array
				cur++;									// and increases the mii count.
			}
		}
		NoOfMiis = cur;									// at the end of checking all 100 miidata blocks, the "cur" value will be the actual number of valid miis
	} else {
		printf("Mii version %c%c%c%c is not compatible with libmii\n", data[0], data[1], data[2], data[3]);
	}
	return miis;
}
