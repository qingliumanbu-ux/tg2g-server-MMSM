/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      lizhen
Version:     1.0
Date:        2023-11-08
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"

/*<remark>=========================================================
/// <summary>
/// 调拨应答
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
int f_wmsm_wm07_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm_t80rya_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm_e2t8m1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送专家系统数据


BM2F_ENTERACE_TELE(cm_0rt8d1_rcv)

int f_cm_0rt8d1_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tqmts30("TQMTS30");
	CModel hqmts30("HQMTS30");
	CModel twma0 = CModel("TWMA0");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);

	/* ***** 应用程序开始处理 ***** */
	EIClass mm0099;
	mm0099.Tables[0].set_TableName("MM0099");
	mm0099.Tables[0].Columns.Add(tmmsm96);
	mm0099.Tables[0].Rows.Clear();

	EIClass t8e2m1;
	t8e2m1.Tables[0].set_TableName("E2T8M1");
	t8e2m1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
	t8e2m1.Tables[0].Rows.Clear();

	EIClass in_23m;
	in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	in_23m.Tables[0].Rows.Add();
	in_23m.Tables[0].Rows[0]["TC_NO"] = "T82322";
	in_23m.Tables.Add();
	in_23m.Tables[1].Columns.Add(tmmsm01);
	try
	{
		blkNum = bcls_rec->Tables.IndexOf("WM07");
		if (blkNum<0)
		{
			bcls_rec->Tables.Add("WM07");
		}
		bcls_rec->Tables["WM07"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec->Tables["WM07"].Rows.Clear();
		if (bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString() == "1"
			&& bcls_rec->Tables[0].Rows[0]["PLANT"].ToString() == "6240")
		{
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			if (!tmmsm01.Query("MAT_NO"))
			{
				sprintf(s.msg, "材料号不存在。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm01.MergeTo(in_23m.Tables[1]);
			in_23m.Tables[1].Rows[0]["TRAN_END_TIME"] = datetime;
			in_23m.Tables[1].Rows[0]["C_STATESIGN"] = "3";

			
			tqmts30["MAT_NO"] = tmmsm01["MAT_NO"];
			if (tqmts30.QueryCount("MAT_NO")>0)
			{
				tqmts30["STATUS_FLAG"] = "2";
				tqmts30.Update("STATUS_FLAG", "MAT_NO");

				tqmts30["AREA"] = "北区";
				tqmts30["HEAT_NO"] = " ";
				tqmts30.Query("AREA,MAT_NO,HEAT_NO");
				hqmts30.CopyFrom(tqmts30);
				hqmts30.Insert();
				tqmts30.Delete("AREA,MAT_NO,HEAT_NO");
			}
			bcls_rec->Tables["WM07"].Rows.Add();
			bcls_rec->Tables["WM07"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"].ToString();
			if ((bcls_rec->Tables[0].Rows[0]["MOVE_PLANT"].ToString()=="6360"|| bcls_rec->Tables[0].Rows[0]["MOVE_PLANT"].ToString() == "6380")
				&&tmmsm01["TRAN_END_TIME"].ToString().Trim()=="")
			{
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);
				
				tmmsm96["C_STATESIGN"] = "3";//1--正向调拨出库，3--正向调拨完成
				if ((CDateTime::Now() - CDateTime::Parse(tmmsm01["SLAB_CUT_TIME"].ToString())).TotalHours() <= 6 && tmmsm01["ST_NO"].ToString().SubstringNE(0, 1) == "1")//不锈
				{
					tmmsm96["C_ISHOTSEND"] = "1";
				}
				if ((CDateTime::Now() - CDateTime::Parse(tmmsm01["SLAB_CUT_TIME"].ToString())).TotalHours() <= 4 && tmmsm01["ST_NO"].ToString().SubstringNE(0, 1) == "2")//碳
				{
					tmmsm96["C_ISHOTSEND"] = "1";
				}
				CString v_shift_no = " ";
				CString v_shift_group = " ";
				CString resp = " ";
				f_epep_get_shift_group("SMCP", datetime, v_shift_no, v_shift_group, conn);
				tmmsm96["HAND_OVER_GROUP"] = v_shift_group;
				tmmsm96["C_DELIVERY_FAC"] = bcls_rec->Tables[0].Rows[0]["MOVE_PLANT"].ToString();
				tmmsm96["C_DELIVERY_STOCK"] = bcls_rec->Tables[0].Rows[0]["MOVE_PLANT"].ToString().Substring(0,3)+"1";
				tmmsm96["TRAN_END_TIME"] = datetime;
				tmmsm96["HAND_OVER_GROUP"] = v_shift_group;
				tmmsm96["EVENT_ID"] = "MM76H";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				Log::Trace("", __FUNCTION__, "v_shift_group[{0}]", v_shift_group);
				//2026.04.21 交库班组对应交库负责人
				if (v_shift_group != "")
				{
					sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TWMSMZD02 T WHERE 1=1 AND T.CODE_CLASS='MMSMJKFZR' AND T.CODE = @v_shift_group ";

					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("v_shift_group", v_shift_group);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						resp = cmd_inq.GetString(1);
					}
					cmd_inq.Close();
					Log::Trace("", __FUNCTION__, "resp[{0}]  ", resp);
				}
				tmmsm96["RESP"] = resp;
				tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);

				tmmsm96.MergeTo(t8e2m1.Tables["E2T8M1"], false);
			}
			else{
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);

				CString v_shift_no = " ";
				CString v_shift_group = " ";
				CString resp = " ";
				f_epep_get_shift_group("SMCP", datetime, v_shift_no, v_shift_group, conn);
				Log::Trace("", __FUNCTION__, "v_shift_group[{0}]", v_shift_group);
				//2026.04.21 交库班组对应交库负责人
				if (v_shift_group != "")
				{
					sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TWMSMZD02 T WHERE 1=1 AND T.CODE_CLASS='MMSMJKFZR' AND T.CODE = @v_shift_group ";

					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("v_shift_group", v_shift_group);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						resp = cmd_inq.GetString(1);
					}
					cmd_inq.Close();
					Log::Trace("", __FUNCTION__, "resp[{0}]  ", resp);
				}
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["EVENT_ID"] = "MM76H";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96["RESP"] = resp;
				tmmsm96.MergeTo(mm0099.Tables["MM0099"], false);
				tmmsm96.MergeTo(t8e2m1.Tables["E2T8M1"], false);
			}
			if (mm0099.Tables["MM0099"].Rows.get_Count() > 0)
			{
				doFlag = f_mmsm99(&mm0099, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				doFlag = f_mmsm_e2t8m1_snd(&t8e2m1, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			doFlag = f_wmsm_wm07_proc(bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			int _ret_23 = 0;
			if (in_23m.Tables[1].Rows.get_Count() > 0)
			{
				_ret_23 = f_t8z_23m_snd(&in_23m, bcls_ret, conn);
			}
		}
		if ( bcls_rec->Tables[0].Rows[0]["PLANT"].ToString() != "6240")
		{
			if (bcls_rec->Tables.IndexOf("T80RYA") < 0)
			{
				bcls_rec->Tables.Add("T80RYA");
				bcls_rec->Tables["T80RYA"].Columns.Add(twma0);
				bcls_rec->Tables["T80RYA"].Rows.Clear();
			}
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			twma0["MAT_NO"] = tmmsm01["MAT_NO"];
			twma0["STOCK_OPER_ORDER"] = "1A";
			twma0.Query("MAT_NO,STOCK_OPER_ORDER");

			bcls_rec->Tables["T80RYA"].Rows.Add();
			bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
			bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER_DIV"] = twma0["STOCK_OPER_ORDER_DIV"];
			bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NO"] = twma0["MAT_NO"];
			bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NUM"] = twma0["MAT_NUM"];
			bcls_rec->Tables["T80RYA"].Rows[0]["MAT_LINE_TYPE"] = twma0["MAT_LINE_TYPE"];
			bcls_rec->Tables["T80RYA"].Rows[0]["MAT_KIND"] = twma0["MAT_KIND"];
			bcls_rec->Tables["T80RYA"].Rows[0]["FACTORY_DIV"] = "LG1";
			bcls_rec->Tables["T80RYA"].Rows[0]["USER_ID"] = "cm_0rt801_rcv";
			if (!bcls_rec->Tables["T80RYA"].Columns.Contains("STOCK_OPER_TIME"))
				bcls_rec->Tables["T80RYA"].Columns.Add(DT_STRING, "STOCK_OPER_TIME");

			bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_TIME"] = datetime;
			bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_NO"] = "AK3";
			bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_PLACE_NO"] = twma0["TO_STOCK_PLACE_NO"];
			bcls_rec->Tables["T80RYA"].Rows[0]["TO_LAYERNO"] = twma0["TO_LAYERNO"];
			//doFlag = f_mmsm_t80rya_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


