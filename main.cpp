#include <stdio.h>
#include <math.h>
#include <graphics.h>
#include <conio.h>
#include <string.h>
#include <Windows.h>
#include <time.h>

#include "data.h"
#include "DataTransition.h"
#include "func.h"
#include "LCD.h"
#include "Display.h"
#include "KeyDeal.h"
#include "ReportData.h"
#include "GetChineseBMP.h"
#include "plc.h"

//#include "CRC.h"
#include "Ccd2ini.h"

#include "fileOper.h"

#include "snake.h"

#include "other.h"

#include "SyncMeter.h"

#define NVM_code

//char EepromFileName[] = "Eeprom.bin";
//FILE * EepromFilePoint;

//#define MainGraph

void Windows_SetSysStatus()
{
	DeviceTypeF = 1;
	InTiaoShiF = 0;
	MaintStateF = 0;
	GpsInF = 1;
	LightByte.all = 0;
	//DisMeasNum = 0;
}

#if 1
char * ReadSpeacialLine(FILE * FilePoint, unsigned int i, char * StrBuf)
{
	//FILE * FilePoint;
	unsigned int WhichLine = i;             //指定要读取哪一行
	unsigned int CurLine = 0;             //当前读取的行

	if(FilePoint == NULL) //判断文件是否存在及可读
	{
		printf("FilePoint error!\n");
		return NULL;
	}

	rewind(FilePoint);

	while (!feof(FilePoint))
	{
		if (CurLine == WhichLine)		//如果读取的是当前行
		{
			if(fgets(StrBuf,512,FilePoint) != NULL)  //读取一行
			{
				//printf("i=%d CurLine=%d  %s",i,CurLine,StrBuf); //输出
				//fclose(FilePoint);                     //关闭文件
				if((StrBuf[0] == '/') && (StrBuf[1] == '/'))
					return NULL;
				if(StrBuf[0] == ';')
					return NULL;
				else
					return StrBuf;
			}
			else
			{
				//printf("i=%d CurLine=%d  文件结尾\n",i,CurLine); //输出
				//fclose(FilePoint);                     //关闭文件
				return NULL;
			}
		}
		fgets(StrBuf,512,FilePoint);  //读取一行,并定位到下一行
		CurLine++;
	}
	//printf("i=%d CurLine=%d 超过最大行数\n",i,CurLine); //输出
	return NULL;
}


#else
char * ReadSpeacialLine(char * filename, unsigned int i, char * StrBuf)
{
	FILE * FilePoint;
	unsigned int WhichLine = i;             //指定要读取哪一行
	unsigned int CurLine = 0;             //当前读取的行

	if((FilePoint = fopen(filename,"r")) == NULL) //判断文件是否存在及可读
	{
		printf("%s open error!\n");
		return NULL;
	}

	while (!feof(FilePoint))
	{
		if (CurLine == WhichLine)		//如果读取的是当前行
		{
			if(fgets(StrBuf,512,FilePoint) != NULL)  //读取一行
			{
				//printf("i=%d CurLine=%d  %s",i,CurLine,StrBuf); //输出
				fclose(FilePoint);                     //关闭文件
				if((StrBuf[0] == '/') && (StrBuf[1] == '/'))
					return NULL;
				if(StrBuf[0] == ';')
					return NULL;
				else
					return StrBuf;
			}
			else
			{
				//printf("i=%d CurLine=%d  文件结尾\n",i,CurLine); //输出
				fclose(FilePoint);                     //关闭文件
				return NULL;
			}
		}
		fgets(StrBuf,512,FilePoint);  //读取一行,并定位到下一行
		CurLine++;
	}
	//printf("i=%d CurLine=%d 超过最大行数\n",i,CurLine); //输出
	fclose(FilePoint);                     //关闭文件
	return NULL;
}
#endif

unsigned short SearchLabelName(char * LabelName, FILE * FilePoint)
{
	unsigned char FindF=0;		//找到标签的标记
	unsigned short LineNum=1;		//行计数
	char StrLine[512];			//用于临时存储读出的字符串

	rewind(FilePoint);

	while (!feof(FilePoint))
	{
		if(fgets(StrLine,512,FilePoint) != NULL)  //读取一行
		{
			if(strstr(StrLine,LabelName) != NULL)			//查找读取出的行字符串StrLine是否包含LabelName
			{
				return LineNum;
				break;
			}
		}
		else
		{
			//printf("i=%d CurLine=%d  文件结尾\n",i,CurLine); //输出
			//fclose(PowerOnFilePoint);                     //关闭文件
		}
		LineNum++;
	}
	return 0;
}



