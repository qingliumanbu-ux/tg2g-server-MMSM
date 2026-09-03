/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     王慧琼
Version:    1.0
Date:       2016-01-25
Description: 工序投料生产实绩保存
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件


//外部函数声明
int f_mmsm2a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2F_ENTERACE(mmsm2a_pro)

int f_mmsm2a_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */

	/* 实体类定义 */
	EIClass bcls_rec_mat;
	bcls_rec_mat.Tables[0].set_TableName("MMSM2A");
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//CString sqlst = bcls_rec->Tables.GetTableName();
		Log::Trace("", "", "--------------222-------------", bcls_rec->Tables.Contains("MMSM_2A_INS"));
		Log::Trace("", "", "--------------333-------------", bcls_rec->Tables.Contains("MMSM_2A_UPD"));

		

		if (bcls_rec->Tables.Contains("MMSM_2A_DEL"))
		{
			Log::Trace("", "", "--------------删除-------------");
			bcls_rec_mat.Tables[0].Clone(bcls_rec->Tables["MMSM_2A_DEL"]);
			if (!bcls_rec_mat.Tables[0].Columns.Contains("PROC_DIV"))
			{
				bcls_rec_mat.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
			}
			for (int i = 0; i < bcls_rec->Tables["MMSM_2A_DEL"].Rows.get_Count(); i++){
				CDataRow & drSrc = bcls_rec->Tables["MMSM_2A_DEL"].Rows[i];
				bcls_rec_mat.Tables[0].Rows.Clear();
				CDataRow & drDes = bcls_rec_mat.Tables[0].Rows.Add();
				drDes.Merge(drSrc);
				drDes["PROC_DIV"] = "D";

				doFlag = f_mmsm2a_proc(&bcls_rec_mat, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}
		if (bcls_rec->Tables.Contains("MMSM_2A_INS"))
		{
			Log::Trace("", "", "--------------新增-------------");
			bcls_rec_mat.Tables[0].Clone(bcls_rec->Tables["MMSM_2A_INS"]);
			if (!bcls_rec_mat.Tables[0].Columns.Contains("PROC_DIV"))
			{
				bcls_rec_mat.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
			}
			for (int i = 0; i < bcls_rec->Tables["MMSM_2A_INS"].Rows.get_Count(); i++){
				CDataRow & drSrc = bcls_rec->Tables["MMSM_2A_INS"].Rows[i];
				bcls_rec_mat.Tables[0].Rows.Clear();
				CDataRow & drDes = bcls_rec_mat.Tables[0].Rows.Add();
				drDes.Merge(drSrc);
				drDes["PROC_DIV"] = "I";
				Log::Trace("", "", "--------------XXX-------------");
				doFlag = f_mmsm2a_proc(&bcls_rec_mat, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}
		if (bcls_rec->Tables.Contains("MMSM_2A_UPD"))
		{
			Log::Trace("", "", "--------------修改-------------");
			bcls_rec_mat.Tables[0].Clone(bcls_rec->Tables["MMSM_2A_UPD"]);
			if (!bcls_rec_mat.Tables[0].Columns.Contains("PROC_DIV"))
			{
				bcls_rec_mat.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
			}
			if (!bcls_rec_mat.Tables[0].Columns.Contains("NEW_HANDWORK_MARK"))
			{
				bcls_rec_mat.Tables[0].Columns.Add(DT_STRING, "NEW_HANDWORK_MARK");
			}
			if (!bcls_rec_mat.Tables[0].Columns.Contains("NEW_COLL_MODE"))
			{
				bcls_rec_mat.Tables[0].Columns.Add(DT_STRING, "NEW_COLL_MODE");
			}
			if (!bcls_rec_mat.Tables[0].Columns.Contains("NEW_WT"))
			{
				bcls_rec_mat.Tables[0].Columns.Add(DT_STRING, "NEW_WT");
			}
			for (int i = 0; i < bcls_rec->Tables["MMSM_2A_UPD"].Rows.get_Count(); i++){
				CDataRow & drSrc = bcls_rec->Tables["MMSM_2A_UPD"].Rows[i];
				bcls_rec_mat.Tables[0].Rows.Clear();
				CDataRow & drDes = bcls_rec_mat.Tables[0].Rows.Add();
				drDes.Merge(drSrc);
				drDes["PROC_DIV"] = "U";
				Record old_mmsm2a = Db::QueryFirst("SELECT * FROM TMMSM2A WHERE PROC_NO = @PROC_NO AND  MAT_CODE=@MAT_CODE AND  PROC_COUNT=@PROC_COUNT", drSrc);
				
				drDes["NEW_HANDWORK_MARK"] = drDes["HANDWORK_MARK"];
				drDes["NEW_COLL_MODE"] = drDes["PRACT_COLL_MODE"];
				drDes["NEW_WT"] = drDes["DEVO_WT"];
				drDes["HANDWORK_MARK"] =  old_mmsm2a.GetCString("PRACT_COLL_MODE");
				drDes["PRACT_COLL_MODE"] = old_mmsm2a.GetCString("PRACT_COLL_MODE");
				drDes["DEVO_WT"] = old_mmsm2a.GetCDecimal("DEVO_WT");

				doFlag = f_mmsm2a_proc(&bcls_rec_mat, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}
		else
		{
			//需要增加处理标记，先删除，再新增
			//删除
			if (!bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
			}

			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "D";


			doFlag = f_mmsm2a_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//新增
			if (!bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
			}

			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "I";
			doFlag = f_mmsm2a_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


