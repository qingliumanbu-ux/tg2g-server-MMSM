/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 取炉次的开始信息
中频炉：tmmsm19
转炉:tmmsm21
电炉:tmmsm20
AOD:tmmsm27
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(mmsm82a_inq2)
int f_mmsm_gyupd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82a_inq2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_where = "";
	CString heat_no = "";

	CDbCommand cmd_inq(conn);

	CModel tmmsmgy05("TMMSMGY05");

	try
	{ 	

			
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString(); 

	/*		sqlstr = " select heat_no,l2_proc_no,VALID_FLAG_2,VALID_FLAG_1"
				" FROM TMMSMGY05"
				" WHERE HEAT_NO=@heat_no"
				;
			EIClass bcls_ret1;
			EIClass bcls_rec1;
			bcls_rec1.Tables[0].Columns.Add(tmmsmgy05);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				tmmsmgy05.Reset();
				if (cmd_inq.GetString(3) != "1")
				{
					bcls_rec1.Tables[0].Rows.Clear();
					bcls_rec1.Tables[0].Rows.Add();
					bcls_rec1.Tables[0].Rows[0]["HEAT_NO"] = cmd_inq.GetString(1); 					
					doFlag = f_mmsm_gyupd(&bcls_rec1, &bcls_ret1, conn);

					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			cmd_inq.Close();  */


			Log::Info("", __FUNCTION__, "heat_no =[{0}]", heat_no);
			sqlstr = " select * from tmmsmgy06"
				" where 1=1"
				" and heat_no = @heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();   			

			

			//取投料信息
			bcls_ret->Tables.Add();
			sqlstr = " select stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,MAT_NAME,LOT_NO,SUM(OUT_STOCK_WT) as DEVO_WT	 "
				" from tmmsm56"
				" where 1=1"
				" and mat_code not in (SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
				" and heat_no = @heat_no"				
				" group  by stat_date,sm_plan_nol2,heat_no,st_no,dev_code,mat_code,MAT_NAME,LOT_NO "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
			cmd_inq.Close();

			bcls_ret->Tables.Add();
			sqlstr = " select mat_no,recv_mat_time,RECEIVE_WEIGHT as mat_act_wt,st_no"
				" from tmmsm01"
				" where heat_no=@heat_no and concess_con_flag!='1'"
				" union all"
				" select mat_no,recv_mat_time,RECEIVE_WEIGHT as mat_act_wt,st_no"
				" from hmmsm01"
				" where 1=1"
				"  and mat_no not in (select IN_MAT_NO from tmmsm35 ) and concess_con_flag!='1' "
				" and heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
			cmd_inq.Close();

			bcls_ret->Tables.Add();
			sqlstr = " select * from tmmsm2a_send"
				" where  HANDLE_DIV!='F' and SEND_FLAG = '1' and RTN_FLAG != '1'"
				" and heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[3]);
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
