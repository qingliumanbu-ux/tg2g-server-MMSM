/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    1.0
Date:
Description: 导入报表
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件




BM2F_ENTERACE(mmsmdrsj_add)

int f_mmsmdrsj_add(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString table_name = " ";
	CString c_div = " ";
	CString	datetime("");
	/*CString v_st_no = "";
	CString v_factory_2 = "";
	CString v_memo_detail = "";
	CString v_grade_type2 = "";
	CString v_c_div = "";
	CString v_heat_no = "";
	CString v_order_no = "";*/
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 业务变量 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	CModel twmsmczts("TWMSMCZTS");

	try
	{

		table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("c_div"))
			c_div = bcls_rec->Tables[0].Rows[0]["c_div"].ToString().TrimOrBlank();
		CModel mmsmsj(table_name);
		for (size_t i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			mmsmsj.MergeFrom(bcls_rec->Tables[1].Rows[i]);
			mmsmsj["REC_CREATOR"] = s.userid;
			mmsmsj["REC_CREATE_TIME"] = datetime;
			if (table_name == "TQMTS0RDR")
			{
				CString v_heat_no = bcls_rec->Tables[1].Rows[i]["HEAT_NO"].ToString().TrimOrBlank();
				CString v_order_no = bcls_rec->Tables[1].Rows[i]["ORDER_NO"].ToString().TrimOrBlank();
				sqlstr = "SELECT DECODE(max(now_row),null,0,max(now_row)) FROM TQMTS0RDR where heat_no='" + v_heat_no + "'and order_no='" + v_order_no + "'";
				cmd_inq.SetCommandText(sqlstr);
				//cmd_inq.ExecuteNonQuery();
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					mmsmsj["NOW_ROW"] = cmd_inq.GetDecimal(1) + 1;
				}

				Log::Trace("", __FUNCTION__, "table_name=[{0}]", table_name);
				Log::Trace("", __FUNCTION__, "now_row =[{0}]", cmd_inq.GetDecimal(1) + 1);
				cmd_inq.Close();
				//mmsmsj["NOW_ROW"] = mmsmsj.QueryCount("HEAT_NO,ORDER_NO") + 1;
			}
			//2026.03.12 增加MMSMFPMXS2N碳钢判废明细导入和不锈钢判废明细导入
			if (table_name == "TMMSMPF")
			{
				if (c_div == "2")
				{
					mmsmsj["C_DIV"] = "2";
				}
				else if (c_div == "1")
				{
					mmsmsj["C_DIV"] = "1";
				}
			}

			if (table_name == "TWMSMCZTS")
			{

				if (mmsmsj.QueryCount("C_DIV,GRADE_TYPE2,FACTORY_2,ST_NO")>0)
				{
					Log::Trace("", __FUNCTION__, "3333=[{0}]", table_name);
					mmsmsj["REC_REVISOR"] = s.userid;
					mmsmsj["REC_REVISE_TIME"] = datetime;
					mmsmsj.Update("REC_REVISOR,REC_REVISE_TIME,MEMO_DETAIL", "C_DIV,GRADE_TYPE2,FACTORY_2,ST_NO");
				}
				else
				{
					CString v_st_no = bcls_rec->Tables[1].Rows[i]["ST_NO"].ToString().TrimOrBlank();
					CString v_memo_detail = bcls_rec->Tables[1].Rows[i]["MEMO_DETAIL"].ToString().TrimOrBlank();
					CString v_factory_2 = bcls_rec->Tables[1].Rows[i]["FACTORY_2"].ToString().TrimOrBlank();
					CString v_grade_type2 = bcls_rec->Tables[1].Rows[i]["GRADE_TYPE2"].ToString().TrimOrBlank();
					CString v_c_div = bcls_rec->Tables[1].Rows[i]["C_DIV"].ToString().TrimOrBlank();

					Log::Trace("", __FUNCTION__, "11111111=[{0}]", table_name);
					Log::Trace("", __FUNCTION__, "v_c_div=[{0}]", v_c_div);
					Log::Trace("", __FUNCTION__, "v_grade_type2=[{0}]", v_grade_type2);
					Log::Trace("", __FUNCTION__, "v_factory_2=[{0}]", v_factory_2);
					Log::Trace("", __FUNCTION__, "v_memo_detail=[{0}]", v_memo_detail);

					sqlstr = "insert into TWMSMCZTS(REC_CREATOR,REC_CREATE_TIME,C_DIV,GRADE_TYPE2,FACTORY_2,ST_NO,MEMO_DETAIL) values "
						" ('" + mmsmsj["REC_CREATOR"].ToString() + "',"
						"'" + mmsmsj["REC_CREATE_TIME"].ToString() + "',"
						"'" + v_c_div + "',"
						"'" + v_grade_type2 + "',"
						"'" + v_factory_2 + "',"
						"'" + v_st_no + "',"
						"'" + v_memo_detail + "')"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
					/*twmsmczts.TrimOrBlank();
					twmsmczts["GUICHENG"] = " ";
					twmsmczts.Insert();*/
				}
			}
			else
			{
				Log::Trace("", __FUNCTION__, "222222=[{0}]", table_name);
				mmsmsj.Insert();
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		Log::Trace("", __FUNCTION__, "ex.GetCode() =[{0}]", ex.GetCode());
		if (ex.GetCode() == 1)
		{
			CMessageFormat::Format(s.msg, "导入重复，请重新导入！sqlcode=[{0}]", arguments, 1);
		}
		else
		{
			CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		}
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


