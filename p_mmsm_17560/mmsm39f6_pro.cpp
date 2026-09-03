/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:lizhen
Date:2025-08-05
Version:1.0
Description: 切废记录确认
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */
int f_mmsm_210044_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mm0011(CString SeqName, CDecimal SeqLen, CString& SeqNo, CDbConnection* conn); // 获取流水号
int f_mmsm_get_density(CString ST_NO, CDecimal& MAT_DENSITY, CDbConnection* conn);	 // 通过钢种计算密度
// 修改板坯主档信息

/*<remark>=========================================================
/// <summary>
/// 炼钢板坯切废管理
/// <para>
/// 炼钢板坯切废管理
/// </para>
///		将处理失败的记录，发送实绩
///
/// </summary>
/// <param name="">炼钢板坯切废管理</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm39f6_pro)

int f_mmsm39f6_pro(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_mat_no;
	CString v_resume_seq_no = ""; // 序号

	CModel tmmsm39("TMMSM39");
	CModel tmmsm39_1("TMMSM39_1");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm96_1("TMMSM96");
	CModel tmmsm3e("TMMSM3E");

	CDbCommand cmd_inq(conn);

	try
	{
		EIClass bcls_rec_210044; // 发送L4二切实绩电文
		bcls_rec_210044.Tables[0].set_TableName("210044");
		bcls_rec_210044.Tables[0].Columns.Add(tmmsm39);
		bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
		bcls_rec_210044.Tables[0].Rows.Add();

		EIClass bcls_MM0099; // 记录改切实绩
		bcls_MM0099.Tables[0].set_TableName("MM0099");
		bcls_MM0099.Tables[0].Columns.Add(tmmsm96);
		bcls_MM0099.Tables[0].Rows.Add();

		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			tmmsm01.Query();
		}

		tmmsm39.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm39.Query("RESUME_SEQ_NO");

		if (tmmsm01["C_STATESIGN"].ToString().Trim() != "0" && tmmsm01["C_STATESIGN"].ToString().Trim() != "" && tmmsm01["C_STATESIGN"].ToString().Trim() != "6")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已不在现场，不允许改切处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		// 当其他反馈标记为空且收货标记不为S时，表示当时未做其他操作，且未收货
		// 当其他标记有值，且收货标记不为S，表示等待反馈中，不允许做该操作
		if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S" && tmmsm01["FINISH_FLAG"].ToString().Trim() == "" && tmmsm01["MEND_FEEDBACK_FLAG"].ToString().Trim() == "" && tmmsm01["DIV_FLAG"].ToString().Trim() == "")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}未收货,不允许分段处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() == "E")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}收货失败,请先在收货界面处理！", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm01["DIV_FLAG"].ToString().Trim() == "1" && tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做中板改切，当前状态为等待制造管理系统反馈,暂不允许再进行该操作", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["MEND_FEEDBACK_FLAG"].ToString().Trim() == "1" && tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做修磨，当前状态为等待制造管理系统反馈,暂不允许再进行该操作", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["FINISH_FLAG"].ToString().Trim() == "1" && tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做改切，当前状态为等待制造管理系统反馈,暂不允许再进行该操作", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		// 判断一下切前量与当前的01实际重量是否一致，如果不一致，表示已经做了改切或是修磨等其他操作。
		if (tmmsm01["MAT_ACT_WT"].ToDecimal() != tmmsm39["CUT_BEFORE_WT"].ToDecimal())
		{
			strcpy(s.sysmsg, "切前的系统重量与录入的切前重量数据不一致，请查看系统重量在此时是否更改了！");
			strcpy(s.msg, s.sysmsg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm39["CUT_AFTER_LEN"].ToDecimal() == 0)
		{
			strcpy(s.sysmsg, "切后长度不能为0 ！");
			strcpy(s.msg, s.sysmsg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm39["FINISH_FLAG"].ToString() != "E")
		{
			strcpy(s.sysmsg, "只能确认处理失败的记录 ！");
			strcpy(s.msg, s.sysmsg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm39["FINISH_FLAG"] = "1"; // 1 待反馈   3 删除待反馈 9处理成功

		tmmsm39.TrimOrBlank();
		tmmsm39.Update("FINISH_FLAG", "RESUME_SEQ_NO");

		if (tmmsm39["CUT_SCRAP_WT"].ToDecimal() != 0)
		{
			tmmsm39_1.CopyFrom(tmmsm39); // 记录履历
			tmmsm39_1.TrimOrBlank();
			tmmsm39_1.Insert();
		}

		tmmsm96.CopyFrom(tmmsm01);
		tmmsm96_1.CopyFrom(tmmsm01);

		tmmsm96_1["EVENT_ID"] = "MM3K";
		tmmsm96_1["EVENT_LINE_TYPE"] = "SM";
		tmmsm96_1["FUNC_ID"] = "f_mmsm39_proc";
		tmmsm96_1["SYSTEM_ID"] = "MMSM";
		tmmsm96_1["EVENT_DESC"] = "改切记录数据录入";

		bcls_MM0099.Tables[0].Rows[0].Merge(tmmsm96_1);
		if (bcls_MM0099.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&bcls_MM0099, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		// 名义规格与实际规格保持一致
		tmmsm96["MAT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
		tmmsm96["MAT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
		tmmsm96["MAT_THICK"] = tmmsm39["CUT_AFTER_THICK"];

		// 画面切废，规格取切后长宽厚
		tmmsm96["MAT_ACT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
		tmmsm96["MAT_ACT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
		tmmsm96["MAT_ACT_THICK"] = tmmsm39["CUT_AFTER_THICK"];
		tmmsm96["MAT_WT"] = tmmsm39["CUT_AFTER_WT"];	 // 切后重量
		tmmsm96["MAT_ACT_WT"] = tmmsm39["CUT_AFTER_WT"]; // 系统重量即实际重量
		// tmmsm96["REAL_TIME_WT"] = tmmsm39["CUT_AFTER_WT"];//实时重量
		tmmsm96["QUALIFIED_WT"] = tmmsm39["CUT_AFTER_WT"]; // 合格产量

		tmmsm96["EVENT_ID"] = "MM37";
		tmmsm96["EVENT_LINE_TYPE"] = "SM";
		tmmsm96["FUNC_ID"] = "f_mmsm39_proc";
		tmmsm96["SYSTEM_ID"] = "MMSM";
		tmmsm96["EVENT_DESC"] = "钢坯切废实绩";

		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		if (tmmsm39["CUT_SCRAP_WT"].ToDecimal() != 0)
		{
			bcls_rec->Tables["MM0099"].Rows.Add();

			bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
		}

		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{

			bcls_rec->Tables["MM0099"].Rows.Clear();
			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["MAT_NO"] = tmmsm39["MAT_NO"];
			tmmsm96["RCV_MAT_FLAG"] = "W";
			tmmsm96["FINISH_FLAG"] = "1";
			tmmsm96["EVENT_ID"] = "MM3F";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["FUNC_ID"] = "mmsm39f3_pro";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["EVENT_DESC"] = "改切新增处理等待反馈";

			bcls_rec->Tables["MM0099"].Rows.Add();
			bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

			tmmsm3e.CopyFrom(tmmsm96);
			doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;

			tmmsm3e["PROD_TIME"] = datetime;
			tmmsm3e["REMARK"] = "改切新增处理等待反馈";
			tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
			tmmsm3e.Insert();

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm39);

		bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "N"; // 新增

		if ((v_mat_no.Substring(8, 2) == "00" || v_mat_no.Substring(8, 2) == "99" || v_mat_no.Substring(8, 2) == "AA" || v_mat_no.Substring(8, 2) == "ZZ") && (tmmsm39["CUTTING_TYPE"].ToString().Trim() == "8" || tmmsm39["CUTTING_TYPE"].ToString().Trim() == "9") && (tmmsm01["MEND_FLAG"].ToString().Trim() == "" || tmmsm01["MEND_FLAG"].ToString().Trim() == "0"))
		{
		}
		else
		{
			if (tmmsm39["CUT_SCRAP_WT"].ToDecimal() != 0)
			{
				/********   太钢定制 发送L4电文 钢坯切废实绩   ***********/
				doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
	}
	catch (CDbException& ex) // 捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006") /*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1; // 数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex) // 捕获应用错误
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
}
