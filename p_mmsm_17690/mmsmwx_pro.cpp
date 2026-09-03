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


BM2F_ENTERACE(mmsmwx_pro)


int f_mmsmwx_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString   v_proc_div = "";
	CString  table_name = "";
	CString c_sql_condition = "";
	CModel tmmsmw1("TMMSMW1");
	CModel tmmsmw2("TMMSMW2");
	CModel tmmsmw3("TMMSMW3");
	CModel tmmsmw4("TMMSMW4");
	CModel zj_mat_ele("ZJ_MAT_ELEMENT");
	CModel zj_sc_ele("ZJ_SCRAP_ELEMENT");
	CModel zj_jsk_mat("ZJ_JISHUKE_MAT");
	CDbCommand cmd_inq(conn);
	try
	{
		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		table_name = bcls_rec->Tables["PARA"].Rows[0]["TABLE_NAME"].ToString();

		Log::Trace("", "", "获取传入参数...");
		Log::Trace("", "", "传入表名：table_name=[{0}]", table_name);
		Log::Trace("", "", "当前时间：nowTime=[{0}]", nowTime);

	
		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("W1_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["W1_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W1_ADD"].Rows.get_Count(); i++)
			{
				tmmsmw1.Reset();
				tmmsmw1.MergeFrom(bcls_rec->Tables["W1_ADD"].Rows[i]);
				tmmsmw1["REC_CREATOR"] = s.userid;
				tmmsmw1["REC_CREATE_TIME"] = nowTime;
				tmmsmw1.TrimOrBlank();
				cmd_inq.SetCommandText("select  nvl(MAX(SEQ_ID),0)+1 from tmmsmw1 ");
				tmmsmw1["SEQ_ID"] = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				sqlstr = "INSERT INTO " + table_name;
				tmmsmw1.Insert();

			}
		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("W1_UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["W1_UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W1_UPD"].Rows.get_Count(); i++)
			{
				tmmsmw1.Reset();
				tmmsmw1.MergeFrom(bcls_rec->Tables["W1_UPD"].Rows[i]);
				tmmsmw1.TrimOrBlank();
				tmmsmw1.Delete("SEQ_ID");

				tmmsmw1.MergeFrom(bcls_rec->Tables["W1_UPD"].Rows[i]);
				tmmsmw1["REC_REVISOR"] = s.userid;
				tmmsmw1["REC_REVISE_TIME"] = nowTime;
				tmmsmw1.TrimOrBlank();
				tmmsmw1.Insert();

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("W1_DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["W1_DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W1_DEL"].Rows.get_Count(); i++)
			{
				tmmsmw1.Reset();
				tmmsmw1.MergeFrom(bcls_rec->Tables["W1_DEL"].Rows[i]);
				sqlstr = "DELETE FROM " + table_name;
				proc_sum += tmmsmw1.Delete("SEQ_ID");
			}
		}


		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("W2_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["W2_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W2_ADD"].Rows.get_Count(); i++)
			{
				tmmsmw2.Reset();
				tmmsmw2.MergeFrom(bcls_rec->Tables["W2_ADD"].Rows[i]);
				tmmsmw2["REC_CREATOR"] = s.userid;
				tmmsmw2["REC_CREATE_TIME"] = nowTime;
				tmmsmw2.TrimOrBlank();
				cmd_inq.SetCommandText("select  nvl(MAX(SEQ_ID),0)+1 from tmmsmw2 ");
				tmmsmw2["SEQ_ID"] = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				sqlstr = "INSERT INTO " + table_name;
				tmmsmw2.Insert();

			}
		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("W2_UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["W2_UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W2_UPD"].Rows.get_Count(); i++)
			{
				tmmsmw2.Reset();
				tmmsmw2.MergeFrom(bcls_rec->Tables["W2_UPD"].Rows[i]);
				tmmsmw2.TrimOrBlank();
				tmmsmw2.Delete("SEQ_ID");

				tmmsmw2.MergeFrom(bcls_rec->Tables["W2_UPD"].Rows[i]);
				tmmsmw2["REC_REVISOR"] = s.userid;
				tmmsmw2["REC_REVISE_TIME"] = nowTime;
				tmmsmw2.TrimOrBlank();
				tmmsmw2.Insert();

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("W2_DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["W2_DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W2_DEL"].Rows.get_Count(); i++)
			{
				tmmsmw2.Reset();
				tmmsmw2.MergeFrom(bcls_rec->Tables["W2_DEL"].Rows[i]);
				sqlstr = "DELETE FROM " + table_name;
				proc_sum += tmmsmw2.Delete("SEQ_ID");
			}
		}

		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("W3_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["W3_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W3_ADD"].Rows.get_Count(); i++)
			{
				tmmsmw3.Reset();
				tmmsmw3.MergeFrom(bcls_rec->Tables["W3_ADD"].Rows[i]);
				tmmsmw3["REC_CREATOR"] = s.userid;
				tmmsmw3["REC_CREATE_TIME"] = nowTime;
				tmmsmw3.TrimOrBlank();
				cmd_inq.SetCommandText("select  nvl(MAX(SEQ_ID),0)+1 from tmmsmw3 ");
				tmmsmw3["SEQ_ID"] = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
				sqlstr = "INSERT INTO " + table_name;
				tmmsmw3.Insert();

			}
		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("W3_UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["W3_UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W3_UPD"].Rows.get_Count(); i++)
			{
				tmmsmw3.Reset();
				tmmsmw3.MergeFrom(bcls_rec->Tables["W3_UPD"].Rows[i]);
				tmmsmw3.TrimOrBlank();
				tmmsmw3.Delete("SEQ_ID");

				tmmsmw3.MergeFrom(bcls_rec->Tables["W3_UPD"].Rows[i]);
				tmmsmw3["REC_REVISOR"] = s.userid;
				tmmsmw3["REC_REVISE_TIME"] = nowTime;
				tmmsmw3.TrimOrBlank();
				tmmsmw3.Insert();

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("W3_DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["W3_DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W3_DEL"].Rows.get_Count(); i++)
			{
				tmmsmw3.Reset();
				tmmsmw3.MergeFrom(bcls_rec->Tables["W3_DEL"].Rows[i]);
				sqlstr = "DELETE FROM " + table_name;
				proc_sum += tmmsmw3.Delete("SEQ_ID");
			}
		}

		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("W4_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["W4_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W4_ADD"].Rows.get_Count(); i++)
			{
				tmmsmw4.Reset();
				tmmsmw4.MergeFrom(bcls_rec->Tables["W4_ADD"].Rows[i]);
				tmmsmw4["REC_CREATOR"] = s.userid;
				tmmsmw4["REC_CREATE_TIME"] = nowTime;
				tmmsmw4.TrimOrBlank();
				sqlstr = "INSERT INTO " + table_name;
				tmmsmw4.Insert();

			}
		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("W4_UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["W4_UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W4_UPD"].Rows.get_Count(); i++)
			{
				tmmsmw4.Reset();
				tmmsmw4.MergeFrom(bcls_rec->Tables["W4_UPD"].Rows[i]);
				tmmsmw4.TrimOrBlank();
				tmmsmw4.Delete("MAT_CODE");
				tmmsmw4.MergeFrom(bcls_rec->Tables["W4_UPD"].Rows[i]);
				tmmsmw4["REC_REVISOR"] = s.userid;
				tmmsmw4["REC_REVISE_TIME"] = nowTime;
				tmmsmw4.TrimOrBlank();
				tmmsmw4.Insert();

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("W4_DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["W4_DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["W4_DEL"].Rows.get_Count(); i++)
			{
				tmmsmw4.Reset();
				tmmsmw4.MergeFrom(bcls_rec->Tables["W4_DEL"].Rows[i]);
				sqlstr = "DELETE FROM " + table_name;
				proc_sum += tmmsmw4.Delete("MAT_CODE");
			}
		}


		if (bcls_rec->Tables.Contains("WE1_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["WE1_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE1_ADD"].Rows.get_Count(); i++)
			{
				zj_mat_ele.Reset();
				zj_mat_ele.MergeFrom(bcls_rec->Tables["WE1_ADD"].Rows[i]);
				zj_mat_ele.TrimOrBlank();
				sqlstr = "插入原料表和废钢表";
				zj_mat_ele.Insert();
				
				zj_sc_ele.Reset();
				zj_sc_ele.MergeFrom(bcls_rec->Tables["WE1_ADD"].Rows[i]);
				zj_sc_ele.TrimOrBlank();
				zj_sc_ele.Insert();

			}
		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("WE1_UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["WE1_UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE1_UPD"].Rows.get_Count(); i++)
			{
				zj_mat_ele.Reset();
				zj_mat_ele.MergeFrom(bcls_rec->Tables["WE1_UPD"].Rows[i]);
				zj_mat_ele.TrimOrBlank();
				zj_mat_ele.Delete("MAT_ID");
				zj_mat_ele.MergeFrom(bcls_rec->Tables["WE1_UPD"].Rows[i]);
				zj_mat_ele.TrimOrBlank();
				zj_mat_ele.Insert();

				zj_sc_ele.Reset();
				zj_sc_ele.MergeFrom(bcls_rec->Tables["WE1_UPD"].Rows[i]);
				zj_sc_ele.TrimOrBlank();
				zj_sc_ele.Delete("MAT_ID");
				zj_sc_ele.MergeFrom(bcls_rec->Tables["WE1_UPD"].Rows[i]);
				zj_sc_ele.TrimOrBlank();
				zj_sc_ele.Insert();

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("WE1_DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["WE1_DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE1_DEL"].Rows.get_Count(); i++)
			{
				zj_mat_ele.Reset();
				zj_mat_ele.MergeFrom(bcls_rec->Tables["WE1_DEL"].Rows[i]);
				sqlstr = "删除原料表和废钢表";
				proc_sum += zj_mat_ele.Delete("MAT_ID");

				zj_sc_ele.Reset();
				zj_sc_ele.MergeFrom(bcls_rec->Tables["WE1_DEL"].Rows[i]);
				proc_sum += zj_sc_ele.Delete("MAT_ID");
			}
		}

		if (bcls_rec->Tables.Contains("WE2_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["WE2_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE2_ADD"].Rows.get_Count(); i++)
			{
				zj_sc_ele.Reset();
				zj_sc_ele.MergeFrom(bcls_rec->Tables["WE2_ADD"].Rows[i]);
				zj_sc_ele.TrimOrBlank();
				sqlstr = "INSERT INTO " + table_name;
				zj_sc_ele.Insert();

			}
		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("WE2_UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["WE2_UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE2_UPD"].Rows.get_Count(); i++)
			{
				zj_sc_ele.Reset();
				zj_sc_ele.MergeFrom(bcls_rec->Tables["WE2_UPD"].Rows[i]);
				zj_sc_ele.TrimOrBlank();
				zj_sc_ele.Delete("MAT_ID");
				zj_sc_ele.MergeFrom(bcls_rec->Tables["WE2_UPD"].Rows[i]);
				zj_sc_ele.TrimOrBlank();
				zj_sc_ele.Insert();

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("WE2_DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["WE2_DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE2_DEL"].Rows.get_Count(); i++)
			{
				zj_sc_ele.Reset();
				zj_sc_ele.MergeFrom(bcls_rec->Tables["WE2_DEL"].Rows[i]);
				sqlstr = "DELETE FROM " + table_name;
				proc_sum += zj_sc_ele.Delete("MAT_ID");
			}
		}


		if (bcls_rec->Tables.Contains("WE3_ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["WE3_ADD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE3_ADD"].Rows.get_Count(); i++)
			{
				zj_jsk_mat.Reset();
				zj_jsk_mat.MergeFrom(bcls_rec->Tables["WE3_ADD"].Rows[i]);
				zj_jsk_mat.TrimOrBlank();
				sqlstr = "INSERT INTO " + table_name;
				zj_jsk_mat.Insert();

			}
		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("WE3_UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["WE3_UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE3_UPD"].Rows.get_Count(); i++)
			{
				zj_jsk_mat.Reset();
				zj_jsk_mat.MergeFrom(bcls_rec->Tables["WE3_UPD"].Rows[i]);
				zj_jsk_mat.TrimOrBlank();
				zj_jsk_mat.Delete("MAT_ID");
				zj_jsk_mat.MergeFrom(bcls_rec->Tables["WE3_UPD"].Rows[i]);
				zj_jsk_mat.TrimOrBlank();
				zj_jsk_mat.Insert();

			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("WE3_DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["WE3_DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["WE3_DEL"].Rows.get_Count(); i++)
			{
				zj_jsk_mat.Reset();
				zj_jsk_mat.MergeFrom(bcls_rec->Tables["WE3_DEL"].Rows[i]);
				sqlstr = "DELETE FROM " + table_name;
				proc_sum += zj_jsk_mat.Delete("MAT_ID");
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
