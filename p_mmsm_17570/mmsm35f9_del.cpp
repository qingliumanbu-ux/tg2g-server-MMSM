/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-08-13
Version:1.0
Description: 炼钢板坯分段撤销
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/





/* ***** 静态函数申明 ***** */

int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_210034_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
/*<remark>=========================================================
/// <summary>
/// 炼钢板坯分段实绩
/// <para>
/// 炼钢分切撤销
/// </para>
/// </summary>
/// <param name="tmmsm35">分切撤销</param>
///
///分切表和主档表子坯数量要保持一致
///分切表和主档表子坯重量要保持一致
///物流状态不为2，调拨状态不为1
///35表要删除子坯数据，35mp表要删除母坯数据
///
///
///
///发送分切电文，标记为D，撤销
///发送切废电文，标记为D，撤销
///
///
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm35f9_del)

int f_mmsm35f9_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CDecimal cutNum = 0;
	CDecimal matTheoryWt = 0;
	int blkNum = 0;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal tmmsm01_mpcount = 0;//01表母坯数量
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_resume_seq_no = "";
	CModel tmmsm35mp("TMMSM35MP");//母坯表
	CModel tmmsm35("TMMSM35");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm3e("TMMSM3E");//反馈表
	CModel tmmsm01_zp("TMMSM01");//子坯数据
	CModel tmmsm96("TMMSM96");

	CDbCommand cmd_inq(conn);

	try
	{


		if (bcls_rec->Tables.Contains("MM0099") == false)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["MM0099"].Rows.Add();
		}

		EIClass bcls_rec_TMMSM35;//获取母坯的全部子坯信息
		bcls_rec_TMMSM35.Tables[0].set_TableName("TMMSM35");
		bcls_rec_TMMSM35.Tables[0].Columns.Add(tmmsm35);
		

		EIClass bcls_rec_210034;//发送L4二切实绩电文
		bcls_rec_210034.Tables[0].set_TableName("210034");
		bcls_rec_210034.Tables[0].Columns.Add(tmmsm01);
		bcls_rec_210034.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");


		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//获取35表母坯下的所有子坯数据
		sqlstr = "SELECT * FROM TMMSM35 WHERE IN_MAT_NO = '"+ tmmsm01["IN_MAT_NO"].ToString() +"'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_rec_TMMSM35.Tables[0]);
		cmd_inq.Close();

		tmmsm01_mpcount = tmmsm01.QueryCount("IN_MAT_NO");

		if (tmmsm01_mpcount != bcls_rec_TMMSM35.Tables[0].Rows.get_Count())
		{
			CFormattable arguments[] = { tmmsm01["IN_MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "母坯{0} 在主档表与分切表子坯数量不一致,不允许分切撤销处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int i = 0; i < bcls_rec_TMMSM35.Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01_zp.Reset();
			tmmsm01_zp["MAT_NO"] = bcls_rec_TMMSM35.Tables[0].Rows[i]["MAT_NO"].ToString();
			tmmsm01_zp.Query("MAT_NO");

			if (tmmsm01_zp["MAT_ACT_WT"].ToDecimal() != bcls_rec_TMMSM35.Tables[0].Rows[i]["MAT_WT"].ToDecimal())
			{
				if (tmmsm01_mpcount != bcls_rec_TMMSM35.Tables[0].Rows.get_Count())
				{
					CFormattable arguments[] = { tmmsm01_zp["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "子坯{0} 在主档表与分切表重量不一致,不允许分切撤销处理", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			/*if (tmmsm01_zp["LOGISTICS_STATUS"].ToString() == "2")
			{
				CFormattable arguments[] = { tmmsm01_zp["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "子坯{0} 当前物流状态为装车开始,不允许分切撤销处理", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}*/

			if (tmmsm01_zp["C_STATESIGN"].ToString() == "1")
			{
				CFormattable arguments[] = { tmmsm01_zp["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "子坯{0} 当前调拨状态为调拨开始,不允许分切撤销处理", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//调用事件  只改标记 在反馈里集中处理
			//bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM3F";
			//bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
			//bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
			//bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm35f9_del";
			//bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01["IN_MAT_NO"];
			//bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = "W";
			//bcls_rec->Tables["MM0099"].Rows[0]["DIV_FLAG"] = "1";

			//tmmsm3e.MergeFrom(bcls_rec->Tables["MM0099"].Rows[0]);
			//tmmsm3e["RECEIVE_BACK_STATUS"] = "W";//等待反馈
			//tmmsm3e["PROD_TIME"] = datetime;
			//tmmsm3e["REMARK"] = "分切删除处理等待反馈";
			//doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
			//if (doFlag < 0)
			//{
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			//tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
			//tmmsm3e.Insert();

			//doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			//if (doFlag < 0)
			//{
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}

			bcls_rec->Tables["MM0099"].Rows.Clear();
			tmmsm96.Reset();
			tmmsm96.CopyFrom(tmmsm01_zp);
			tmmsm96["EVENT_ID"] = "MM3F";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "mmsm35f9_del";
			tmmsm96["RCV_MAT_FLAG"] = "W";
			tmmsm96["DIV_FLAG"] = "1";
			tmmsm96["EVENT_DESC"] = "材料分切子坯删除";
			tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			////分切表数据删除
			//tmmsm35["MAT_NO"] = tmmsm01_zp["MAT_NO"];
			//tmmsm35.Delete("MAT_NO");


			bcls_rec_210034.Tables[0].Rows.Add();
			bcls_rec_210034.Tables[0].Rows[i].Merge(tmmsm01_zp);
			bcls_rec_210034.Tables[0].Rows[i]["MAT_NO"] = tmmsm01_zp["MAT_NO"];
			bcls_rec_210034.Tables[0].Rows[i]["DEAL_FLAG"] = "D";//删除

			

		}


		

		//调用事件，将母坯从历史档拉回
	/*  bcls_rec->Tables["MM0099"].Rows.Clear();
		tmmsm96.Reset();
		tmmsm96["MAT_NO"] = tmmsm01["IN_MAT_NO"];
		tmmsm96["EVENT_ID"] = "MM19";
		tmmsm96["EVENT_LINE_TYPE"] = "SM";
		tmmsm96["SYSTEM_ID"] = "MMSM";
		tmmsm96["FUNC_ID"] = "mmsm35f9_del";
		tmmsm96["EVENT_DESC"] = "材料分切撤销母坯拉回";
		tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		

		/********   太钢定制 发送L4电文 二切实绩   ***********/
		if (bcls_rec_210034.Tables[0].Rows.get_Count()>0)
		{
			doFlag = f_mmsm_210034_snd(&bcls_rec_210034, bcls_ret, conn);
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
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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
