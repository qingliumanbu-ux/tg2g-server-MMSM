/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description:消耗工序基表维护
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/


/* ***** 静态函数申明 ***** */


BM2F_ENTERACE(mmsmws_pro)


int f_mmsmws_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;
	int  proc_sum = 0;				//操作总数
	CDbCommand cmd_sql(conn); //与DB 建立连接。
	CString msgstr = "提示信息:";	//提示信息。

	CString sqlstr = "";
	CString next_date = "";
	CString   v_proc_div = "";
	CString  table_name = "";
	CString c_sql_condition = "";
	CModel tmmsmws("TMMSMWS");
	CModel tmmsmws_temp("TMMSMWS");
	CModel tmmsmws_next("TMMSMWS");
	CDbCommand cmd_inq(conn);
	try
	{
		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", "", "当前时间：nowTime=[{0}]", nowTime);

		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("WE1_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["WE1_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE1_ADD"].Rows.get_Count(); i++)
			{
				tmmsmws.Reset();
				tmmsmws.MergeFrom(bcls_rec->Tables["WE1_ADD"].Rows[i]);
				tmmsmws["DATE_C"] = tmmsmws["DATE_C"].ToString().SubstringNE(0, 6);
				if (tmmsmws.QueryCount("DATE_C,MAT_CODE")==0)
				{
					
					tmmsmws["FACTORY_DIV"] = "LG1";
					tmmsmws["UNIT"] = "TON";
					tmmsmws["REC_CREATOR"] = s.userid;
					tmmsmws["REC_CREATE_TIME"] = nowTime;
					tmmsmws.TrimOrBlank();
					sqlstr = "INSERT INTO TMMSMWS";
					tmmsmws.Insert();
				}
				else
				{
					tmmsmws.Update("IN_STOCK_WT", "DATE_C,MAT_CODE");
				}
			}

		
		}

		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("WE2_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["WE2_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE2_ADD"].Rows.get_Count(); i++)
			{
				tmmsmws.Reset();
				tmmsmws.MergeFrom(bcls_rec->Tables["WE2_ADD"].Rows[i]);
				tmmsmws["DATE_C"] = tmmsmws["DATE_C"].ToString().SubstringNE(0, 6);
				if (tmmsmws.QueryCount("DATE_C,MAT_CODE") == 0)
				{
					tmmsmws["FACTORY_DIV"] = "LG1";
					tmmsmws["UNIT"] = "TON";
					tmmsmws["REC_CREATOR"] = s.userid;
					tmmsmws["REC_CREATE_TIME"] = nowTime;
					tmmsmws.TrimOrBlank();
					sqlstr = "INSERT INTO TMMSMWS";
					tmmsmws.Insert();
				}
				else
				{
					tmmsmws.Update("OUT_STOCK_WT", "DATE_C,MAT_CODE");
				}

			}
		}

	
		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("WE4_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["WE4_ADD"].Rows.get_Count());
		
			for (int i = 0; i < bcls_rec->Tables["WE4_ADD"].Rows.get_Count(); i++)
			{
				tmmsmws.Reset();
				tmmsmws.MergeFrom(bcls_rec->Tables["WE4_ADD"].Rows[i]);
				tmmsmws["DATE_C"] = tmmsmws["DATE_C"].ToString().SubstringNE(0, 6);
				if (tmmsmws.QueryCount("DATE_C,MAT_CODE") == 0)
				{
					tmmsmws["FACTORY_DIV"] = "LG1";
					tmmsmws["UNIT"] = "TON";
					tmmsmws["REC_CREATOR"] = s.userid;
					tmmsmws["REC_CREATE_TIME"] = nowTime;
					tmmsmws.TrimOrBlank();
					sqlstr = "INSERT INTO TMMSMWS";
					tmmsmws.Insert();
				}
				else
				{
					tmmsmws.Update("STOCK_WGT", "DATE_C,MAT_CODE");
				}

			}
			//下月期初
			sqlstr = " SELECT TO_CHAR(ADD_MONTHS(TO_DATE(" + tmmsmws["DATE_C"].ToString() + ", 'YYYY-MM'), 1), 'YYYYMM') FROM DUAL ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				next_date = cmd_inq.GetString(1);

			}
			cmd_inq.Close();
			for (int i = 0; i < bcls_rec->Tables["WE4_ADD"].Rows.get_Count(); i++)
			{
				tmmsmws_next.Reset();
				tmmsmws_next.MergeFrom(bcls_rec->Tables["WE4_ADD"].Rows[i]);
				tmmsmws_next["DATE_C"] = next_date;
				tmmsmws_next["STOCK_INI_WT"] = tmmsmws_next["STOCK_WGT"];
				tmmsmws_next["STOCK_WGT"] =0;
				if (tmmsmws_next.QueryCount("DATE_C,MAT_CODE") == 0)
				{
					tmmsmws_next["FACTORY_DIV"] = "LG1";
					tmmsmws_next["UNIT"] = "TON";
					tmmsmws_next["REC_CREATOR"] = s.userid;
					tmmsmws_next["REC_CREATE_TIME"] = nowTime;
					tmmsmws_next.TrimOrBlank();
					sqlstr = "INSERT INTO TMMSMWS";
					tmmsmws_next.Insert();
				}
				else
				{
					tmmsmws_next.Update("STOCK_INI_WT", "DATE_C,MAT_CODE");
				}

			}
		}

		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("WE5_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["WE5_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE5_ADD"].Rows.get_Count(); i++)
			{
				tmmsmws.Reset();
				tmmsmws_temp.Reset();
				tmmsmws.MergeFrom(bcls_rec->Tables["WE5_ADD"].Rows[i]);
				tmmsmws_temp.MergeFrom(bcls_rec->Tables["WE5_ADD"].Rows[i]);
				tmmsmws["DATE_C"] = tmmsmws["DATE_C"].ToString().SubstringNE(0, 6);
				if (tmmsmws.QueryCount("DATE_C,MAT_CODE") == 0)
				{
					tmmsmws["FACTORY_DIV"] = "LG1";
					tmmsmws["UNIT"] = "TON";
					tmmsmws["REC_CREATOR"] = s.userid;
					tmmsmws["REC_CREATE_TIME"] = nowTime;
			
					tmmsmws.TrimOrBlank();
					sqlstr = "INSERT INTO TMMSMWS";
					tmmsmws.Insert();
				}
				else
				{
					tmmsmws_temp.Query("DATE_C,MAT_CODE");
					tmmsmws["SOUTH_TO_WT"] = tmmsmws["SOUTH_TO_WT"].ToDecimal() + tmmsmws_temp["SOUTH_TO_WT"].ToDecimal();
					tmmsmws.Update("SOUTH_TO_WT", "DATE_C,MAT_CODE");
				}

			}
		}

		if (bcls_rec->Tables.Contains("WE6_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["WE6_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE6_ADD"].Rows.get_Count(); i++)
			{
				tmmsmws.Reset();
				tmmsmws_temp.Reset();
				tmmsmws.MergeFrom(bcls_rec->Tables["WE6_ADD"].Rows[i]);
				tmmsmws_temp.MergeFrom(bcls_rec->Tables["WE6_ADD"].Rows[i]);
				tmmsmws["DATE_C"] = tmmsmws["DATE_C"].ToString().SubstringNE(0, 6);
				if (tmmsmws.QueryCount("DATE_C,MAT_CODE") == 0)
				{
					tmmsmws["FACTORY_DIV"] = "LG1";
					tmmsmws["UNIT"] = "TON";
					tmmsmws["REC_CREATOR"] = s.userid;
					tmmsmws["REC_CREATE_TIME"] = nowTime;
					tmmsmws.TrimOrBlank();
					sqlstr = "INSERT INTO TMMSMWS";
					tmmsmws.Insert();
				}
				else
				{
					tmmsmws_temp.Query("DATE_C,MAT_CODE");
					tmmsmws["TO_SOUTH_WT"] = tmmsmws["TO_SOUTH_WT"].ToDecimal() + tmmsmws_temp["TO_SOUTH_WT"].ToDecimal();
					tmmsmws.Update("TO_SOUTH_WT", "DATE_C,MAT_CODE");
				}

			}
		}


		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);



	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