void int_ta1()
{
	unsigned char Line,i,j,CharTemp,NoLpB,TempLpNum;
	uint16 Tempbuf[20] = {0xffff};
//	uint16 IntTemp;
	
	//////////////////////
//	uint16 u16len;
//	uint8 u8TxRpt[CanMaxLen];
	////////////////////

#if 0
	uart0_rec_intta1();  // wwb add 2014-03-26

	Ta1Time = 0;

	
	CycTime++;
	if (CycTime >= 3000)		//5分钟 卡在主循环
	{
		Reset_soft(3);
	}
#endif
	
#if 1
	if (CycleDisF1 == 1)		//2秒 主动上送报文循环显示标志位 
	{
		time1++;
		if (time1 >= 20)
		{
			time1 = 0;
			CycleDisF2 = 1;
		}
	}
#endif
	
#if 0
	if (InMainCyc == 1)					//2分钟 死循环中判断上电是否完成
	{
		PowSearchTime++;
		if(PowSearchTime >= 1200)						//2 min 
		{
			PowSearchTime = 0;
			UR_judge_powon();
			
			if(PowOnFlag == 0)
			{
				SendCom = 0x31;
				#ifdef _ADD_MMI_OPTION_
				PowOnPara = 0x0d;
				#else
				PowOnPara = 0x00;
				#endif
				UR_com_send();
				InitComF = 0;
			}
		}
	}

	if (InMainCyc == 1)							//死循环
	{
		if(SerialPortLive == 0)						//如果串口死了
		{
			RptSearchTime++;
			if(RptSearchTime >= 400)						//40秒 重启
			{
				RptSearchTime = 0;
				UR_judge_powon();
			
				if (PowOnFlag == 1)
				{			
					SendCom = 0x31;
					#ifdef _ADD_MMI_OPTION_
					PowOnPara = 0x0d;
					#else
					PowOnPara = 0x00;
					#endif
					UR_com_send();
					InitComF = 0;
				}
				Reset_soft(4);
			}
		}
		else
		{
			RptSearchTime = 0;
		}
	}	
	
	
	

	if (InMainCyc == 1)									//30分钟 RAM自检标记
	{
		RamSelfTime++;
		if(RamSelfTime >= 18000)						//half hour
		{
			RamSelfTime = 0;
			C_Ram_ErrF = 1;
		}
	}

	//add by zhangwei 2009-3-30 19:01:51
	#define _CtrlTimeDelay 300
	if(CtrlState == 1)								//30秒 遥控状态 30秒内收到26报文才会报操作成功
	{
		CtrlTimeCnt++;
		if (CtrlTimeCnt >= _CtrlTimeDelay)						//30秒
		{
			CtrlState = 0;
			CtrlTimeCnt = 0;
		}
	}
#endif

#if 0
	if ((PWaitF == 1) && (KeyStatus == 0xff))		//请等待标记
	{
		time2++;
		time8++;
		ProtocalTime++;
		if (time8 >= 300)           //超过30秒后，返回
		{
			PWaitF = 0;
			time8 = 0;
			time2 = 0;
			ProtocalTime = 0;
			if(PathStack[PathStaP - 2] == code_XieBoLiang)			//谐波量
			{
				RptDisF2 = 1;
				LcdClearF = 1;
				KeyStatus = 0x17;
				CursorDisF2 = 1;
				PathStaP--;
				PWaitF = 0;
			}
			else if (Send09F == 1)		//定值发送
			{
				PWaitF = 0;
				KeyStatus = 0x03;
				RptDisF2 = 1;
				CursorDisF2 = 1;
				LcdClearF = 1;
				PathStaP--; 	
				SetDisF = 0;
				Send09F = 0;
			}
			else
			{
				PWaitF = 0;
				UR_return_LastMenu();		
			}
		}
		else
		{
			if (time2 >= 10)			//1秒
			{
				time2 = 0;
				if (InWriteF == 0)				//没有驱动在操作液晶
				{
					if((InProtocalSet == 1)&&(ProtoWaitF == 1))		//召唤定值或召唤间隔层GOOSE报文统计或召唤规约  并 规约等待
					{
						if(ProtocalTime >= 100)
						{

							ProtocalTime = 0;
							if(ProtocalF ==1)
							{						  	
								Protocal_ErrF = 1;
								UR_NoProtocal_dis();
							}
							if(InGooseRcv == 1)
							{
								Goose_ErrF = 1;
								UR_NoGoose_dis();
							}														
						}
						else
						{
							if(PwaitNum >= 5)
								PwaitNum = 0; 
							UR_OtherGra_char(PwaitNum);
							PwaitNum++;
						}

					}				
					else						//0.5秒 等待漏斗转动一次
					{
						if(PwaitNum >= 5)
							PwaitNum = 0; 
						UR_OtherGra_char(PwaitNum);
						PwaitNum++;
					}
				}
			}			
		}
	}
#endif

#if 0
	if (C_Rom_NeedPartErrF == 1)		//跟自检有关吗?
	{
		C_RomTime++;
		if (C_RomTime >= 3000)			//5 min
		{
			C_Rom_NeedPartErrF = 0;
			C_RomTime = 0;
			C_Rom_PartErrF = 1;
		}
	}
#endif
	
#if 0
	#ifdef _ADD_MMI_OPTION_
	if (1 == PowOnField.CfgOfMmi)			//需要召唤MMI选项??
	{
		PowOnWaitTime.CfgOfMmi++;
		if(PowOnWaitTime.CfgOfMmi >= 60)	//6秒
		{
			PowOnField.CfgOfMmi = 0;
			PowOnWaitTime.CfgOfMmi = 0;
			SendCom = 0x31;
			PowOnCallNum++;
			if (PowOnCallNum >= 3)
			{
				PowOnPara = 0x00;
				PowOnCallNum = 0;
			}
			else
				PowOnPara = 0x0d;
			UR_com_send();
		}
	}
	#endif
	
	
	if (PowOnField.CpuName == 1)				//6秒 发送31-00 召唤CPU名称
	{
		PowOnWaitTime.CpuName++;
		if (PowOnWaitTime.CpuName >= 60)
		{
			PowOnField.CpuName = 0;
			PowOnWaitTime.CpuName = 0;
			SendCom = 0x31;
			PowOnCallNum++;
			if (PowOnCallNum >= 3)
			{
				PowOnPara = 0x01;
				PowOnCallNum = 0;
			}
			else
			{
				#ifdef _ADD_MMI_OPTION_
				PowOnPara = 0x0d;
				#else
				PowOnPara = 0x00;
				#endif
			}
			UR_com_send();
		}
	}

	if (PowOnField.RunCpu == 1)					//6秒 发送31-01 召唤CPU运行状态
	{
		PowOnWaitTime.RunCpu++;
		if (PowOnWaitTime.RunCpu >= 60)
		{
			PowOnField.RunCpu = 0;
			PowOnWaitTime.RunCpu = 0;
			SendCom = 0x31;
			PowOnCallNum++;
			if (PowOnCallNum >= 3)
			{
				PowOnPara = 0x02;
				PowOnCallNum = 0;
			}
			else
				PowOnPara = 0x01;
			UR_com_send();
		}
	}

	if (PowOnField.LpChna == 1)		//6秒 发送31-02 召唤压板中文
	{
		PowOnWaitTime.LpChna++;
		if (PowOnWaitTime.LpChna >= 60)
		{
			PowOnField.LpChna = 0;
			PowOnWaitTime.LpChna = 0;
			SendCom = 0x31;
			PowOnCallNum++;
			if (PowOnCallNum >= 3)
			{
				PowOnPara = 0x03;
				PowOnCallNum = 0;
			}
			else
				PowOnPara = 0x02;

			UR_com_send();
		}
	}

	if (PowOnField.CycChna == 1)	//6秒 发送31-03 召唤循环模拟量中文
	{
		PowOnWaitTime.CycChna++;
		if (PowOnWaitTime.CycChna >= 60)
		{
			PowOnField.CycChna = 0;
			PowOnWaitTime.CycChna = 0;
			SendCom = 0x31;
			PowOnCallNum++;
			if (PowOnCallNum >= 3)
			{
				PowOnPara = 0x04;
				PowOnCallNum = 0;
			}
			else
				PowOnPara = 0x03;
			UR_com_send();
		}
	}

	if (PowOnField.InputChna == 1)		//6秒 发送31-04 召唤开入配置
	{
		PowOnWaitTime.InputChna++;
		if (PowOnWaitTime.InputChna >= 60)
		{
			PowOnField.InputChna = 0;
			PowOnWaitTime.InputChna = 0;
			SendCom = 0x31;
			PowOnCallNum++;
			if (PowOnCallNum >= 3)
			{
				PowOnPara = 0x05;
				PowOnCallNum = 0;
			}
			else
				PowOnPara = 0x04;
			UR_com_send();
		}
	}

	if (PowOnField.OutChna == 1)		//6秒 发送31-05 召唤开出传动中文
	{
		PowOnWaitTime.OutChna++;
		if (PowOnWaitTime.OutChna >= 60)
		{
			PowOnField.OutChna = 0;
			PowOnWaitTime.OutChna = 0;
			SendCom = 0x31;
			PowOnCallNum++;
			if (PowOnCallNum >= 3)
			{
				PowOnPara = 0x08;
				PowOnCallNum = 0;
			}
			else
				PowOnPara = 0x05;

			UR_com_send();
		}
	}

	if (PowOnField.SetChna == 1)		//6秒 发送31-08 召唤定值索引及中文
	{
		PowOnWaitTime.SetChna++;
		if (PowOnWaitTime.SetChna >= 60)
		{
			PowOnField.SetChna = 0;
			PowOnWaitTime.SetChna = 0;
			SendCom = 0x31;
			PowOnCallNum++;
			if (PowOnCallNum >= 3)
			{
				PowOnPara = 0x09;
				PowOnCallNum = 0;
			}
			else
				PowOnPara = 0x08;

			UR_com_send();
		}
	}

	if (PowOnField.ChanChna == 1)		//6秒 发送31-09 召唤模拟量配置(菜单显示)
	{
		PowOnWaitTime.ChanChna++;
		if (PowOnWaitTime.ChanChna >= 60)
		{
			PowOnField.ChanChna = 0;
			PowOnWaitTime.ChanChna = 0;
			SendCom = 0x31;
			PowOnCallNum++;
			if (PowOnCallNum >= 3)
			{
				PowOnPara = 0x0a;
				PowOnCallNum = 0;
			}
			else
			{
				PowOnPara = 0x09;
				ask09flg = 0x59;        //add 2015-5-9 9:21
				ask09t = 0;             //add 2015-5-9 9:21
			}
			UR_com_send();
		}
	}

	if (PowOnField.OtherInput == 1)		//6秒 发送31-05 召唤其他间隔开入地址
	{
		PowOnWaitTime.OtherInput++;
		if(PowOnWaitTime.OtherInput >= 60)
		{
			PowOnField.OtherInput = 0;
			PowOnWaitTime.OtherInput = 0;
			PowOnCallNum++;
			if(PowOnCallNum >= 3)
			{
				PowOnCallNum = 0;
				SendCom = rpt50[_E2Content];		//add by zhangwei 2009-11-4 16:29:45
				UR_com_send();		
			}
			else
			{
				SendCom = 0x31;
				PowOnPara = 0x0a;
				UR_com_send();
				ask0aflg = 0x5a;
				ask0at = 0;
			}
		}
	}
#endif

#if 0
	if(PowOnField.MainG == 1)			//貌似进不来
	{
		PowOnWaitTime.MainG++;
		if (PowOnWaitTime.MainG >= 60)
		{
			PowOnField.MainG = 0;
			PowOnWaitTime.MainG = 0;
			PowOnCallNum++;
			if (PowOnCallNum >= 3)
			{
				PowOnCallNum = 0;
			}
			else
			{
				//SendCom = 0x50;
				SendCom = rpt50[_E2Content];		//add by zhangwei 2009-11-4 16:29:45
				UR_com_send();
			}
		}
	}
#endif
	
#if 0
	C_LcdTime++;  
	if (C_LcdTime >= 300)					//每30秒 液晶初始化一次??????
	{
		if (InWriteF == 0)
		{
			//r_lcd_init(0); // wwb delet for test.can not open!
			C_LcdTime = 0;
		}
	} 
#endif

	if((LocateConF == 1) && (GraMeaF == 0) && (RLStatusF != 0))			//主接线图操作状态，0.5秒 可变元件闪烁
	{
		MainFlashTime++;
		if(MainFlashTime == 5)
		{
			if((InWriteF == 0) && (KeyStatus != 0x08))
				UR_MainEle_cur(EleCurNo_M, ele_temp, ele2_temp);
		}
		if(MainFlashTime >= 10)
		{
			if((InWriteF == 0) && (KeyStatus != 0x08))
				UR_MainEle_char(EleCurNo_M, ele_temp, ele2_temp);
			MainFlashTime = 0;
		}
	}

#if 0
	if(CallComTimeF == 1)			//10秒 循环召唤报文0x30置可发送标记
	{
		CallComTime++;
		// 10秒向Master召唤一次报文, 不受控制字 SecCall 的约束
		{
			if(CallComTime >= 100)					//10s
			{
				CallComF = 1;
				CallComTimeF = 0;
				CallComTime = 0;
			}
		}
	}
#endif
	
#if 0
	if ((InMainCyc == 1) && (PCExist == 0))			//在死循环内 并且 没有与PC通讯  发送循环召唤报文
	{
		if (CallComF == 1)		//10s
		{
			if (SendSioF == 0)		//串口不发送报文
			{
				if ((AllowSendF == 1)&&(AllowSendF_1==0))	
				{
/////////////////////////////////////////////
					u16len = 0;
					u8TxRpt[u16len++] = 0x05;
					u8TxRpt[u16len++] = TargetAdr;
					u8TxRpt[u16len++] = SourseAdr;
					u8TxRpt[u16len++] = 0x30;
					u8TxRpt[u16len++] = TimeComP;
					u8TxRpt[u16len++] = 0x30 + TimeComP;
					can0_WriteTxBuff(1,u8TxRpt);
/////////////////////////////////////////////
					if(TimeComP >= 0x06)
					{
						TimeComP = 0x00;
					}
					else
					{
						TimeComP++;
					}
					
					if(TimeComP == 0x04 )
						TimeComP++;
					CallComF = 0;
					CallComTimeF = 1;	
					CallComTime = 0; 
					C_Sio_ErrNum++;
				}
			}
		}
	}
#endif

#if 0
	if (AllowSendStartTimeF == 1)		//发送后开始计时标志位，
	{
		time6++;
		if (time6 >= 50)				//5s 等到5秒如果没有收到报文则
		{
			AllowSendStartTimeF = 0;	//MMI自己清除报文发送计时
			time6 = 0;
			AllowSendF = 1;						//允许发送标记重新置1
		}
	}
#endif

#if 0
	if(LightFlashF == 1)		//运行灯闪烁
	{
		time5++;
		if(time5 == 10)
			Led_process(0,LED_GREEN,LED_ON);
		if(time5 >= 20)
		{
			Led_process(0,LED_GREEN,LED_OFF);
			time5 = 0;
		}
	}
#endif

#if 0
	if(Recv56F == 1)		//收到56报文后，在主循环中重启的标记
	{
		Rpt56_resetCnt++;
		if(Rpt56_resetCnt > 50)
		{
			Rpt56_resetCnt = 0;
			Rpt56_resetF = 1;
		}
	}
#endif

#if 1
	if(KeyComeF == 0)			//如果没有按键操作
	{
		KeyOverTime++;
		if (KeyOverTime >= 3000)		//5分钟后没有按键操作，返回循环显示
		{
			//printf("====按键无操作，自动返回 KeyStatus=0x%02X====\n",KeyStatus);
			//printf("%d年%02d月%02d日 %02d:%02d:%02d.%03d\n",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond,SysTime.wMilliseconds);
			KeyOverTime = 0;
			UR_return_normal();
			Send04F = 1;
			////////////////////2003.10.16
			if (PcSendDataF == 1)			//与PC通讯的话，重新进行上电召唤
			{
				SendCom = 0x31;
				PowOnPara = 0x0d;
				UR_com_send();
				PcSendDataF = 0;
			}
			////////////////////2003.10.16
			RecvMoreByte = 0;
			if (InTiaoShiF == 1)		//如果在调试状态	自动退出调试状态
			{
				InTiaoShiF = 0;
				//V3.05 2005.11.15 wzm
				ControlObject = 0;
				ControlCom = 0x14;
				SendComF = 1;
				SendCom = 0x1e;
				AutoExitTiaoShi =1;
			}
		}
	}
#endif
	
#if 0
	if (PowOnFlag == 1)			//30分钟 E2P自检标记
	{
		T_E2pSelfC++;
		if (T_E2pSelfC >= _T_E2pSelfC)
		{
			C_E2p_ErrF = 1;
			T_E2pSelfC = 0;
		}
	}
#endif
	
	
	if (PowOnFlag == 1)				//如果已经上电，显示循环显示界面
	{
		if(KeyStatus == 0x00)	//循环显示界面
		{
			//printf("NormalLockF=%d,NormalNoChangeF=%d,NormalDisNum=%d\n",NormalLockF,NormalNoChangeF,NormalDisNum);
			if (InWriteF == 1)
			{
				return;
			}
			if(NormalDisNum != 3)		//不是主接线图
				UR_TimeTitle_dis(0);
				
			if(NormalDisTime <= 0)
			{
				NormalDisTime = 40;			//4秒计时
				if(NormalLockF == 0)		//没有锁屏
				{
					if(NormalNoChangeF == 0)	//如果显示下一类信息
					{
						NormalDisNum++;
						if(NormalDisNum == 2)		//显示档位
						{
							CharTemp = TapPosition.PhaseNum;
							if((CharTemp == 0) || (BayType != JianGe))
								NormalDisNum++;
						}
						
//						if (NormalDisNum >= NormalDisTotalNum)	//如果到了最后一类，
						if (NormalDisNum >= 4)	//如果到了最后一类，
						{
							NormalDisNum = 0;				//回到第一类循环信息(模拟量)
							if(TotalMeasNum == 0)		//如果模拟量个数等于0
							{
								NormalDisNum = 1;			//则显示第2类
								//r_lcd_clear_1(0, 1);	//清时间标题行??
							}
						}
					}
				}
				
				switch (NormalDisNum) 
				{
					case 0:		//循环显示-遥测
					{
						r_lcd_clear();
						Line = 1;		
						CharTemp = (unsigned char) ROM_LINE_CHAR / ColPerRow_Meas;					
						do
						{
							for (i = 0; i < ColPerRow_Meas; i++)
							{
								LcdPos = ROM_ROWCHAR_BYTE * Line + CharTemp * i + DisFormat_meas_Col[0];
								switch(BayType)
								{
								case JianGe:
								case BianKaiGuanAndChuXian:
								{
									switch(Normal_Meas_Char[DisMeasNum])
									{
									case 92:	//U4a->Usyn
										r_str_char((uint16 * )Meas_Char_92_0);
										break;
									case 109:	//f4->Fsyn
										r_str_char((uint16 * )Meas_Char_109_0);
										break;
									default:
										r_str_char((uint16 * )Meas_Char[Normal_Meas_Char[DisMeasNum]]);
										break;
									}
									break;
								}
								case ZhuBianL:
								case MuXian:
								case GongYong:
								default:
									r_str_char((uint16 * )Meas_Char[Normal_Meas_Char[DisMeasNum]]);
									break;
								}
								//r_str_char((uint16 *)Meas_Char[Normal_Meas_Char[DisMeasNum]]);
								LcdPos = ROM_ROWCHAR_BYTE * Line + CharTemp * i + DisFormat_meas_Col[1];
								r_single_char(ROM_EQU_CHAR);
								UR_float2asc(Meas[DisMeasNum]);
								for (j = 0; j < FAscN; j++)
									r_single_char(Float2AscBuf[j]);
								DisMeasNum++;
								if (DisMeasNum >= TotalMeasNum)
									break;
							}
							if (DisMeasNum >= TotalMeasNum)
								break;
							Line++;
							NormalNoChangeF = 1;
						}
						while (Line < ROM_LCD_ROW - 1);			//display the measurement

						if (DisMeasNum >= TotalMeasNum)
						{
							DisMeasNum = 0;
							NormalNoChangeF = 0;
						}

						if (NormalLockF == 0)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_lock;
							r_single_char(ROM_SBCspace_ASC);
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_lock;
							r_str_char((uint16 *)ROM_SuoPing);
						}

						//Wangjd add S 2014-2-8 20:56
						if(DeviceTypeF == 1)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Bay;
							r_str_char((uint16 * )code_BayType_Sim[BayType - 1]);
						}

						if (MaintStateF == 0)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Test;
							r_single_char(ROM_SBCspace_ASC);
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Test;
							r_str_char((uint16 * )ROM_MaintState);
						}

						//Wangjd add E 2014-2-8 20:57
						if (GpsInF == 1)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_GPS;
							r_single_char(ROM_Star);
						}	
						break;
					}
					case 1:		//循环显示-压板
					{
#if 0		//XbinModify 2017-4-21
						r_lcd_clear_1(1, ROM_LCD_ROW);
						LcdPos = ROM_ROWCHAR_BYTE * 1;
						r_str_char((uint16 *)ROM_STR_CurLP);
						NoLpB = 0;
							
						for (i = 0; i < Lp_FourInOne_Buf[0]; i++)
						{
							if (BitField[Lp_FourInOne_Buf[1+i]-1].lp == 1)
							{
								NoLpB = 1;
								break;
							}
						}
						
						if (GpsInF == 1)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_GPS;
							r_single_char(ROM_Star);         // that is  '*'
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_GPS;
							r_single_char(ROM_space_ASC);
						}					

						if ((Lp_FourInOne_Buf[0] == 0) || (NoLpB == 0))
						{
							LcdPos = ROM_ROWCHAR_BYTE * ((ROM_LCD_ROW - 1)/2) + (ROM_LINE_CHAR - UR_len_calchar((uint16 *)ROM_NoLP))/2;
							r_str_char((uint16 *)ROM_NoLP);
							return;
						}

						for (Line = 2; Line < ROM_LCD_ROW - 1;)
						{
							if (BitField[Lp_FourInOne_Buf[1+DisLpNum]-1].lp == 1)
							{
								LcdPos = ROM_ROWCHAR_BYTE * Line + DisFormat_LPrpt_Col[0];
								GetLpName((uint8)BayType,Lp_FourInOne_Buf[1+DisLpNum],Tempbuf);
								r_str_char(Tempbuf);

								LcdPos = ROM_ROWCHAR_BYTE * Line + DisFormat_LPrpt_Col[1];
								r_str_char((uint16 *)ROM_STR_TouRu);
								if (DisLpNum < TotalLpNum_soft)
									r_single_char(ROM_Soft_CHAR);             //Ruan
								else
									r_single_char(ROM_Hard_CHAR);	            //Ying
								Line++;
							}
							DisLpNum++;
							if (DisLpNum >= Lp_FourInOne_Buf[0])
								break;
						}
						
						TempLpNum = DisLpNum;
						NoLpB = 0;
						while (TempLpNum < Lp_FourInOne_Buf[0])
						{
							if (BitField[Lp_FourInOne_Buf[1+TempLpNum]-1].lp == 1)
							{
								NoLpB = 1;
								break;
							}
							TempLpNum++;
						}
						NormalNoChangeF = 1;
						if (NoLpB == 0)
						{
							DisLpNum = 0;
							NormalNoChangeF = 0;
						}

						if (DisLpNum >= (TotalLpNum_soft + TotalLpNum_hard))
						{
							DisLpNum = 0;
							NormalNoChangeF = 0;
						}
						if (NormalLockF == 0)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_lock;
							r_single_char(ROM_SBCspace_ASC);
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_lock;
							r_str_char((uint16 *)ROM_SuoPing);
						}
						if(DeviceTypeF == 1)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Bay;
							r_str_char((uint16 * )code_BayType_Sim[BayType - 1]);
						}
						if (MaintStateF == 0)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Test;
							r_single_char(ROM_SBCspace_ASC);
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Test;
							r_str_char((uint16 * )ROM_MaintState);
						}
						break;
