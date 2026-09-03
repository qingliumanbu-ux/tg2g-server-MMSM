/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 特棒外购料特棒信息新增
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 特棒外购料特棒信息新增
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
 

//外部函数声明
int f_pmom_get_no(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(mmsmqcsma2_ins)

int f_mmsmqcsma2_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	int i = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmmsmqc_bp("TMMSMQC_BP");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	Log::Trace("", __FUNCTION__, "Cdbcommmad 定义完成");

	CString str = "";
	bcls_rec->WriteHTML(str);
	//Log::Trace("", __FUNCTION__, "blk= [{0}]", (const char*)str);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 替换数据列名 */

		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			bcls_rec->Tables[0].Columns.SetColumnName(bcls_rec->Tables[0].Columns.IndexOf("ST_NO"), "OLD_ST_NO");//ERP钢号
		if (bcls_rec->Tables[0].Columns.Contains("MAT_WT"))
			bcls_rec->Tables[0].Columns.SetColumnName(bcls_rec->Tables[0].Columns.IndexOf("MAT_WT"), "MAT_WT_SP");//实盘重量
		if (bcls_rec->Tables[0].Columns.Contains("SURFACE_DECIDE_CODE"))
			bcls_rec->Tables[0].Columns.SetColumnName(bcls_rec->Tables[0].Columns.IndexOf("SURFACE_DECIDE_CODE"), "SURFACE_DECIDE_CODE");//ERP表面判定				
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_NO"))
			bcls_rec->Tables[0].Columns.SetColumnName(bcls_rec->Tables[0].Columns.IndexOf("STOCK_PLACE_NO"), "STOCK_PLACE_NO");//ERP库位号
		if (bcls_rec->Tables[0].Columns.Contains("ARCHIVE_STAMP_NO"))
			bcls_rec->Tables[0].Columns.SetColumnName(bcls_rec->Tables[0].Columns.IndexOf("ARCHIVE_STAMP_NO"), "DIFFERENT_DESC");//差别类型
		
		Log::Trace("", __FUNCTION__, "替换数据列名 完成");
		/* 获取输入参数 */
		Log::Trace("", __FUNCTION__, "开始循环 共[{0}]条数据", bcls_rec->Tables[0].Rows.get_Count());
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsmqc_bp.Reset();
			tmmsmqc_bp.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsmqc_bp["PROCESS_FLAG"] = "0";
			tmmsmqc_bp.TrimOrBlank();
			

			Log::Trace("", __FUNCTION__, "tmmsmqc_bp.MAT_NO			= [{0}]", (const char*)tmmsmqc_bp["MAT_NO"].ToString());
			/* 检查输入参数合法性 */
			if (tmmsmqc_bp.QueryCount("MAT_NO") != 0)
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 材料号不止一个.";
			}

		/*	if (tmmsmqc_bp["MAT_NO"].ToString().GetLength() != 12)
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 材料号不是20位.";
			}*/

			if (tmmsmqc_bp["UNIT_CODE"].ToString().Trim() == "")
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 机组代码为空.";
			}
			Log::Trace("", __FUNCTION__, "11111111");
			if (tmmsmqc_bp["FACTORY_DIV"].ToString().Trim() == "")
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 厂别区分为空.";
			}
			Log::Trace("", __FUNCTION__, "22222222");
			if (tmmsmqc_bp["PROD_TIME"].ToString().Trim() == "")
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 生产时刻为空.";
			}
			Log::Trace("", __FUNCTION__, "333333");
			if (tmmsmqc_bp["OLD_ST_NO"].ToString().Trim() == "")
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 出钢记号为空.";
			}

			if (tmmsmqc_bp["MAT_THICK"].ToDecimal().ToFloat() <= 0)
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 材料厚度不大于0.";
			}

			if (tmmsmqc_bp["MAT_WIDTH"].ToDecimal().ToFloat() <= 0)
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 材料宽度不大于0.";
			}

			if (tmmsmqc_bp["MAT_WIDTH"].ToDecimal().ToFloat() <= 0)
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 材料宽度不大于0.";
			}

			if (tmmsmqc_bp["MAT_LEN"].ToDecimal().ToInt32() <= 0)
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 材料长度不大于0.";
			}


			if (tmmsmqc_bp["MAT_THEORY_WT"].ToDecimal().ToFloat() <= 0)
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 材料重量不大于0.";
			}

			if (tmmsmqc_bp["HEAT_NO"].ToString().Trim() == "")
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 熔炼号为空.";
			}

			if (tmmsmqc_bp["STOCK_NO"].ToString().Trim() == "")
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 库号为空.";
			}

			if (tmmsmqc_bp["FACTORY_DIV"].ToString() == "A1" || tmmsmqc_bp["FACTORY_DIV"].ToString() == "A2")
			{
				if (tmmsmqc_bp["STOCK_PLACE_NO"].ToString().Trim() == "")
				{
					tmmsmqc_bp["PROCESS_FLAG"] = "9";
					tmmsmqc_bp["PROCESS_DESC"] = " 库位号为空.";
				}

				if (tmmsmqc_bp["STOCK_NO"].ToString().GetLength() > 3)
				{
					tmmsmqc_bp["PROCESS_FLAG"] = "9";
					tmmsmqc_bp["PROCESS_DESC"] = " 库位号超过3位.";
				}

				if (tmmsmqc_bp["HALL_NO"].ToString().Trim() == "")
				{
					tmmsmqc_bp["PROCESS_FLAG"] = "9";
					tmmsmqc_bp["PROCESS_DESC"] = " 跨号为空.";
				}


				if (tmmsmqc_bp["ROWNO"].ToString().Trim() == "")
				{
					tmmsmqc_bp["PROCESS_FLAG"] = "9";
					tmmsmqc_bp["PROCESS_DESC"] = " 行号为空.";
				}

				if (tmmsmqc_bp["COLUMN_NO"].ToString().Trim() == "")
				{
					tmmsmqc_bp["PROCESS_FLAG"] = "9";
					tmmsmqc_bp["PROCESS_DESC"] = " 列号为空.";
				}
			}
			
			
			/*if (tmmsmqc_bp["LAYERNO"].ToDecimal().GetLength() > 3)
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 层号超过3位.";
			}

			if (tmmsmqc_bp["LAYERNO"].ToDecimal().Find('-') > 0)
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 层号格式错误.";
			}*/

			/*if (tmmsmqc_bp["SURFACE_DECIDE_CODE"].ToString().Trim() == "")
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 表面判定代码为空.";
			}*/

			if (tmmsmqc_bp["DIFFERENT_DESC"].ToString().GetLength() > 4)
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 差别类型超长.";
			}

			if (tmmsmqc_bp["PRODUCT_CODE"].ToString().Trim() == "")
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 产副品代码为空.";
			}

			if (tmmsmqc_bp["PRODUCT_FLAG"].ToString().Trim() == "")
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 成品标记为空.";
			}

			/*if (tmmsmqc_bp["STOCK_STAY_TIME"].ToString().Trim() == "")
			{
				tmmsmqc_bp["PROCESS_FLAG"] = "9";
				tmmsmqc_bp["PROCESS_DESC"] = " 库存时间为空.";
			}*/
			tmmsmqc_bp["MAT_NO_OLD"] = tmmsmqc_bp["MAT_NO"];
			Log::Trace("", __FUNCTION__, "55555555");
			tmmsmqc_bp.Insert();
			Log::Trace("", __FUNCTION__, "	第[{0}]条数据，材料号[{1}]", i, (const char*)tmmsmqc_bp["MAT_NO"].ToString());
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetMsg(), i + 1 };
		CMessageFormat::Format(s.msg, "校验信息[{0}]，第[{1}]条数据", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
