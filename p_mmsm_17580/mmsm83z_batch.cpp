/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:     lizhen
Version:    1.0
Date:		2025-07-17
Description:H01H02自动上料批处理
**************************************************/
//框架用头文件
#include "stdafx.h"
int f_mmsm83z_bacth_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

// service入口
BM2F_ENTERACE(mmsm83z_batch)
//-EP_SYSTEM_HEAD_END

int f_mmsm83z_batch(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	int i_idx = 0;
	CString sqlstr = "";
	CString stat_date = "";
	CString begin_time = "";
	CString end_time = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal i_count = 0;
	int j = 0;
	CString seq_id = "0";
	CString send_flag = "0";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	

	EIClass bcls_ret4;
	EIClass bcls_ret2;

	CModel tmmsm83("TMMSM83");

	CString deal_flag = "";

	begin_time = CDateTime::Now().AddDays(-31).ToString("yyyyMMddHHmmss");
	end_time = CDateTime::Now().AddHours(-1).ToString("yyyyMMddHHmmss");
	CString date_time = CDateTime::Now().ToString("yyyyMMddHHmmss");

	Log::Info("", __FUNCTION__, "begin_time =[{0}],end_time=[{1}]", begin_time, end_time);

	try
	{

		EIClass EI_in;
		EI_in.Tables[0].Columns.Add(DT_STRING, "C_ORDERID");
		EI_in.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO");
		EI_in.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO1");
		EI_in.Tables[0].Columns.Add(DT_STRING, "C_X_ITEM");
		EI_in.Tables[0].Columns.Add(DT_STRING, "BUNKER_NO_ORIGINAL");
		EI_in.Tables[0].Columns.Add(DT_DECIMAL, "STOCK_WT");
		EI_in.Tables[0].Columns.Add(tmmsm83);

		sqlstr = " SELECT B.C_ORDERID,\
			A.BUNKER_NO,\
			a.BUNKER_NO_ORIGINAL                       BUNKER_NO1,\
			b.X_ITEM                                   C_X_ITEM,\
			decode(BELT, 'G105', 'XLG1#PD', 'XLG2#PD') BUNKER_NO_ORIGINAL,\
			A.REAL_WEIGHT * 1000                       STOCK_WT,\
			A.SEQ_CODE,\
			A.FLAG1,\
			A.TIME_STAMPS,\
			A.MAT_CODE,\
			C.MAT_CODE                                 MAT_CODE1\
			FROM TMMSM83 A\
			LEFT JOIN(select row_number() over(partition by MAT_CODE order by DATE_START DESC) rwid, t81v.*\
				from tmmsm81v t81v) B ON A.MAT_CODE = B.MAT_CODE and b.rwid = 1\
			LEFT JOIN TMMSM60 C ON A.BUNKER_NO = C.BUNKER_NO\
			WHERE 1 = 1\
			AND A.FLAG1 = ' '\
			AND A.BUNKER_NO_ORIGINAL IN('H01', 'H02')\
			AND A.TIME_STAMPS >= '2025071711000000'\
			AND B.C_STATE = '1' ";
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmmsm83);
			
			EI_in.Tables[0].Rows.Clear();
			EI_in.Tables[0].Rows.Add();
			
			EI_in.Tables[0].Rows[0]["C_ORDERID"] = cmd_inq.GetString(1);
			EI_in.Tables[0].Rows[0]["BUNKER_NO"] = cmd_inq.GetString(2);
			EI_in.Tables[0].Rows[0]["BUNKER_NO1"] = cmd_inq.GetString(3);
			EI_in.Tables[0].Rows[0]["C_X_ITEM"] = cmd_inq.GetString(4);
			EI_in.Tables[0].Rows[0]["BUNKER_NO_ORIGINAL"] = cmd_inq.GetString(5);
			EI_in.Tables[0].Rows[0]["STOCK_WT"] = cmd_inq.GetDecimal(6);
			EI_in.Tables[0].Rows[0]["SEQ_CODE"] = cmd_inq.GetString(7);
			Log::Trace("", __FUNCTION__,"FLAG1[{0}]TIME_STAMPS[{1}]", cmd_inq.GetString(7), cmd_inq.GetString(8));
			if (cmd_inq.GetString(10)== cmd_inq.GetString(11))
			{
				doFlag = f_mmsm83z_bacth_proc(&EI_in, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm831_upd_mmlc83失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm83["REC_REVISE_TIME"] = date_time;
				tmmsm83["REC_REVISOR"] = s.userid;
				tmmsm83["FLAG1"] = "1";
				tmmsm83.Update("FLAG1,REC_REVISE_TIME,REC_REVISOR", "SEQ_CODE");
			}
			else
			{
				
				tmmsm83["REC_REVISE_TIME"] = date_time;
				tmmsm83["REC_REVISOR"] = s.userid;
				tmmsm83["FLAG1"] = "E";
				tmmsm83["REMARK"] = "高位料仓"+ cmd_inq.GetString(2) +"物料代码与MES配置不同。";
				tmmsm83.Update("FLAG1,REC_REVISE_TIME,REC_REVISOR,REMARK", "SEQ_CODE");
			}
		}
		cmd_inq.Close();
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
