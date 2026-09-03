/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    KE2111
Version:    1.0
Date:     2024-11-15
Description:
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"


// service入口
BM2F_ENTERACE(mrcr10a_pro)

int f_mrcr10a_pro(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义	

	/* ***** 静态变量定义 ***** */

	int fetchRowCount = 0;
	int i;
	int ret;
	int RowCount = 0;
	CDecimal i_count = 0;
	int doFlag = 0;
	int blkNum = 0;

	CString	datetime("");
	CString	v_table_name = "";

	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_del(conn);
	CString picture_no = "";//画面号
	CString fn_no = "";//功能键号
	CString  sqlstr = "";





	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{


		if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME"))
		{
			v_table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString();
		}

		CModel mr00xx(v_table_name);
		Log::Info("", __FUNCTION__, "v_table_name   =[{0}]", v_table_name);
		/* 获得传入参数 */
		/* 维护事件表 */





		if (bcls_rec->Tables.IndexOf("INS") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["INS"].Rows.get_Count(); i++)
			{
				mr00xx.Reset();
				mr00xx.MergeFrom(bcls_rec->Tables["INS"].Rows[i]);
				mr00xx.TrimOrBlank();

				mr00xx["DATE_TIME"] = datetimeNow.SubstringNE(0, 8);
				mr00xx["SEQ_NO"] = Db::QeuryCDecimal("select max(seq_no)+1 from " + v_table_name + " where DATE_TIME = '" + datetimeNow.SubstringNE(0, 8) + "' ");
				//mr00xx.Print();


				/* 新增事件信息 */
				mr00xx["REC_CREATOR"] = s.userid;   //记录创建责任者
				mr00xx["REC_CREATE_TIME"] = datetimeNow;   //记录创建时刻

				mr00xx.TrimOrBlank();



				

				mr00xx.Insert();

			}

		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				mr00xx.Reset();

				mr00xx.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				mr00xx.TrimOrBlank();


				/* 修改事件信息 */
				mr00xx["REC_REVISOR"] = s.userid;
				mr00xx["REC_REVISE_TIME"] = datetimeNow;
				mr00xx.TrimOrBlank();

				mr00xx.Delete("DATE_TIME,SEQ_NO");
				mr00xx.Insert();

			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				mr00xx.Reset();
				mr00xx.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);
				mr00xx.TrimOrBlank();


				/* 删除事件信息 */
				mr00xx.Delete("DATE_TIME,SEQ_NO");


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

	return doFlag;

}
