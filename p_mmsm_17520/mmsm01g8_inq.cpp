/*<remark>=========================================================
/// <summary>
/// 板坯库存信息查询
/// <para>
/// 
/// </para>
/// <para>数据库表：
///
///</para>
/// <para>主调用函数：        </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns>板坯库存信息查询 </returns>
===========================================================</remark>*/

#include "stdafx.h"

BM2F_ENTERACE(mmsm01g8_inq)


int f_mmsm01g8_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	CString sqlstr = " ";
	try
	{
		CString v_factory_div = "";
		CString sqlstr_count = "";
		CString sql_where = ""; 
		CString sqlstr = "";
		CDbCommand cmd_inq(conn);

		int		TotalRecordCount = 0;;
		int PageSize, RecordForm;

		v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		if (v_factory_div.Find(","))
		{
			v_factory_div = v_factory_div.Replace(",","','");
		}

		PageSize = bcls_rec->Tables["PageInfo"].Rows[0]["PageSize"];
		RecordForm = bcls_rec->Tables["PageInfo"].Rows[0]["RecordFrom"];

		Log::Info("", __FUNCTION__, "FACTORY_DIV =[{0}];", v_factory_div);
		Log::Info("", __FUNCTION__, "分页信息：PageSize=[{0}];RecordFrom=[{1}]", PageSize, RecordForm);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				" SELECT MAT_DESTION,"
				"        CASE WHEN TRIM(NEXT_WHOLE_BACKLOG_CODE) = '' THEN '无合同' ELSE NEXT_WHOLE_BACKLOG_CODE END AS NEXT_WHOLE_BACKLOG_CODE, "
				"		 ROUND(NVL(SUM(CASE WHEN MAT_STATUS = '20' THEN MAT_WT END), 0),3) AS WT_20,"
				"		 NVL(SUM(CASE WHEN MAT_STATUS = '20' THEN MAT_NUM END), 0) AS NUM_20,"
				"		 NVL(SUM(CASE WHEN MAT_STATUS = '23' THEN MAT_WT END), 0) AS WT_23,"
				"		 NVL(SUM(CASE WHEN MAT_STATUS = '23' THEN MAT_NUM END), 0) AS NUM_23,"
				"		 NVL(SUM(CASE WHEN MAT_STATUS = '24' THEN MAT_WT END), 0) AS WT_24,"
				"		 NVL(SUM(CASE WHEN MAT_STATUS = '24' THEN MAT_NUM END), 0) AS NUM_24,"
				"		 NVL(SUM(CASE WHEN MAT_STATUS = '29' THEN MAT_WT END), 0) AS WT_29,"
				"		 NVL(SUM(CASE WHEN MAT_STATUS = '29' THEN MAT_NUM END), 0) AS NUM_29,"
				"		 NVL(SUM(CASE WHEN MAT_STATUS in('21', '22') THEN MAT_WT END), 0) AS WT_21_22,"
				"		 NVL(SUM(CASE WHEN MAT_STATUS in('21', '22') THEN MAT_NUM END), 0) AS NUM_21_22,"
				"        NVL(SUM(CASE WHEN MAT_STATUS in('20','21','22','23','24','29') THEN MAT_WT END), 0) AS WT_CT, "
				"		 NVL(SUM(CASE WHEN MAT_STATUS in('20','21','22','23','24','29') THEN MAT_NUM END), 0) AS NUM_CT"
				"   FROM TMMSM01"
				"  WHERE MAT_DESTION <> ' '";

			sqlstr_count =
				" SELECT COUNT(*) "
				"   FROM TMMSM01 "
				"  WHERE MAT_DESTION <> ' ' ";
			
			if (v_factory_div.Trim() != "")
			{
				sql_where += " AND FACTORY_DIV IN ('" + v_factory_div + "') ";
			}
			
			sqlstr = sqlstr + sql_where + "GROUP BY NEXT_WHOLE_BACKLOG_CODE, MAT_DESTION ,FACTORY_DIV ORDER BY MAT_DESTION ";
			sqlstr_count = sqlstr_count + sql_where + "GROUP BY NEXT_WHOLE_BACKLOG_CODE, MAT_DESTION ,FACTORY_DIV ORDER BY MAT_DESTION ";
			break;
		}
		Log::Trace("", __FUNCTION__, "sqlstr_count			= [{0}]", (const char*)sqlstr_count);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		
		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], RecordForm, PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error，sqlcode=[{0},{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		Log::Info("", __FUNCTION__, "erro=[{0}];", s.sysmsg);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


