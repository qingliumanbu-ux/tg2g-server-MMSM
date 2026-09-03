/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     mfj
Version:    1.0
Date:       2024-04-22
Description: 炼钢校秤记录自动生成原始记录
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢校秤记录自动生成原始记录
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/


//业务头文件




BM2F_ENTERACE(mmsmxcjl_zdsc)

int f_mmsmxcjl_zdsc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	
	CString	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString	prod_date = CDateTime::Now().ToString("yyyyMMdd");
	int date_days = 0;
	int cmd_count = 0;//查询的循环次数
	CString next_month = "";//下个月1号
	CString now_month_lastday = "";//这个月最后一天 即 当月的总天数
	CDecimal v_counterweigh_c = 0;//碳钢砝码重量
	CDecimal v_counterweigh_s = 0;//不锈钢砝码重量

	/* 实体类定义 */
	CModel tmmsm33xcjl("TMMSM33XCJL");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获取当月天数
		next_month = CDateTime::Parse(prod_date).AddMonths(1).ToString("yyyyMM") + "01000000";
		now_month_lastday = CDateTime::Parse(next_month).AddDays(-1).ToString("yyyyMMddHHmmss").Substring(6, 2);
		CDecimal days_of_month = CDecimal::Parse(now_month_lastday);

		//获取砝码重量
		sqlstr = "SELECT CODE FROM TWMSMZD02 WHERE CODE_CLASS ='MMXCJLFM' ORDER BY CODE_DESC_2_CONTENT";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while(cmd_inq.Read())
		{
			if (cmd_count == 0)
			{
				v_counterweigh_s = cmd_inq.GetDecimal(1);
			}
			else
			{
				v_counterweigh_c = cmd_inq.GetDecimal(1);
			}
			cmd_count++;
		}
		cmd_inq.Close();

		//循环天数，每天又分白夜两个班次，故一天增加两条记录
		for (int i = 1; i <= days_of_month; i++)
		{
			tmmsm33xcjl.Reset();
			if (i < 10)
			{
				//Log::Trace("", __FUNCTION__, "日期= [{0}]", datetime.Substring(0, 6) + "0" + to_string(i));
				tmmsm33xcjl["PROD_DATE"] = datetime.Substring(0, 6) +"0"+ to_string(i);
			}
			else
			{
				//Log::Trace("", __FUNCTION__, "日期111= [{0}]", datetime.Substring(0, 6) + to_string(i));
				tmmsm33xcjl["PROD_DATE"] = datetime.Substring(0, 6) + to_string(i);
			}
			
			tmmsm33xcjl["PROD_SHIFT_NO"] = "1";//1  夜班   2  白   3中
			tmmsm33xcjl["COUNTERWEIGH_S"] = v_counterweigh_s;//不锈钢砝码重量
			tmmsm33xcjl["CALIBRA_REQUIRE_S"] = "不锈钢校秤使用(" + v_counterweigh_s.ToString() + "t)标准物校秤,校秤后标准物放于热修磨5区(2#辊道东侧)";//校秤要求
			tmmsm33xcjl["COUNTERWEIGH_C"] = v_counterweigh_c;//碳钢砝码重量
			tmmsm33xcjl["CALIBRA_REQUIRE_C"] = "碳钢校秤使用(" + v_counterweigh_c.ToString() + "t)标准物校秤,校秤后标准物放于F18东侧标准物存放台(4#辊道西侧)";
			tmmsm33xcjl["PROOFREAD_REAMRK_4_AB"] = "停用";
			if (tmmsm33xcjl.QueryCount("PROD_DATE,PROD_SHIFT_NO")>0)
			{
				continue;
			}

			tmmsm33xcjl.Insert();

			tmmsm33xcjl.Reset();//将数据清掉，下面重新赋值


			if (i < 10)
			{
				tmmsm33xcjl["PROD_DATE"] = datetime.Substring(0, 6) + "0" + to_string(i);
			}
			else
			{
				tmmsm33xcjl["PROD_DATE"] = datetime.Substring(0, 6) + to_string(i);
			}

			tmmsm33xcjl["PROD_SHIFT_NO"] = "2";//1  夜班   2  白   3中
			tmmsm33xcjl["COUNTERWEIGH_S"] = v_counterweigh_s;//不锈钢砝码重量
			tmmsm33xcjl["CALIBRA_REQUIRE_S"] = "不锈钢校秤使用(" + v_counterweigh_s.ToString() + "t)标准物校秤,校秤后标准物放于热修磨5区(2#辊道东侧)";//校秤要求
			tmmsm33xcjl["COUNTERWEIGH_C"] = v_counterweigh_c;//碳钢砝码重量
			tmmsm33xcjl["CALIBRA_REQUIRE_C"] = "碳钢校秤使用(" + v_counterweigh_c.ToString() + "t)标准物校秤,校秤后标准物放于F18东侧标准物存放台(4#辊道西侧)";
			tmmsm33xcjl["PROOFREAD_REAMRK_4_AB"] = "停用";
			if (tmmsm33xcjl.QueryCount("PROD_DATE,PROD_SHIFT_NO")>0)
			{
				continue;
			}

			tmmsm33xcjl.Insert();








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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

