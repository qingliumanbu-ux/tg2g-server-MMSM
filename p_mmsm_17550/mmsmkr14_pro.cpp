/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2014-07-08
Description: KR脱硫生产实绩增删改
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件



BM2F_ENTERACE(mmsmkr14_pro)

int f_mmsmkr14_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString v_proc_div = " ";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 实体类定义 */
	CModel tmmsmkr14("TMMSMKR14");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			Log::Info("", __FUNCTION__, "row=[{0}]", bcls_rec->Tables[0].Rows.get_Count());
			v_proc_div = bcls_rec->Tables[0].Rows[i]["PROC_DIV"].ToString();
			Log::Info("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
			tmmsmkr14.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsmkr14.Print();
			tmmsmkr14["FACTORY_DIV"] = "LG1";
			if (v_proc_div == "I")
			{
				tmmsmkr14["REC_CREATE_TIME"] = dateNow;
				tmmsmkr14["REC_CREATOR"] = s.userid;
				tmmsmkr14["L2_PROC_NO"] = tmmsmkr14["DES_ID"].ToString();
				tmmsmkr14["PROC_NO"] = tmmsmkr14["DES_ID"].ToString();
				tmmsmkr14["STATION_ID"] = "D";//设备类型固定为D   必写，不然加料维护报错  mfj  20231121
				tmmsmkr14.Insert();
			}
			else if (v_proc_div == "U")
			{
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM	tmmsmkr14 "
					"   WHERE  DES_ID	= @DES_ID";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("DES_ID", tmmsmkr14["DES_ID"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsmkr14["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsmkr14["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsmkr14["REC_REVISE_TIME"] = dateNow;
				tmmsmkr14["REC_REVISOR"] = s.userid;
				Log::Info("", __FUNCTION__, "DES_ID=[{0}]", tmmsmkr14["DES_ID"].ToString());
				tmmsmkr14.Delete("DES_ID");
				tmmsmkr14.Insert();

			}
			else if (v_proc_div == "D")
			{
				tmmsmkr14.Delete();
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