#else
						r_lcd_clear_1(1, ROM_LCD_ROW);
						LcdPos = ROM_ROWCHAR_BYTE * 1;
						r_str_char((uint16 *)ROM_STR_CurLP);
	//					Line = 2;
						NoLpB = 0;

							
						for (i = 0; i < (TotalLpNum_soft + TotalLpNum_hard); i++)
						{
							if(LpChar[1+i] >= 17)	//XbinModify 2017-4-21 如果是功能压板则不显示
								break;

							if (BitField[LpChar[1+i]-1].lp == 1)
							{
								NoLpB = 1;
								break;
							}
						}
						
						if (GpsInF == 1)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_GPS;
							r_single_char(ROM_Star);         // that is  '*'
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_GPS;
							r_single_char(ROM_space_ASC);
						}					

						if (((TotalLpNum_soft + TotalLpNum_hard) == 0) || (NoLpB == 0))
						{
							LcdPos = ROM_ROWCHAR_BYTE * ((ROM_LCD_ROW - 1)/2) + (ROM_LINE_CHAR - UR_len_calchar((uint16 *)ROM_NoLP))/2;
							r_str_char((uint16 *)ROM_NoLP);
							return;
						}

						for (Line = 2; Line < ROM_LCD_ROW - 1;)
						{
							if (BitField[LpChar[1+DisLpNum]-1].lp == 1)
							{
								LcdPos = ROM_ROWCHAR_BYTE * Line + DisFormat_LPrpt_Col[0];					
								switch(BayType)
								{
								case JianGe:
								{
									switch(LpChar[1+DisLpNum])
									{
									case 2:
										r_str_char((uint16 *)Lp_Char[40]);
										break;
									case 6:
										r_str_char((uint16 *)Lp_Char[41]);
										break;
									case 8:
										r_str_char((uint16 *)Lp_Char[42]);
										break;
									default:
										r_str_char((uint16 *)Lp_Char[LpChar[1+DisLpNum]]);
										break;
									}
									break;
								}
								case BianKaiGuanAndChuXian:
								{
									switch(LpChar[1+DisLpNum])
									{
									case 2:
										r_str_char((uint16 *)Lp_Char[40]);
										break;
									case 8:
										r_str_char((uint16 *)Lp_Char[42]);
										break;
									default:
										r_str_char((uint16 *)Lp_Char[LpChar[1+DisLpNum]]);
										break;
									}
									break;
								}
								default:
									r_str_char((uint16 *)Lp_Char[LpChar[1+DisLpNum]]);
									break;
								}
								LcdPos = ROM_ROWCHAR_BYTE * Line + DisFormat_LPrpt_Col[1];
								r_str_char((uint16 *)ROM_STR_TouRu);
								if (DisLpNum < TotalLpNum_soft)
									r_single_char(ROM_Soft_CHAR);             //Ruan
								else
									r_single_char(ROM_Hard_CHAR);	            //Ying
								Line++;
							}
							DisLpNum++;
							if (DisLpNum >= (TotalLpNum_soft + TotalLpNum_hard))
								break;
						}
						
						TempLpNum = DisLpNum;
						NoLpB = 0;
						while (TempLpNum < (TotalLpNum_soft + TotalLpNum_hard))
						{
							if (BitField[LpChar[1+TempLpNum]-1].lp == 1)
							{
								NoLpB = 1;
								break;
							}
							TempLpNum++;
						}
						NormalNoChangeF = 1;
						if (NoLpB == 0)
						{
							DisLpNum = 0;
							NormalNoChangeF = 0;
						}

						if (DisLpNum >= (TotalLpNum_soft + TotalLpNum_hard))
						{
							DisLpNum = 0;
							NormalNoChangeF = 0;
						}
						if (NormalLockF == 0)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_lock;
							r_single_char(ROM_SBCspace_ASC);
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_lock;
							r_str_char((uint16 *)ROM_SuoPing);
						}
						if(DeviceTypeF == 1)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Bay;
							r_str_char((uint16 * )code_BayType_Sim[BayType - 1]);
						}
						if (MaintStateF == 0)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Test;
							r_single_char(ROM_SBCspace_ASC);
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Test;
							r_str_char((uint16 * )ROM_MaintState);
						}
						//if (GpsInF == 1)
						//{
						//	LcdPos = ROM_LINE_CHAR - 7;
						//	r_single_char(42);
						//}
						//else
						//{
						//	LcdPos = ROM_LINE_CHAR - 7;
						//	r_single_char(32);
						//}
						break;
