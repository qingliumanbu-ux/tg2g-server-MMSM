/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:
Version:    1.0
Date:
Description: 炼钢助手知识库维护
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件




BM2F_ENTERACE(wmsmczts_pro)

int f_wmsmczts_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString table_name = " ";
	CString	datetime("");
	CString v_operate = "";
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 业务变量 */

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		v_operate = bcls_rec->Tables[0].Rows[0]["PRO_DIV"].ToString().Trim();
		table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().TrimOrBlank().ToUpper();
		CModel wmsm(table_name);
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			wmsm.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			/*wmsm["REC_CREATOR"] = s.userid;
			wmsm["REC_CREATE_TIME"] = datetime;*/
			if (v_operate == "U")
			{
				Log::Trace("", __FUNCTION__, "table_name =[{0}]", table_name);

				if (table_name == "TWMSMCZTS")
				{
					if (wmsm.QueryCount("C_DIV,GRADE_TYPE2,FACTORY_2,ST_NO")>0)
					{
						wmsm["REC_REVISOR"] = s.userid;
						wmsm["REC_REVISE_TIME"] = datetime;
						wmsm.Update("REC_REVISOR,REC_REVISE_TIME,MEMO_DETAIL", "C_DIV,GRADE_TYPE2,FACTORY_2,ST_NO");
					}
				}

				if (table_name == "TWMSMCZTS_GC")
				{
					wmsm["REC_REVISOR"] = s.userid;
					wmsm["REC_REVISE_TIME"] = datetime;
					wmsm.TrimOrBlank();
					wmsm.Update("REC_REVISOR,REC_REVISE_TIME,SPE_MIN,SPE_MAX,MAIN_AIM", "ELM_CODE,GRADE_TYPE2,FACTORY_2,ST_NO");
				}

				if (table_name == "TWMSMSGAL")
				{
					wmsm["REC_REVISOR"] = s.userid;
					wmsm["REC_REVISE_TIME"] = datetime;
					wmsm.TrimOrBlank();
					wmsm.Update("MEMO_DETAIL", "FACTORY_2,ST_NO");
				}
			}
			//U3修改工艺红线，U4作业区要点
			if (v_operate == "U3")
			{
				if (table_name == "TWMSMCZTS_GYHX")
				{
					wmsm["REC_REVISOR"] = s.userid;
					wmsm["REC_REVISE_TIME"] = datetime;
					wmsm.TrimOrBlank();
					wmsm.Update("REC_REVISOR,REC_REVISE_TIME,MEMO_DETAIL", "FACTORY_2,C_DIV");
				}
			}
			else if (v_operate == "U4"){
				if (table_name == "TWMSMCZTS_GYHX")
				{
					wmsm["REC_REVISOR"] = s.userid;
					wmsm["REC_REVISE_TIME"] = datetime;
					wmsm.TrimOrBlank();
					wmsm.Update("REC_REVISOR,REC_REVISE_TIME,MEMO_DETAIL1", "FACTORY_2,C_DIV,DEV_CODE");
				}
			}

			if (v_operate == "D")
			{
				Log::Trace("", __FUNCTION__, "table_name =[{0}]", table_name);

				if (table_name == "TWMSMCZTS")
				{
					if (wmsm.QueryCount("C_DIV,GRADE_TYPE2,FACTORY_2,ST_NO")>0)
					{
						wmsm.Delete("C_DIV,GRADE_TYPE2,FACTORY_2,ST_NO");
					}
				}
				if (table_name == "TWMSMCZTS_GC")
				{
					wmsm.Delete("FACTORY_2,ELM_CODE,GRADE_TYPE2,ST_NO");
				}
				if (table_name == "TWMSMCZTS_GYHX")
				{
					wmsm.Delete("C_DIV,FACTORY_2,DEV_CODE");
				}
				if (table_name == "TQMTS0RDR")
				{
					wmsm.Delete("HEAT_NO,ORDER_NO,NOW_ROW");

					Log::Trace("", __FUNCTION__, "HEAT_NO =[{0}]", wmsm["HEAT_NO"].ToString());
					Log::Trace("", __FUNCTION__, "ORDER_NO =[{0}]", wmsm["ORDER_NO"].ToString());
					Log::Trace("", __FUNCTION__, "NOW_ROW =[{0}]", wmsm["NOW_ROW"].ToString());
					CString v_heat_no = wmsm["HEAT_NO"].ToString();
					CString v_order_no = wmsm["ORDER_NO"].ToString();
					CString v_now_row = wmsm["NOW_ROW"].ToString();

					//删除对应已发送请求并返回数据的信息
					sqlstr = " DELETE FROM TQMTS0R05 WHERE 1=1 "
						"	AND HEAT_NO = '" + v_heat_no + "'"
						"	AND ORDER_NO = '" + v_order_no + "'"
						"	AND NOW_ROW = '" + v_now_row + "'"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();
				}
				if (table_name == "TWMSMSGAL")
				{
					wmsm.Delete("FACTORY_2,ST_NO");
				}
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
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


