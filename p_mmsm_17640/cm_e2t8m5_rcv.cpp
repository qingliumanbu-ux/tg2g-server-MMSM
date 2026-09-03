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
/// 板坯水爆信息电文接收
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wm00_cal_layerno(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE_TELE(cm_e2t8m5_rcv)

int f_cm_e2t8m5_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */
	CString v_prod_shift_no = "";//班次
	CString v_prod_shift_group = "";//班组
	CDecimal v_layerno = 0;
	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */ 
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);

	CModel tmmsm37("TMMSM37");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	//将table【0】设置为table【TMMSM37】,并将结构体37表结构赋给table【TMMSM37】

	/*EIClass bcls_rec_MMSM37;
	bcls_rec_MMSM37.Tables[0].set_TableName("TMMSM37");
	bcls_rec_MMSM37.Tables[0].Clear();
	bcls_rec_MMSM37.Tables[0].Columns.Add(tmmsm37);
	bcls_rec_MMSM37.Tables[0].Rows.Add();*/

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


#pragma region  将接口字段与表字段对应
			 
			if (!bcls_rec->Tables["INT_MES_SLABMOVE1"].Columns.Contains("TABLE_NAME") && !bcls_rec->Tables["INT_MES_SLABMOVE1"].Columns.Contains("AGGREGATE_NAME"))
			{
				Log::Trace("", "", "该表没有字段");
			}
			else
			{
				Log::Trace("", "", "111");
				tmmsm37["DEV_CODE"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["AGGREGATE_NAME"].ToString().Trim();
				Log::Trace("", "", "222");
				tmmsm37["SLAB_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["SLAB_NUMBER"].ToString().Trim();
			    tmmsm37["MAT_NO"] = tmmsm37["SLAB_NO"].ToString().SubstringNE(0, 8) + tmmsm37["SLAB_NO"].ToString().SubstringNE(15, 2);
				tmmsm37["VIRTUAL_SLAB_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["VIRTUAL_SLAB_ID"].ToString().Trim();
				tmmsm37["PICKUP_LOC_TYPE"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["PICKUP_LOC_TYPE"].ToString().Trim();
				tmmsm37["PICKUP_LOC_ID"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["PICKUP_LOC_ID"].ToString().Trim();
				tmmsm37["PICKUP_LAYER_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["PICKUP_LAYER_NO"].ToDecimal();
				tmmsm37["PICKUP_POS_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["PICKUP_POS_NO"].ToDecimal();
				Log::Trace("", "", "333");
				tmmsm37["DEPOSIT_LOC_TYPE"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["DEPOSIT_LOC_TYPE"].ToString().Trim();
				tmmsm37["DEPOSIT_LOC_ID"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["DEPOSIT_LOC_ID"].ToString().Trim();
				tmmsm37["DEPOSIT_LAYER_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["DEPOSIT_LAYER_NO"].ToDecimal();
				tmmsm37["DEPOSIT_POS_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["DEPOSIT_POS_NO"].ToDecimal();
				tmmsm37["MOVEMENT_C"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["MOVEMENT_DATE"].ToString().Trim();
				tmmsm37["INITIAL_STORE_C"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["INITIAL_STORE_DATE"].ToString().Trim();
				Log::Trace("", "", "444");
				tmmsm37.TrimOrBlank();

				//获取班次班组
				if (tmmsm37["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm37["PROD_SHIFT_GROUP"].ToString().Trim() == "")
				{
					f_epep_get_shift_group("SMCP", tmmsm37["MOVEMENT_C"].ToString(), v_prod_shift_no, v_prod_shift_group, conn);
					tmmsm37["PROD_SHIFT_NO"] = v_prod_shift_no;
					tmmsm37["PROD_SHIFT_GROUP"] = v_prod_shift_group;
					
				}

				if (tmmsm37.QueryCount("SLAB_NO") > 0)
				{
					Log::Trace("", "", "666");
					tmmsm37.Update("*", "SLAB_NO");
				}
				else
				{
					Log::Trace("", "", "555");
					tmmsm37.Insert();
				}
				
				

				tmmsm01["SLAB_NO"] = tmmsm37["SLAB_NO"];
				if (!tmmsm01.Query("SLAB_NO"))
				{
					sprintf(s.msg, "查询材料出错!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm01.MergeFrom(bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]);
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["STOCK_PLACE_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["DEPOSIT_LOC_ID"].ToString();
				tmmsm96["OLD_STOCK_PLACE_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["PICKUP_LOC_ID"].ToString();
				tmmsm96["EVENT_ID"] = "WM04";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["FUNC_ID"] = "cm_e2t8m5_rcv";


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
	/*bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["DEV_CODE"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["AGGREGATE_NAME"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["SLAB_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["SLAB_N"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["VIRTUAL_SLAB_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["VIRTUAL_SLAB_ID"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["PICKUP_LOC_TYPE"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["PICKUP_LOC_TYPE"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["PICKUP_LOC_ID"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["PICKUP_LOC_ID"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["PICKUP_LAYER_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["PICKUP_LAYER_NO"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["PICKUP_POS_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["PICKUP_POS_NO"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["DEPOSIT_LOC_TYPE"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["DEPOSIT_LOC_TYPE"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["DEPOSIT_LOC_ID"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["DEPOSIT_LOC_ID"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["DEPOSIT_LAYER_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["DEPOSIT_LAYER_NO"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["DEPOSIT_POS_NO"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["DEPOSIT_POS_NO"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["MOVEMENT_C"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["MOVEMENT_C"];
	bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]["INITIAL_STORE_C"] = bcls_rec->Tables["INT_MES_SLABMOVE1"].Rows[0]["INITIAL_STORE_C"];*/

#pragma endregion

	//tmmsm37.MergeFrom(bcls_rec_MMSM37.Tables["TMMSM37"].Rows[0]);
	//tmmsm37.Insert();

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