#endif
					}
					case 2:		//循环显示-档位
					{
						if((BayType == BianKaiGuanAndChuXian) ||
							(BayType == MuXian) ||
							(BayType == GongYong))
						{
							NormalNoChangeF = 0;
							break;
						}
						UR_TransformerPosition_dis();
						
						if (NormalLockF == 0)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_lock;
							r_single_char(ROM_SBCspace_ASC);
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_lock;
							r_str_char((uint16 *)ROM_SuoPing);
						}
					    //Wangjd add S 2014-2-8 20:56
						if(DeviceTypeF == 1)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Bay;
							r_str_char((uint16 * )code_BayType_Sim[BayType - 1]);
						}
						if (MaintStateF == 0)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Test;
							r_single_char(ROM_SBCspace_ASC);
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Test;
							r_str_char((uint16 * )ROM_MaintState);
						}
						//Wangjd add E 2014-2-8 20:57
						if (GpsInF == 1)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_GPS;
							r_single_char(ROM_Star);
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_GPS;
							r_single_char(ROM_space_ASC);
						}	
						
						break;
					}
					case 3:		//循环显示-主接线图
					{
						CursorDisMainF = 0;
						UR_MainG_dis();									//display the main graph
						if(NormalLockF == 0)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_lock;
							r_single_char(ROM_SBCspace_ASC);
						}
						else
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_lock;
							r_str_char((uint16 *)ROM_SuoPing);
						}
						if(InTiaoShiF == 1)
						{
							LcdPos = ROM_LINE_CHAR - _MarkPos_Debug;
							r_str_char((uint16 *)ROM_TiaoShi);
						}	
						break;
					}
				}
			}
			NormalDisTime--;
		}
	}
	else		//否则，显示www.sf-auto.com
	{
		if(KeyStatus == 0x00)
		{
			time9++;
			if(time9 >= 5)
			{
				time9 = 0;
				if(InterAdrDisNum >= InterAdrTotalNum)
					return;
				if(InterAdrDisNum >= ROM_LINE_CHAR)
				{
					LcdPos = (ROM_LCD_ROW - 1) * ROM_ROWCHAR_BYTE;
					r_str_char((uint16 *)&ROM_STR_InternetAdr[InterAdrTotalNum - ROM_LINE_CHAR]);
				}
				else
				{
					LcdPos = (ROM_LCD_ROW - 1) * ROM_ROWCHAR_BYTE + ROM_LINE_CHAR - 2 - InterAdrDisNum;
					r_str_char((uint16 *)ROM_STR_InternetAdr);
				} 
				r_str_char((uint16 *)ROM_STR_Null);
				InterAdrDisNum++;
			}
		}
	}
}




