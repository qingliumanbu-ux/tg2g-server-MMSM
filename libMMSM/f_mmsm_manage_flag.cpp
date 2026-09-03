/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2016-10-30
Version:1.0
Description: 根据锭坯型返回管理模式（按批还是按件）
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

BM2_FUNCTION_EXPORT
int f_mmsm_manage_flag(const CString& slab_type, const CString& ingot_code, CString& manage_flag, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int ret = 0;
	CString sqlstr = "";
	CString v_manage_status = "";
	CString v_ingot_flag = "";


	try
	{
		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		//Log::Info("", __FUNCTION__, "slab_type=[{0}]", slab_type);

		//判断按批按支还是按锭坯型

		sqlstr = "SELECT CODE"
			"  FROM TEP0002 "
			"  WHERE CODE_CLASS 	=  'MS41'"
			"  AND CODE_DESC_2_CONTENT = '1'";


		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			manage_flag = cmd_sql.GetString(1);

		}
		cmd_sql.Close();


		//Log::Info("", __FUNCTION__, "manage_flag11111111111=[{0}]", manage_flag);


		if (manage_flag.Trim() == "0")//根据锭坯型判断
		{
			//Log::Info("", __FUNCTION__, " ingot_code=[{0}]", ingot_code);

			sqlstr = "SELECT CODE_DESC_1_CONTENT "
				"  FROM TEP0002 "
				"  WHERE CODE_CLASS 	=  'PM2D'"
				"  AND CODE =  @ingot_code";


			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("ingot_code", ingot_code);
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				v_ingot_flag = cmd_sql.GetString(1);

			}
			cmd_sql.Close();



			if (slab_type == "1") // 板坯
			{
				manage_flag = "2";
			}
			else
			{
				if (v_ingot_flag.Trim() == "02")//按支
				{
					manage_flag = "2";
				}
				else if (v_ingot_flag.Trim() == "01")//按批
				{
					manage_flag = "1";
				}
				else
				{
					manage_flag = "2";
				}

			}

		}



		//Log::Info("", __FUNCTION__, "manage_flag=[{0}]", manage_flag);


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

	return doFlag;

}
