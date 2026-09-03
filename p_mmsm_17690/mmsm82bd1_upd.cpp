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
int f_mmsm_21c004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm89(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_t8e2yy_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
/* ***** 静态函数申明 ***** */

// service入口
BM2F_ENTERACE(mmsm82bd1_upd)

int f_mmsm82bd1_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int   blkNum;
	int doFlag = 0;
	CString sqlstr = "";
	CString sql = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString s_bunker_no = "";
	CString s_bunker_no_original = "";
	CString bunker_no1 = "";
	CString excludeCode = "";
	CString matCode = "";
	int		TotalRecordCount = 0;
	int i_idx = 0;
	int n_idx = 0;
	int d_idx = 0;
	CDecimal cd_stock_wt = 0;
	CDecimal cd_seq_no = 0;
	CDecimal nd_seq_no = 0;
	//系统的分页类信息。
	CPageInfo pageInfo;
	CModel tmmsm60("TMMSM60");
	CModel tmmsm60_O("TMMSM60");
	CModel tmmsm85("TMMSM85");
	CModel tmmsm85_O("TMMSM85");
	CModel tmmsm85_Z("TMMSM85");
	CModel tmmsm85_Z1("TMMSM85");
	CModel tmmsm89("TMMSM89");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass bcls_rec_tmmsm89_log;
	bcls_rec_tmmsm89_log.Tables[0].Columns.Add(tmmsm89);
	EIClass EITable;
	try
	{
		//测试接口 废钢料篮计量信息21C004
		blkNum = bcls_rec->Tables.IndexOf("MMLCSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMLCSND");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DEAL_FLAG"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DEAL_FLAG");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("TC_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "TC_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WORK_SEQ_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WORK_SEQ_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("FACTORY_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "FACTORY_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("DST_STOCK_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "DST_STOCK_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WORK_DATE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WORK_DATE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BASKET_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BASKET_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("MAT_CNAME"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "MAT_CNAME");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SRC_STOCK_CODE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SRC_STOCK_CODE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("SRC_STOCK_PLACE"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "SRC_STOCK_PLACE");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUY_ORDER_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BUY_ORDER_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("NET_WGT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "NET_WGT");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("WEIGH_TIME"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "WEIGH_TIME");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_NO"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BUNKER_NO");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("STOCK_WT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "STOCK_WT");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_GROSS_WT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BUNKER_GROSS_WT");
		}
		if (!bcls_rec->Tables["MMLCSND"].Columns.Contains("BUNKER_DEDUCT_WT"))
		{
			bcls_rec->Tables["MMLCSND"].Columns.Add(DT_STRING, "BUNKER_DEDUCT_WT");
		}
		////料槽料篮配料传资源时排除的物料编码
		//sqlstr =
		//	" SELECT CODE FROM TEP0002 WHERE CODE_CLASS='MMLC01' ";
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteReader();
		//while(cmd_inq.Read())
		//{
		//	matCode = cmd_inq.GetString(1);
		//	excludeCode += matCode + ",";
		//}
		//cmd_inq.Close();
		


		tmmsm60["BUNKER_NO"] = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString();
		
		sqlstr = "SELECT BACK_C2,BACK_C3,BACK_C1 "
			"   FROM tmmsm60 "
			"  WHERE 1=1  "
			" AND  bunker_no = @bunker_no "
			;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("bunker_no", tmmsm60["BUNKER_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			if (cmd_inq.GetString(3) == "0")
			{
				sprintf(s.msg, "该料篮物料未使用无法发送二级!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (cmd_inq.GetString(1) == "1")
			{
				sprintf(s.msg, "该料篮物料已发送到镍板库无法发送二级!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (cmd_inq.GetString(2) == "0")
			{
				sprintf(s.msg, "该料篮物料已发送二级,无法重复发送!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		cmd_inq.Close();

		tmmsm60["UPLOAD_301"] = "1";
		tmmsm60["BACK_C3"] = "0";
		tmmsm60.Update("UPLOAD_301,BACK_C3","BUNKER_NO");


		Log::Trace(" ", __FUNCTION__, "BUNKER_NO=[{0}]", tmmsm60["BUNKER_NO"].ToString());
		//料篮料槽抛送资源的，只抛送F00--F05和排除表开头及排除表的东西
		sqlstr = " select * from tmmsm85 where  MAT_CODE LIKE 'F%' AND substr(MAT_CODE, 1, 3) NOT in ('F06') and mat_code not IN ( SELECT CODE FROM TEP0002 WHERE CODE_CLASS='MMLC01') AND BUNKER_NO=@BUNKER_NO ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm60["BUNKER_NO"].ToString());
		cmd_inq.ExecuteReader();
		Log::Trace(" ", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
		while (cmd_inq.Read())
		{
			tmmsm85_O.Reset();
			cmd_inq.Fetch(tmmsm85_O);

			//按原来的实绩流水号删除20250522
			/*CString SeqNo1 = "";
			CString serial_number = "";
			sqlstr = "  SELECT LPAD(TO_CHAR(MMLC_LS.NEXTVAL),18 ) FROM DUAL ";
			cmd_inq1.SetCommandText(sqlstr);
			cmd_inq1.ExecuteReader();
			if (cmd_inq1.Read())
			{
				SeqNo1 = cmd_inq1.GetString(1).Trim();

			}
			cmd_inq1.Close();

			serial_number = "EG" + SeqNo1;
			Log::Trace(" ", __FUNCTION__, "serial_number =[{0}]", serial_number);
			tmmsm85_O["SERIAL_NUMBER"] = serial_number;*/


			Log::Trace(" ", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm85_O["BUNKER_NO"].ToString());
			Log::Trace(" ", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm85_O["MAT_CODE"].ToString());
			Log::Trace(" ", __FUNCTION__, "WEIGH_NO =[{0}]", tmmsm85_O["WEIGH_NO"].ToString());
			Log::Trace(" ", __FUNCTION__, "SEQ_NO =[{0}]", tmmsm85_O["SEQ_NO"].ToString());
			tmmsm85_O["UPLOAD_301"] = "1";
			tmmsm85_O["UPLOAD_TIME"] = datetime;
			tmmsm85_O.Update("UPLOAD_301,UPLOAD_TIME", "BUNKER_NO,MAT_CODE,WEIGH_NO,SEQ_NO");

			bcls_rec->Tables["MMLCSND"].Rows.Clear();
			bcls_rec->Tables["MMLCSND"].Rows.Add();
			bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "D";
			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C004";
			bcls_rec->Tables["MMLCSND"].Rows[0]["WORK_SEQ_NO"] = tmmsm85_O["SERIAL_NUMBER"]; //实绩流水号
			bcls_rec->Tables["MMLCSND"].Rows[0]["FACTORY_CODE"] = "6240"; //工厂代码
			bcls_rec->Tables["MMLCSND"].Rows[0]["DST_STOCK_CODE"] = "6241"; //目的库区代码
			bcls_rec->Tables["MMLCSND"].Rows[0]["WORK_DATE"] = tmmsm85_O["RECEIVE_DATA_TIME"]; //作业日期
			bcls_rec->Tables["MMLCSND"].Rows[0]["BASKET_NO"] = tmmsm85_O["BUNKER_NO"]; //料篮号
			bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = tmmsm85_O["MAT_CODE"]; //物料代码
			bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CNAME"] = tmmsm85_O["MAT_NAME"]; //物料名称
			bcls_rec->Tables["MMLCSND"].Rows[0]["SRC_STOCK_CODE"] = "6062"; // 源库区代码
			bcls_rec->Tables["MMLCSND"].Rows[0]["SRC_STOCK_PLACE"] = tmmsm85_O["BUNKER_NO_ORIGINAL"]; //源库位代码
			bcls_rec->Tables["MMLCSND"].Rows[0]["BUY_ORDER_NO"] = tmmsm85_O["MISSING_NO"]; //采购订单号
			bcls_rec->Tables["MMLCSND"].Rows[0]["NET_WGT"] = tmmsm85_O["STOCK_WT"];  //净重
			bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_TIME"] = tmmsm85_O["TIME_1"]; //称量时刻	
			bcls_rec->Tables["MMLCSND"].Rows[0]["BUNKER_GROSS_WT"] = tmmsm85_O["STOCK_WT"].ToDecimal() + tmmsm85_O["BUNKER_DEDUCT_WT"].ToDecimal();
			bcls_rec->Tables["MMLCSND"].Rows[0]["BUNKER_DEDUCT_WT"] = tmmsm85_O["BUNKER_DEDUCT_WT"];
			doFlag = f_mmsm_21c004_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c004_snd失败-------");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		
		/*tmmsm60["BACK_C3"] = "1";

		tmmsm60.Update("BACK_C3", "BUNKER_NO");*/
		sqlstr = " SELECT * FROM TMMSM85 WHERE bunker_no = @bunker_no ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BUNKER_NO", tmmsm60["BUNKER_NO"].ToString());
		cmd_inq.ExecuteQuery(EITable.Tables[0]);
		
		if (EITable.Tables[0].Rows.get_Count() > 0)
		{
			for (int i = 0; i < EITable.Tables[0].Rows.get_Count(); i++)
			{
				tmmsm89.Reset();
				tmmsm89.MergeFrom(EITable.Tables[0].Rows[i]);
				tmmsm89["EVENT_CODE"] = "UP302";
				tmmsm89["EVENT_DESC"] = "上传302";
				tmmsm89["EVENT_NAME"] = "料槽料篮上料";
				tmmsm89["REC_CREATOR"] = s.userid;
				tmmsm89["REC_CREATE_TIME"] = datetime;
				tmmsm89["BUNKER_TYPE_ORIGINAL"] = "AODBOX";
				tmmsm89["BUNKER_NAME_ORIGINAL"] = "AOD料槽";
				bcls_rec_tmmsm89_log.Tables[0].Rows.Add();
				bcls_rec_tmmsm89_log.Tables[0].Rows[i].Merge(tmmsm89);
			}
		}
		cmd_inq.Close();
		if (bcls_rec_tmmsm89_log.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm89(&bcls_rec_tmmsm89_log, bcls_ret, conn);
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
