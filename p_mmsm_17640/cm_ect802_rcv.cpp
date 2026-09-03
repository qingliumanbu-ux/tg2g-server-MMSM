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
///
///连铸电子记录
///初判数据
///单独存表-并更新主档表，将连铸初判数据更新进去
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);		//物料跟踪函数

int f_mmsm_210046_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//材料等级发送
BM2F_ENTERACE_TELE(cm_ect802_rcv)
int f_cm_ect802_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 业务变量 */
	CString v_resume_seq_no = "";//序号
	int tmmsm01_count = 0;
	/* 实体类定义 */
	CModel tmmsm3f("TMMSM3F");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	
	try
	{

		if (bcls_rec->Tables.IndexOf("MM0099") < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
			bcls_rec->Tables["MM0099"].Rows.Clear();
		}

		tmmsm3f["ID"] = bcls_rec->Tables[0].Rows[0]["id"].ToDecimal();
		tmmsm3f["STRAND_NO"] = bcls_rec->Tables[0].Rows[0]["strand_number"].ToString();
		tmmsm3f["SLAB_NO"] = bcls_rec->Tables[0].Rows[0]["slab_name"].ToString();
		tmmsm3f["CK_RESULT"] = bcls_rec->Tables[0].Rows[0]["expect_result"].ToString();
		tmmsm3f["TIME_STAMPS"] = bcls_rec->Tables[0].Rows[0]["timestamp"].ToString();

		if (tmmsm3f["SLAB_NO"].ToString().Trim() != "")
		{
			tmmsm3f["MAT_NO"] = tmmsm3f["SLAB_NO"].ToString().SubstringNE(0, 8) + tmmsm3f["SLAB_NO"].ToString().SubstringNE(15, 2);
		}

		tmmsm01["MAT_NO"] = tmmsm3f["MAT_NO"];
		tmmsm01_count = tmmsm01.QueryCount("MAT_NO");
		if (tmmsm01_count > 0)
		{
			tmmsm3f["USE_LOGO"] = "1";
		}
		else
		{
			tmmsm3f["USE_LOGO"] = "0";
		}

		doFlag = f_mm0011("TMMSM3F_SEQ", 8, v_resume_seq_no, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm3f["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
		tmmsm3f.TrimOrBlank();
		tmmsm3f.Insert();

		//调用事件  只改标记 在反馈里集中处理
		bcls_rec->Tables["MM0099"].Rows.Add();
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM3G";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "cm_ect802_rcv";
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm3f["MAT_NO"];
		bcls_rec->Tables["MM0099"].Rows[0]["CASTING_PRE_JUDGMENT"] = tmmsm3f["CK_RESULT"];
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "连铸初判数据更新";

		//只有01表才处理
		if (tmmsm01_count > 0)
		{
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		
		//2026.08.18 接收材料等级后发送产销
		EIClass snd_210046;
		snd_210046.Tables[0].set_TableName("210046");
		snd_210046.Tables[0].Columns.Add(tmmsm3f);
		tmmsm3f.MergeTo(snd_210046.Tables[0]);

		doFlag = f_mmsm_210046_snd(&snd_210046, bcls_ret, conn);
		if (doFlag < 0)
		{
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