void main()
{

/*	输入尺寸和数据，显示图像
	int widthx, heighty;
	unsigned char HexBuf[HEXMAX],AsciiBuf[HEXMAX*6];
	unsigned char * point;

	printf("宽度:");
	scanf("%d",&widthx);
	widthx = widthx/8;
	printf("高度:");
	scanf("%d",&heighty);
	printf("ASCII:");
	gets((char *)AsciiBuf);

	getchar();
	Ascii2Hex(AsciiBuf, HexBuf);

	point = HexBuf;

	point = (unsigned char *)ChineseZK01;
	DrawPhoto(heighty, widthx, point);
	getchar();
	getchar();
*/
	char SaveFileName[256]="PrintScreen";
	MOUSEMSG mouse;
	char keyboardF=0;
	unsigned char RptPNo=0;
	uint16 _setName=0;
	unsigned char mouseF=0;
	unsigned long int count_i=0;
	time_t Time_zero1=0,Time_zero2=0;
	WORD ms=0,ms_temp=0;
	WORD Sec=0,Sec_temp=0;
	float df,du,ds;
	short asd;
	float asd2;

	FILE * PowerOnFilePoint;
	FILE * CycReportFilePoint;
//	FILE * ResponFilePoint;
	char PowerOnFileName[] = "Report_PowerOn.ini";
	char CycReportFileName[] = "Report_Cycle.ini";
	char ResReportFileName[] = "Report_Respon.ini";
	char StrLine[512];
	int FileSize;
	unsigned int CycRptNo = 0;	//循环上送报文序号
	unsigned char EepromFileOK = 0;
	unsigned char LoadEepromFile_Count=0;
	
	unsigned long Ta1Sec=0;		//1秒中断计数器
	unsigned long Ta10ms=0;		//10毫秒中断计数器
	unsigned long Ta100ms=0;	//100毫秒中断计数器

	unsigned Byte_temp;

	unsigned char DstBuf[32] = 
	{
		0xcc,0xcc,0xcc,0xcc, 0xcc,0xcc,0xcc,0xcc, 0xcc,0xcc,0xcc,0xcc, 0xcc,0xcc,0xcc,0xcc,
		0xcc,0xcc,0xcc,0xcc, 0xcc,0xcc,0xcc,0xcc, 0xcc,0xcc,0xcc,0xcc, 0xcc,0xcc,0xcc,0xcc
	};

//	TLV TlvBuf;
//	char str[1024];
	unsigned char TlvVal[] = {0x80, 0x0d, 0x92, 0x02, 0x02, 0x00, 0x93, 0x03, 0x31, 0x08, 0x16, 0x94, 0x02, 0x65, 0x19, 0};

#ifdef Enable_PLC
	PlcTable_Read(_WorkTbl,&PlcTable);
	StruPlcTable_printf(&PlcTable);
	GseRTDataBase_init(&GseRTDataBase);
	Plc_Exe();
	getchar();
#endif
	
#if 0
	//char str[]="国";
	GetChineseBMP_func();
	getchar();
#endif

#if 0
	float werwer=191.1415926;
	printf("%8.3f",werwer);
	getchar();
#endif

#if 0	//角度计算测试
	float ang=330;

	ang = Angle_Cale(ang);
	printf("ang=%0.3f\n",ang);
	getchar();
#endif

#if 0
	test111();
	getchar();
#endif

#if 0	//测试data_copy函数
	struct haha
	{
		unsigned int haha_i;
		float haha_f;
		unsigned char haha_s[5];
	};

	struct asd
	{
		unsigned int test_int;
		float test_float;
		char *test_str;
		struct haha test_struct;
	};

	struct asd t1,t2;
	char teststr[] = "我要测试用";

	t1.test_int = 10;
	t1.test_float = 11.11;
	t1.test_str = teststr;
	t1.test_struct.haha_i = 100;
	t1.test_struct.haha_f = 111.11;
	memcpy(t1.test_struct.haha_s,"1111",5);



	memcpy(&t2,&t1,sizeof(asd));
	printf("test_int=%d\n",t2.test_int);
	printf("test_float=%f\n",t2.test_float);
	printf("test_str=%s\n",t2.test_str);
	printf("t2.test_struct.haha_i=%d\n",t2.test_struct.haha_i);
	printf("t2.test_struct.haha_f=%f\n",t2.test_struct.haha_f);
	printf("t2.test_struct.haha_s=%s\n",t2.test_struct.haha_s);
	getchar();


#endif

#if 0
	SetDefaultDC();
	system("pause");
#endif

#if 0
	printf("CT:%d//%d  PT:%d//%d  ",111,222,333,444);
	system("pause");
#endif

#if 0 //测试写默认直流配置
	SetDefaultDC();

	getchar();
#endif

#if 0	//测试字符串转数字
	int i;

	char str[100] = "254.566.692.49.65535";
	unsigned short Des[16] = {0};
	uint16 UMeas_default[] = {254, 460, 253, 0xffff};	//测量侧
	uint16 USync_default[] = {271, 534, 253, 0xffff};	//抽取侧

	Str2UnShort(str, Des);
	for(i=0; Des[i]!=0; i++)
	{
		printf("%d, ",Des[i]);
	}
//	printf("\n长度=%d\n",sizeof(UMeas_default));
	getchar();

#endif

#if 0	//测试zip文件打开函数
	FILE * fptrZIP;
	fptrZIP = fopen("北京四方四统一测控装置2.0模型20170809.zip","rb");
	if(fptrZIP != NULL)
	{
		OpenZipFile(fptrZIP);
		fclose(fptrZIP);
	}
	getchar();
#endif

#if 0	//测试master中float3转标准4字节float的函数
	unsigned char pSrcData[4] = {0x00,0xfa,0x0b,0x00};
	unsigned char pDestData[4] = {0x00, 0xC8, 0x07, 0x00};
	RealBaseDataToStFloat(_ThreeByte,pSrcData, pDestData);

	getchar();
#endif

#if 0	//测试 JudgeDoubleDI函数
	uint16 DiNo;
	uint8 DoubleDiNo;
	uint8 DiCpuNo;

	DIBoardDoubleNum[0] = 4;
	DIBoardDoubleNum[1] = 5;
	DIBoardDoubleNum[2] = 6;
	DIBoardDoubleNum[3] = 7;
	DIBoardDoubleNum[4] = 8;
	
	RunCpu[DICpuType][0].b = 1;
	RunCpu[DICpuType][1].b = 0;
	RunCpu[DICpuType][2].b = 1;
	RunCpu[DICpuType][3].b = 0;
	RunCpu[DICpuType][4].b = 1;

	for(DiCpuNo=0; DiCpuNo<5; DiCpuNo++)
	{
		for(DiNo=0; DiNo<24; DiNo++)
		{
			DoubleDiNo = JudgeDoubleDI(DiCpuNo*24 + DiNo, _IsHard);
			printf("CpuNo=%d DiNo=%d DoubleDiNo=%d\n",DiCpuNo,DiNo,DoubleDiNo);
		}
		system("pause");
		system("cls");
	}
	system("pause");
#endif

#if 0
	uint16 testnum,asd;
	for(asd=0;asd<6;asd++)
	{
		for(testnum=0;testnum<128;testnum++)
		{
			UR_hex2single_fillzero(testnum,asd);
		}
		system("pause");
		system("cls");
	}
#endif

#if 0	//测试张炜写的3字节BCD转4字节float
	unsigned char bcd[]={0x90,0x10,0x00};
	float value_f;
	value_f = r_3BcdToFloat(bcd);

	printf("value_f=%f\n",value_f);

	system("pause");

#endif

#if 0
	unsigned char data[5] = {0x08,0x40,0x00,0x00,0x00};
	unsigned long ptemp;
	float tempfloat=2.0;
	ptemp = (data[1]<<24)+(data[2]<<16)+(data[3]<<8)+(data[4]);
//	tempfloat = *(float*)(&data[1]);
	memcpy(&tempfloat,&ptemp,sizeof(float));

	printf("tempfloat=%f\n",tempfloat);
//	printf("ptemp=%08X tempfloat=%f",ptemp,tempfloat);
#endif

#if 0
	//                      ----------秒----------
	unsigned char SOC1[8] = {0x59, 0x00, 0x0C, 0xD5, 0x44, 0x5A, 0x1C, 0x0A};
	unsigned char SOC2[8] = {0x00, 0x00, 0x00, 0x16, 0x90, 0x17, 0x73, 0x2a};

	Utc2local(SOC2);
#endif

#if 0
	unsigned long float4=0x99;

	printf("float4=%08X\n",&float4);
	float4 = Float4toFloat3(1.0);
	printf("Float4toFloat3=%08x\n",float4);
	float4 = s32To3Float((long)(65536*1.0));
	printf("s32To3Float=%08x\n",float4);

	getchar();
#endif


#if 0
	unsigned long Float3,Float3_t;
	float float4,float4_t;

#if 1
	float4tofloat3(7.999944);
#else
	//2147483648
	//2000000000
	printf("正在验证:\n");
	for(Float3=7000; Float3<100000000; Float3++)
	{
		switch(Float3)
		{
		case 10000000:
			printf("已完成10%%\n");
			break;
		case 20000000:
			printf("已完成20%%\n");
			break;
		case 30000000:
			printf("已完成30%%\n");
			break;
		case 40000000:
			printf("已完成40%%\n");
			break;
		case 50000000:
			printf("已完成50%%\n");
			break;
		case 60000000:
			printf("已完成60%%\n");
			break;
		case 70000000:
			printf("已完成70%%\n");
			break;
		case 80000000:
			printf("已完成80%%\n");
			break;
		case 90000000:
			printf("已完成90%%\n");
			break;
		}
		Float3_t = UR_float4tofloat3(Float3);
		float4 = UR_3hex2float(((unsigned char *)&Float3_t)[0],((unsigned char *)&Float3_t)[1],((unsigned char *)&Float3_t)[2]);
		float4_t = ((float)Float3)/10000;

//		if(float4_t>float4*1.02 || float4_t<float4*0.98)
		{
			printf("Float3=%d\n",Float3);
			printf("Float3_t[012]=%02x %02x %02x\n",((unsigned char *)&Float3_t)[0],((unsigned char *)&Float3_t)[2],((unsigned char *)&Float3_t)[2]);
			printf("float4=%f\n",float4);
			printf("float4_t=%f\n\n",float4_t);
		}
		getchar();
	}
#endif
	printf("校验完成\n");
	getchar();
#endif

#if 0
	unsigned char cccc[] = {0x8b, 0xff, 0xff, 0x40};
	float ftemp;
	uint32 u32Tmp;

	u32Tmp = (uint32)cccc[0] + ((uint32)(cccc[1])<<8) +((uint32)(cccc[2])<<16) +((uint32)(cccc[3])<<24);

	ftemp = *(float*)(&u32Tmp);
	ftemp = (double)8.999944;
	printf("ftemp=%f\n",ftemp);
	u32Tmp = s32To3Float( (sint32)(65536 * ftemp));
	printf("Float3=%02x %02x %02x %02x\n",((unsigned char *)&u32Tmp)[0],((unsigned char *)&u32Tmp)[1],((unsigned char *)&u32Tmp)[2],((unsigned char *)&u32Tmp)[3]);
	getchar();
#endif

#if 0		//默认压板测试
	SetDefaultLP();
	getchar();
#endif

#if 0
	char dirname[] = "MEAS";
	char exname[] = ".icd";
	char filename[65] = {0};
	char filepathname[65] = {0}; 

	SP.gBayType = 0x41;

	if(SearchFile(filename,dirname,exname)==1)
		sprintf(filepathname,"%s/%s",dirname,filename);
	printf("filepathname=%s\n",filepathname);

	FoundProgramVerXml();
#endif

#if 0
	unsigned int temp=0xffffffff;
	unsigned int * crc=&temp;
	unsigned char crc_ascii[4];
	int size;
	unsigned char str1[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	char * buf;
	char * buf_t;

	#if 0
		#if 1
			size = sizeof(str1);
			buf = (xrchar_t*)xrMalloc(xrSZOF(xrchar_t)*size + 200);
			xrMemZero(buf, xrSZOF(xrchar_t)*size + 200);
			memcpy(buf, str1, size);
			*crc = crcbyTableUtf8(*crc, buf, size);
			xrMemFree(buf);
			printf("CRC1=0x%08X\n\n",*crc);

		#else
			size = sizeof(str1);
			buf = (unsigned char*)malloc(sizeof(unsigned char)*size + 200);
			memcpy(buf,str1,size);
			crc = 0xffffffff;
			crc = crcbyTableUtf8(crc,buf,size-1);
			printf("CRC=0x%08X\n",crc);
		#endif
	#endif

	#if 1
		xrerr_t rel = XR_EOK;
		xrint_t rellen;
		char str_t[1024];
		xrfile_t * pfile=NULL;
		char filename[] = "CL5031_S1.cid";
		char s1[] = "站控层";
		unsigned char s2[] = "站控层";

		rel = xrFileOpen(&pfile,filename,0);
		if(rel != XR_EOK) 
		{
			printf("XR_CCD_FILE_LOAD_ERROR\n");
		}

		size = (xrint_t)xrFileSize(pfile);
		printf("size=%d\n",size);
		buf =(xrchar_t*) xrMalloc(xrSZOF(xrchar_t)*size + 20000);
		buf_t = buf+size;
		xrMemZero(buf,xrSZOF(xrchar_t)* size+200);
		if (buf==NULL) 
		{
			printf("XR_NO_MEMERY");
		}

		rellen = xrFileRead(pfile,buf,size);

		printf("rellen=%d\n",rellen);
		*crc = crcbyTableUtf8(*crc,(unsigned char*)buf,size);
		printf("CRC=0x%08X\n",*crc);
	
		s1[0] = 0x0d;
		s1[1] = 0x0a;	
		s2[0] = 0x0d;
		s2[1] = 0x0a;
		//5633CD0E
		getchar();
	#endif
#endif

#if 0
		r_str_char_W((uint16 *)Str_SetDevTypeWillRestart,NULL);
		getchar();
#endif

#if 0
		SP.gBayType = 0x01;
		FoundProgramVerXml();
#endif

#if 0
		unsigned char ver[2]={0};
		Ascii2VerNum("v1.28",ver);

		printf("VER=V%X.%02X\n",ver[0],ver[1]);
#endif

#if 0
		unsigned short crc;


		crc = GetFileCRC16_XMODEM("MEAS/CSI200FC_BK-G-2.0_E717.icd");
		printf("GetFileCRC16_XMODEM=%04X\n",crc);

		crc = CalCrc16_SiFang("MEAS/CSI200FC_BK-G-2.0_E717.icd");
		printf("CalCrc16_SiFang=%04X\n",crc);

		crc = GetBinaryFileCrc16("MEAS/CSI200FC_BK-G-2.0_E717.icd");
		printf("GetBinaryFileCrc16=%04X\n",crc);

		crc = CRC16_2("MEAS/CSI200FC_BK-G-2.0_E717.icd");
		printf("CRC16_2=%04X\n",crc);

		//XMODEM
		crc = CalaCRC16_char("MEAS/CSI200FC_BK-G-2.0_E717.icd");
		printf("CalaCRC16_char=%04X\n",crc);

		//XMODEM
		crc = CalaCRC16_char8("MEAS/CSI200FC_BK-G-2.0_E717.icd");
		printf("CalaCRC16_char8=%04X\n",crc);
		getchar();
#endif

//	asdasdasdasd();

	/*
	void UR_MMIErr_deal(unsigned char Type, unsigned char ParaIn1, unsigned char ParaIn2, unsigned char RWe2p, unsigned char CRC, const uint16 * str)
	*/
//	UR_MMIErr_deal(0x0c, 0x68, 0x00, 0x01, 0x00, code_r_CheckEe2_Portion);

	//value_f = (float)sizeof(SetAscii)/1024;
	//printf("SetAscii=%fkB\n",value_f);
	//value_f = (float)sizeof(Set1_Buf)/1024;
	//printf("Set1_Buf=%fkB\n",value_f);
	//value_f = (float)sizeof(Set3_Buf)/1024;
	//printf("Set3_Buf=%fkB\n",value_f);
	//value_f = (float)sizeof(RepExplain)/1024;
	//printf("RepExplain=%fkB\n",value_f);


	/*float ChanVal;
	ChanVal=UR_3hex2float(0x01,0x00,0x10);*/

	//uint16 ASCII_BCD[]={ROM_minus_ASC,1,16,0,0,0};

	////////////////////////
	initgraph(320, 240, SHOWCONSOLE);   // 这里和 TC 略有区别
//	initgraph(320, 240, NOCLOSE);   // 这里和 TC 略有区别
	
	//initgraph(64, 56, SHOWCONSOLE);   // 这里和 TC 略有区别
	setbkcolor(_backdropcolor);
	setbkmode(TRANSPARENT);
	cleardevice();
	//setlinecolor(GREEN);
	//line(255, 0, 255, 240);

#if 0
	#if 1
	for(asd=0;asd<=100;asd++)
	#else
	asd = 5;
	#endif
	{
		srand((unsigned) time(NULL));
		asd2 = ((float)(rand()%100));
		df = -2+(((float)asd)*(4.0/100.0));
		//printf("df=%f\n",df);
		du = -100 + ((float)asd)*(200.0/100.0);
		ds = ((float)asd)*(180.0/100.0);
		Dis_SyncMeter(asd2,50.0,100-asd2,50.0,df,du,ds,1.0);
		getchar();
	}
#endif


	//UR_TapPosition_char_windows(0,89);

	//BayType = (enum DevType)2;
	//DeviceTypeF = 1;
	PowOnFlag = 1;
	SendMultiP_input = 0;
	Windows_SetSysStatus();

	//windows_Rpt_explain(rpt5d);
	//windows_Rpt_explain(rpt51);
	//windows_Rpt_explain(rpt5e);
	//windows_Rpt_explain(rpt52);
	//windows_Rpt_explain(rpt53);
	//windows_Rpt_explain(rpt7d);
	//windows_Rpt_explain(rpt58);
	//windows_Rpt_explain(rpt59);
	//windows_Rpt_explain(rpt5a);
	//windows_Rpt_explain(rpt5b);

	//windows_Rpt_explain(rpt31);
	//windows_Rpt_explain(rpt32);
	//windows_Rpt_explain(rpt3a);
	//windows_Rpt_explain(rpt3b);

////加载eeprom.bin文件 开始//////////////////////////////////////////////////
	//EepromFilePoint = fopen(EepromFileName,"rb+");
	//if(EepromFilePoint == NULL)
	//	printf("EepromFilePoint == NULL\n");
	//else
	//	fclose(EepromFilePoint);

#if 0	//测试TLV解码
	TlvBuf.tag = TlvVal[0];
	TlvBuf.len = TlvVal[1];
	TlvBuf.val = &TlvVal[2];
	verTLV2ASCII_test(&TlvBuf,str);
#endif

	while(EepromFileOK == 0)
	{
		if(LoadEepromFile_Count>=10)
		{
			printf("加载%s失败，程序退出\n",EepromFileName);
			system("pause");
			return;
		}
		LoadEepromFile_Count++;
		if((EepromFilePoint = fopen(EepromFileName,"rb+")) != NULL)
		{
			fseek (EepromFilePoint, 0, SEEK_END);   ///将文件指针移动文件结尾
			FileSize = ftell (EepromFilePoint); ///求出当前文件指针距离文件开始的字节数
			if(FileSize >= 0x80*32)
			{
				EepromFileOK = 1;
				printf("file\"%s\" Open OK.\n",EepromFileName);
//				LightByte.all = r_ReadIIc_PNByte(_E2Block_Light, _E2Byte_Light, WorkStatus);
//				printf("LightByte.all=0x%02x\n",LightByte.all);
			}
			else
			{
				fclose(EepromFilePoint);
				printf("file\"%s\" Size not 4096byte\n",EepromFileName);
				if((EepromFilePoint = fopen(EepromFileName,"wb+")) != NULL)
				{
					printf("file\"%s\" Create OK.\n",EepromFileName);
					rewind(EepromFilePoint);
					fseek(EepromFilePoint, 0, 0);
					for(count_i=0; count_i<128; count_i++)
					{
						fwrite(DstBuf, sizeof(DstBuf), 1, EepromFilePoint);
					}
				}
				fclose(EepromFilePoint);
			}
		}
		else
		{
			printf("file\"%s\" Open Error.\n",EepromFileName);
			if((EepromFilePoint = fopen(EepromFileName,"wb+")) != NULL)
			{
				printf("file\"%s\" Create OK.\n",EepromFileName);
				for(count_i=0; count_i<128; count_i++)
				{
					fwrite(DstBuf, sizeof(DstBuf), 1, EepromFilePoint);
				}
			}
			else
			{
				printf("file\"%s\" Create Error.\n",EepromFileName);
			}
			fclose(EepromFilePoint);
		}
	}

	if(EepromFilePoint != NULL)
	{
		Byte_temp = r_ReadIIc_PNByte(_E2Block_MainG, _E2Byte_MainG, MMITestStatus);					//xbin xiugai 2016-5-23 排除ckmmi_main_init

		if (Byte_temp == 0xa6)							//the eeprom have be written
		{
			NormalDisTotalNum = 4;

			if (r_CheckEe2(_E2Block_60rpt, _E2Byte_60rpt, 200, 1))
			{				
				for (count_i = 0; count_i < (unsigned long)(Ee2ReadCek[_E2Len] + 3); count_i++)
				{
					rpt60[count_i] = Ee2ReadCek[count_i];
				}
				UR_read_60();
			}
			else
				UR_MMIErr_deal(EepErrType, _E2Block_60rpt, _E2Byte_60rpt, 1, 0, code_r_CheckEe2_60rpt);	

			if (r_CheckEe2(_E2Block_61rpt, _E2Byte_61rpt, 200, 1))
			{
				for (count_i = 0; count_i < (unsigned long)(Ee2ReadCek[_E2Len] + 3); count_i++)
					rpt61[count_i] = Ee2ReadCek[count_i];
				UR_read_61();
			}
			else
				UR_MMIErr_deal(EepErrType, _E2Block_61rpt, _E2Byte_61rpt, 1, 0, code_r_CheckEe2_61rpt);	

			if (r_CheckEe2(_E2Block_62rpt, _E2Byte_62rpt, 200, 1))
			{
				for (count_i = 0; count_i < (unsigned long)(Ee2ReadCek[_E2Len] + 3); count_i++)
					rpt62[count_i] = Ee2ReadCek[count_i];
				UR_read_62();
			}
			else
				UR_MMIErr_deal(EepErrType, _E2Block_62rpt, _E2Byte_62rpt, 1, 0, code_r_CheckEe2_62rpt);	

			if (r_CheckEe2(_E2Block_63rpt, _E2Byte_63rpt, 70, 1))
			{
				for (count_i = 0; count_i < (unsigned long)(Ee2ReadCek[_E2Len] + 3); count_i++)
					rpt63[count_i] = Ee2ReadCek[count_i];
				UR_read_63();
			}
			else
				UR_MMIErr_deal(EepErrType, _E2Block_63rpt, _E2Byte_63rpt, 1, 0, code_r_CheckEe2_63rpt);	

			if (r_CheckEe2(_E2Block_64rpt, _E2Byte_64rpt, 100, 1))
			{
				for (count_i = 0; count_i < (unsigned long)(Ee2ReadCek[_E2Len] + 3); count_i++)
					rpt64[count_i] = Ee2ReadCek[count_i];
				UR_read_64();
			}
			else
				UR_MMIErr_deal(EepErrType, _E2Block_64rpt, _E2Byte_64rpt, 1, 0, code_r_CheckEe2_64rpt);	

			if (r_CheckEe2(_E2Block_50rpt, _E2Byte_50rpt, 70, 1))
			{
				for (count_i = 0; count_i < (unsigned long)(Ee2ReadCek[_E2Len] + 3); count_i++)
					rpt50[count_i] = Ee2ReadCek[count_i];
				if((rpt50[_E2Content] != 0x50) && (rpt50[_E2Content] != 0x55))
					rpt50[_E2Content] = 0x50;
			}
			else
				UR_MMIErr_deal(EepErrType, _E2Block_50rpt, _E2Byte_50rpt, 1, 0, code_r_CheckEe2_50rpt);
		}
		else
		{
			NormalDisTotalNum = 3;
			rpt60[_E2Content] = 0;
			rpt61[_E2Content] = 0;
			rpt62[_E2Content] = 0;
			rpt63[_E2Content] = 0;
			rpt64[_E2Content] = 0;
		}
	}




////加载eeprom.bin文件 结束//////////////////////////////////////////////////

	if((PowerOnFilePoint = fopen(PowerOnFileName,"r")) != NULL)	//如果读上电报文文件成功
	{
		printf("┏━━━━━━━━━━━━━━━━┓\n");
		printf("┃      上电报文文件读取成功      ┃%s\n",PowerOnFileName);
		printf("┗━━━━━━━━━━━━━━━━┛\n\n");
		while (!feof(PowerOnFilePoint))
		{
			if(fgets(StrLine,512,PowerOnFilePoint) != NULL)  //读取一行
			{
				windows_Rpt(StrLine);
			}
			else
			{
				//printf("i=%d CurLine=%d  文件结尾\n",i,CurLine); //输出
				fclose(PowerOnFilePoint);                     //关闭文件
			}
		}
		fclose(PowerOnFilePoint);                     //关闭文件
	}
	else
	{
		printf("读取%s文件失败，使用默认配置\n",PowerOnFileName);
		windows_Rpt(ASCII_Rpt_PowerOn_5d);
		windows_Rpt(ASCII_Rpt_PowerOn_51);
		windows_Rpt(ASCII_Rpt_PowerOn_5e);
		windows_Rpt(ASCII_Rpt_PowerOn_52);
		windows_Rpt(ASCII_Rpt_PowerOn_53);
		windows_Rpt(ASCII_Rpt_PowerOn_7d);
		windows_Rpt(ASCII_Rpt_PowerOn_58);
		windows_Rpt(ASCII_Rpt_PowerOn_59);
		windows_Rpt(ASCII_Rpt_PowerOn_5a);
		windows_Rpt(ASCII_Rpt_PowerOn_5b);
	}

	windows_Rpt_explain(rpt60buf);
	windows_Rpt_explain(rpt61buf);
	windows_Rpt_explain(rpt62buf);
	windows_Rpt_explain(rpt63buf);
	windows_Rpt_explain(rpt64buf);
	windows_Rpt_explain(rpt50buf);

	CycReportFilePoint = fopen(CycReportFileName,"r");
	for(CycRptNo=0; CycRptNo<8; CycRptNo++)
	{
		if(CycReportFilePoint != NULL)
		{
			ReadSpeacialLine(CycReportFilePoint,CycRptNo,StrLine);
			//printf("i=%d %s",CycRptNo,StrLine);
			windows_Rpt(StrLine);
		}
		else
		{
			//printf("i=%d StrBuf:NULL\n",CycRptNo);
			windows_Rpt(ACSII_Rpt_CycSend[CycRptNo]);
		}
	}
	if(CycReportFilePoint != NULL)
	{
		fclose(CycReportFilePoint);
	}


	//FillVersionRpt();
	//printf("Para[]=");
	//for(i=0;i<=Para[0];i++)
	//{
	//	printf(" %02x",Para[i]);
	//}
	//printf("\n");
	//printf("i=%d\n",i);

	//UR_rpt1_dis();

/*---循环显示界面测试用
	//CurLine = 0;
	//LcdPos = CurLine * ROM_ROWCHAR_BYTE + 0;
	//InTiaoShiF = 1;
	//DeviceTypeF = 1;
	//BayType = (DevType)2;
	//MaintStateF = 1;
	//GpsInF = 1;
	//NormalLockF = 1;
	//UR_TimeTitle_dis(LcdPos);
	//RptP = rpt36;
	//windows_Rpt_explain(RptP);
---*/

	
	Windows_SetSysStatus();
#ifdef MainGraph
	//--主接线图画图测试
	CursorDisMainF = 0;
	InTiaoShiF = 0;
	MaintStateF = 0;
	xunhuanxianshi(2);

	for(;;)
	{
		mouse = GetMouseMsg();
		if(mouse.mkLButton == 1)
			printf("mouse:x=%d y=%d\n",mouse.x,mouse.y);
		if(mouse.mkCtrl == 1)
			break;
	}

#endif
	//PaintRound(40,120,9);
	////PaintRound(160,120,10);
	////PaintRound(280,120,10);

	//r_circle_draw(100,120,9);

	//菜单测试///////////////////////////
#if 0
	//UR_return_normal();
	MenuDisF = 0;
	UR_loading_dis(0);
#else
	MenuDisF = 0;
	UR_return_normal();
#endif
	//菜单测试//////////////////////////
	////////////////////////////////////
	LP_Init();
	#if 0	//测试设置默认压板
		GetLocalTime(&SysTime);
		Windows_Fill_time();
		SetDefaultLP();
	#endif
	for(;;)
	{

#if 0		//1秒中断计数器
		Time_zero1 = time(0);
		if(Time_zero1 != Time_zero2)
		{
			Ta1Sec++;
			Time_zero2 = Time_zero1;
			SysTime = localtime(&Time_zero1);
			printf("%d年%02d月%02d日 %02d:%02d:%02d\n",SysTime->tm_year+1900,SysTime->tm_mon+1,SysTime->tm_mday,SysTime->tm_hour,SysTime->tm_min,SysTime->tm_sec);
		}
#endif

		/////==========10ms软中断计数器==============
		GetLocalTime(&SysTime);
		ms = SysTime.wMilliseconds;
		Sec = SysTime.wSecond;



		//10毫秒软中断
		if(ms_temp/10 != ms/10)
		{
			Ta10ms++;
			Ta100ms++; 
			ms_temp = ms;
			//printf("%d年%02d月%02d日 %02d:%02d:%02d.%03d\n",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond,SysTime.wMilliseconds);
		}

				//1秒软中断
		if(Sec_temp != Sec)
		{
			Ta1Sec++;
			Sec_temp = Sec;
			Windows_Fill_time();
			//printf("NormalNoChangeF=%d NormalDisNum=%d\n",NormalNoChangeF,NormalDisNum);
		}

		////========100ms软中断计数器===================
		if(Ta10ms>=10)
		{
			Ta10ms = 0;
			int_ta1();
		}

		//10秒软中断
		//if(Ta1Sec>=10)
		//{
		//	Ta1Sec = 0;
		//	if(ReadSpeacialLine(CycReportFileName,CycRptNo,StrLine) != NULL)
		//	{
		//		//printf("i=%d %s",CycRptNo,StrLine);
		//		windows_Rpt(StrLine);
		//	}
		//	else
		//	{
		//		//printf("i=%d StrBuf:NULL\n",CycRptNo);
		//		windows_Rpt(ACSII_Rpt_CycSend[CycRptNo]);
		//	}
		//	CycRptNo = (CycRptNo>=7)?0:CycRptNo+1;
		//}

		if(Ta100ms>=10)
		{
			Ta100ms = 0;
			if(SnakeGameF == 1)
			{
//				printf("SpeedTa100ms=%d  speed=%d\n",SpeedTa100ms,speed);
				if(SpeedTa100ms >= (6-speed))
				{
					Show();
					SpeedTa100ms = 1;
				}
				else
					SpeedTa100ms++;
			}
		}

		if(PowerOnWaitF == 1)
		{
			PrintfPowerOnWait(PowerOnWaitNum/200);
			printf("PowerOnWaitNum = %d\n",PowerOnWaitNum/200);
			if(PowerOnWaitNum == 799)
				PowerOnWaitNum = 0;
			else
				PowerOnWaitNum++;
			//getchar();
		}
		if (MenuDisF == 1)
			UR_menu_dis(MenuHead);
		if(RptDisF1 == 1)
			UR_rpt1_dis();
		if (RptDisF2 == 1)		//辅助显示缓冲区的显示任务标志位
			UR_rpt2_dis();
		if (DialogDisF == 1)	//对话框显示任务标志位
			UR_Dialog_dis();	//显示对话框
		if (CycleDisF2 == 1)	//主动上送的报文循环显示标志位
		{
			KeyStatus = 0x06;	//主动上送的2728报文循环显示时的按键操作
			CycleDisF2 = 0;
			r_lcd_clear();
			if (Cur_LcdP1 >= 61)
				Cur_LcdP1 = 0;
			else
				Cur_LcdP1++;

			if (LcdP1[Cur_LcdP1] == NULL)
				Cur_LcdP1 = 0;	
			UR_rpt1_dis();
		}
		keyboardF = 0;

		if(MouseHit())
		{
			mouse = GetMouseMsg();
			count_i++;
			//printf("GetMouseMsg进来了%d\n",count_i);
			if(mouse.mkLButton == 1)
				printf("mouse:x=%d y=%d\n",mouse.x,mouse.y);
		}
		if (kbhit()) 
		{
			Key = getch();
			keyboardF = 1;
			KeyComeF = 1;
		}
		if(keyboardF)
		{
			//printf("LcdClearF=%d\t",LcdClearF);
			switch(Key)
			{
			case 0:
			case 224:
				break;
			case 's':
				SeveScreen();
				break;
			case 'p':
#if 0
				for(_setName=0;_setName<=SetIndex[3][1]+10;_setName++)
					Printf_SetName(_setName);
				printf("定值%d,占用显示位置最大=%d\n",tttttt[0][0],tttttt[0][1]);
				printf("定值%d,字符数最多=%d\n",tttttt[1][0],tttttt[1][1]);
#else
				windows_Rpt_explain(Rpt38_tongxinzhongduan[RptPNo]);
				if(RptPNo == 13)
					RptPNo=0;
				else
					RptPNo++;
#endif
				break;
			case 27:		//ESC
				//fclose(EepromFilePoint);
				return;
			case 72:
				printf("keyboard=↑\t");
				break;
			case 80:
				printf("keyboard=↓\t");
				break;
			case 75:
				printf("keyboard=←\t");
				break;
			case 77:
				printf("keyboard=→\t");
				break;
			case 8:
				printf("keyboard=Quit\t");
				break;
			case 13:
				printf("keyboard=Set\t");
				break;
			default:
				printf("keyboard=0x%02x(%c=%d)\t",Key,Key,Key);
				break;
			}
		}
		if (KeyComeF == 1)		//等待按键放松，判断来的键值处理任务标志位
		{
			if(Key == 0 || Key == 224 || Key == 'p')
			{
				KeyComeF = 0;
				continue;
			}
			printf("KeyStatus=0x%02x\n",KeyStatus);
			UR_key_deal();		//按键处理
		}

#if 0
		if(keyboardF)
		{
			UR_key_deal();
			/*printf("RptDisF1=%d RptDisF2=%d\n",RptDisF1,RptDisF2);*/
		}
#endif

		if(SendComF == 1)
		{
			SendComF = 0;
			UR_com_send();
		}

		//Sleep(100);
	}

	//fclose(EepromFilePoint);
	getchar();
}


