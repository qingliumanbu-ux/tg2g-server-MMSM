/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      李振
Version:     1.0
Date:        2023-11-18
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 板坯称重数据电文接收
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wm00_cal_layerno(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE_TELE(cm_e2t8w3_rcv)

int f_cm_e2t8w3_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	int blkNum_MOV = 0;
	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);
	CModel twma0("TWMA0");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CDecimal v_layerno = 0;
	blkNum = bcls_rec->Tables.IndexOf("MM0099");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		bcls_rec->Tables["MM0099"].Rows.Clear();
	}

	EIClass rec_stack_layer;
	EIClass ret_stack_layer;
	rec_stack_layer.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	rec_stack_layer.Tables[0].Columns.Add(DT_STRING, "STOCK_NO");
	rec_stack_layer.Tables[0].Rows.Add();

	try
	{
		bcls_rec->Tables[0].Columns["SLAB_NUMBER"].set_ColumnName("SLAB_NO");
		bcls_rec->Tables[0].Columns["VIRTUAL_SLAB_ID"].set_ColumnName("PONO_SLAB");
		
		tmmsm01["SLAB_NO"] = bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString();
		if (!tmmsm01.Query("SLAB_NO"))
		{
			sprintf(s.msg, "查询材料出错!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm96.CopyFrom(tmmsm01);
		tmmsm96["STOCK_PLACE_NO"] = bcls_rec->Tables[0].Rows[0]["DEPOSIT_LOC_ID"].ToString();
		tmmsm96["OLD_STOCK_PLACE_NO"] = bcls_rec->Tables[0].Rows[0]["PICKUP_LOC_ID"].ToString();
		tmmsm96["EVENT_ID"] = "WM04";
		tmmsm96["EVENT_LINE_TYPE"] = "00";
		tmmsm96["SYSTEM_ID"] = "MMSM";
		tmmsm96["FUNC_ID"] = "cm_e2t8w3_rcv";
		

		rec_stack_layer.Tables[0].Rows[0]["STOCK_PLACE_NO"] = tmmsm96["STOCK_PLACE_NO"];
		rec_stack_layer.Tables[0].Rows[0]["STOCK_NO"] = "SYA";

		doFlag = f_wm00_cal_layerno(&rec_stack_layer, &ret_stack_layer, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		v_layerno = ret_stack_layer.Tables[0].Rows[0]["LAYERNO"].ToDecimal();
		tmmsm96["LAYERNO"] = v_layerno;
		tmmsm96.MergeTo(bcls_rec->Tables["MM0099"]);
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
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


