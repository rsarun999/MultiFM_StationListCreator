
/****************************************************************************************
 *	 FileName	 : Gui_RadioMainScreen.cpp
 *	 Description : 
 ****************************************************************************************
****************************************************************************************/
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "RdsLookUpTable.h"

void RdsLookUpTable::SaveRdsList(RDS_DB_LIST pRdsList[])
{
	uint32_t RdsListSize = (sizeof(RDS_DB_LIST)*MAX_RDS_DB);
	ofstream arrayData;
	arrayData.open("c:\\rds_list.txt", ofstream::out | ofstream::app);
	if (arrayData.is_open())
	{
		for(int k=0;k<MAX_RDS_DB;k++)
		{
        	arrayData<<pRdsList[k].Pi<<pRdsList[k].CN<< endl; //Outputs array to txtFile
    	}
		arrayData.close();
	}
}

void RdsLookUpTable::RDSListPiUpdate(uint16_t updatePi, uint8_t u8_Ecc)
{
	uint8_t index = 0;
	uint8_t i = 0;
	uint8_t last = 0;
	uint8_t RdsListPiLastNum = 0;
	if (RdsListPiLastNum >= (MAX_RDS_DB - 1))
	{
		last = 0;
	}
	else
	{
		last = RdsListPiLastNum + 1;
	}
	index = NO_EXIST;
	for (i = 0; i < MAX_RDS_DB; i++)
	{
		if (RdsList[i].Pi == updatePi)
		{
			index = i;
			break;
		}
	}
	/* There is no PI in list*/
	if (index == NO_EXIST)
	{
		for (i = 0; i < MAX_RDS_DB; i++)
		{
			if (RdsList[i].Pi == 0)
			{
				index = i;
				break;
			}
		}
		/* 1. EMPTY LIST SLOT*/
		if (index != NO_EXIST)
		{
			bool duplication = false;
			memset(&RdsList[index], 0x00, sizeof(RDS_DB_LIST));
			RdsList[index].Pi = updatePi;
			//LOGV("++ [%s]: RdsList[index].Pi = updatePi:%d \r\n", __FUNCTION__,updatePi);
			if (mRdsLUT->GetPiType(RdsList[index].Pi) != 2)
			{
				RdsList[index].CN = mRdsLUT->GetCN(RdsList[index].Pi);
			}
			else
			{
				RdsList[index].CN = usedCountry();
			}
			duplication = mRdsLUT->GetPs(RdsList[index].Pi, RdsList[index].CN, RdsList[index].Ps);
			if (RdsList[index].Ps[0] != 0)
			{
				if (duplication == true)
				{
					RdsList[index].MaintenanceCount = 5;
				}
				else
				{
					RdsList[index].MaintenanceCount = 15;
				}
			}
			SaveRdsList(RdsList);
		}
		/* 2. REPLACE LIST SLOT*/
		else
		{
			for (i = 0; i < MAX_RDS_DB; i++)
			{
				/*Check Preset PI */
				for (uint8_t j = 0; j < INTEGRATED_PRESET_NUM; j++)
				{
					if (1) //RdsList[last].Pi == mRadioService->Radio.u16_PresetPI[j])
					{
						index = j;
					}
				}
				if (index == NO_EXIST)
				{
					bool duplication = false;
					/* Delete PI*/
					RdsList[last].Pi = 0;
					RdsList[last].Pty = 0;
					RdsList[last].CN = 0;
					memset(&RdsList[last].Ps, 0x00, 8);
					RdsList[last].MaintenanceCount = 0;
					memset(&RdsList[last].Af, 0x00, EXTENDED_AF);
					/*Register PI*/
					RdsList[last].Pi = updatePi;
					if (mRdsLUT->GetPiType(RdsList[last].Pi) != 2)
					{
						RdsList[last].CN = mRdsLUT->GetCN(RdsList[last].Pi);
					}
					else
					{
						RdsList[last].CN = usedCountry();
					}
					duplication = mRdsLUT->GetPs(RdsList[last].Pi, RdsList[last].CN, RdsList[last].Ps);
					if (RdsList[last].Ps[0] != 0)
					{
						if (duplication == true)
						{
							RdsList[last].MaintenanceCount = 5;
						}
						else
						{
							RdsList[last].MaintenanceCount = 15;
						}
					}
					RdsListPiLastNum = last;
					//LOGV("++ [%s]: RdsList[index].Pi = updatePi:%d \r\n", __FUNCTION__,updatePi);
					SaveRdsList(RdsList);
					break;
				}
				if (last >= (MAX_RDS_DB - 1))
				{
					last = 0;
				}
				else
				{
					last++;
				}
				index = NO_EXIST;
			}
		}
	}
}

uint8_t RdsLookUpTable::usedCountry()
{
	/****************************************************************************************
	 *	 FunctionName	: usedCountry
	 *	 Description	: Determines the most used country in RDS station list with PI_type:0
	****************************************************************************************/
	int max_count = 0;

	for (int i = 0; i < MAX_RDS_DB; i++)
	{
		int count = 1;
		for (int j = i + 1; j < MAX_RDS_DB; j++)
			if ((RdsList[i].CN == RdsList[j].CN) && (mRdsLUT->GetPiType(RdsList[i].Pi) == 0) && (mRdsLUT->GetPiType(RdsList[j].Pi) == 0))
				count++;
		if (count > max_count)
			max_count = count;
	}

	for (int i = 0; i < MAX_RDS_DB; i++)
	{
		int count = 1;
		for (int j = i + 1; j < MAX_RDS_DB; j++)
			if ((RdsList[i].CN == RdsList[j].CN) && (mRdsLUT->GetPiType(RdsList[i].Pi) == 0) && (mRdsLUT->GetPiType(RdsList[j].Pi) == 0))
				count++;
		if (count == max_count)
			return RdsList[i].CN;
	}
	return 1;
}

