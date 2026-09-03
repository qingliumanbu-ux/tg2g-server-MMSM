/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      178053
Version:     1.0
Date:        2019-12-09 15:28:08
Description: PES侧接收炼钢钢坯物料信息倒灌(MMS->炼钢PES)电文
**************************************************/

//框架头文件
#include "stdafx.h" 
#include "epex.h"

/*<remark>=========================================================
/// <summary>
/// PES侧接收炼钢钢坯物料信息倒灌(MMS->炼钢PES)电文
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件




//外部函数声明
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
int f_wm00_queue(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//int f_sm00_madan_red(EIClass *bcls_rec,EIClass *bcls_ret,CDbConnection * conn);

BM2F_ENTERACE_TELE(cm_0020ma_rcv)


int f_cm_0020ma_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	CModel tqmtqq0("TQMTQQ0");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加并设置块名 */
		blkNum = bcls_rec->AtBlkName("MM0099");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "STOCK_NO");
			//bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
		}
		blkNum = bcls_rec->AtBlkName("WM00QUE");
		if (blkNum <= 0)
		{
			blkNum = bcls_rec->AddBlock();
			bcls_rec->SetBlkName(blkNum, "WM00QUE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_EXEC_SEQ_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "TRANS_TOOL");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "UNIT_CODE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "NEXT_UNIT_CODE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_DESTION");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");
			bcls_rec->Tables["WM00QUE"].Rows.Add(); // 创建一行
		}
		blkNum = bcls_rec->AtBlkName("madan_red");
		if (blkNum <= 0)
		{
			blkNum = bcls_rec->AddBlock();
			bcls_rec->SetBlkName(blkNum, "madan_red");
			bcls_rec->Tables["madan_red"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["madan_red"].Columns.Add(DT_STRING, "CONFM_PLAN_NO");
			bcls_rec->Tables["madan_red"].Rows.Add(); // 创建一行
		}

		/* 获取输入参数 */
		tmmsm96.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm96.TrimOrBlank();

		Log::Trace("", __FUNCTION__, "传入参数 tmmsm96.EVENT_ID			={0}", tmmsm96["EVENT_ID"].ToString());
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm96.MAT_NO				={0}", tmmsm96["MAT_NO"].ToString());

		/* 检查输入参数合法性 */
		if (tmmsm96["EVENT_ID"].ToString().Trim() == "")
		{
			strcpy(s.msg, "事件号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm96["MAT_NO"].ToString().Trim() == "")
		{
			//sprintf(s.msg,_RES("GCRSS0000035")/*材料号不能为空*/);
			strcpy(s.msg, "材料号不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/* 判断板坯主档是否有该材料 */
		tmmsm01["MAT_NO"] = tmmsm96["MAT_NO"];
		if (tmmsm01.QueryCount("MAT_NO") > 0)
		{
			sprintf(s.msg, "该材料[%s]已存在!", (const char*)tmmsm96["MAT_NO"].ToString());
			throw CApplicationException(-1, s.msg, log.Location);

		}

		hmmsm01["MAT_NO"] = tmmsm96["MAT_NO"];
		if (hmmsm01.QueryCount("MAT_NO") > 0)
		{
			hmmsm01.Query("MAT_NO");
			tmmsm96["MAT_DESTION"] = hmmsm01["MAT_DESTION"];
			tmmsm96["MAT_LINE_TYPE"] = hmmsm01["MAT_LINE_TYPE"];
			tmmsm96["FIN_ST_NO"] = hmmsm01["FIN_ST_NO"];
			tmmsm96["HOT_CHARGE_FLAG"] = hmmsm01["HOT_CHARGE_FLAG"];
			tmmsm96["HOT_SEND_FLAG"] = hmmsm01["HOT_SEND_FLAG"];
			hmmsm01.Delete("MAT_NO");
		}

		if (tmmsm96["EVENT_ID"].ToString().Trim() == "" || tmmsm96["EVENT_ID"].ToString().Trim() == "WM08")
		{
			tmmsm96["EVENT_ID"] = "WM27";
			tmmsm96["EVENT_LINE_TYPE"] = "00";
		}

		/* 调用物料跟踪 */
		tmmsm96["FUNC_ID"] = "cm_0020ma_rcv";
		tmmsm96["SYSTEM_ID"] = "MMSM";
		tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 热轧钢卷发货红冲 调用发货函数 */
		if (tmmsm96["EVENT_ID"].ToString().Trim() == "SM02")
		{
			/* 调用发货函数 */
			bcls_rec->Tables["madan_red"].Rows[0]["MAT_NO"] = tmmsm96["MAT_NO"];
			bcls_rec->Tables["madan_red"].Rows[0]["CONFM_PLAN_NO"] = tmmsm96["CONFM_PLAN_NO"];
			//doFlag = f_sm00_madan_red(bcls_rec, bcls_ret,conn);	
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		

		/* 调用仓库入库队列 */
		if (tmmsm96["EVENT_ID"].ToString().Trim() == "MM27")  //外购料倒灌
		{
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1E"; //1E-外购入库
		}
		if (tmmsm96["EVENT_ID"].ToString().Trim() == "SM02") //热轧钢卷发货红冲
		{
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1N"; //1N-码单红冲入库
		}
		if (tmmsm96["EVENT_ID"].ToString().Trim() == "PM19") //热轧钢卷材料从厂外库转入厂内库
		{
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1G"; //1G-转库入库
		}
		if (tmmsm96["EVENT_ID"].ToString().Trim() == "WM27")	//材料跨产线数据倒灌
		{
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1G"; //1G-转库入库
		}

		if (tmmsm96["EVENT_ID"].ToString().Trim() == "MM27"&& tmmsm96["MAT_SHAPE_FLAG"].ToString().Trim() == "1")  //板坯
		{
			tmmsm96["AIM_STORE"] = "SA1";
		}
		else if (tmmsm96["EVENT_ID"].ToString().Trim() == "MM27" && tmmsm96["MAT_SHAPE_FLAG"].ToString().Trim() == "A")  //板坯
		{
			tmmsm96["AIM_STORE"] = "SA2";
		}

		bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NO"] = tmmsm96["MAT_NO"];
		bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NUM"] = 1;
		bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_NO"] = tmmsm96["AIM_STORE"];
		bcls_rec->Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "I";
		doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 外购料倒入成分信息 */
		if (tmmsm96["EVENT_ID"].ToString().Trim() == "MM27")
		{
			if (tmmsm96["PONO"].ToString().Trim() == "")
			{
				//sprintf(s.msg,_RES("MM00S0000072")/*制造命令号不能为空。*/);
				strcpy(s.msg, "制造命令号不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 校验炉次成分是否存在 */
			tqmtqq0["PONO"] = tmmsm96["PONO"];
			if (tqmtqq0.QueryCount("PONO") > 0)
			{
				/* 存在则按PONO删除炉次成分信息 */
				tqmtqq0.Delete("PONO");
			}

			/* 新增炉次成份表 */
			for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
			{
				tqmtqq0.Reset();
				tqmtqq0.MergeFrom(bcls_rec->Tables[1].Rows[i]);
				tqmtqq0.TrimOrBlank();

				//Log::Trace("", __FUNCTION__, "传入参数 tqmtqq0.ELM_CODE			={0}", tqmtqq0["ELM_CODE"].ToString());
				//Log::Trace("", __FUNCTION__, "传入参数 tqmtqq0.ELM_NAME			={0}", tqmtqq0["ELM_NAME"].ToString());
				//Log::Trace("", __FUNCTION__, "传入参数 tqmtqq0.ELM_ACT			={0}", tqmtqq0["ELM_ACT"].ToDecimal());

				/* 元素顺序从1开始，否则跳出循环 */
				if (tqmtqq0["ELM_POS"].ToDecimal() <= 0)
					break;

				/* 新增炉次成份表 */
				tqmtqq0["PONO"] = tmmsm96["PONO"];
				tqmtqq0["ST_NO"] = tmmsm96["ST_NO"];

				tqmtqq0["REC_CREATE_TIME"] = datetime;
				tqmtqq0["REC_CREATOR"] = "0020MA";
				tqmtqq0.TrimOrBlank();
				tqmtqq0.Insert();
			}
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


