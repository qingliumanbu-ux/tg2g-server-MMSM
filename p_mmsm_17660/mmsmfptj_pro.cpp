/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:
Date:     2023/3/11
Description: 查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmfptj_pro)

int f_mmsmfptj_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString sqlstr_order = "";
	int TotalRecordCount = 0;

	CString table_name = "";
	CString v_heat_no = "";
	CString v_st_no = "";
	CString v_dev_code = "";
	CString v_mat_no = "";
	CString rec_create_time = "";
	CString v_prod_time_from = "";
	CString v_prod_time_to = "";

	CString vapply = "";
	CString vmatno = "";
	CString vstatus = "";
	CString vcarno = "";
	CString v_area = "";
	CString	datetime("");
	CDbCommand cmd_inq(conn);
	CString rec_create_time_1 = "";
	CModel tmmsmfptj("TMMSMFPTJ");
	int proc_sum = 0;
	CString msgstr = "提示信息:";	//提示信息


	try
	{

		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", "", "当前时间datetime[{0}],", datetime);

		/************************增加*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
					Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());

					Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);

					tmmsmfptj.Reset();
					tmmsmfptj.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);

					tmmsmfptj["REC_CREATOR"] = s.userid;			//记录创建责任者
					tmmsmfptj["REC_CREATE_TIME"] = datetime;		//记录创建时刻
					tmmsmfptj["DATE_TIME"] = bcls_rec->Tables["PARA"].Rows[0]["END_TIME"].ToString();

					//期末库存 = 期初库存 + 当期产生合计 - 当期交废 - 利用品
					/*tmmsmfptj["STOCK_END_WT"] = tmmsmfptj["STOCK_INI_WT"].ToDecimal() + tmmsmfptj["WIPQUATITY"].ToDecimal() - tmmsmfptj["WASTE_WT"].ToDecimal() - tmmsmfptj["XF_WT"].ToDecimal();
					Log::Trace("", __FUNCTION__, "期末库存 = [{0}]", tmmsmfptj["STOCK_END_WT"].ToDecimal());*/

					Log::Trace("", __FUNCTION__, "这里2", "");
					proc_sum += tmmsmfptj.Insert();
					Log::Trace("", __FUNCTION__, "这里3", "");

				}

		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{

			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());

				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);

				tmmsmfptj.Reset();
				tmmsmfptj.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);

				tmmsmfptj["REC_REVISOR"] = s.userid;			//记录修改责任者
				tmmsmfptj["REC_REVISE_TIME"] = datetime;		//记录修改时刻

				//期末库存 = 期初库存 + 当期产生合计 - 当期交废 - 利用品
				tmmsmfptj["STOCK_END_WT"] = tmmsmfptj["STOCK_INI_WT"].ToDecimal() + tmmsmfptj["WIPQUATITY"].ToDecimal() - tmmsmfptj["WASTE_WT"].ToDecimal() - tmmsmfptj["XF_WT"].ToDecimal();
				Log::Trace("", __FUNCTION__, "期末库存 = [{0}]", tmmsmfptj["STOCK_END_WT"].ToDecimal());

				Log::Trace("", __FUNCTION__, "这里2", "");
				proc_sum += tmmsmfptj.Update("REC_REVISOR,REC_REVISE_TIME,STOCK_INI_WT,XF_WT,STOCK_END_WT,STOCK_OUT_WT", "DATE_TIME,STEEL_TYPE_CNAME");
				Log::Trace("", __FUNCTION__, "这里3", "");

			}

		}

		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);

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
