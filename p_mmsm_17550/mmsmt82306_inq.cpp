/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XX
Version:    1.0
Date:
Description: 连铸退钢水查询
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件

int f_t82306_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量投料
BM2F_ENTERACE(mmsmt82306_inq)

int f_mmsmt82306_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	CString sql_insert = " ";
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_2a(conn);
	CString station_id = " ";
	CString v_proc_div = " ";
	CString	datetime("");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsmgy05("TMMSMGY05");
	CString id = " ";
	CString sm_plan_nol2 = " ";
	CString heat_no = " ";
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	blkNum = bcls_rec->Tables.IndexOf("T823");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("T823");
	}
	bcls_rec->Tables["T823"].Columns.Add(tmmsm2a);
	if (!bcls_rec->Tables["T823"].Columns.Contains("DEAL_FLAG"))
	{
		bcls_rec->Tables["T823"].Columns.Add(DT_STRING, "DEAL_FLAG");
	}
	try
	{
		sqlstr =" select a.SM_PLAN_NOL2,A.L2_PROC_NO, A.HEAT_NO, a.LADLE_ARRIVE_TIME "
			" from TMMSM31 A "
			" LEFT JOIN TMMSMGY05 C ON A.SM_PLAN_NOL2 = C.SM_PLAN_NOL2 "
			" WHERE A.REC_CREATOR != 'QC' "
			" and A.LADLE_ARRIVE_TIME != ' ' AND C.SEND_T823 != '1' "
			" AND  LADLE_ARRIVE_TIME< @datetime "
			" AND  LADLE_ARRIVE_TIME> @begintime ";
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Set("datetime", CDateTime::Now().AddHours(-4).ToString("yyyyMMddHHmmss"));
		cmd_sql.Parameters.Set("begintime", CDateTime::Now().AddDays(-150).ToString("yyyyMMddHHmmss"));
		cmd_sql.ExecuteReader();
		int i = 0;
		while (cmd_sql.Read())
		{
			sm_plan_nol2 = cmd_sql.GetString(1);
			heat_no = cmd_sql.GetString(3);
			Log::Info("", __FUNCTION__, "heat_no  =[{0}],sm_plan_nol2=[{1}]", heat_no,sm_plan_nol2);
			cmd_2a.SetCommandText(
				" select  DEV_CODE,HEAT_NO,SM_PLAN_NOL2,MAT_CODE,MAT_NAME,ST_NO,sum(OUT_STOCK_WT) DEVO_WT from TMMSM56 "
				"  WHERE SM_PLAN_NOL2 = '" + sm_plan_nol2 + "' AND HEAT_NO='" + heat_no + "' and DEVO_WT!=0 group by DEV_CODE, HEAT_NO, SM_PLAN_NOL2, MAT_CODE, MAT_NAME, ST_NO ");
			cmd_2a.ExecuteReader();
			while (cmd_2a.Read())
			{
				cmd_2a.Fetch(tmmsm2a);
				bcls_rec->Tables["T823"].Rows.Clear();
				tmmsmgy05["SEND_T823"] = "1";
				tmmsmgy05["SEND_T823_TIME"] = datetime;
				tmmsmgy05["SM_PLAN_NOL2"] = sm_plan_nol2;
				tmmsmgy05.Update("SEND_T823,SEND_T823_TIME", "SM_PLAN_NOL2");
				Log::Info("", __FUNCTION__, "tmmsm2a  =[{0}]", tmmsm2a["MAT_NAME"].ToString());
				tmmsm2a.MergeTo(bcls_rec->Tables["T823"], false);
				bcls_rec->Tables["T823"].Rows[0]["DEAL_FLAG"] = "I";
				doFlag = f_t82306_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_2a.Close();

			i++;
			if (i == 10)
			{
				tpcommit(0);
				tpbegin(0, 0);
				i = 0;
			}
		}
		cmd_sql.Close();
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


