/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   夏梦影
Version:    1.0
Date:     2023-09-21 17:13:56
Description: 高位料仓查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsm84v_inq)

int f_mmsm84v_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString sqlstr1 = "";
	CString sqlstr_count1 = "";
	CString sqlstr_temp1 = "";
	CString type_code = "";
	CString mat_code = "";
	CString mat_name = "";
	CString weigh_no = "";
	CString bunker_no = "";
	CString mat_rcv_time = "";
	CString mat_rcv_time_to = "";
	CDecimal seq_no = 0;
	int TotalRecordCount = 0;
	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	try
	{

		if (bcls_rec->Tables[0].Columns.Contains("TYPE_CODE"))
			type_code = bcls_rec->Tables[0].Rows[0]["TYPE_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME"))
			mat_name = bcls_rec->Tables[0].Rows[0]["MAT_NAME"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "type_code=[{0}],mat_code=[{1}],mat_name= [{2}]", type_code, mat_code, mat_name);
		if (type_code == "1")  //物料编码的库存量
		{
			sqlstr = " SELECT A.MAT_CODE ,MAX(B.MAT_NAME) MAT_NAME,SUM(STOCK_WT) STOCK_WT"
				" FROM TMMSM60 A "
				" LEFT JOIN TMMSM50 B ON A.MAT_CODE = B.MAT_CODE"
				" WHERE 1=1"
				;
			if (mat_code.Trim() != "")
			{
				sqlstr = sqlstr + " and A.mat_code like '%'||@mat_code||'%' ";
			}
			if (mat_name.Trim() != "")
			{
				sqlstr = sqlstr + " and B.mat_name like '%'||@mat_name||'%' ";
			}
			sqlstr = sqlstr + " GROUP BY A.MAT_CODE ORDER BY A.MAT_CODE";

			Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_code", mat_code);
			cmd_inq.Parameters.Set("mat_name", mat_name);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		if (type_code == "2")  //物料编码对应的库存	(料仓号、计量单号、物料编码)
		{
			mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();

			sqlstr = " select mat_code,mat_name,WEIGH_NO, BUNKER_NO, STOCK_WT"
				" ,nvl((select MAT_RCV_TIME from tmmsm81 t2 where t2.WEIGH_NO = t1.WEIGH_NO and rownum=1),' ') TIME_INSTOCK"
				" from ("
				" select mat_code,mat_name,WEIGH_NO, BUNKER_NO,sum(STOCK_WT) STOCK_WT"					
				" from tmmsm85 "
				" where 1=1 "
				" and MAT_CODE = @mat_code"
				" group by mat_code,mat_name,WEIGH_NO, BUNKER_NO"
				") t1"
				" ORDER BY WEIGH_NO"
				;
			Log::Trace("", __FUNCTION__, "mat_code = [{0}],sqlstr=[{0}]", mat_code,sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_code", mat_code);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		if (type_code == "3")  //物料编码、计量单号对应的库存
		{
			mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
			weigh_no = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString().Trim();

			sqlstr = " select * "
				" from tmmsm85"
				" where 1=1 "
				" and MAT_CODE = @mat_code"
				" and WEIGH_NO = @weigh_no "
				" ORDER BY BUNKER_NO"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_code", mat_code);
			cmd_inq.Parameters.Set("weigh_no", weigh_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}
		if (type_code == "4")  //物料编码、计量单号、料仓号、序号对应的库存
		{
			mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
			weigh_no = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString().Trim();
			bunker_no = bcls_rec->Tables[0].Rows[0]["BUNKER_NO"].ToString().Trim();
			seq_no = bcls_rec->Tables[0].Rows[0]["SEQ_NO"].ToDecimal();

			sqlstr = " select * "
				" from tmmsm85"
				" where 1=1 "
				" and MAT_CODE = @mat_code"
				" and WEIGH_NO = @weigh_no "
				" and BUNKER_NO = @bunker_no"
				" AND SEQ_NO = @seq_no "
				" ORDER BY BUNKER_NO"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_code", mat_code);
			cmd_inq.Parameters.Set("weigh_no", weigh_no);
			cmd_inq.Parameters.Set("bunker_no", bunker_no);
			cmd_inq.Parameters.Set("seq_no", seq_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}

		if (type_code == "5")  //物料编码、计量单号、时间
		{
			mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
			weigh_no = bcls_rec->Tables[0].Rows[0]["WEIGH_NO"].ToString().Trim();
			mat_rcv_time = bcls_rec->Tables[0].Rows[0]["MAT_RCV_TIME"].ToString();
			mat_rcv_time_to = bcls_rec->Tables[0].Rows[0]["MAT_RCV_TIME_TO"].ToString();

			sqlstr = " select mat_code,mat_name,WEIGH_NO, BUNKER_NO,time_instock,sum(STOCK_WT) STOCK_WT "
				" from tmmsm85"
				" where 1=1 "
				" and MAT_CODE = @mat_code"
				;
			if (weigh_no.Trim() != "")
				sqlstr = sqlstr + " and WEIGH_NO = @weigh_no ";
			if (mat_rcv_time_to.Trim() != "")
				sqlstr = sqlstr + " and time_instock <= @mat_rcv_time_to ";
			if (mat_rcv_time.Trim() != "")
				sqlstr = sqlstr + " and time_instock >= @mat_rcv_time ";
				
				sqlstr = sqlstr + " group by mat_code,mat_name,WEIGH_NO, BUNKER_NO,time_instock ORDER BY BUNKER_NO"
				;
				Log::Trace("", __FUNCTION__, "mat_code = [{0}],weigh_no=[{1}],mat_rcv_time=[{2}],mat_rcv_time_to = [{3}],sqlstr=[{4}]", mat_code, weigh_no, mat_rcv_time.SubstringNE(0, 8), mat_rcv_time_to.SubstringNE(0, 8) + "235959",sqlstr);

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_code", mat_code);
			cmd_inq.Parameters.Set("weigh_no", weigh_no);
			cmd_inq.Parameters.Set("bunker_no", bunker_no);
			cmd_inq.Parameters.Set("mat_rcv_time_to", mat_rcv_time_to.SubstringNE(0,8)+"235959");
			cmd_inq.Parameters.Set("mat_rcv_time", mat_rcv_time.SubstringNE(0, 8));
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
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
