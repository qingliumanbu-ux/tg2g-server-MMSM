/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   孟凡杰
Version:    1.0
Date:     2024-01-08
Description: 工艺卡有新钢种时，板坯修正系数表中增加对应的数据
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件


//外部函数声明

BM2_FUNCTION_EXPORT
int f_mmsm33czxs_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm33czxs_proc";                //定义函数英文名称  
	CString FunctionCname = "工艺卡有新钢种时，板坯修正系数表中增加对应的数据";          //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;

	CString sqlstr = "";
	CString v_proc_div = "";
	CString v_pract_rcv_flag = "";
	CString v_factory_div = "";
	CString v_st_no = "";//钢种
	CString v_c_div = "";//碳锈区分   2是碳钢   1是不锈钢

	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_strand = "";//循环流号

	EIClass inBlock1;
	EIClass outBlock1;

	/*数据库操作类定义 */
	CDbCommand cmd_sql(conn); //与DB 建立连接。

	/* 实体类定义 */
	CModel tmmsm33czxs("TMMSM33CZXS");

	//初始化实体类
	tmmsm33czxs.Reset();


	try
	{
		CPageInfo pageInfo;

		
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("C_DIV"))
			v_c_div = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString().Trim();

		if (v_c_div == "2")//碳钢
		{
			v_strand = "XABCDEFZ";
			for (int i = 0; i < 8; i++)
			{
				tmmsm33czxs.Reset();
				if (i == 0)
				{
					tmmsm33czxs["STRAND_NO"] = "CX";
				}
				else
				{
					tmmsm33czxs["STRAND_NO"] = v_strand.Substring(i,1);
					Log::Info("", __FUNCTION__, "STRAND_NO=[{0}]", tmmsm33czxs["STRAND_NO"].ToString());
				}
				
				tmmsm33czxs["ST_NO"] = v_st_no;
				tmmsm33czxs["C_DIV"] = v_c_div;
				tmmsm33czxs["COE_A"] = 1;
				tmmsm33czxs["COE_B"] = 1;
				tmmsm33czxs["COE_B_UPPER_LIMIT"] = 1.05;
				tmmsm33czxs["COE_B_LOWER_LIMIT"] = 0.95;
				tmmsm33czxs["UPDATE_TIME_LIMIT"] = 30;//30分钟
				if (!tmmsm33czxs.QueryCount("ST_NO,STRAND_NO"))//根据主键去查，没有就新增，有就不处理
				{
					tmmsm33czxs.Insert();
				}
					

			}
		}
		else if (v_c_div == "1") //不锈钢
		{
			v_strand = "XABZ";
			for (int i = 0; i < 4; i++)
			{
				tmmsm33czxs.Reset();
				if (i == 0)
				{
					tmmsm33czxs["STRAND_NO"] = "CX";
				}
				else
				{
					tmmsm33czxs["STRAND_NO"] = v_strand.Substring(i, 1);
					Log::Info("", __FUNCTION__, "STRAND_NO=[{0}]", tmmsm33czxs["STRAND_NO"].ToString());
				}

				tmmsm33czxs["ST_NO"] = v_st_no;
				tmmsm33czxs["C_DIV"] = v_c_div;
				tmmsm33czxs["COE_A"] = 1;
				tmmsm33czxs["COE_B"] = 1;
				tmmsm33czxs["COE_B_UPPER_LIMIT"] = 1.05;
				tmmsm33czxs["COE_B_LOWER_LIMIT"] = 0.95;
				tmmsm33czxs["UPDATE_TIME_LIMIT"] = 30;//30分钟
				if (!tmmsm33czxs.QueryCount("ST_NO,STRAND_NO"))//根据主键去查，没有就新增，有就不处理
				{
					tmmsm33czxs.Insert();
				}
			}
		}











		  

	}
	/*捕获数据库操作异常*/
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		//LogTrace(1,1,"%s",(const char*)sqlstr);
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "DB error:" + sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	//LogTrace(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	////LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);
	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;

}

