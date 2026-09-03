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
/// 连铸流数据
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
#if  defined _SYS_PES
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号


BM2F_ENTERACE_TELE(cm_e2t8c1_rcv)

int f_cm_e2t8c1_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CModel tmmsm31a("TMMSM31A");
	CString v_proc_div = " ";
	CString v_resume_seq_no = " ";

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString seq("");
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");


	try
	{
		for (size_t i = 0; i < bcls_rec->Tables["INT_MES_STRAND_DATA"].Rows.get_Count(); i++)
		{
			//工号
			tmmsm31a["DEV_CODE"] = bcls_rec->Tables["INT_MES_STRAND_DATA"].Rows[0]["AGGREGATE_NAME"].ToString();
			//流号
			tmmsm31a["STRAND_NO"] = bcls_rec->Tables["INT_MES_STRAND_DATA"].Rows[0]["STRAND_NUMBER"].ToString();
			//总浇铸长度
			tmmsm31a["TOTAL_CAST_LEN"] = bcls_rec->Tables["INT_MES_STRAND_DATA"].Rows[0]["TOTAL_CASTING_LENGTH"].ToString();
			//浇铸速率
			tmmsm31a["CASTING_SPEED"] = bcls_rec->Tables["INT_MES_STRAND_DATA"].Rows[0]["CASTING_SPEED"].ToString();
			//状态
			tmmsm31a["STATUS"] = bcls_rec->Tables["INT_MES_STRAND_DATA"].Rows[0]["STATUS"].ToString();
			tmmsm31a["REC_CREATOR"] = s.userid;
			tmmsm31a["REC_CREATE_TIME"] = datetime;
			tmmsm31a["START_TIME"] = datetime;
			doFlag = f_mm0011("TMMSM31A_SEQ", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", "dateNow", "dateNow = {0}", dateNow);
			Log::Trace("", "v_resume_seq_no", "v_resume_seq_no = {0}", v_resume_seq_no);
			seq = dateNow + v_resume_seq_no;
			tmmsm31a["PROD_SEQ_NO"] = seq;
			tmmsm31a.TrimOrBlank();
			tmmsm31a.Insert();
#if defined(_SYS_PES)
			//调用发送电文
			blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MMSMSND");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("STRAND_NO"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "STRAND_NO");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("HEAT_NO"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "HEAT_NO");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("REC_CREATE_TIME"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "REC_CREATE_TIME");
			}
			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			bcls_rec->Tables["MMSMSND"].Rows.Add();
			bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM31A";
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] =v_proc_div;
			bcls_rec->Tables["MMSMSND"].Rows[0]["STRAND_NO"] =tmmsm31a["STRAND_NO"];
			bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tmmsm31a["HEAT_NO"];
			bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATE_TIME"] = tmmsm31a["REC_CREATE_TIME"];

			//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
#endif
		}
		//tmmsm31a.MergeFrom(bcls_rec->Tables["INT_MES_STRAND_DATA"].Rows[0]);
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


