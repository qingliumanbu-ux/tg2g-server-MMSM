/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 获取消耗工序基表
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsmzxhbblh_ins)

int f_mmsmzxhbblh_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString v_proc_div = "";
	CString sqlstr = "";
	CString sqlstr_temp = "";
	CString v_table_type = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CModel tmmsmzxhbb_lh("TMMSMZXHBB_LH");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	tmmsmzxhbb_lh.Reset();
	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		
		v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
		Log::Trace("", __FUNCTION__, "v_proc_div =[{0}]", v_proc_div);
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmzxhbb_lh.Reset();
			tmmsmzxhbb_lh.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (v_proc_div == "I")
			{
				if (tmmsmzxhbb_lh.QueryCount("PROD_DATE,ITEM_TYPE") == 1)
				{
					sprintf(s.msg, "生产日期[%s]的炉号已维护!", (const char*)tmmsmzxhbb_lh["PROD_DATE"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsmzxhbb_lh["REC_CREATE_TIME"] = nowTime;
				tmmsmzxhbb_lh["TIME_STAMPS"] = nowTime;
				tmmsmzxhbb_lh["REC_CREATOR"] = s.userid;
				tmmsmzxhbb_lh.TrimOrBlank();
				tmmsmzxhbb_lh.Insert();
			}
			else if (v_proc_div == "U")
			{
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM 	tmmsmzxhbb_lh "
					"   WHERE  PROD_DATE	= @PROD_DATE  AND   ITEM_TYPE	= @ITEM_TYPE  ";

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("PROD_DATE", tmmsmzxhbb_lh["PROD_DATE"].ToString());
				cmd_sql.Parameters.Set("ITEM_TYPE", tmmsmzxhbb_lh["ITEM_TYPE"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsmzxhbb_lh["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsmzxhbb_lh["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsmzxhbb_lh["REC_REVISE_TIME"] = nowTime;
				tmmsmzxhbb_lh["REC_REVISOR"] = s.userid;
				
				tmmsmzxhbb_lh.Delete("PROD_DATE,ITEM_TYPE");
				tmmsmzxhbb_lh.TrimOrBlank();
				tmmsmzxhbb_lh.Insert();
			}
			else if (v_proc_div == "D")
			{
				tmmsmzxhbb_lh.TrimOrBlank();
				tmmsmzxhbb_lh.Delete("PROD_DATE,ITEM_TYPE");
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
