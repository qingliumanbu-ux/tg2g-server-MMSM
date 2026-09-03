/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-05-25
Description: 生产实绩校验及公共处理
<para>

***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

  

//外部函数声明
int f_mmsm_get_pono(const CString& heat_no, CString& pono, CString& factory_div, CDbConnection * conn);
int f_mmsm_confirm_flag(const CString& factory_div, const CString& heat_no, CString& heat_confirm_flag, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm_sj_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm_sj_proc";                //定义函数英文名称  
	CString FunctionCname = "炼钢生产实绩校验及处理";              //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	CString sqlstr = "";
	CString v_heat_confirm_flag = "";
	CString v_proc_div = "";
	CString v_table_type = "";
	CString v_dev_code = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";

	CString v_heat_no = "";
	CString v_pono = "";
	CString v_factory_div = "";

	try
	{
		CPageInfo pageInfo;

		/* 数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */

	CModel tmmsm00("TMMSM00");

		//初始化实体类

		tmmsm00.Reset();


		/*获取输入参数,tmmsm00头文件主要用于校验*/
		tmmsm00.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm00.TrimOrBlank();
		tmmsm00.Print();


		//Log::Trace("", __FUNCTION__, "tmmsm00["FACTORY_DIV"] =[{0}]", tmmsm00["FACTORY_DIV"].ToString());

		/*判断是否存在指定块*/
		if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))  //表名
			v_table_type = bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("DEV_CODE"))  //设备代码
			v_dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))  //设备代码
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank().ToUpper();

		Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		Log::Trace("", __FUNCTION__, "v_table_type=[{0}]", v_table_type);
		Log::Trace("", __FUNCTION__, "v_dev_code=[{0}]", v_dev_code);

		//不锈钢转炉，电炉和中频炉都存在熔炼号为空
		/*if (v_heat_no.Trim() == "")
		{
			strcpy(s.msg, "主信息熔炼号不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/

		//将此段先注释掉，因画面传输的数据可能会未有厂别  mfj  20231204
		//if (tmmsm00["FACTORY_DIV"].ToString().Trim() == "")
		//{
		//	//f_mmsm_get_pono(tmmsm00["HEAT_NO"].ToString(), tmmsm00["PONO"].ToString(), tmmsm00["FACTORY_DIV"].ToString(), conn);
		//	f_mmsm_get_pono(v_heat_no, v_pono, v_factory_div, conn);
		//	tmmsm00["HEAT_NO"] = v_heat_no;
		//	tmmsm00["PONO"] = v_pono;
		//	tmmsm00["FACTORY_DIV"] = v_factory_div;

		//}

		// 如果传入STATION_ID为空，取设备代码第一位
		/*if (tmmsm00["STATION_ID"].ToString().Trim() == "")
		{
			tmmsm00["STATION_ID"] = v_dev_code.SubstringNE(0, 1);
		}*/

		//如果设备代码仍然取不到，根据传入的表名取TPSSMD1中的设备类型

		if (tmmsm00["STATION_ID"].ToString().Trim() == "")
		{
			//当设备代码不为空的时候，根据设备代码去TPSSMD1表取对应STATION_ID,SATAION_NO
			if (v_dev_code.Trim() != "")
			{
				cmd_sql.SetCommandText(" select STATION_ID,STATION_NO  from TPSSMD1 where DEV_CODE='" + v_dev_code + "' ");
				cmd_sql.ExecuteReader();
				if (cmd_sql.Read())
				{
					tmmsm00["STATION_ID"] = cmd_sql.GetString(1);
					tmmsm00["STATION_NO"] = cmd_sql.GetString(2);

				}
				cmd_sql.Close();
			}
			else if (v_table_type.Trim() == "TMMSM21")
			{
				tmmsm00["STATION_ID"] = "B";
			}
			else if (v_table_type.Trim() == "TMMSM22")
			{
				tmmsm00["STATION_ID"] = "A";
			}
			else if (v_table_type.Trim() == "TMMSM23")
			{
				tmmsm00["STATION_ID"] = "R";
			}
			else if (v_table_type.Trim() == "TMMSM24")
			{
				tmmsm00["STATION_ID"] = "F";
			}
			else if (v_table_type.Trim() == "TMMSM25")
			{
				tmmsm00["STATION_ID"] = "V";
			}
			else if (v_table_type.Trim() == "TMMSM26")
			{
				tmmsm00["STATION_ID"] = "S";
			}
			else if (v_table_type.Trim() == "TMMSM31")
			{
				tmmsm00["STATION_ID"] = "C";
			}
		}

		Log::Trace("", __FUNCTION__, "STATION_ID=[{0}]  STATION_NO = [{1}]", tmmsm00["STATION_ID"].ToString(), tmmsm00["STATION_NO"].ToString());
		//当不为不锈钢转炉(Y) 电炉(X) 中频炉(Z)时，对熔炼号进行判空判断
		/*if (tmmsm00["STATION_ID"].ToString() != "Y" && 
			tmmsm00["STATION_ID"].ToString() != "X" &&
			tmmsm00["STATION_ID"].ToString() != "Z" &&
			v_heat_no.Trim()== "")
		{
			strcpy(s.msg, "主信息熔炼号不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/


		if (tmmsm00["STATION_NO"].ToString().Trim() == "")
		{
			tmmsm00["STATION_NO"] = v_dev_code.SubstringNE(1, 1);
		}

		tmmsm00["STATION_NO"] = tmmsm00["STATION_NO"].ToString().Substring(tmmsm00["STATION_NO"].ToString().GetLength() - 1, 1);


		if (tmmsm00["STATION_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "设备站号不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}


		/*if (tmmsm00["ST_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "内部钢种不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
*/
		//已炉次确定则返回
		doFlag = f_mmsm_confirm_flag(tmmsm00["FACTORY_DIV"].ToString(), tmmsm00["HEAT_NO"].ToString(), v_heat_confirm_flag, conn);

		if (doFlag < 0)
		{
			//Log::Trace("", __FUNCTION__, "f_mmsm_confirm_flag=[{0}]", "调用f_mmsm_confirm_flag失败");
			strcpy(s.msg, "调用f_mmsm_confirm_flag失败!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//if (v_heat_confirm_flag != "0" && v_heat_confirm_flag != "")
		//{
		//	//Log::Trace("", __FUNCTION__, "f_mmsm_confirm_flag=[{0}]", "该制造命令号已经炉次确定");
		//	strcpy(s.msg, "该制造命令号已经炉次确定"); //系统错误信息
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//Log::Trace("", __FUNCTION__, "f_mmsm_get_pono前=[{0}]", v_table_type);

		//if (tmmsm00["PONO"].ToString().Trim() == "")
		//{
		//	//f_mmsm_get_pono(tmmsm00["HEAT_NO"].ToString(), tmmsm00["PONO"].ToString(), tmmsm00["FACTORY_DIV"].ToString(), conn);
		//	f_mmsm_get_pono(v_heat_no, v_pono, v_factory_div, conn);
		//	tmmsm00["HEAT_NO"] = v_heat_no;
		//	tmmsm00["PONO"] = v_pono;
		//	tmmsm00["FACTORY_DIV"] = v_factory_div;
		//}
		//Log::Trace("", __FUNCTION__, "f_mmsm_get_pono后=[{0}]", v_table_type);

		//Log::Trace("", __FUNCTION__, "f_epep_get_shift_group前=[{0}]", v_table_type);

		if ((tmmsm00["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm00["PROD_SHIFT_GROUP"].ToString().Trim() == "") && (tmmsm00["END_TIME"].ToString().Trim() != ""))
		{
			//f_epep_get_shift_group("SM", tmmsm00["START_TIME"].ToString(), tmmsm00["PROD_SHIFT_NO"].ToString(), tmmsm00["PROD_SHIFT_GROUP"].ToString(), conn);
			f_epep_get_shift_group("SMDD", tmmsm00["END_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
			tmmsm00["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
		
		}

		//Log::Trace("", __FUNCTION__, "f_epep_get_shift_group后=[{0}]", v_table_type);

		if (v_proc_div == "I")
		{
			tmmsm00["REC_CREATE_TIME"] = dateNow;
			tmmsm00["REC_CREATOR"] = s.userid;
		}
		else if (v_proc_div == "U")
		{

			sqlstr = " SELECT REC_CREATE_TIME,REC_CREATOR  FROM " + v_table_type + " WHERE 1=1 ";

			if (tmmsm00["HEAT_NO"].ToString().Trim() != "")
			{
				sqlstr += " AND HEAT_NO = @heat_no";
			}
			if (tmmsm00["PROC_NO"].ToString().Trim() != "")
			{
				sqlstr += " AND PROC_NO = @proc_no";
			}
			//Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("heat_no", tmmsm00["HEAT_NO"].ToString());
			cmd_sql.Parameters.Set("proc_no", tmmsm00["PROC_NO"].ToString());
			cmd_sql.ExecuteReader();

			if (cmd_sql.Read())
			{
				tmmsm00["REC_CREATE_TIME"] = cmd_sql.GetString(1);
				tmmsm00["REC_CREATOR"] = cmd_sql.GetString(2);
			}
			cmd_sql.Close();

			tmmsm00["REC_REVISE_TIME"] = dateNow;
			tmmsm00["REC_REVISOR"] = s.userid;

		}


		if (!bcls_ret->Tables.Contains("TMMSM00"))
		{
			bcls_ret->Tables.Add("TMMSM00");
		}

		tmmsm00.MergeTo(bcls_ret->Tables["TMMSM00"], false);



		/*设置系统返回参数*/
		strcpy(s.msg, _RES("GCRSS0000002"));//处理成功。  

	}



	/*捕获数据库操作异常*/
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		//LogTrace(1,1,"%s",(const char*)sqlstr);
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "DB error:" + sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

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

	//LogTrace(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	////LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);
	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;

}

