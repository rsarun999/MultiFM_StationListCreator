#include "RdsLookUpTable.h"

int main () 
{	   
    RdsLookUpTable mRdsLUT1; //= new RdsLookUpTable();
    //mRdsLUT1 = new RdsLookUpTable();
	//uint32_t RdsListSize = (sizeof(mRdsLUT1.RdsList)*MAX_RDS_DB);
	uint8_t PsName[8] = {0x20,0x20,0x45,0x43,0x43,0x41,0x20,0x20};
	uint8_t u8_data[8] = {0};
	uint8_t* ptr = &PsName[0];

	mRdsLUT1.InitLUT();
	
	//test vectors
	mRdsLUT1.RDSListPiUpdate(0x1003, 0xE0);
	mRdsLUT1.RDSListPiUpdate(0x1002, 0xE0);
	mRdsLUT1.RDSListPiUpdate(0x1005, 0xE0);
	
	//update LUT call
	mRdsLUT1.UpdateLUT(0x1005, mRdsLUT1.usedCountry(), ptr);

    return 0;
} 