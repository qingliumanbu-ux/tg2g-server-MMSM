/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-07-04
Description: 炼钢实绩模板配置参数维护
***********************************************************************/


/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

 

// service入口
BM2F_ENTERACE(mmsmpara_pro)

int f_mmsmpara_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_proc_div = "";
	CString v_program_name = "";
	CString v_copy_prog_name = "";
	int   v_seq_no = 0;
	

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel tmmsmpara("TMMSMPARA");


	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROGRAM_NAME"))
			v_program_name = bcls_rec->Tables[0].Rows[0]["PROGRAM_NAME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("COPY_PROG_NAME"))
			v_copy_prog_name= bcls_rec->Tables[0].Rows[0]["COPY_PROG_NAME"].ToString();

		Log::Trace(" ", __FUNCTION__, "v_proc_div =[{0}]", v_proc_div);
		Log::Trace(" ", __FUNCTION__, "v_program_name =[{0}]", v_program_name);
		Log::Trace(" ", __FUNCTION__, "v_copy_prog_name =[{0}]", v_copy_prog_name);

		if (v_proc_div == "C") //复制
		{
			tmmsmpara["PROGRAM_NAME"] = v_copy_prog_name;

			Log::Trace(" ", __FUNCTION__, "tmmsmpara.PROGRAM_NAME =[{0}]", tmmsmpara["PROGRAM_NAME"].ToString());

			tmmsmpara.Delete("PROGRAM_NAME");

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr = " SELECT * FROM TMMSMPARA "
					"  WHERE PROGRAM_NAME = @program_name ";


				break;
			}

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("program_name", v_program_name);
			cmd_inq.ExecuteReader();

			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsmpara);
				tmmsmpara["PROGRAM_NAME"] = v_copy_prog_name;
				tmmsmpara.Insert();
			}
			cmd_inq.Close();


		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmpara.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (v_proc_div == "I")
			{
				tmmsmpara["REC_CREATE_TIME"] = dateNow;
				tmmsmpara["REC_CREATOR"] = s.userid;
				tmmsmpara.Insert();
			}
			else if (v_proc_div == "U")
			{
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM	TMMSMPARA "
					"   WHERE  PROGRAM_NAME	= @program_name";

				Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("program_name", tmmsmpara["PROGRAM_NAME"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsmpara["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsmpara["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsmpara["REC_REVISE_TIME"] = dateNow;
				tmmsmpara["REC_REVISOR"] = s.userid;

				tmmsmpara.Delete("PROGRAM_NAME,PARA_NAME");
				tmmsmpara.Insert();

			}
			else if (v_proc_div == "D")
			{
				tmmsmpara.Delete("PROGRAM_NAME,PARA_NAME");
			}
			
			else if (v_proc_div == "S")//调序
			{
				
				v_seq_no++;

				tmmsmpara["SEQ_NO"] = v_seq_no;

				Log::Trace(" ", __FUNCTION__, "v_seq_no =[{0}]", v_seq_no);
				Log::Trace(" ", __FUNCTION__, "program_name =[{0}]", tmmsmpara["PROGRAM_NAME"].ToString());
				Log::Trace(" ", __FUNCTION__, "para_name =[{0}]", tmmsmpara["PARA_NAME"].ToString());
				
				tmmsmpara.Update("seq_no", "program_name,para_name");

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
