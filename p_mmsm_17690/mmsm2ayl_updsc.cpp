/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-06-01 17:13:56
Description: 原辅料主信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */
int f_mmsm_21c005_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_21b006_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm2ayl_updsc)

int f_mmsm2ayl_updsc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString deal_flag = "";
	int		TotalRecordCount = 0;
	CString datetime("");
	CString datetime1 = " ";
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	datetime1 = datetime.Substring(0,8);
	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm85("TMMSM85");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm60("TMMSM60");
	CModel tmmsm50("TMMSM50");
	CModel tmmsm2ayl("TMMSM2A_YL");
	CModel tmmsm2ayl0("TMMSM2A_YL");
	CModel tmmsm2ayl1("TMMSM2A_YL1");

	CDbCommand cmd_inq(conn);

	try
	{
		try{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		deal_flag = bcls_rec->Tables[1].Rows[0]["DEAL_FLAG"].ToString();

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			/*tmmsm2ayl.Reset();
			tmmsm2ayl1.Reset();
			tmmsm2ayl0.Reset();
			tmmsm50.Reset();*/

			tmmsm2ayl.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm2ayl0.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm2ayl.Print();

			tmmsm2ayl.Query("PROC_NO,PROC_COUNT,PROD_SEQ_NO,MAT_CODE,SEQ_NO_2A");
			if (deal_flag=="I")
			{
				if (tmmsm2ayl["REMARK_4"].ToString() == "1")
				{
					sprintf(s.msg, "消耗数据已发送请重新选择！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			
			

			CString SeqNo = " ";
			CString missing_no = " ";

			sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_XH.NEXTVAL),7 ,'0') FROM DUAL ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				SeqNo = cmd_inq.GetString(1).Trim();
			}
			cmd_inq.Close();
			missing_no = "TLN" + datetime1 + SeqNo;
			tmmsm2ayl["REMARK_1"] = missing_no;
			if (deal_flag=="I")
			{
				tmmsm2ayl["REMARK_4"] = "1";
			}
			else
			{
				tmmsm2ayl["REMARK_4"] = "0";
			}
			
			tmmsm2ayl.Update("REMARK_4,REMARK_1", "PROC_NO,PROC_COUNT,SEQ_NO_2A,PROD_SEQ_NO,MAT_CODE");
			tmmsm2ayl1.CopyFrom(tmmsm2ayl);

			tmmsm2ayl1.TrimOrBlank();
			tmmsm2ayl1["DEVO_TIME"] = datetime;
			tmmsm2ayl1["REC_CREATOR"] = s.userid;   //记录创建责任者
			tmmsm2ayl1["REC_CREATE_TIME"] = datetime;   //记录创建时刻
			//tmmsm2ayl1["REMARK_4"] = "1";

			tmmsm2ayl1.Insert();

			tmmsm60["MAT_CODE"] = tmmsm2ayl0["MAT_CODE"];
			tmmsm60["BUNKER_NO"] = tmmsm2ayl0["STK_NO"];
			/*tmmsm60.Query("BUNKER_NO");*/

			//发送消耗-资源/铁区
			blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MMLCSND");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("PROC_NO"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "PROC_NO");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("HEAT_NO"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "HEAT_NO");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STK_NO"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "STK_NO");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("LOT_NO"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "LOT_NO");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CODE");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SEQ_NO_2A"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SEQ_NO_2A");
			}
			if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_NO"))
			{
				bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WEIGH_NO");
			}
			bcls_rec->Tables["MMLCSND"].Rows.Clear();
			bcls_rec->Tables["MMLCSND"].Rows.Add();
			//给资源/铁区发消耗实绩
			//SYSTEM_ID_MAT 物料来源系统   资源-C 铁区-B
			bcls_rec->Tables["MMLCSND"].Rows[0]["HEAT_NO"] = tmmsm2ayl0["HEAT_NO"];
			bcls_rec->Tables["MMLCSND"].Rows[0]["PROC_NO"] = tmmsm2ayl0["PROC_NO"];
			bcls_rec->Tables["MMLCSND"].Rows[0]["STK_NO"] = tmmsm60["BUNKER_NO"];
			bcls_rec->Tables["MMLCSND"].Rows[0]["LOT_NO"] = tmmsm2ayl0["LOT_NO"];
			bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = tmmsm2ayl0["MAT_CODE"];
			bcls_rec->Tables["MMLCSND"].Rows[0]["SEQ_NO_2A"] = tmmsm2ayl0["SEQ_NO_2A"];
			bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_NO"] = tmmsm2ayl0["WEIGH_NO"];
			bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = deal_flag;
			tmmsm50["MAT_CODE"] = tmmsm2a["MAT_CODE"];
			tmmsm50.Query("MAT_CODE");
			if (tmmsm50["SYSTEM_ID_MAT"].ToString() == "B")
			{
				bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21B006";

				doFlag = f_mmsm_21b006_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21b006_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else
			{
				bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C005";
				doFlag = f_mmsm_21c005_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c005_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
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
