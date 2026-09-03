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
/// 测温数据
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

int f_mmsm2b_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE_TELE(cm_e2t8t2_rcv)

int f_cm_e2t8t2_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);
	CDbCommand cmd_inq_count(conn);
	CModel tmmsm2b("TMMSM2B");
	CString proc_no = " ";
	EIClass tmmsm2b_back;
	CString dev_code = "";

	//加入函数的表

	tmmsm2b_back.Tables.Add();
	if (!tmmsm2b_back.Tables[0].Columns.Contains("PROC_DIV"))
	{
		tmmsm2b_back.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
	}
	if (!tmmsm2b_back.Tables[0].Columns.Contains("AREA_ID"))
	{
		tmmsm2b_back.Tables[0].Columns.Add(DT_STRING, "AREA_ID");
	}
	//tmmsm2b_back.Tables[0].Rows.Add();
	tmmsm2b_back.Tables[0].Columns.Add(tmmsm2b);
	//tmmsm2b_back.Tables[0].Rows.Add();

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		Log::Info("", __FUNCTION__, "1111=[{0}]", "1111");
		for (size_t i = 0; i < bcls_rec->Tables["INT_MES_GEN_DATA"].Rows.get_Count(); i++)
		{
			//工号
			dev_code = bcls_rec->Tables["INT_MES_GEN_DATA"].Rows[i]["AGGREGATE_NAME"].ToString();
			Log::Trace("", "dev_code", "dev_code = {0}", dev_code);
			//查询设备站号和设备代码
			cmd_inq_code.SetCommandText(" select STATION_ID,STATION_NO  from TPSSMD1 where DEV_CODE=@DEV_CODE ");
			cmd_inq_code.Parameters.Clear();
			cmd_inq_code.Parameters.Set("DEV_CODE", dev_code);
			cmd_inq_code.ExecuteReader();
			if (cmd_inq_code.Read())
			{
				tmmsm2b["STATION_ID"] = cmd_inq_code.GetString(1);
				tmmsm2b["STATION_NO"] = cmd_inq_code.GetString(2);

			}
			cmd_inq_code.Close();
			tmmsm2b["DEV_CODE"] = dev_code;
			//处理号
			tmmsm2b["HEAT_NO"] = bcls_rec->Tables["INT_MES_GEN_DATA"].Rows[i]["HEAT_NUMBER"].ToString();
			tmmsm2b["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_GEN_DATA"].Rows[i]["HEAT_NUMBER"].ToString();
			//下一罐钢包重量
			tmmsm2b["LADLE_NET_WEIGHT"] = bcls_rec->Tables["INT_MES_GEN_DATA"].Rows[i]["LADLE_NET_WEIGHT"].ToString();
			//下一罐中间包重量
			tmmsm2b["TUNDISH_NET_WEIGHT"] = bcls_rec->Tables["INT_MES_GEN_DATA"].Rows[i]["TUNDISH_NET_WEIGHT"].ToString();
			cmd_inq_count.SetCommandText("select  nvl(MAX(PROC_COUNT),0)+1 from tmmsm2b where  HEAT_NO ='" + tmmsm2b["HEAT_NO"].ToString() + "' ");
			tmmsm2b["PROC_COUNT"] = cmd_inq_count.ExecuteScalar();
			Log::Info("", __FUNCTION__, "PROC_COUNT=[{0}]", tmmsm2b["PROC_COUNT"].ToString());
			cmd_inq_count.Close();
			//查询熔炼号
			cmd_inq.SetCommandText(" SELECT PROC_NO FROM TPSSM12 WHERE HEAT_NO=@HEAT_NO and DEV_CODE=@DEV_CODE "
				" union "
				" SELECT PROC_NO FROM TPSSM42 WHERE HEAT_NO =@HEAT_NO and DEV_CODE =@DEV_CODE ");
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("HEAT_NO", tmmsm2b["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("DEV_CODE", tmmsm2b["DEV_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				proc_no = cmd_inq.GetString(1);
			}
			Log::Trace("", "PROC_NO", "PROC_NO = {0}", tmmsm2b["PROC_NO"].ToString());
			Log::Trace("", "heat_no", "heat_no = {0}", proc_no);
			cmd_inq.Close();
			//熔炼号
			tmmsm2b["PROC_NO"] = proc_no;
			//中间包温度
			tmmsm2b["STEEL_TEMP"] = bcls_rec->Tables["INT_MES_GEN_DATA"].Rows[i]["TUNDISH_TEMP"].ToString();
			//开浇时间
			tmmsm2b["MEAS_TEMP_TIME"] = bcls_rec->Tables["INT_MES_GEN_DATA"].Rows[i]["TUNDISH_TIME"].ToString();
			//终浇时间(预估)
			tmmsm2b["ESTIMATED_END"] = bcls_rec->Tables["INT_MES_GEN_DATA"].Rows[i]["ESTIMATED_END"].ToString();
			tmmsm2b["REC_CREATOR"] = s.userid;
			tmmsm2b["REC_CREATE_TIME"] = datetime;
			tmmsm2b.MergeTo(tmmsm2b_back.Tables[0], false);
			Log::Info("", __FUNCTION__, "PROC_DIV=[{0}]", "3333");
			tmmsm2b_back.Tables[0].Rows[0]["PROC_DIV"] = "I";
			/*doFlag = f_mmsm2b_proc(&tmmsm2b_back, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
			//存连铸测温数据
			tmmsm2b.TrimOrBlank();
			tmmsm2b.Insert();
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


