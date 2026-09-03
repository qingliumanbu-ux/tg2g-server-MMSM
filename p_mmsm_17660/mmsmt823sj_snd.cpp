/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    3.0
Date:		2018-06-21
Description:自动抛实绩消耗
**************************************************/
//框架用头文件
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsmt823sj_snd)
//-EP_SYSTEM_HEAD_END

int f_t823s4_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送BOF
int f_t823s7_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送LF
int f_t823s8_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送RH
int f_t823s5_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送EAF
int f_t823s3_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送IF
int f_t823s6_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送AOD
int f_t823s9_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送VOD
int f_t823sa_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送SA
int f_t823sb_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发送SB


int f_mmsmt823sj_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString recv_mat_time = " ";
	CString heat_no = "";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CModel tmmsmgy05("TMMSMGY05");
	CModel tmmsmt823sj("TMMSMT823SJ");



	//当前时间前4个小时
	//begin_time = "20241129160000";
	//end_time = "20241202100000";
	begin_time = CDateTime::Now().AddHours(-1).ToString("yyyyMMddHHmmss");
	end_time = CDateTime::Now().AddHours(0).ToString("yyyyMMddHHmmss");

	Log::Info("", __FUNCTION__, "begin_time =[{0}],end_time=[{1}]", begin_time, end_time);


	EIClass bcls_ret1;
	EIClass bcls_rec1;
	bcls_rec1.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec1.Tables[0].Columns.Add(DT_STRING, "PROC_NO");
	bcls_rec1.Tables[0].Columns.Add(DT_STRING, "L2_PROC_NO");
	bcls_rec1.Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
	bcls_rec1.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec1.Tables[0].Rows.Add();


	try
	{

		sqlstr = " SELECT T.HEAT_NO FROM TMMSMGY05 T "
			" WHERE 1=1"
			" and T.RECV_MAT_TIME <= '" + end_time + "'"
			" and T.RECV_MAT_TIME >= '" + begin_time + "'"
			" ORDER BY T.RECV_MAT_TIME "
			;
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.Parameters.Set("begin_time", begin_time);
		cmd_inq1.Parameters.Set("end_time", end_time);
		cmd_inq1.SetCommandText(sqlstr);
		cmd_inq1.ExecuteReader();
		while (cmd_inq1.Read())
		{
			heat_no = cmd_inq1.GetString(1);

			//BOF-T823S4-TMMSM21
			sqlstr =
				" select HEAT_NO,PROC_NO,SM_PLAN_NOL2 from TMMSM21 where heat_no='" + heat_no + "'"
				;
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{
				//给智慧质量发送BOF
				bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
				bcls_rec1.Tables[0].Rows[0]["PROC_NO"] = cmd_inq.GetString(2);
				bcls_rec1.Tables[0].Rows[0]["SM_PLAN_NOL2"] = cmd_inq.GetString(3);
				doFlag = f_t823s4_snd(&bcls_rec1, &bcls_ret1, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t823s4_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		    cmd_inq.Close();

			sqlstr =
				" select HEAT_NO,PROC_NO,L2_PROC_NO from TMMSM24 where heat_no='" + heat_no + "'"
				;
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{
				//给智慧质量发送LF
				bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
				bcls_rec1.Tables[0].Rows[0]["PROC_NO"] = cmd_inq.GetString(2);
				bcls_rec1.Tables[0].Rows[0]["L2_PROC_NO"] = cmd_inq.GetString(3);
				Log::Info("", __FUNCTION__, "HEAT_NO =[{0}],L2_PROC_NO=[{1}]", cmd_inq.GetString(1), cmd_inq.GetString(3));

				doFlag = f_t823s7_snd(&bcls_rec1, &bcls_ret1, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t823s7_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_inq.Close();

			sqlstr =
				" select HEAT_NO,PROC_NO,L2_PROC_NO from TMMSM23 where heat_no='" + heat_no + "'"
				;
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{
				//给智慧质量发送RH
				bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
				bcls_rec1.Tables[0].Rows[0]["PROC_NO"] = cmd_inq.GetString(2);
				bcls_rec1.Tables[0].Rows[0]["L2_PROC_NO"] = cmd_inq.GetString(3);
				doFlag = f_t823s8_snd(&bcls_rec1, &bcls_ret1, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t823s8_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_inq.Close();

			sqlstr =
				" select HEAT_NO,PROC_NO,L2_PROC_NO from TMMSM20 where heat_no='" + heat_no + "'"
				;
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			
			while (cmd_inq.Read())
			{
				//给智慧质量发送EAF
				bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
				bcls_rec1.Tables[0].Rows[0]["PROC_NO"] = cmd_inq.GetString(2);
				bcls_rec1.Tables[0].Rows[0]["L2_PROC_NO"] = cmd_inq.GetString(3);
				doFlag = f_t823s5_snd(&bcls_rec1, &bcls_ret1, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t823s5_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_inq.Close();

			sqlstr =
				" select HEAT_NO,L2_PROC_NO from TMMSM19 where heat_no='" + heat_no + "'"
				;
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{
				//给智慧质量发送IF
				bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
				bcls_rec1.Tables[0].Rows[0]["L2_PROC_NO"] = cmd_inq.GetString(2);
				doFlag = f_t823s3_snd(&bcls_rec1, &bcls_ret1, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t823s3_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_inq.Close();

			sqlstr =
				" select HEAT_NO,PROC_NO,L2_PROC_NO from TMMSM27 where heat_no='" + heat_no + "'"
				;
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
		
			while (cmd_inq.Read())
			{
				//给智慧质量发送AOD
				bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
				bcls_rec1.Tables[0].Rows[0]["PROC_NO"] = cmd_inq.GetString(2);
				bcls_rec1.Tables[0].Rows[0]["L2_PROC_NO"] = cmd_inq.GetString(3);
				doFlag = f_t823s6_snd(&bcls_rec1, &bcls_ret1, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t823s6_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			cmd_inq.Close();

			sqlstr =
				" select HEAT_NO,PROC_NO,L2_PROC_NO from TMMSM25 where heat_no='" + heat_no + "'"
				;
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			
			while (cmd_inq.Read())
			{
				//给智慧质量发送VOD
				bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
				bcls_rec1.Tables[0].Rows[0]["PROC_NO"] = cmd_inq.GetString(2);
				bcls_rec1.Tables[0].Rows[0]["L2_PROC_NO"] = cmd_inq.GetString(3);				
				doFlag = f_t823s9_snd(&bcls_rec1, &bcls_ret1, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t823s9_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			}
			cmd_inq.Close();

			sqlstr =
				" select HEAT_NO,L2_PROC_NO from TMMSM31 where heat_no='" + heat_no + "'"
				;
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{
				//给智慧质量发送连铸
				bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
				bcls_rec1.Tables[0].Rows[0]["L2_PROC_NO"] = cmd_inq.GetString(2);
				doFlag = f_t823sa_snd(&bcls_rec1, &bcls_ret1, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t823sa_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			}
			cmd_inq.Close();

			sqlstr =
				" select HEAT_NO,MAT_NO from TMMSM34 where heat_no='" + heat_no + "'"
				;
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{
				//给智慧质量发送修磨
				bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1);
				bcls_rec1.Tables[0].Rows[0]["MAT_NO"] = cmd_inq.GetString(2);
				doFlag = f_t823sb_snd(&bcls_rec1, &bcls_ret1, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_t823sb_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			}
			cmd_inq.Close();

		}

		cmd_inq1.Close();	
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
