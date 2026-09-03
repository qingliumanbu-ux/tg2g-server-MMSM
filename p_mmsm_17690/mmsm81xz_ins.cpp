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

// service入口
BM2F_ENTERACE(mmsm81xz_ins)

int f_mmsm81xz_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString cs_receive_data_time_from = "";
	CString cs_receive_data_time_to = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString datetime1("");
	CString datetime("");
	datetime1 = CDateTime::Today().ToString("yyyyMMdd");
	datetime1 = datetime1.Substring(2, 6);

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel tmmsm81("TMMSM81_S");
	CModel tmmsm81s("TMMSM81_S");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm50("TMMSM50");

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

		tmmsm81.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm81s["WEIGH_NO"] = tmmsm81["WEIGH_NO"];
		if (tmmsm81s.QueryCount("WEIGH_NO")==1)
		{
			sprintf(s.msg, "计量单号已存在请重新生成");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//20250206wcm
		tmmsm50["MAT_CODE"] = tmmsm81["MAT_CODE"];
		if (tmmsm50.QueryCount("MAT_CODE") == 1)
		{
			CString mat_code = "";
			mat_code = tmmsm50["MAT_CODE"];
			CString mat_type = Db::QueryCString("select mat_type from tmmsm50 where mat_code='" + mat_code + "'");
			Log::Trace(" ", __FUNCTION__, "mat_type = [{0}]", mat_type);
			if (mat_type.Trim() == "")
			{
				strcpy(s.msg, "物料编码" + mat_code + "的物料类型不能为空,请先配置!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		tmmsm81.TrimOrBlank();
		if (tmmsm81["WEIGH_NO"].ToString().Trim()=="")
		{
			datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

			CString  dh = "S2N" + datetime + EPGetNextSeq("SQ_JLYLID", conn);
			tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
			tmmsm81["RECEIVE_DATA_TIME"] = datetime;
			//tmmsm81["TIME_INSTOCK"] = datetime;
			tmmsm81["WEIGH_NO"] = dh;
			tmmsm81["REC_CREATOR"] = s.userid;   //记录创建责任者
			tmmsm81["REC_CREATE_TIME"] = datetime;   //记录创建时刻
			tmmsm81["WT_DATE_TIME"] = datetime;   //称重时刻
			tmmsm81["DST_STOCK_CODE"] = "6241";
			tmmsm81["SRC_STOCK_CODE"] = "6241";
			// 标记确认时那个画面新增的数据 便于查询 5 自循环物料收货画面
			tmmsm81["MARK_POS_CODE"] = "5";	
			tmmsm81["FORM_EDIT_FLAG"] = "0";
			tmmsm50["MAT_CODE"] = tmmsm81["MAT_CODE"];
			/*CString mat_code_lot_no = "";
			if (tmmsm50.QueryCount("MAT_CODE") == 1)
			{
				tmmsm50.Query("MAT_CODE");

				mat_code_lot_no = tmmsm50["MAT_CODE_L2"].ToString() + "@" + tmmsm50["LOT_NO"].ToString();
			}*/
			tmmsm81.Insert();
		}
		else
		{
			datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
			/*CString  dh = "S" + datetime + EPGetNextSeq("SQ_JLYLID", conn);*/
			tmmsm81["STOCK_WT"] = tmmsm81["NET_WT"];
			//tmmsm81["WEIGH_NO"] = dh;
			tmmsm81["RECEIVE_DATA_TIME"] = datetime;
			tmmsm81["DST_STOCK_CODE"] = "6241";
			tmmsm81["SRC_STOCK_CODE"] = "6241";
			tmmsm81["WT_DATE_TIME"] = datetime;   //称重时刻
			//tmmsm81["TIME_INSTOCK"] = datetime;
			tmmsm81["REC_CREATOR"] = s.userid;   //记录创建责任者
			tmmsm81["REC_CREATE_TIME"] = datetime;   //记录创建时刻
			// 标记确认时那个画面新增的数据 便于查询 5 自循环物料收货画面
			tmmsm81["MARK_POS_CODE"] = "5";
			//tmmsm81.Update("STOCK_WT,NET_WT","WEIGH_NO");
			tmmsm81["FORM_EDIT_FLAG"] = "0";
			tmmsm50["MAT_CODE"] = tmmsm81["MAT_CODE"];
			/*CString mat_code_lot_no = "";
			if (tmmsm50.QueryCount("MAT_CODE") == 1)
			{
				tmmsm50.Query("MAT_CODE");

				mat_code_lot_no = tmmsm50["MAT_CODE_L2"].ToString() + "@" + tmmsm50["LOT_NO"].ToString();
			}*/
			tmmsm81.Insert();
		}
		
		
		//--------------------------------
		//获取传入参数
		/*tmmsm85.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Info("", __FUNCTION__, "BUNKER_NO =[{0}]", tmmsm85["BUNKER_NO"].ToString());
		Log::Info("", __FUNCTION__, "MAT_NAME =[{0}]", tmmsm85["MAT_NAME"].ToString());
		Log::Info("", __FUNCTION__, "MAT_CODE =[{0}]", tmmsm85["MAT_CODE"].ToString());*/

		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:				// MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:
		//	sqlstr = "   SELECT HEAT_NO,MAT_CODE,PROD_DATE,DEV_CODE,SM_PLAN_NOL2,MAX(MAT_NAME) MAT_NAME,SUM(DEVO_WT) DEVO_WT FROM TMMSM2A WHERE 1=1 "
		//		;
		//	if (tmmsm2a["PROD_DATE"].ToString().Trim() != "")
		//	{
		//		sqlstr += " and PROD_DATE = @PROD_DATE";
		//	}

		//	if (tmmsm2a["HEAT_NO"].ToString().Trim() != "")
		//	{
		//		sqlstr += " and HEAT_NO = @HEAT_NO";
		//	}

		//	if (tmmsm2a["DEV_CODE"].ToString().Trim() != "")
		//	{
		//		sqlstr += " and DEV_CODE = @DEV_CODE";
		//	}

		//	sqlstr += " and PROD_DATE = '20240223'";

		//	sqlstr_temp = " GROUP BY HEAT_NO,MAT_CODE,PROD_DATE,DEV_CODE,SM_PLAN_NOL2";
		//	sqlstr = sqlstr + sqlstr_temp;
		//}
		//Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		//cmd_inq.Close();


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
