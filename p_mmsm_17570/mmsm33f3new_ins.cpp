/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wxf
Version:    1.0
Date:       2023-07-05
Description: 板坯切断新增
**************************************************/
//框架头文件
#include "stdafx.h" 

//业务头文件


//外部函数声明

int f_mmsm33_insert(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE(mmsm33f3new_ins)

int f_mmsm33f3new_ins(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */


	/* 实体类定义 */


	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		/*if (bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString().Trim() == "5")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				if (bcls_rec->Tables[0].Columns.Contains("MEASURE_WT_FLAG"))
					bcls_rec->Tables[0].Rows[i]["MEASURE_WT_FLAG"] = "0";
			}
		}*/

		if (bcls_rec->Tables[0].Columns.Contains("CUT_TIMES1") && bcls_rec->Tables[0].Columns.Contains("CUT_TIMES5")
			&& bcls_rec->Tables[0].Columns.Contains("CUT_TIMES2") && bcls_rec->Tables[0].Columns.Contains("CUT_TIMES6")
			&& bcls_rec->Tables[0].Columns.Contains("CUT_TIMES3") && bcls_rec->Tables[0].Columns.Contains("CUT_TIMES7")
			&& bcls_rec->Tables[0].Columns.Contains("CUT_TIMES4") )
		{
			Log::Trace("", "", "新切割");
			for (CDecimal i = 1; i <= 7; i = i + 1)
			{
				if (bcls_rec->Tables[0].Rows[0]["CUT_TIMES" + i.ToString()].ToDecimal() > 0)
				{
					EIClass inBlock;
					inBlock.Copy(bcls_rec->Tables.get_DataSet());
					if (inBlock.Tables[0].Columns.Contains("STRAND_NO") == false)
						inBlock.Tables[0].Columns.Add(DT_STRING, "STRAND_NO");
					if (inBlock.Tables[0].Columns.Contains("CUT_TIMES") == false)
						inBlock.Tables[0].Columns.Add(DT_STRING, "CUT_TIMES");
					if (inBlock.Tables[0].Columns.Contains("PRACT_COLL_MODE") == false)
						inBlock.Tables[0].Columns.Add(DT_STRING, "PRACT_COLL_MODE");
					inBlock.Tables[0].Rows[0]["PRACT_COLL_MODE"] = "0";
					inBlock.Tables[0].Rows[0]["STRAND_NO"] = i.ToString();
					inBlock.Tables[0].Rows[0]["CUT_TIMES"] = inBlock.Tables[0].Rows[0]["CUT_TIMES" + i.ToString()];
					Log::Trace("", "", "STRAND_NO = [{0}]", i.ToString());
					Log::Trace("", "", "CUT_TIMES = [{0}]", inBlock.Tables[0].Rows[0]["CUT_TIMES"].ToDecimal());

					doFlag = f_mmsm33_insert(&inBlock, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
			}
		}
		else
		{
			Log::Trace("", "", "老切割");
			doFlag = f_mmsm33_insert(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		//strcpy(s.msg, "程序测试中，暂停操作！");
		//throw CApplicationException(-1, s.msg, log.Location);
	}

	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


