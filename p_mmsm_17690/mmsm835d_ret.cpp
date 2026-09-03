/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   songwei
Version:    1.0
Date:
Description: 原料模板画面维护
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



/* ***** 静态函数申明 ***** */
int f_mmsm_21c004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(mmsm835d_ret)

int f_mmsm835d_ret(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int blkNum = 0;
	int doFlag = 0;
	CString s_formname = "";
	CString v_proc_div = "";
	CString sqlstr = "";
	CString sqlstr_temp = "";
	CString v_table_name = "";//表名称。
	CString v_mat_code = "";
	CString v_mat_id = "";
	CString v_dev_code = "";
	CString v_mat_name = "";
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");

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

		CModel tmmsm89("TMMSM89");
		Log::Info("", __FUNCTION__, "v_table_name =[{0}]", v_table_name);
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm89.Reset();
			tmmsm89.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			//发资源电文
		
			Log::Info("", __FUNCTION__, "RECEIVE_DATA_TIME =[{0}]", tmmsm89["RECEIVE_DATA_TIME"].ToString());
			bcls_rec->Tables["MMLCSND"].Rows.Clear();
			bcls_rec->Tables["MMLCSND"].Rows.Add();
			bcls_rec->Tables["MMLCSND"].Rows[0]["DEAL_FLAG"] = "D";
			bcls_rec->Tables["MMLCSND"].Rows[0]["TC_NO"] = "21C004";
			bcls_rec->Tables["MMLCSND"].Rows[0]["WORK_SEQ_NO"] = tmmsm89["SERIAL_NUMBER"]; //实绩流水号
			bcls_rec->Tables["MMLCSND"].Rows[0]["FACTORY_CODE"] = "6240"; //工厂代码
			bcls_rec->Tables["MMLCSND"].Rows[0]["DST_STOCK_CODE"] = "6241"; //目的库区代码
			bcls_rec->Tables["MMLCSND"].Rows[0]["WORK_DATE"] = tmmsm89["RECEIVE_DATA_TIME"]; //作业日期
			bcls_rec->Tables["MMLCSND"].Rows[0]["BASKET_NO"] = tmmsm89["BUNKER_NO"]; //料篮号
			bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CODE"] = tmmsm89["MAT_CODE"]; //物料代码
			bcls_rec->Tables["MMLCSND"].Rows[0]["MAT_CNAME"] = tmmsm89["MAT_NAME"]; //物料名称
			bcls_rec->Tables["MMLCSND"].Rows[0]["SRC_STOCK_CODE"] = "6062"; // 源库区代码
			bcls_rec->Tables["MMLCSND"].Rows[0]["SRC_STOCK_PLACE"] = tmmsm89["BUNKER_NO_ORIGINAL"]; //源库位代码
			bcls_rec->Tables["MMLCSND"].Rows[0]["BUY_ORDER_NO"] = tmmsm89["MISSING_NO"]; //采购订单号
			bcls_rec->Tables["MMLCSND"].Rows[0]["NET_WGT"] = tmmsm89["STOCK_WT"];  //净重
			bcls_rec->Tables["MMLCSND"].Rows[0]["WEIGH_TIME"] = tmmsm89["RECEIVE_DATA_TIME"]; //称量时刻	

			doFlag = f_mmsm_21c004_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_21c004_snd失败-------");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//BACK_CODE_1=1 代表历史数据冲销
			tmmsm89["BACK_CODE_1"] = "1";
			tmmsm89.Update("BACK_CODE_1", "RESUME_SEQ_NO");

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
