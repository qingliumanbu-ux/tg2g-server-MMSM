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
/// 倒灌电文
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//调用仓库接口，进行板坯入库   太钢定制  
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送板坯丢失
int f_mmsm_t80rya_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


BM2F_ENTERACE_TELE(cm_0rt8m0_rcv)

int f_cm_0rt8m0_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CModel hmmsm01("HMMSM01");
	CModel twmsma0("TWMSMA0");
	CModel tmmsm96("TMMSM96");
	CModel tqmts30("TQMTS30");
	CModel twma0 = CModel("TWMA0");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_code(conn);


	blkNum = bcls_rec->Tables.IndexOf("WM_STOCK");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("WM_STOCK");
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");        //库业务类型
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_DECIMAL, "STOCK_OPER_ORDER_DIV");   //业务类型内区分
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_NO");				//库号
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");			//材料库位号
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "ROWNO");					//行号
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "COLUMN_NO");				//列号
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "LAYERNO");					//层号
		bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_POSITION");	//库位内位置
	}
	bcls_rec->Tables["WM_STOCK"].Rows.Clear();

	EIClass inblock;
	inblock.Tables[0].Columns.Add(tmmsm01);
	inblock.Tables[0].Rows.Clear();

	if (bcls_rec->Tables.IndexOf("T80RYA") < 0)
	{
		bcls_rec->Tables.Add("T80RYA");
		bcls_rec->Tables["T80RYA"].Columns.Add(twma0);
		bcls_rec->Tables["T80RYA"].Rows.Clear();
	}
	try
	{
		if (bcls_rec->Tables.IndexOf("MM0099") < 0) {
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
			
		}
		bcls_rec->Tables["MM0099"].Rows.Clear();
		if (bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim() == "") {
			sprintf(s.msg, "材料号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		

		
		hmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		twmsma0["MAT_NO"] = hmmsm01["MAT_NO"];
		if (twmsma0.Query("MAT_NO"))
		{
			twmsma0.Delete("MAT_NO");
		}
		tqmts30["MAT_NO"] = hmmsm01["MAT_NO"];
		if (tqmts30.QueryCount("MAT_NO") > 0)
		{
			/*tqmts30["STATUS_FLAG"] = "1";
			tqmts30.Update("STATUS_FLAG", "MAT_NO");*/
		}
		CString STOCK_OPER_ORDER = "1A";
		if (hmmsm01.Query("MAT_NO"))
		{
			STOCK_OPER_ORDER = "1Q";
			hmmsm01.Delete("MAT_NO");

		
			hmmsm01.MergeTo(inblock.Tables[0], false);
			
			if (hmmsm01["MAT_DESTION"].ToString().Trim() != "")
			{
				if (hmmsm01["MAT_DESTION"].ToString().SubstringNE(0,1) == "P")
					hmmsm01["MAT_DESTION"] = "10";
				if (hmmsm01["MAT_DESTION"].ToString().SubstringNE(0, 1) == "R")
					hmmsm01["MAT_DESTION"] = "11";
				if (hmmsm01["MAT_DESTION"].ToString().SubstringNE(0, 1) == "F" || hmmsm01["MAT_DESTION"].ToString().SubstringNE(0, 1) == "G"
					|| hmmsm01["MAT_DESTION"].ToString().SubstringNE(0, 1) == "H" || hmmsm01["MAT_DESTION"].ToString().SubstringNE(0, 1) == "I")
					hmmsm01["MAT_DESTION"] = "20";
				if (hmmsm01["MAT_DESTION"].ToString().SubstringNE(0, 1) == "J")
					hmmsm01["MAT_DESTION"] = "30";
				if (hmmsm01["MAT_DESTION"].ToString().SubstringNE(0, 1) == "T")
					hmmsm01["MAT_DESTION"] = "41";
				if (hmmsm01["MAT_DESTION"].ToString().SubstringNE(0, 1) == "L")
					hmmsm01["MAT_DESTION"] = "40";
			}
		}
		else {
			hmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			hmmsm01["SLAB_NO"] = hmmsm01["MAT_NO"];//非本厂来料，由于拿不到板坯号，喷印号，特用坯号赋值-lz20240520
			hmmsm01["PRINT_NO"] = hmmsm01["MAT_NO"];
			hmmsm01["BATCH"] = hmmsm01["CHARG"];
			hmmsm01["SG_GRADE_1"] = hmmsm01["SG_SIGN"];
			if (hmmsm01["WHOLE_BACKLOG"].ToString().Trim() != "" && hmmsm01["NEXT_WHOLE_BACKLOG_CODE"].ToString().Trim())
			{
				CString mat_des = hmmsm01["WHOLE_BACKLOG"].ToString().SubstringNE(hmmsm01["WHOLE_BACKLOG"].ToString().Find(hmmsm01["NEXT_WHOLE_BACKLOG_CODE"].ToString()) + 2, 1);
				Log::Trace("", "", "mat_des", mat_des);
				if (mat_des.Trim() != "")
				{
					if (mat_des == "P")
						hmmsm01["MAT_DESTION"] = "10";
					if (mat_des == "R")
						hmmsm01["MAT_DESTION"] = "11";
					if (mat_des == "F"|| mat_des == "G"|| mat_des == "H"||mat_des == "I")
						hmmsm01["MAT_DESTION"] = "20";
					if (mat_des == "J")
						hmmsm01["MAT_DESTION"] = "30";
					if (mat_des == "T")
						hmmsm01["MAT_DESTION"] = "41";
					if (mat_des == "L")
						hmmsm01["MAT_DESTION"] = "40";
				}
			}
			hmmsm01["CONCESS_CON_FLAG"] = "1";
		}
		tmmsm01.CopyFrom(hmmsm01);
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (tmmsm01["MAT_TRACK_NO"].ToString().GetLength() > 18)
		{
			tmmsm01["MAT_TRACK_NO"] = tmmsm01["MAT_TRACK_NO"].ToString().Substring(0, 18);
		}
		//tmmsm01["DEV_CODE"] = bcls_rec->Tables[0].Rows[0]["MAT_ORIGIN_DETAIL"].ToString().SubstringNE(1, 2);
		int wm_stock_count = 0;
		if (true&& STOCK_OPER_ORDER=="1A") //
		{
			bcls_rec->Tables["WM_STOCK"].Rows.Add();
			bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["MAT_NO"] = tmmsm01["MAT_NO"];
			bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_OPER_ORDER"] = STOCK_OPER_ORDER;					//库业务类型
			bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_OPER_ORDER_DIV"] = "1";					//业务类型内区分
			bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_NO"] = "SYA";			//库号
			bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_PLACE_NO"] = "SYA";						//材料库位号
			bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["ROWNO"] = " ";								//行号
			bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["COLUMN_NO"] = " ";							//列号
			bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["LAYERNO"] = 0;								//层号
			bcls_rec->Tables["WM_STOCK"].Rows[wm_stock_count]["STOCK_PLACE_POSITION"] = "1";				//库位内位置
			wm_stock_count++;
		}
		else {
			tmmsm01["STOCK_NO"] = "SYA";
			tmmsm01["STOCK_PLACE_NO"] = "SYA";
			
		}

		//喷印号不取电文数据，产销层没有  mfj 20240516
		tmmsm01["PRINT_NO"] = hmmsm01["PRINT_NO"];
		//产线类型为SM  mfj 20240516  否则盘库查不到
		tmmsm01["MAT_LINE_TYPE"] = "SM";
		tmmsm01["UNIT_CODE"] = hmmsm01["DEV_CODE"];
		tmmsm01["APN"] = hmmsm01["APN"];
		tmmsm01["STOCK_NO"] = "SYA";
		tmmsm01["STOCK_L2"] = "SYA";
		if (tmmsm01["BATCH"].ToString().Trim() == "")
		{
			tmmsm01["BATCH"] = tmmsm01["MAT_NO"];
		}
		//本厂回料用历史库存地，外厂来料或变号回来用库号判断
		if (hmmsm01.QueryCount("MAT_NO")>0)
		{
			tmmsm01["LGORT"] = hmmsm01["LGORT"];
		}
		else
		{
			if (bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString() == "AK2")
			{
				tmmsm01["LGORT"] = "6246";
			}
			else
			{
				tmmsm01["LGORT"] = "6242";
			}
		}
		if (tmmsm01["ST_NO"].ToString().SubstringNE(0,1)=="1"
			|| tmmsm01["ST_NO"].ToString().SubstringNE(0, 1) == "4")
		{
			tmmsm01["C_DIV"] = "1";
		}
		else
		{
			tmmsm01["C_DIV"] = "2";
		}
		if (tmmsm01["MAT_NO"].ToString().SubstringNE(0,2)=="B3")
		{
			tmmsm01["DEV_CODE"] = "C8";
			tmmsm01["UNIT_CODE"] = "C8";
		}
		if (tmmsm01["MAT_NO"].ToString().SubstringNE(0, 2) == "B4"|| tmmsm01["MAT_NO"].ToString().SubstringNE(0, 2) == "B5")
		{
			tmmsm01["DEV_CODE"] = "C7";
			tmmsm01["UNIT_CODE"] = "C7";
		}
		if (tmmsm01["DEV_CODE"].ToString().Trim() == "")
		{
			CString s_dev = Db::QueryCString("select DEV_CODE from hmmsm01 where HEAT_NO='" + bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString() + "' AND ROWNUM=1");
			if (s_dev.Trim() == "")
			{
				if (tmmsm01["C_DIV"].ToString() == "1")
				{
					s_dev = "C0";
				}
				if (tmmsm01["C_DIV"].ToString() == "2")
				{
					s_dev = "C4";
				}
			}
			tmmsm01["DEV_CODE"] = s_dev;
			tmmsm01["UNIT_CODE"] = s_dev;
		}
		tmmsm01["C_STATESIGN"] = "0";
		tmmsm01["LOGISTICS_STATUS"] = "0";
		tmmsm01["LOAD_SCHEME_NO"] = " ";
		tmmsm01["PRACTICE_NO"] = " ";
		tmmsm01["PRE_LOAD_FLAG"] = "0";
		tmmsm01["RCV_MAT_FLAG"] = "S";
		tmmsm01["MAT_LEN"] = tmmsm01["MAT_ACT_LEN"];
		tmmsm01["MAT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
		tmmsm01["MAT_THICK"] = tmmsm01["MAT_ACT_THICK"];
		tmmsm01["MAT_WT"] = tmmsm01["MAT_ACT_WT"];
		tmmsm01["MAT_DESTION"] = hmmsm01["MAT_DESTION"];
		tmmsm01["PROD_SHIFT_GROUP"] = hmmsm01["PROD_SHIFT_GROUP"];
		tmmsm01["PROD_TIME"] = hmmsm01["PROD_TIME"];
		tmmsm01["C_STATESIGN"] = "0";
		tmmsm01["C_DELIVERY_FAC"] = " ";
		tmmsm01["C_DELIVERY_STOCK"] = " ";
		tmmsm01["DST_STOCK_CODE"] = " ";
		tmmsm01["UNLOAD_CODE"] = " ";
		tmmsm01["C_ISHOTSEND"] = " ";
		tmmsm01["TRAN_TIME"] = " ";
		tmmsm01["TRAN_END_TIME"] = " ";
		tmmsm01["C_DELIVERYID"] = " ";
		tmmsm01["ARCHIVE_TIME"] = " ";
		tmmsm01["TRANSFER_FLAG"] = "0";
		tmmsm01["COMPANY_CODE"] = "1";//代表倒灌回来的  lz20240906
		if (bcls_rec->Tables[0].Rows[0]["COMPLEX_DECIDE_CODE"].ToString().Trim() == "1")
		{
			tmmsm01["USAGE_DECISION"] = "3001";
		}
		if (bcls_rec->Tables[0].Rows[0]["COMPLEX_DECIDE_CODE"].ToString().Trim() == "2")
		{
			tmmsm01["USAGE_DECISION"] = "3013";
		}

		tmmsm96.CopyFrom(tmmsm01);
		tmmsm96["EVENT_ID"] = "WM20";
		tmmsm96["EVENT_LINE_TYPE"] = "SM";
		tmmsm96["SYSTEM_ID"] = "MMSM";
		tmmsm96["FUNC_ID"] = "cm_0rt8m0_rcv";
		tmmsm96["EVENT_DESC"] = "倒灌新增";
		tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
		tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

		bcls_rec->Tables["T80RYA"].Rows.Clear();
		bcls_rec->Tables["T80RYA"].Rows.Add();
		bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER"] = "1Q";
		bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER_DIV"] = "1";
		bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
		bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NUM"] = 1;
		bcls_rec->Tables["T80RYA"].Rows[0]["MAT_LINE_TYPE"] = tmmsm01["MAT_LINE_TYPE"];
		bcls_rec->Tables["T80RYA"].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
		bcls_rec->Tables["T80RYA"].Rows[0]["FACTORY_DIV"] = "LG1";
		bcls_rec->Tables["T80RYA"].Rows[0]["USER_ID"] = "cm_0rt801_rcv";
		if (!bcls_rec->Tables["T80RYA"].Columns.Contains("STOCK_OPER_TIME"))
			bcls_rec->Tables["T80RYA"].Columns.Add(DT_STRING, "STOCK_OPER_TIME");

		bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_TIME"] = datetime;
		bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_NO"] = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString();
		bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_PLACE_NO"] = "0";
		bcls_rec->Tables["T80RYA"].Rows[0]["TO_LAYERNO"] = 0;

		

		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0) {
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		doFlag = f_mmsm_t80rya_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (bcls_rec->Tables["WM_STOCK"].Rows.get_Count() > 0) {
			Log::Trace("", "", "WM_STOCK", bcls_rec->Tables["WM_STOCK"].Rows.get_Count());
			//doFlag = f_wmsmsm_stock_in(bcls_rec, bcls_ret, conn);  //太钢定制   产出时入库
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (inblock.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
			if (doFlag < 0) {
				throw CApplicationException(-1, s.msg, s.svc_name);
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


