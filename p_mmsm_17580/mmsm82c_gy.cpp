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
BM2F_ENTERACE(mmsm82c_gy)
int f_mmsm_gyins2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm82c_gy(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString begin_time = "";
	CString end_time = "";
	CString sqlstr_where = "";
	CString heat_no = "";
	CString affirm_flag = "0";

	CDbCommand cmd_inq(conn);
	CModel tmmsmgy05("TMMSMGY05");

	EIClass bcls_ret1;

	try
	{ 	

			
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString(); 
			tmmsmgy05.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			sqlstr = " select AFFIRM_FLAG from tmmsmgy05"
				" where 1=1"
				" and heat_no=@heat_no"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				affirm_flag = cmd_inq.GetString(1);	 
			}
			cmd_inq.Close();
			
			/*if (affirm_flag != "1")
			{			
			doFlag = f_mmsm_gyins2(bcls_rec, &bcls_ret1, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			}*/

				sqlstr = " select t.* "
					" from tmmsmgy06 t"
					" where 1=1"
					" and heat_no = @heat_no"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
				cmd_inq.Close();

				if (!bcls_ret->Tables[0].Columns.Contains("HJ_WT"))
				{
					bcls_ret->Tables[0].Columns.Add(DT_STRING, "HJ_WT");
				}
				if (!bcls_ret->Tables[0].Columns.Contains("FL_WT"))
				{
					bcls_ret->Tables[0].Columns.Add(DT_STRING, "FL_WT");
				}
				if (!bcls_ret->Tables[0].Columns.Contains("QT_WT"))
				{
					bcls_ret->Tables[0].Columns.Add(DT_STRING, "QT_WT");
				}
				for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
				{
					sqlstr = " select sum(case when mat_code in (select mat_code from tmmsm50 where mat_type in ('1','2','4')) then DEVO_WT else 0 end)/1000"
						",sum(case when mat_code in(select mat_code from tmmsm50 where mat_type in('3')) then DEVO_WT else 0 end)/1000"
						",sum(case when mat_code in(select mat_code from tmmsm50 where mat_type NOT in('1','2','4','3')) then DEVO_WT else 0 end)/1000"
						" from "
						"( select mat_code,DEVO_WT"
						" from tmmsm2a_yl"
						" where 1=1"
						" and mat_code != 'TS0000'"	 //有使用投料进行投铁水
						" and dev_code = @dev_code"
						" and l2_proc_no = @l2_proc_no"
						" union all"
						" select mat_code,DEVO_WT FROM TMMSM2A_TS"
						" where 1=1"
						" and dev_code = @dev_code"
						" and mat_code = 'TS0000'"
						" and l2_proc_no = @l2_proc_no)"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("dev_code", bcls_ret->Tables[0].Rows[i]["DEV_CODE"].ToString());
					cmd_inq.Parameters.Set("l2_proc_no", bcls_ret->Tables[0].Rows[i]["L2_PROC_NO"].ToString());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						bcls_ret->Tables[0].Rows[i]["HJ_WT"] = cmd_inq.GetDecimal(1);
						bcls_ret->Tables[0].Rows[i]["FL_WT"] = cmd_inq.GetDecimal(2); 
						bcls_ret->Tables[0].Rows[i]["QT_WT"] = cmd_inq.GetDecimal(3);
					}
					cmd_inq.Close();
				}

		
			//取投料信息
			bcls_ret->Tables.Add();
			sqlstr = " select heat_no,proc_no,L2_PROC_NO,SM_PLAN_NOL2,PROC_COUNT,DEVO_TIME,MAT_CODE,MAT_NAME,DEVO_WT,DEV_CODE,STK_NO,WEIGH_NO, QUALITY_BATCH_NO,ID_2A,REMARK_2,SEQ_NO_2A,lot_no,DEVO_WT_SEND"
				" FROM ("
				" select heat_no,proc_no,L2_PROC_NO,SM_PLAN_NOL2,PROC_COUNT,DEVO_TIME,MAT_CODE,MAT_NAME,DEVO_WT,DEV_CODE,STK_NO,WEIGH_NO, QUALITY_BATCH_NO,ID_2A,REMARK_2,SEQ_NO_2A,lot_no"
				" ,case when mat_code in (SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG != '1') then DEVO_WT else 0 end DEVO_WT_SEND"
				" from tmmsm2A_yl t1"
				" where 1=1"
				" and mat_code != 'TS0000'"	 //有使用投料进行投铁水
				//" and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
				" and exists ( select 1 from tmmsmgy06 t2 where t2.l2_proc_no=t1.l2_proc_no and t2.heat_no = @heat_no)"					
				" union all"
				" select heat_no,proc_no,L2_PROC_NO,SM_PLAN_NOL2,0 PROC_COUNT,DEVO_TIME,MAT_CODE,MAT_NAME,DEVO_WT,DEV_CODE,' ' STK_NO,' ' WEIGH_NO, ' ' QUALITY_BATCH_NO,' ' ID_2A,' ' AS REMARK_2,' ' SEQ_NO_2A,' ' lot_no"
				", DEVO_WT AS DEVO_WT_SEND "
				" from tmmsm2a_ts t1"
				" where 1=1"
				" and HANDLE_DIV != 'H'"
				" and mat_code = 'TS0000'"
				" and heat_no = @heat_no" 				
				")"
				" order by dev_code,DEVO_TIME,ID_2A,mat_code" 				
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
			cmd_inq.Close();

			bcls_ret->Tables.Add();
			sqlstr = " select heat_no,L2_PROC_NO,PROC_NO,dev_code,mat_code,mat_name,lot_no,sum(DEVO_WT) DEVO_WT,count(1) count_num"
				" from ("
				" select heat_no,L2_PROC_NO,PROC_NO,dev_code,mat_code,mat_name,ID_2A,lot_no,sum(DEVO_WT) DEVO_WT"
				" from tmmsmgy08"
				" where 1=1"
				" and mat_code not in ( SELECT mat_code FROM TMMSM50 WHERE SEND_FLAG = '1')"
				" and heat_no = @heat_no"
				" group by  heat_no,L2_PROC_NO,PROC_NO,dev_code,mat_code,mat_name,ID_2A,lot_no"
				" )"
				"  group by  heat_no,L2_PROC_NO,PROC_NO,dev_code,mat_code,mat_name,lot_no"
				" order by dev_code,mat_code"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
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
