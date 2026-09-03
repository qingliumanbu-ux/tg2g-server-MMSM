/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     Simon Li
Version:    1.0
Date:       2024-02-19
Description: 铁水分配信息查询
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(mmsm11_inq)

int f_mmsm11_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 		//服务调用日志输出

	int doFlag = 0;					//服务调用返回值

	/* 分页信息定义 */
	CPageInfo pageInfo;
	int	TotalRecordCount = 0;

	CModel tmmsm11("TMMSM11");		//炼钢铁水信息表实体类

	CDbCommand cmd(conn);			//数据库操作对象定义

	/* sql语句变量定义 */
	CString sqlstr;
	CString sqlstr_count = "";
	CString sqlstr_temp = "";

	/* 传入参数定义 */
	CString recv_end_time_s = "";		//受铁结束时间起
	CString recv_end_time_e = "";		//受铁结束时间止


	try
	{
		//tmmsm11接收传入参数信息
		tmmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//通过字段名接收传入参数信息
		if (bcls_rec->Tables[0].Columns.Contains("RECV_END_TIME_S"))
			recv_end_time_s = bcls_rec->Tables[0].Rows[0]["RECV_END_TIME_S"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("RECV_END_TIME_E"))
			recv_end_time_e = bcls_rec->Tables[0].Rows[0]["RECV_END_TIME_E"].ToString();

		//sql语句赋初值
		sqlstr_count = " SELECT COUNT(1)  FROM  TMMSM11 WHERE 1=1 ";
		sqlstr = " SELECT *  FROM TMMSM11 WHERE 1=1 ";

		//------------------ sql拼接 ---------------------------------

		if (tmmsm11["BF_ID"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND BF_ID = @tmmsm11.BF_ID";
		}

		if (tmmsm11["IRON_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND IRON_NO = @tmmsm11.IRON_NO";
		}

		if (tmmsm11["TPC_YL_NO"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND TPC_YL_NO = @tmmsm11.TPC_YL_NO";
		}

		if (tmmsm11["TPC_ID"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND TPC_ID = @tmmsm11.TPC_ID";
		}

		if (tmmsm11["RECV_FLAG"].ToString().Trim() != "")
		{
			sqlstr_temp += " AND RECV_FLAG = @tmmsm11.RECV_FLAG";
		}
		else{
			sqlstr_temp += " AND RECV_FLAG != '6' ";
		}

		if (recv_end_time_s.Trim() != "")
		{
			sqlstr_temp += " AND TPC_ST_END_TIME	>= @recv_end_time_s";
		}

		if (recv_end_time_e.Trim() != "")
		{
			sqlstr_temp += " AND TPC_ST_END_TIME	<= @recv_end_time_e";
		}

		sqlstr_count = sqlstr_count + sqlstr_temp;

		sqlstr_temp += " ORDER BY TICODE DESC";

		sqlstr = sqlstr + sqlstr_temp;


		//------------------ sql拼接 ---------------------------------

		//分页查询
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("tmmsm11.BF_ID", tmmsm11["BF_ID"].ToString());
		cmd.Parameters.Set("tmmsm11.IRON_NO", tmmsm11["IRON_NO"].ToString());
		cmd.Parameters.Set("tmmsm11.TPC_YL_NO", tmmsm11["TPC_YL_NO"].ToString());
		cmd.Parameters.Set("tmmsm11.TPC_ID", tmmsm11["TPC_ID"].ToString());
		cmd.Parameters.Set("tmmsm11.RECV_FLAG", tmmsm11["RECV_FLAG"].ToString());
		cmd.Parameters.Set("recv_end_time_s", recv_end_time_s);
		cmd.Parameters.Set("recv_end_time_e", recv_end_time_e);
		cmd.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd.Close();

		cmd.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd.ExecuteScalar().ToInt32();
		cmd.Close();

		//返回分页总数量信息
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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
	cmd.Close();
	
	return doFlag;		//返回-1时事务将回滚，返回为0是事务将提交
}