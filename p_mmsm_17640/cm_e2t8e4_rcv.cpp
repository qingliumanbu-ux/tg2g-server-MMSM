/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      XXX
Version:     1.0
Date:        2023-10-23
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// IF能源消耗数据
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



BM2F_ENTERACE_TELE(cm_e2t8e4_rcv)

int f_cm_e2t8e4_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_inq_code(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CModel tmmsm2f("TMMSM2F");
	CDecimal devo_wt = 0;
	CString tmmsm2f_name = "";
	CString dev_code = "";

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		for (size_t i = 0; i < bcls_rec->Tables["INT_MES_ENERGY_IF"].Rows.get_Count(); i++)
		{
			for (size_t j = 0; j < bcls_rec->Tables["INT_MES_ENERGY_IF"].Columns.get_Count(); j++)
			{
				///工号
				dev_code = bcls_rec->Tables["INT_MES_ENERGY_IF"].Rows[0]["AGGREGATE_NAME"].ToString();
				Log::Trace("", "dev_code", "dev_code = {0}", dev_code);
				//查询设备站号和设备代码
				cmd_inq_code.SetCommandText(" select STATION_ID,STATION_NO  from TPSSMD1 where DEV_CODE=@DEV_CODE ");
				cmd_inq_code.Parameters.Clear();
				cmd_inq_code.Parameters.Set("DEV_CODE", dev_code);
				cmd_inq_code.ExecuteReader();
				if (cmd_inq_code.Read())
				{
					tmmsm2f["STATION_ID"] = cmd_inq_code.GetString(1);
					tmmsm2f["STATION_NO"] = cmd_inq_code.GetString(2);

				}
				cmd_inq_code.Close();
				tmmsm2f["DEV_CODE"] = dev_code;
				tmmsm2f_name = bcls_rec->Tables["INT_MES_ENERGY_IF"].Columns[j].get_ColumnName();
				devo_wt = bcls_rec->Tables["INT_MES_ENERGY_IF"].Rows[i][tmmsm2f_name].ToDecimal();
				if (tmmsm2f_name == "COMPRESSED_AIR" && devo_wt != 0){
					tmmsm2f["MAT_NAME"] = "压缩空气";
					tmmsm2f["DEVO_WT"] = devo_wt;
					Log::Trace("", "", "MAT_NAME=[{0}] devo_wt[{1}]", tmmsm2f["MAT_NAME"].ToString(), devo_wt);
				}
				else if (tmmsm2f_name == "COOLING_WATER_HIGH" && devo_wt != 0){
					tmmsm2f["MAT_NAME"] = "高温冷却水";
					tmmsm2f["DEVO_WT"] = devo_wt;
					Log::Trace("", "", "MAT_NAME=[{0}] devo_wt[{1}]", tmmsm2f["MAT_NAME"].ToString(), devo_wt);
				}
				else if (tmmsm2f_name == "COOLING_WATER_TRANSFORMER" && devo_wt != 0){
					tmmsm2f["MAT_NAME"] = "变压器冷却水";
					tmmsm2f["DEVO_WT"] = devo_wt;
					Log::Trace("", "", "MAT_NAME=[{0}] devo_wt[{1}]", tmmsm2f["MAT_NAME"].ToString(), devo_wt);
				}
				else if (tmmsm2f_name == "POWER1" && devo_wt != 0){
					tmmsm2f["MAT_NAME"] = "功率1";
					tmmsm2f["DEVO_WT"] = devo_wt;
					Log::Trace("", "", "MAT_NAME=[{0}] devo_wt[{1}]", tmmsm2f["MAT_NAME"].ToString(), devo_wt);
				}
				else if (tmmsm2f_name == "POWER2" && devo_wt != 0){
					tmmsm2f["MAT_NAME"] = "功率2";
					tmmsm2f["DEVO_WT"] = devo_wt;
					Log::Trace("", "", "MAT_NAME=[{0}] devo_wt[{1}]", tmmsm2f["MAT_NAME"].ToString(), devo_wt);
				}
				else
				{
					continue;
				}
				tmmsm2f["REC_CREATOR"] = s.userid;
				tmmsm2f["REC_CREATE_TIME"] = datetime;
				tmmsm2f["START_TIME"] = datetime;
				tmmsm2f.TrimOrBlank();
				tmmsm2f.Insert();
			}
		}
		//tmmsm2f.MergeFrom(bcls_rec->Tables["INT_MES_ENERGY_IF"].Rows[0]);
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

	return doFlag;
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


