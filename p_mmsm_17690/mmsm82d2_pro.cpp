/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 原料模板画面维护
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm82d2_pro)
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82d2_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   n_count = 0;
	int   blkNum;
	int  proc_sum = 0;				//操作总数
	CDbCommand cmd_sql(conn); //与DB 建立连接。
	CString msgstr = "提示信息:";	//提示信息。
	CString sqlstr = "";
	CString sqlstr0 = "";
	CString sqlstr1 = "";
	CString c_sql_condition = "";
	CDecimal seq_id = 0;
	CDecimal count1 = 0;
	CDecimal count2= 0;
	CDecimal sum_radio = 0;
	CString heat_no = "";
	CString l2_proc_no = "";
	try
	{
		EIClass bcls_rec_xh;
		EIClass bcls_ret_xh;
		bcls_rec_xh.Tables[0].set_TableName("xh");
		bcls_rec_xh.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_rec_xh.Tables[0].Rows.Add();
		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		CModel tmmsmyl("TMMSMGY09");
		Log::Trace("", "", "获取传入参数...");

		Log::Trace("", "", "当前时间：nowTime=[{0}]", nowTime);
		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());
			
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				
				tmmsmyl.Reset();
				tmmsmyl.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				tmmsmyl.TrimOrBlank();
				Log::Trace("", "", "L2_PROC_NO=[{0}]", tmmsmyl["L2_PROC_NO"].ToString().Trim());
				const CDecimal ratioNum = tmmsmyl["RATIO_NUM"].ToDecimal();
				//提前校验比例是否为正数
				if (ratioNum <= 0)
				{
					strcpy(s.msg, CString::Format("L2_PROC_NO=[%s]的比例值[%s]必须大于0", tmmsmyl["L2_PROC_NO"].ToString().Trim(), ratioNum.ToString()));
					throw CApplicationException(-1, s.msg, log.Location);
		
				}
				if (tmmsmyl.QueryCount("L2_PROC_NO,HEAT_NO")>0)
				{
					Log::Trace("", "", "HEAT_NO=[{0}]", tmmsmyl["HEAT_NO"].ToString().Trim());
					tmmsmyl["REC_REVISOR"] = s.userid;
					tmmsmyl["REC_REVISE_TIME"] = nowTime;
					tmmsmyl["RATIO_NUM"] = bcls_rec->Tables["UPD"].Rows[i]["RATIO_NUM"].ToDecimal();
					tmmsmyl.TrimOrBlank();
					proc_sum += tmmsmyl.Update("RATIO_NUM", "L2_PROC_NO,HEAT_NO");
				}
				else
				{
					Log::Trace("", "", "HEAT_NO2=[{0}]", tmmsmyl["HEAT_NO"].ToString().Trim());
					tmmsmyl["RATIO_NUM"] = bcls_rec->Tables["UPD"].Rows[i]["RATIO_NUM"].ToDecimal();
				    Log::Trace("", "", "RATIO_NUM=[{0}]", tmmsmyl["RATIO_NUM"].ToDecimal());
					tmmsmyl["REC_CREATOR"] = s.userid;
					tmmsmyl["REC_CREATE_TIME"] = nowTime;
                    tmmsmyl.TrimOrBlank();
				     proc_sum += tmmsmyl.Insert();
					
				}
			}
			
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				tmmsmyl.Reset();
				tmmsmyl.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				tmmsmyl.TrimOrBlank();
				Log::Trace("", "", "L2_PROC_NO=[{0}]", tmmsmyl["L2_PROC_NO"].ToString().Trim());
				//判断表TMMSMGY09中L2_PROC_NO等于tmmsmyl["L2_PROC_NO"].ToString().Trim()的RATIO_NUM的和为100且每一个RATIO_NUM大于0
				sqlstr ="SELECT COALESCE(SUM(RATIO_NUM), 0) AS SUM_RATIO,COUNT(CASE WHEN RATIO_NUM > 0 THEN 1 END) AS COUNT_GT0,COUNT(1) AS TOTAL_COUNT FROM TMMSMGY09 WHERE L2_PROC_NO = @L2_PROC_NO";
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Set("L2_PROC_NO", tmmsmyl["L2_PROC_NO"].ToString().Trim());
				cmd_sql.ExecuteReader();
				
				if (cmd_sql.Read())
				{
					sum_radio = cmd_sql.GetDecimal(1);
					count1 = cmd_sql.GetDecimal(1);
					count2 = cmd_sql.GetDecimal(1);
				}
				cmd_sql.Close();
				if (sum_radio != 100 || count1 != count2)
				{
					strcpy(s.msg, "比例不符合!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				heat_no = tmmsmyl["HEAT_NO"].ToString().Trim();
				if (heat_no != "")
				{
					bcls_rec_xh.Tables[0].Rows[0]["HEAT_NO"] = heat_no;
					doFlag = f_mmsm_gyupd(&bcls_rec_xh, &bcls_ret_xh, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

                 }
				
			}
			
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			
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