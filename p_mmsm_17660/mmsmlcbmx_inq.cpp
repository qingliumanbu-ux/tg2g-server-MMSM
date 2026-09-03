/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     bhy
Version:    1.0
Date:       2024-09-11
Description: 炉成本查询
**************************************************/
//框架头文件
#include "stdafx.h"	 

BM2F_ENTERACE(mmsmlcbmx_inq)

int f_mmsmlcbmx_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString v_heatnr = "";
	CString v_cast_div_no_1 = "";
	CString v_assist_heatno = "";
	CString v_grade_type = "";
	CString v_route = "";
	CString v_mat_type = "";
	CString v_mat_code = "";
	CString type_flag = "0";
	CString mat_type_flag = "0";
	CString v_mat_class_desc = "";
	CString zh = "";
	
	
	CString v_from = "";//开始时刻
	CString v_to = "";//开始时刻
	CPageInfo pageInfo;



	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables[0].Columns.Contains("HEATNR"))
		{
			v_heatnr = bcls_rec->Tables[0].Rows[0]["HEATNR"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CAST_DIV_NO_1"))
		{
			v_cast_div_no_1 = bcls_rec->Tables[0].Rows[0]["CAST_DIV_NO_1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("ASSIST_HEATNO"))
		{
			v_assist_heatno = bcls_rec->Tables[0].Rows[0]["ASSIST_HEATNO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("GRADE_TYPE1"))
		{
			v_grade_type = bcls_rec->Tables[0].Rows[0]["GRADE_TYPE1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("F_ROUTE1"))
		{
			v_route = bcls_rec->Tables[0].Rows[0]["F_ROUTE1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_TYPE_DESC"))
		{
			v_mat_type = bcls_rec->Tables[0].Rows[0]["MAT_TYPE_DESC"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE_DR"))
		{
			v_mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE_DR"].ToString().Trim();
		}
		
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
		{
			v_from = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().SubstringNE(0,8);
		}
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
		{
			v_to = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().SubstringNE(0, 8);
		}

		if (bcls_rec->Tables[0].Columns.Contains("TYPE_FLAG"))
		{
			type_flag = bcls_rec->Tables[0].Rows[0]["TYPE_FLAG"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_TYPE_DESC1"))
		{
			mat_type_flag = bcls_rec->Tables[0].Rows[0]["MAT_TYPE_DESC1"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CLASS_DESC"))
		{
			v_mat_class_desc = bcls_rec->Tables[0].Rows[0]["MAT_CLASS_DESC"].ToString().Trim();
		}

		if (bcls_rec->Tables[0].Columns.Contains("ZH"))
		{
			zh = bcls_rec->Tables[0].Rows[0]["ZH"].ToString();
		}



		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			if (zh == "ZH")
			{
				if (type_flag == "1")
				{
					sqlstr = "select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY "
						" ,nvl(tt.GRADE_TYPE3,' ' ) GRADE_TYPE3"
						" ,case when t.MAT_TYPE_DESC in ('回收','步骤费') then t.MAT_TYPE_DESC else t3.MAT_CLASS_DESC end MAT_CLASS_DESC"
						" ,DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) WT_PERT"
						" ,t31.CAST_DIV_NO as CAST_DIV_NO_1"
						" from tqmtscb03a_mx t  "
						" left join tqmtscb09_dr tt on tt.STEEL_GRADE = t.GRADE_ID "
						" left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr"
						" left join tmmsm31 t31 on t31.heat_no=t.heatnr"
						" where 1=1";
				}
				else
				{
					sqlstr = "select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY "
						" ,nvl(tt.GRADE_TYPE3,' ' ) GRADE_TYPE3"
						" ,case when t.MAT_TYPE_DESC in ('回收','步骤费') then t.MAT_TYPE_DESC else t3.MAT_CLASS_DESC end MAT_CLASS_DESC"
						" ,DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) WT_PERT"
						" ,t31.CAST_DIV_NO as CAST_DIV_NO_1"
						" from tqmtscb01a_mx t  "
						" left join tqmtscb09_dr tt on tt.STEEL_GRADE = t.GRADE_ID "
						" left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr"
						" left join tmmsm31 t31 on t31.heat_no=t.heatnr"
						" where 1=1";
				}
			}
			else
			{


				if (type_flag == "1")
				{
					sqlstr = "select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY "
						" ,nvl(tt.GRADE_TYPE3,' ' ) GRADE_TYPE3"
						" ,case when t.MAT_TYPE_DESC in ('回收','步骤费') then t.MAT_TYPE_DESC else t3.MAT_CLASS_DESC end MAT_CLASS_DESC"
						" ,DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) WT_PERT"
						" ,t31.CAST_DIV_NO as CAST_DIV_NO_1"
						" from tqmtscb03_mx t  "
						" left join tqmtscb09_dr tt on tt.STEEL_GRADE = t.GRADE_ID "
						" left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr"
						" left join tmmsm31 t31 on t31.heat_no=t.heatnr"
						" where 1=1";
				}
				else
				{
					sqlstr = "select t.AOD_BOF_E_DTIME,t.HEATNR,t.ASSIST_HEATNO,t.TS_SHIFTNO,t.GRADE_ID,t.GRADE_TYPE1,t.F_ROUTE1,t.QUALIFIED_WT,t.AREA_CODE,t.STATION_NAME,t3.MAT_TYPE_DESC,t.MAT_CODE_DR,t.MAT_NAME_DR,t.INCLUDE_NI,t.INCLUDE_CR,t.INCLUDE_MO,t.CONVERSION_PRICE,t.WEIGHT,t.COST,t.COST_PERT,t.UNIT_PRICE_XY,t.COST_PERT_XY "
						" ,nvl(tt.GRADE_TYPE3,' ' ) GRADE_TYPE3"
						" ,case when t.MAT_TYPE_DESC in ('回收','步骤费') then t.MAT_TYPE_DESC else t3.MAT_CLASS_DESC end MAT_CLASS_DESC"
						" ,DECODE(QUALIFIED_WT,0,0,round(WEIGHT*1000/QUALIFIED_WT,3)) WT_PERT"
						" ,t31.CAST_DIV_NO as CAST_DIV_NO_1"
						" from tqmtscb01_mx t  "
						" left join tqmtscb09_dr tt on tt.STEEL_GRADE = t.GRADE_ID "
						" left join TQMTSCB08_DR t3 on t3.mat_code_dr=t.mat_code_dr"
						" left join tmmsm31 t31 on t31.heat_no=t.heatnr"
						" where 1=1";
				}
			}
			

			if (v_heatnr != ""){
				sqlstr += " AND HEATNR like @v_heatnr||'%'";
			}
			if (v_cast_div_no_1 != ""){
				sqlstr += " AND CAST_DIV_NO_1 = @v_cast_div_no_1";
			}
			if (v_assist_heatno != ""){
				sqlstr += " AND ASSIST_HEATNO = @v_assist_heatno";
			}
			if (v_grade_type != ""){
				sqlstr += " AND GRADE_TYPE1 = @v_grade_type";
			}
			if (v_route != ""){
				sqlstr += " AND F_ROUTE1 = @v_route";
			}
			if (v_mat_type != ""){
				sqlstr += " AND t.MAT_TYPE_DESC = @v_mat_type";
			}
			if (v_mat_code != ""){
				sqlstr += " AND t.MAT_CODE_DR = @v_mat_code";
			}
			if (v_mat_class_desc != ""){
				sqlstr += " AND t3.MAT_CLASS_DESC = @v_mat_class_desc";
			}
			
			if (v_from.Trim() != "")
			{
				sqlstr += " AND AOD_BOF_E_DTIME>= @v_from";
			}
			if (v_to.Trim() != "")
			{
				sqlstr += " AND AOD_BOF_E_DTIME<= @v_to";
			}
			if (mat_type_flag== "1")
			{
				sqlstr += " AND trim(t3.MAT_TYPE_DESC) is null ";
			}
			Log::Trace("", __FUNCTION__, "flag1[{0}]  ", mat_type_flag);
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", v_from);
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", v_to);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_heatnr", v_heatnr);
			cmd_inq.Parameters.Set("v_cast_div_no_1", v_cast_div_no_1);
			cmd_inq.Parameters.Set("v_assist_heatno", v_assist_heatno);
			cmd_inq.Parameters.Set("v_grade_type", v_grade_type);
			cmd_inq.Parameters.Set("v_route", v_route);
			cmd_inq.Parameters.Set("v_mat_type", v_mat_type);
			cmd_inq.Parameters.Set("v_mat_code", v_mat_code);
			cmd_inq.Parameters.Set("v_mat_class_desc", v_mat_class_desc);
		
			cmd_inq.Parameters.Set("v_from", v_from);
			cmd_inq.Parameters.Set("v_to", v_to);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
		}


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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