RdsLookUpTable::RdsLookUpTable()
{
	//mRdsLUT = nullptr;
	//ALOGV("++ [%s] is called \r\n", __FUNCTION__);
}

void RdsLookUpTable::UpdateLUT(uint16_t u16_PI, uint8_t CN, uint8_t PsName[])
{
	uint32_t u32_cnt = 0;
	uint32_t RdsLutSize = (sizeof(RDS_LUT) * MAX_RDS_LUT);
	uint8_t failed = 0;
	int32_t fd = -1;
	uint8_t pi_CN = 0;

	if (IsInitialized == 1)
	{
		if (mRdsLUT_table != nullptr)
		{
			RDS_LUT *pTarget = nullptr;
			uint32_t u32_Duplication = 0;
			uint8_t update = 0;
			//ALOGV("++ [%s] DATA LOAD \r\n", __FUNCTION__);
			auto range = Lutmap.equal_range(u16_PI);
			for (auto i = range.first; i != range.second; ++i)
			{
				u32_Duplication++;
				pTarget = i->second;
				if (pTarget != nullptr)
				{
					/* Single PI Code match, status=0 or 1, copy the PS*/
					if (pTarget->PiType != 2)
					{
						for (uint8_t u8_cnt2 = 0; u8_cnt2 < 8; u8_cnt2++)
						{
							pTarget->PsName[u8_cnt2] = PsName[u8_cnt2];
						}
						update = 1;
						//ALOGV("++ [%s] update single PI PS to LUT:%s \r\n", __FUNCTION__,PsName);
						break;
					}
					/*Multiple PI case, access PSN only from PI out of all valid country in RDS DB*/
					else
					{
						pi_CN = pTarget->CN;
						if (pi_CN == CN)
						{
							for (uint8_t u8_cnt2 = 0; u8_cnt2 < 8; u8_cnt2++)
							{
								pTarget->PsName[u8_cnt2] = PsName[u8_cnt2];
							}
							update = 1;
							//ALOGV("++ [%s] update Multi-PI PS to LUT:%s \r\n", __FUNCTION__,PsName);
							break;
						}
					}
				}
			}
			if (update == 1)
			{				
				ofstream arrayData1;
				arrayData1.open("c:\\lut_list.txt", ofstream::out | ofstream::app);
				if (arrayData1.is_open())
				{
					for(int k=0;k<MAX_RDS_LUT;k++)
					{
        				arrayData1<<RdsLUT[k].CN <<" " << RdsLUT[k].PiType << RdsLUT[k].PiCode << RdsLUT[k].PsName << endl; //Outputs array to txtFile
    				}
					arrayData1.close();
				}
			}
		}
	}
}
void RdsLookUpTable::InitLUT()
{
	IsInitialized = 1;	
}

uint8_t RdsLookUpTable::GetCN(uint16_t Pi)
{
	RDS_LUT *pTarget = nullptr;
	auto range = Lutmap.equal_range(Pi);
	for (auto i = range.first; i != range.second; ++i)
	{
		pTarget = i->second;
		if (pTarget != nullptr)
		{
			return pTarget->CN;
		}
	}
	return 0;
}

uint8_t RdsLookUpTable::GetPiType(uint16_t Pi)
{
	RDS_LUT *pTarget = nullptr;
	auto range = Lutmap.equal_range(Pi);
	for (auto i = range.first; i != range.second; ++i)
	{
		pTarget = i->second;
		if (pTarget != nullptr)
		{
			return pTarget->PiType;
		}
	}
	return 0;
}

bool RdsLookUpTable::GetPs(uint16_t Pi, uint8_t CN, uint8_t Ps[])
{
	bool b_result = false;
	RDS_LUT *pTarget = nullptr;
	uint32_t u32_Duplication = 0;
	uint8_t u8_cnt = 0;
	uint8_t pi_CN = 0;
	uint8_t u8_ProtectMultipleCopy = 0;
	auto range = Lutmap.equal_range(Pi);
	for (auto i = range.first; i != range.second; ++i)
	{
		u32_Duplication++;
		pTarget = i->second;
		if (pTarget != nullptr)
		{
			/* Single PI Code match, status=0 or 1, copy the PS*/
			if (pTarget->PiType != 2)
			{
				if (u8_ProtectMultipleCopy == 0)
				{
					for (u8_cnt = 0; u8_cnt < 8; u8_cnt++)
					{
						Ps[u8_cnt] = pTarget->PsName[u8_cnt];
					}
					//ALOGV("++ [%s] Get Matching PS from LUT:%s \r\n", __FUNCTION__,Ps);
					u8_ProtectMultipleCopy = 1;
				}
			}
			/*Multiple PI case, access PSN only from PI out of valid country in RDS DB*/
			else
			{
				pi_CN = pTarget->CN;
				if (pi_CN == CN)
				{
					for (u8_cnt = 0; u8_cnt < 8; u8_cnt++)
					{
						Ps[u8_cnt] = pTarget->PsName[u8_cnt];
					}
					//ALOGV("++ [%s] Multi-PI Get proper PS from LUT:%s \r\n", __FUNCTION__,Ps);
				}
			}
		}
	}
	/*Multiple PI code exists, search for */
	if (u32_Duplication > 1)
	{
		b_result = true;
	}
	return b_result;
}
