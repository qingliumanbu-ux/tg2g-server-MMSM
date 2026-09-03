/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      178053
Version:     1.0
Date:        2019-11-22 16:24:30
Description: MMS侧接收炼钢板坯信息同步(炼钢PES->MMS)电文
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"

/*<remark>=========================================================
/// <summary>
/// MMS侧接收炼钢板坯信息同步(炼钢PES->MMS)电文
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
 
  

//外部函数声明
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);		//物料跟踪函数

BM2F_ENTERACE_TELE(cm_2000mz_rcv)

int f_cm_2000mz_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");

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

		EIClass bcls_rec_QM02;//材料表面判定
		bcls_rec_QM02.Tables[0].set_TableName("MM0099");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_MAKER");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_TIME");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "DEFECT_CODE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");

		EIClass bcls_rec_QM17;//材料质量封锁
		bcls_rec_QM17.Tables[0].set_TableName("MM0099");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		//bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_REMARK");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_MAKER");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "REL_TIME");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "HOLD_CAUSE_CODE");
		bcls_rec_QM17.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");

		EIClass bcls_rec_QM18;//材料质量释放
		bcls_rec_QM18.Tables[0].set_TableName("MM0099");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		//bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "HOLD_REMARK");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_MAKER");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_TIME");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "DEFECT_CODE");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");

		/* 获得传入参数 */
		tmmsm96.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm96.TrimOrBlank();

		/* 打印传入参数 */
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm96.MAT_NO		= [{0}]", tmmsm96["MAT_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm96.EVENT_ID		= [{0}]", tmmsm96["EVENT_ID"].ToString());

		/* 检查输入参数合法性 */
		if (tmmsm96["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm96["EVENT_ID"].ToString().Trim() == "")
		{
			strcpy(s.msg, "事件号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 查询该材料是否存在 */
		tmmsm01["MAT_NO"] = tmmsm96["MAT_NO"];
		if (tmmsm01.QueryCount("MAT_NO") <= 0)
		{
			sprintf(s.msg, "材料号[%s]不存在！", (const char*)tmmsm96["MAT_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 调用物料跟踪 */
		tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (tmmsm96["EVENT_ID"].ToString().Trim() == "WMBP"&& tmmsm96["SURFACE_DECIDE_CODE"].ToString().Trim() == "1")
		{
			bcls_rec_QM02.Tables[0].Rows.Add();
			bcls_rec_QM02.Tables[0].Rows[0]["EVENT_ID"] = "QM02";
			bcls_rec_QM02.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_QM02.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec_QM02.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
			bcls_rec_QM02.Tables[0].Rows[0]["MAT_NO"] = tmmsm96["MAT_NO"];
			bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_CODE"] = "1";// 1:合格
			bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_MAKER"] = s.userid;
			bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_TIME"] = datetime;
			bcls_rec_QM02.Tables[0].Rows[0]["DEFECT_CODE"] = " ";
			bcls_rec_QM02.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";

			doFlag = f_mmsm99(&bcls_rec_QM02, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			bcls_rec_QM18.Tables[0].Rows.Add();
			bcls_rec_QM18.Tables[0].Rows[0]["EVENT_ID"] = "QM18";
			bcls_rec_QM18.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_QM18.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec_QM18.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
			bcls_rec_QM18.Tables[0].Rows[0]["MAT_NO"] = tmmsm96["MAT_NO"];
			bcls_rec_QM18.Tables[0].Rows[0]["REL_REMARK"] = "表判合格解封锁";
			bcls_rec_QM18.Tables[0].Rows[0]["REL_MAKER"] = s.userid;
			bcls_rec_QM18.Tables[0].Rows[0]["REL_TIME"] = datetime;
			bcls_rec_QM18.Tables[0].Rows[0]["DEFECT_CODE"] = " ";
			bcls_rec_QM18.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";

			doFlag = f_mmsm99(&bcls_rec_QM18, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else if (tmmsm96["EVENT_ID"].ToString().Trim() == "WMBP" && tmmsm96["SURFACE_DECIDE_CODE"].ToString().Trim() == "2")
		{
			bcls_rec_QM02.Tables[0].Rows.Add();
			bcls_rec_QM02.Tables[0].Rows[0]["EVENT_ID"] = "QM02";
			bcls_rec_QM02.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_QM02.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec_QM02.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
			bcls_rec_QM02.Tables[0].Rows[0]["MAT_NO"] = tmmsm96["MAT_NO"];
			bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_CODE"] = "2";
			bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_MAKER"] = s.userid;
			bcls_rec_QM02.Tables[0].Rows[0]["SURFACE_DECIDE_TIME"] = datetime;
			bcls_rec_QM02.Tables[0].Rows[0]["DEFECT_CODE"] = " ";
			bcls_rec_QM02.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";

			doFlag = f_mmsm99(&bcls_rec_QM02, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			bcls_rec_QM17.Tables[0].Rows.Add();
			bcls_rec_QM17.Tables[0].Rows[0]["EVENT_ID"] = "QM17";
			bcls_rec_QM17.Tables[0].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_QM17.Tables[0].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec_QM17.Tables[0].Rows[0]["FUNC_ID"] = "wmsmsma2_send";
			bcls_rec_QM17.Tables[0].Rows[0]["MAT_NO"] = tmmsm96["MAT_NO"];
			bcls_rec_QM17.Tables[0].Rows[0]["REL_REMARK"] = "表判不合封锁";
			bcls_rec_QM17.Tables[0].Rows[0]["REL_MAKER"] = s.userid;
			bcls_rec_QM17.Tables[0].Rows[0]["REL_TIME"] = datetime;
			bcls_rec_QM17.Tables[0].Rows[0]["HOLD_CAUSE_CODE"] = "WMBP";
			bcls_rec_QM17.Tables[0].Rows[0]["DEFECT_CLASS"] = " ";

			doFlag = f_mmsm99(&bcls_rec_QM17, bcls_ret, conn);
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

