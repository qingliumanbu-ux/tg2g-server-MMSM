/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      KE2111
Version:     1.0
Date:        2019-11-22 16:20:24
Description: 模拟收货成功
**************************************************/

//框架头文件
#include "stdafx.h"



int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);		//物料跟踪函数
int f_mm0011(CString SeqName, CDecimal SeqLen, CString& SeqNo, CDbConnection* conn);	//获取流水号


BM2F_ENTERACE(test_0rt801)

int f_test_0rt801(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_resume_seq_no = "";//序号
	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm3e("TMMSM3E");
	CModel tmmsm01_query("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		EIClass INFO;
		sqlstr = " select MAT_NO,'S' RECEIVE_BACK_STATUS from tmmsm01 where RCV_MAT_FLAG='W' ";
		Log::Trace(" ", __FUNCTION__, "v_proc_div =[{0}]", __LINE__);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(INFO.Tables[0]);
		cmd_inq.Close();
		if (INFO.Tables[0].Rows.get_Count() > 0)
		{
			if (bcls_rec->Tables.IndexOf("MM0099") < 0)
			{
				bcls_rec->Tables.Add("MM0099");
				bcls_rec->Tables["MM0099"].Rows.Clear();
			}
			for (int i = 0; i < INFO.Tables[0].Rows.get_Count(); i++)
			{
				tmmsm01.MergeFrom(INFO.Tables[0].Rows[i]);
				if (!tmmsm01.Query("MAT_NO"))
				{
					sprintf(s.msg, "未查到板坯数据！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm3e.CopyFrom(tmmsm01);
				tmmsm3e.MergeFrom(INFO.Tables[0].Rows[i]);

				doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
				tmmsm3e.Insert();
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["EVENT_ID"] = "MM34";
				tmmsm96["EVENT_LINE_TYPE"] = "SM";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96["RCV_MAT_FLAG"] = INFO.Tables[0].Rows[i]["RECEIVE_BACK_STATUS"];
				tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
			}

			Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
			if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
			{
				doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


