/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-05-25
Description: 板坯切断炉次查询
***********************************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

int f_t8f009_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//中频炉-能源
int f_t8f007_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//AOD能源发送
int f_t8f010_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源电炉
int f_t8f001_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源-转炉
int f_t8f004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源实绩-转炉
int f_t8f005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源LE
int f_t8f006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源RH
int f_t8f008_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源VOD
int f_t8f011_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源连铸
int f_t8f003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源-加料

// service入口
BM2F_ENTERACE(mmsmtf08_xf)

int f_mmsmtf08_xf(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = " ";
	CString sqlstr_temp = " ";
	CString v_heat_no = " ";
	CString l2_proc_no = " ";
	int   blkNum;
	CString dev_code = " ";
	CString proc_no = " ";

	CDbCommand cmd_inq(conn);

	try
	{
		//能源
		blkNum = bcls_rec->Tables.IndexOf("T8F");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("T8F");
		}

		if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_DIV");
		}

		if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_NO");
		}

		if (!bcls_rec->Tables["T8F"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables["T8F"].Columns.Contains("PROD_SEQ_NO"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROD_SEQ_NO");
		}
		//PROC_COUNT
		if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_COUNT"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_COUNT");
		}
		//L2_PROC_NO
		if (!bcls_rec->Tables["T8F"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}
		if (!bcls_rec->Tables["T8F"].Columns.Contains("SM_PLAN_NOL2"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
		}
		bcls_rec->Tables["T8F"].Rows.Add();
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			v_heat_no = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString();
			cmd_inq.SetCommandText(" select HEAT_NO, DEV_CODE, PROC_NO, L2_PROC_NO "
				" from(select HEAT_NO, DEV_CODE, PROC_NO, L2_PROC_NO "
				" from tmmsm19 "
				" UNION ALL "
				" select HEAT_NO, DEV_CODE, PROC_NO, L2_PROC_NO "
				" from tmmsm20 "
				" UNION ALL "
				" select HEAT_NO, DEV_CODE, PROC_NO, L2_PROC_NO "
				" from tmmsm21 "
				" UNION ALL "
				" select HEAT_NO, DEV_CODE, PROC_NO, L2_PROC_NO "
				" from tmmsm23 "
				" UNION ALL "
				" select HEAT_NO, DEV_CODE, PROC_NO, L2_PROC_NO "
				" from tmmsm24 "
				" UNION ALL "
				" select HEAT_NO, DEV_CODE, PROC_NO, L2_PROC_NO "
				" from tmmsm25 "
				" UNION ALL "
				" select HEAT_NO, DEV_CODE, PROC_NO, L2_PROC_NO "
				" from tmmsm26 "
				" UNION ALL "
				" select HEAT_NO, DEV_CODE, PROC_NO, L2_PROC_NO "
				" from tmmsm27 "
				" UNION ALL "
				" select HEAT_NO, DEV_CODE, PROC_NO, L2_PROC_NO "
				" from tmmsm31) "
				" where 1 = 1 "
				" AND HEAT_NO = '"+v_heat_no+"' "
				" ");
			cmd_inq.ExecuteReader();
			while  (cmd_inq.Read())
			{
				dev_code = cmd_inq.GetString(2);
				proc_no = cmd_inq.GetString(3);
				l2_proc_no = cmd_inq.GetString(4);
				bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = "U";
				bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = l2_proc_no;
				bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = v_heat_no;
				bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"] = proc_no;
				if (dev_code.Substring(0, 1) == "Z"){
					doFlag = f_t8f009_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (dev_code.Substring(0, 1) == "F"){
					doFlag = f_t8f005_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (dev_code.Substring(0, 1) == "R"){
					doFlag = f_t8f006_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (dev_code.Substring(0, 1) == "V"){
					doFlag = f_t8f008_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (dev_code.Substring(0, 1) == "A"){
					doFlag = f_t8f007_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (dev_code.Substring(0, 1) == "E"){
					doFlag = f_t8f010_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (dev_code.Substring(0, 1) == "C"){
					doFlag = f_t8f011_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (dev_code.Substring(0, 1) == "B"){
					doFlag = f_t8f004_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			Log::Trace("", "dev_code", "dev_code = {0}", dev_code);
			cmd_inq.Close();
		}
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
