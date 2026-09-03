/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   孟凡杰
Version:    1.0
Date:     2024-04-25 9:13:56
Description: 成品废品明细查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(mmsmfpmx_inq1)

int f_mmsmfpmx_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_fore_back = "";//先后浇	1 先浇带后浇  0 后浇带先浇
	//CString v_heat_no = "";
	CString v_strand_no = "";
	CDecimal v_cast_div_no = 0;
	CString v_cast_no = "";
	CString v_rec_create_time = "";

	//CModel tmmsmfp("TMMSMFP");

	CDbCommand cmd_inq(conn);

	try
	{
		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("FORE_BACK"))
			v_fore_back = bcls_rec->Tables[0].Rows[0]["FORE_BACK"].ToString();
		Log::Info("", __FUNCTION__, "v_fore_back =[{0}]", v_fore_back);
		//if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			//v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		    //v_heat_no_num = CDecimal::Parse(v_heat_no.Substring(3, 5))+1;
			//Log::Info("", __FUNCTION__, "v_heat_no_num =[{0}]", v_heat_no_num);
		if (bcls_rec->Tables[0].Columns.Contains("STRAND_NO"))
			v_strand_no = bcls_rec->Tables[0].Rows[0]["STRAND_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("CAST_NO"))
			v_cast_no = bcls_rec->Tables[0].Rows[0]["CAST_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("CAST_DIV_NO"))
			v_cast_div_no = bcls_rec->Tables[0].Rows[0]["CAST_DIV_NO"].ToDecimal();
		if (bcls_rec->Tables[0].Columns.Contains("SLAB_CUT_TIME"))
			v_rec_create_time = bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME"].ToString();
		Log::Info("", __FUNCTION__, "v_cast_no =[{0}]", v_cast_no);
		Log::Info("", __FUNCTION__, "v_cast_div_no =[{0}]", v_cast_div_no);
		Log::Info("", __FUNCTION__, "v_rec_create_time =[{0}]", v_rec_create_time);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			
			if (v_fore_back.Trim() == "1")	//先浇找后浇
			{
				Log::Info("", __FUNCTION__, "v_fore_back =[{0}]", v_fore_back);

				sqlstr = " SELECT T1.*,T2.SG_GRADE_1 FROM "
					" (SELECT T.*, row_number() over(partition by STRAND_NO "
					" ORDER BY REC_CREATE_TIME) AS ROW_NUM FROM "
					" (SELECT * FROM TMMSM33 WHERE 1=1 ";
				/*sqlstr = " SELECT T1.*,T2.SG_GRADE_1 FROM( SELECT * FROM ( "
					" SELECT T.*, row_number() over(partition by STRAND_NO, CAST_NO, CAST_DIV_NO ORDER BY REC_CREATE_TIME) AS ROW_NUM FROM "
					" (SELECT * FROM TMMSM33 WHERE 1 = 1)T) WHERE ROW_NUM = 1 )T1 "
					" LEFT JOIN TQMTS0X T2 ON T1.ST_NO = T2.ST_NO WHERE 1=1";*/
				/*sqlstr = "  SELECT * FROM ( "
					" SELECT T.*, row_number() over(partition by STRAND_NO, CAST_NO, CAST_DIV_NO ORDER BY REC_CREATE_TIME) AS ROW_NUM FROM "
					" (SELECT * FROM TMMSM33 WHERE 1 = 1)T) WHERE ROW_NUM = 1 ";*/
				/*if (v_heat_no != "")
				{
					v_heat_no_1 = v_heat_no.Substring(0, 3) + v_heat_no_num.ToString();
					Log::Info("", __FUNCTION__, "v_heat_no_1 =[{0}]", v_heat_no_1);
					if (v_heat_no_1.GetLength() == 6)
					{
						v_heat_no_1 = v_heat_no.Substring(0, 3) + "00"+ v_heat_no_num.ToString();
					}
					if (v_heat_no_1.GetLength() == 7)
					{
						v_heat_no_1 = v_heat_no.Substring(0, 3) + "0" + v_heat_no_num.ToString();
					}
					sqlstr += " AND  HEAT_NO	=  '" + v_heat_no_1 + "'";
					sqlstr += " AND  HEAT_NO	=  '" + v_heat_no + "'";
				}*/
				//v_cast_div_no = v_cast_div_no + 1;
				if (v_rec_create_time != "")
				{
					sqlstr += " AND  SLAB_CUT_TIME	> '" + v_rec_create_time + "'" ;
				}
				if (v_strand_no != "")
				{
					sqlstr += " AND  STRAND_NO	= '" + v_strand_no + "'";
				}
				sqlstr += ")T )T1 LEFT JOIN TQMTS0X T2 ON T1.ST_NO = T2.ST_NO WHERE 1 = 1 AND ROW_NUM = 1 ";
				/*if (v_cast_no != "")
				{
					sqlstr += " AND  CAST_NO	= '" + v_cast_no + "'";
				}
				sqlstr += " AND  CAST_DIV_NO	= '" + v_cast_div_no.ToString() + "'";*/
			}
			else    //后浇找先浇
			{
				sqlstr = " SELECT T1.*, T2.SG_GRADE_1 FROM "
					" (SELECT T.*, row_number() over(partition by STRAND_NO "
					" ORDER BY REC_CREATE_TIME DESC) AS ROW_NUM FROM "
					" (SELECT * FROM TMMSM33 WHERE 1=1 ";
				/*sqlstr = " SELECT T1.*,T2.SG_GRADE_1 FROM( SELECT * FROM ( "
					" SELECT T.*, row_number() over(partition by STRAND_NO, CAST_NO, CAST_DIV_NO ORDER BY REC_CREATE_TIME DESC) AS ROW_NUM FROM "
					" (SELECT * FROM TMMSM33 WHERE 1 = 1)T) WHERE ROW_NUM = 1 )T1 "
					" LEFT JOIN TQMTS0X T2 ON T1.ST_NO = T2.ST_NO WHERE 1=1";*/

				//v_cast_div_no = v_cast_div_no - 1;
				if (v_rec_create_time != "")
				{
					sqlstr += " AND  SLAB_CUT_TIME	< '" + v_rec_create_time + "'";
				}
				if (v_strand_no != "")
				{
					sqlstr += " AND  STRAND_NO	= '" + v_strand_no + "'";
				}
				sqlstr += ")T )T1 LEFT JOIN TQMTS0X T2 ON T1.ST_NO = T2.ST_NO WHERE 1 = 1 AND ROW_NUM = 1 ";
				/*if (v_cast_no != "")
				{
					sqlstr += " AND  CAST_NO	= '" + v_cast_no + "'";
				}
				sqlstr += " AND  CAST_DIV_NO	= '" + v_cast_div_no.ToString() + "'";*/
			}
			//sqlstr += " ORDER BY  PROD_TIME";
			break;
		}
		//cmd_inq.Parameters.Set("v_c_div", v_c_div);
		//cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
		cmd_inq.Parameters.Set("v_strand_no", v_strand_no);
		cmd_inq.Parameters.Set("v_cast_no", v_cast_no);
		cmd_inq.Parameters.Set("v_cast_div_no", v_cast_div_no);
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
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
