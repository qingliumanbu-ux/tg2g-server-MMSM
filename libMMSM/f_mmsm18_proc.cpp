/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-07-04
Description: 炼钢混铁炉实绩处理
***********************************************************************/


/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 

 

int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm18_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_proc_div = "";
	CString v_pract_coll_mode = "";
	CString v_factory_div = "";

	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel tmmsm18("TMMSM18");
	
 
 	try
	{

	
		v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString();
		v_pract_coll_mode = bcls_rec->Tables["PARA"].Rows[0]["PRACT_COLL_MODE"].ToString();
		v_factory_div = bcls_rec->Tables["PARA"].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm18.Reset();
			tmmsm18.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm18["FACTORY_DIV"] = v_factory_div;
			tmmsm18.TrimOrBlank();

			if (tmmsm18["START_TIME"].ToString().GetLength() != 14)
			{
				tmmsm18["START_TIME"] = dateNow;
			}
			if (tmmsm18["END_TIME"].ToString().GetLength() != 14)
			{
				tmmsm18["END_TIME"] = dateNow;
			}


			if (tmmsm18["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm18["PROD_SHIFT_GROUP"].ToString().Trim() == "")
			{
				//f_epep_get_shift_group("SM", tmmsm18["START_TIME"].ToString(), tmmsm18["PROD_SHIFT_NO"].ToString(), tmmsm18["PROD_SHIFT_GROUP"].ToString(), conn);
				CString PROD_SHIFT_NO = "";
				CString PROD_SHIFT_GROUP = "";
				f_epep_get_shift_group("SM", tmmsm18["START_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
				tmmsm18["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
				tmmsm18["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
			}

			

			tmmsm18.Print();

			if (v_proc_div == "I")
			{
				tmmsm18["REC_CREATE_TIME"] = dateNow;
				tmmsm18["REC_CREATOR"] = s.userid;
				tmmsm18.Insert();
			}
			else if (v_proc_div == "U")
			{
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM	TMMSM18 "
					"   WHERE  PROC_NO	= @proc_no";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("proc_no", tmmsm18["PROC_NO"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm18["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsm18["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsm18["REC_REVISE_TIME"] = dateNow;
				tmmsm18["REC_REVISOR"] = s.userid;

				tmmsm18.Delete();
				tmmsm18.Insert();

			}
			else if (v_proc_div == "D")
			{
				tmmsm18.Delete();
			}


			#if defined(_SYS_PES)
			//调用发送电文
			blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MMSMSND");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TPD_NO"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TPD_NO");
			}

			bcls_rec->Tables["MMSMSND"].Rows.Add();

			bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM18";
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;
			bcls_rec->Tables["MMSMSND"].Rows[0]["TPD_NO"] = tmmsm18["TPD_NO"];

			doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			#endif


		}
		

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
