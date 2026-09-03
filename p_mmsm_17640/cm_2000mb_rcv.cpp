/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      178053
Version:     1.0
Date:        2019-12-13 16:23:40
Description: MMS侧接收炼钢钢坯信息修改(炼钢PES->MMS)电文
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"

/*<remark>=========================================================
/// <summary>
/// MMS侧接收炼钢钢坯信息修改(炼钢PES->MMS)电文
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
 

//外部函数声明
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);		//物料跟踪函数

BM2F_ENTERACE(cm_2000mb_rcv)


int f_cm_2000mb_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
		}

		/* 获得传入参数 */
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm01.TrimOrBlank();

		/* 打印传入参数 */
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

		/* 检查输入参数合法性 */
		if (tmmsm01["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 材料是否在当前档 */
		if (tmmsm01.QueryCount("MAT_NO") <= 0)
		{
			sprintf(s.msg, "输入的材料号[%s]在当前档不存在!", (const char*)tmmsm01["MAT_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 修改物料主档 */
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_THICK");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_WIDTH");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_LEN");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_ACT_THICK");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_ACT_WIDTH");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_ACT_LEN");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_ACT_INNER_DIA");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_ACT_OUTER_DIA");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_THEORY_WT");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_ACT_WT");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MEASURE_WT_FLAG");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_WT");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NUM");
		bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM03";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "cm_2000mb_rcv";
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_THICK"] = tmmsm01["MAT_THICK"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_LEN"] = tmmsm01["MAT_LEN"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_THICK"] = tmmsm01["MAT_ACT_THICK"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_LEN"] = tmmsm01["MAT_ACT_LEN"];
		//bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_INNER_DIA"] = tmmsm01.MAT_ACT_INNER_DIA;
		//bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_OUTER_DIA"] = tmmsm01.MAT_ACT_OUTER_DIA;
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"] = tmmsm01["MAT_ACT_WT"];
		bcls_rec->Tables["MM0099"].Rows[0]["MEASURE_WT_FLAG"] = tmmsm01["MEASURE_WT_FLAG"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_WT"] = tmmsm01["MAT_WT"];
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NUM"] = tmmsm01["MAT_NUM"];
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
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


