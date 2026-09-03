/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2050a-06-06
Description: 铁水领料实绩新增
**************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include
#include "stdafx.h"


 
/*函数申明*/
//int f_mmsm15a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

                                     
// service入口
BM2F_ENTERACE(mmsm50af3_ins)
//-EP_SYSTEM_HEAD_END                                                  
int f_mmsm50af3_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn)
{
    
/****** 定义函数名称 ***** */
CString FunctionEname = "f_mmsm50af3_ins";                //定义函数英文名称  
CString FunctionCname = "铁水领料实绩_信息新增";              //定义函数中文名称

CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

 //程序用变量
int   doFlag = 0;
  
    
 try
 {
		
			/* 设置块名 */
		//bcls_rec->Tables.SetTableName(0,"MMSM50A");

		//if(!bcls_rec->Tables["MMSM50A"].Columns.Contains("PRACT_COLL_MODE"))
		//{
		//	bcls_rec->Tables["MMSM50A"].Columns.Add(DT_STRING,"PRACT_COLL_MODE"); 
		//}

		//if(!bcls_rec->Tables["MMSM50A"].Columns.Contains("PROC_DIV"))
		//{
		//	bcls_rec->Tables["MMSM50A"].Columns.Add(DT_STRING,"PROC_DIV"); 
		//}

		//bcls_rec->Tables["MMSM50A"].Rows[0]["PRACT_COLL_MODE"] = "0";

		//bcls_rec->Tables["MMSM50A"].Rows[0]["PROC_DIV"] = "I";
		//
		//doFlag = f_mmsm15a_proc(bcls_rec, bcls_ret, conn);
		//if (doFlag < 0)
		//{
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		//
  //

		///*设置系统返回参数*/
		//strcpy(s.msg,  _RES("GCRSS0000002"));//处理成功。  

	}


	
	catch(CApplicationException& cae)
	{
		doFlag	= cae.GetCode();
		strcpy(s.msg, (const char*)cae.GetMsg());
	}
	catch(CDbException& cde)
	{
		doFlag = -1;
		strcpy(s.msg, (const char*)cde.GetMsg());
	}
	catch(CException& ce)
	{
		doFlag	= ce.GetCode();
		strcpy(s.msg, (const char*)ce.GetMsg());
	}


    //EDLog(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	////EDLog(1, 1, " **************%s end*****************", (const char*)FunctionEname);

	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;
	}
  
  
  
  
    
    
    
	 