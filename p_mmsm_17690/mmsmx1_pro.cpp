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
BM2F_ENTERACE(mmsmx1_pro)

int f_mmsmx1_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString v_proc_div = "";
	CString sqlstr = "";
	CString sqlstr_temp = "";
	CString v_table_name = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{

		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
		v_table_name = bcls_rec->Tables["PARA"].Rows[0]["TABLE_NAME"].ToString();
		CModel tmmsmyl(v_table_name);
		Log::Info("", __FUNCTION__, "v_table_name =[{0}]", v_table_name);
		Log::Trace("", __FUNCTION__, "v_proc_div =[{0}]", v_proc_div);
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmyl.Reset();
			tmmsmyl.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			Log::Trace("", __FUNCTION__, "MEMO_DETAIL =[{0}]", tmmsmyl["MEMO_DETAIL"].ToString());
			Log::Trace("", __FUNCTION__, "YEAR_MON =[{0}]", tmmsmyl["YEAR_MON"].ToString());
			if (v_proc_div == "I")
			{

				tmmsmyl["REC_CREATE_TIME"] = nowTime;
				tmmsmyl["REC_CREATOR"] = s.userid;
				cmd_inq.SetCommandText("select  nvl(MAX(SEQ_NO),0)+1 from " + v_table_name + " where  YEAR_MON = '" + tmmsmyl["YEAR_MON"].ToString().Trim() + "' ");
				tmmsmyl["SEQ_NO"] = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Insert();
			}
			else if (v_proc_div == "U")
			{
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM 	" + v_table_name + " "
					"   WHERE  YEAR_MON	= @YEAR_MON  AND  SEQ_NO	= @SEQ_NO ";

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("YEAR_MON", tmmsmyl["YEAR_MON"].ToString());
				cmd_sql.Parameters.Set("SEQ_NO", tmmsmyl["SEQ_NO"].ToDecimal());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsmyl["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsmyl["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsmyl["REC_REVISE_TIME"] = nowTime;
				tmmsmyl["REC_REVISOR"] = s.userid;

				tmmsmyl.Delete("SEQ_NO,YEAR_MON");
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Insert();
			}
			else if (v_proc_div == "D")
			{
				tmmsmyl.TrimOrBlank();
				tmmsmyl.Delete("SEQ_NO,YEAR_MON");
			}
			else if (v_proc_div == "F")
			{
				//完成标记
				tmmsmyl["REC_REVISE_TIME"] = nowTime;
				tmmsmyl["REC_REVISOR"] = s.userid;
				Log::Trace("", __FUNCTION__, "FIN_CONFM_FLAG =[{0}]", tmmsmyl["FIN_CONFM_FLAG"].ToString());
				if (tmmsmyl["FIN_CONFM_FLAG"])
				{
					tmmsmyl["FINISH_TIME"] = nowTime.SubstringNE(0,8);
				}
				tmmsmyl.Update("REC_REVISE_TIME,REC_REVISOR,FIN_CONFM_FLAG,FINISH_TIME","SEQ_NO,YEAR_MON");
			}


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
