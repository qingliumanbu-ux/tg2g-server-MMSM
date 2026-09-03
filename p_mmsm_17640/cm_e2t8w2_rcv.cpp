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
int f_mmsm_t80rya_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//入库
int f_mmsm_t80ryb_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//出库
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_stock_out(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_stock_log(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);



BM2F_ENTERACE_TELE(cm_e2t8w2_rcv)

int f_cm_e2t8w2_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	int blkNum_IN = 0;
	/* 业务变量 */
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */
	CString c_mat_destion("");
	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr1;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);
	EPEX epex(&s, conn);

	CModel twma0("TWMA0");
	CModel twmsma0("TWMSMA0");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm35("TMMSM35");
	CModel hmmsm01("HMMSM01");
	CModel twma4 = CModel("TWMA4");
	//入库队列
	EIClass t80rya;
	t80rya.Tables[0].set_TableName("T80RYA");
	t80rya.Tables[0].Columns.Add(twma0);
	t80rya.Tables[0].Rows.Clear();
	//出库队列
	EIClass t80ryb;
	t80ryb.Tables[0].set_TableName("T80RYB");
	t80ryb.Tables[0].Columns.Add(twma0);
	t80ryb.Tables[0].Rows.Clear();

	blkNum = bcls_rec->Tables.IndexOf("MM0099");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		bcls_rec->Tables["MM0099"].Rows.Clear();
	}
	EIClass bcls_stock_in;
	blkNum = bcls_stock_in.Tables.IndexOf("WM_STOCK");
	if (blkNum < 0)
	{
		bcls_stock_in.Tables.Add("WM_STOCK");
		bcls_stock_in.Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_stock_in.Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");        //库业务类型
		bcls_stock_in.Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");   //业务类型内区分
		bcls_stock_in.Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_NO");				//库号
		bcls_stock_in.Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");			//材料库位号
		bcls_stock_in.Tables["WM_STOCK"].Columns.Add(DT_STRING, "ROWNO");					//行号
		bcls_stock_in.Tables["WM_STOCK"].Columns.Add(DT_STRING, "COLUMN_NO");				//列号
		bcls_stock_in.Tables["WM_STOCK"].Columns.Add(DT_DECIMAL, "LAYERNO");					//层号
		bcls_stock_in.Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_POSITION");	//库位内位置
	}
	bcls_stock_in.Tables["WM_STOCK"].Rows.Clear();

	EIClass bcls_rec_stock_log;
	bcls_rec_stock_log.Tables[0].set_TableName("WM_STOCK_LOG");
	bcls_rec_stock_log.Tables[0].Rows.Clear();



	try
	{
		CString v_stock_no = "";
		bcls_rec->Tables[0].Columns["SLAB_NUMBER"].set_ColumnName("SLAB_NO");
		bcls_rec->Tables[0].Columns["VIRTUAL_SLAB_ID"].set_ColumnName("PONO_SLAB");
		bcls_rec->Tables[0].Columns["LOCATION"].set_ColumnName("MAT_DESTION");
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		hmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (bcls_rec->Tables[0].Rows[0]["MAT_DESTION"].ToString() == "SYA") //入库
		{
			if (!tmmsm01.Query("SLAB_NO"))
			{
				sprintf(s.msg, "查询材料出错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			sqlstr = " select * from TWMA0 where MAT_NO = '" + tmmsm01["MAT_NO"].ToString() + "' and STOCK_OPER_ORDER like '1%' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(twma0);
				bcls_stock_in.Tables["WM_STOCK"].Rows.Add();
				bcls_stock_in.Tables["WM_STOCK"].Rows[blkNum_IN]["MAT_NO"] = twma0["MAT_NO"];
				bcls_stock_in.Tables["WM_STOCK"].Rows[blkNum_IN]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
				bcls_stock_in.Tables["WM_STOCK"].Rows[blkNum_IN]["STOCK_OPER_ORDER_DIV"] = twma0["STOCK_OPER_ORDER_DIV"];
				bcls_stock_in.Tables["WM_STOCK"].Rows[blkNum_IN]["STOCK_NO"] = twma0["STOCK_NO"];
				bcls_stock_in.Tables["WM_STOCK"].Rows[blkNum_IN]["STOCK_PLACE_NO"] = twma0["STOCK_NO"];
				bcls_stock_in.Tables["WM_STOCK"].Rows[blkNum_IN]["LAYERNO"] = 0;
				bcls_stock_in.Tables["WM_STOCK"].Rows[blkNum_IN]["ROWNO"] = " ";
				bcls_stock_in.Tables["WM_STOCK"].Rows[blkNum_IN]["COLUMN_NO"] = " ";
				bcls_stock_in.Tables["WM_STOCK"].Rows[blkNum_IN]["STOCK_PLACE_POSITION"] = twma0["STOCK_NO"];

				twma0.Delete("MAT_NO,STOCK_OPER_ORDER");
				blkNum_IN++;
			}
			if (blkNum_IN == 0)
			{
				/*sprintf(s.msg, "材料[%s]没有相应的入库队列!", (const char*)tmmsm01["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);*/
			}
			if (bcls_stock_in.Tables[0].Rows.get_Count()>0)
			{
				//doFlag = f_wmsmsm_stock_in(&bcls_stock_in, bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			

		}
		else if (bcls_rec->Tables[0].Rows[0]["MAT_DESTION"].ToString() == "SOLD"
			|| bcls_rec->Tables[0].Rows[0]["MAT_DESTION"].ToString() == "HF"
			|| bcls_rec->Tables[0].Rows[0]["MAT_DESTION"].ToString() == "CS-HSM"
			|| bcls_rec->Tables[0].Rows[0]["MAT_DESTION"].ToString() == "SS-HSM"
			|| (bcls_rec->Tables[0].Rows[0]["MAT_DESTION"].ToString() == "STC2" && tmmsm01.Query("SLAB_NO"))) //出库
		{
			if (!tmmsm01.Query("SLAB_NO"))
			{
				sprintf(s.msg, "查询材料出错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			sqlstr1 = " SELECT STOCK_NO FROM TWM01 WHERE STOCK_NO_ANOTHER='" + bcls_rec->Tables[0].Rows[0]["MAT_DESTION"].ToString() + "' ";
			cmd_sql.SetCommandText(sqlstr1);
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				v_stock_no = cmd_sql.GetString(1);
			}
			else
			{
				sprintf(s.msg, "库区[%s]在三级没有配置，请在WM01SMS2N维护!", (const char*)bcls_rec->Tables[0].Rows[0]["MAT_DESTION"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_sql.Close();

			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["STOCK_NO"] = v_stock_no;
			tmmsm96["STOCK_L2"] = bcls_rec->Tables[0].Rows[0]["MAT_DESTION"].ToString();
			tmmsm96["EVENT_ID"] = "WM02";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "cm_e2t8w2_rcv";
			tmmsm96.MergeTo(bcls_rec->Tables["MM0099"]);
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmmsm01.MergeTo(t80ryb.Tables[0]);

			//仓库履历
			twma4.CopyFrom(tmmsm01);
			twma4["STOCK_OPER_ORDER"] = "2G";
			twma4["STOCK_NO"] = v_stock_no;
			twma4["STOCK_L2"] = Db::QueryCString(" SELECT STOCK_NO_ANOTHER FROM TWM01 WHERE STOCK_NO='" + v_stock_no + "' ");
			twma4["FROM_STOCK_NO"] = tmmsm01["STOCK_NO"];
			twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);
			doFlag = f_mmsm_t80ryb_snd(&t80ryb, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			doFlag = f_wmsmsm_stock_log(&bcls_rec_stock_log, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

		}
		else if (bcls_rec->Tables[0].Rows[0]["MAT_DESTION"].ToString() == "STC2")//
		{
			/*if (!)
			{
				sprintf(s.msg, "未在历史档找到该材料!");
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
			if (hmmsm01.QueryCount("SLAB_NO")==1)
			{
				hmmsm01.Query("SLAB_NO");
				
			}else if (hmmsm01.QueryCount("SLAB_NO") > 1)
			{
				sqlstr = " select MAT_NO from (select * from tmmsm35 where SLAB_NO='"+ hmmsm01["SLAB_NO"].ToString() +"' order by REC_CREATE_TIME desc) where rownum=1 ";
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					hmmsm01["MAT_NO"] = cmd_inq.GetString(1);
				}
				else
				{
					return 0;
				}
				cmd_inq.Close();
				hmmsm01.Query("MAT_NO");
			}
			else
			{
				return 0;
			}
			//hmmsm01["MAT_NO"] = hmmsm01["MAT_NO"];
			hmmsm01["HEAT_NO"] = hmmsm01["SLAB_NO"].ToString().SubstringNE(0, 8);
			hmmsm01["MAT_LEN"] = bcls_rec->Tables[0].Rows[0]["ACTUAL_LENGTH"].ToDecimal() * 1000;
			hmmsm01["MAT_WIDTH"] = bcls_rec->Tables[0].Rows[0]["WIDTH_HEAD"].ToDecimal() * 1000;
			hmmsm01["MAT_THICK"] = bcls_rec->Tables[0].Rows[0]["ACTUAL_THICKNESS"].ToDecimal() * 1000;
			hmmsm01["MAT_WT"] = (bcls_rec->Tables[0].Rows[0]["WEIGHT"].ToDecimal() / 1000).Round(3);
			hmmsm01["ST_NO"] = hmmsm01["SLAB_NO"].ToString().SubstringNE(8, 6);
			if (hmmsm01["C_DELIVERY_FAC"].ToString() != "6360")
			{
				return 0;
			}
			twmsma0.CopyFrom(hmmsm01);
			twmsma0["REC_CREATE_TIME"] = datetime;
			twmsma0["REC_CREATOR"] = "E2T8W2";
			twmsma0.Insert();
		}
		else
		{
			sprintf(s.msg, "库区[%s]在三级没有配置!", (const char*)bcls_rec->Tables[0].Rows[0]["MAT_DESTION"]);
			throw CApplicationException(-1, s.msg, log.Location);
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
