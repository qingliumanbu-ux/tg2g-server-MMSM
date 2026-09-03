/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-11-25
Description: 按PONO查询物料
***********************************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/ 

/* ***** 静态函数申明 ***** */


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// CC工序实绩查询
/// <para>
/// 1.根据熔炼号、处理号等条件进行CC实绩查询。
/// 
/// </para>
/// <para>数据库表：          </para>
/// <para>主调用函数：        </para>
/// </summary>
/// <param name="">                      </param>
/// <param name="">                    </param>
/// <returns> CC实绩 </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm01pf2_inq1)

int f_mmsm01pf2_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr					= "";
	CString sqlstr2 = "";
	CString sqlstr_count			= "";
	CString sqlstr_temp				= "";
	
	CString ch_start_time_f			= "";
	CString ch_start_time_t			= "";
	CString v_table_type = "";//表名称
	CString v_pono = "";
	CString v_heat_no = "";
	CString v_archive_flag = "";
	int	TotalRecordCount = 0;
	CDecimal v_mat_tube_t = 0;
	CDecimal v_mat_wt_t = 0;
	CDecimal v_mat_tube_h = 0;
	CDecimal v_mat_wt_h = 0;
	int blkNum;
	CString v_func_id = ""; //功能号。 
	CString  v_item_ename = "";
	CString c_sql_condition = "";
	CString v_group_by = "";
	CString v_item_display = "";
	CString v_item_group = "";
	CString v_item_ename_d = "";
	int   fetchRowCount = 0;

	//系统的分页类信息。
	CPageInfo pageInfo; 
 
	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	try
	{
		
	
		if (bcls_rec->Tables[0].Columns.Contains("PONO"))
			v_pono= bcls_rec->Tables[0].Rows[0]["PONO"].ToString().TrimOrBlank().ToUpper();

		if (bcls_rec->Tables[0].Columns.Contains("ARCHIVE_FLAG"))
			v_archive_flag = bcls_rec->Tables[0].Rows[0]["ARCHIVE_FLAG"].ToString();

	/*	if (v_table_type.Trim() == "")
		{
			sprintf(s.msg, "【表名称】不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		} */
		
			
		/* ***** 打印输入参数 ***** */
		////Log::Info("", __FUNCTION__, "v_pono  =[{0}]", v_pono);
		////Log::Info("", __FUNCTION__, "v_heat_no   =[{0}]", v_heat_no);

		if (v_archive_flag.Trim() == "T")//在线
		{
			v_table_type = "TMMSM01";
		}
		else if (v_archive_flag.Trim() == "H")//历史
		{
			v_table_type = "HMMSM01";
		}

		


		/* ***** 打印输入参数 ***** */

		////Log::Info("", __FUNCTION__, "PONO      =[{0}]", v_pono);
		////Log::Info("", __FUNCTION__, "ARCHIVE_FLAG      =[{0}]", v_archive_flag);
		////Log::Info("", __FUNCTION__, "v_table_type      =[{0}]", v_table_type);


		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:				// MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:

			/*if (v_archive_flag.Trim() != "")
			{
				sqlstr = " SELECT *  FROM " + v_table_type + " WHERE PONO = @pono ";
		
			}
			else
			{
				sqlstr = " SELECT *  FROM  TMMSM01  WHERE PONO = @pono ";
				sqlstr2 += " UNION SELECT *  FROM HMMSM01  WHERE PONO = @pono ";

				sqlstr = sqlstr + sqlstr2;

			}*/

			////根据ED54 中配置的信息，拼接需修改的字段。
			////======================
			
			v_func_id = "MMSM01P_INQD";
			c_sql_condition = " select t.item_ename from ted54 t where t.func_id = @func_id   and item_ename <>'MAT_NO' AND item_ename<>'ORIGIN_MAT_NO' order by t.class_code,t.seq_no";

			
			cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句
			// 设置SQL中的变量
			cmd_sql.Parameters.Set("func_id", v_func_id); //功能号。
			cmd_sql.ExecuteReader(); //执行读取

		
			while (cmd_sql.Read()) //循环读取
			{
				v_item_ename = cmd_sql.GetString(1);
				v_item_ename_d = cmd_sql.GetString(1);

				//Log::Trace("", __FUNCTION__, "v_item_ename=[{0}]", v_item_ename);
			
				if (v_item_ename_d.Trim() == "MAT_NUM" || v_item_ename_d.Trim() == "MAT_NUM_CUT" || v_item_ename_d.Trim() == "MAT_WT" || v_item_ename_d.Trim() == "MAT_ACT_WT" || v_item_ename_d.Trim() == "MAT_THEORY_WT")
				{
					v_item_ename_d = " SUM(" + v_item_ename + ")" +  " " + v_item_ename;

					//Log::Trace("", __FUNCTION__, "v_item_ename_d=[{0}]", v_item_ename_d);
				}
				
						
				if (fetchRowCount == 0)
				{
					v_item_group = v_item_ename;
					v_item_display = v_item_ename_d;
				}
				else 
				{
					v_item_group = v_item_group + "," + v_item_ename; //需group by的字段
					v_item_display = v_item_display + "," + v_item_ename_d; //需group by的字段
				}

				fetchRowCount++;
			
			}
			cmd_sql.Close(); //关闭游标 
		
			//Log::Trace("", __FUNCTION__, "v_item_group=[{0}]", v_item_group);
			//Log::Trace("", __FUNCTION__, "v_item_display=[{0}]", v_item_display);

			sqlstr = " SELECT " + v_item_group + " FROM " + v_table_type + " WHERE PONO = @pono " + " GROUP BY  " + v_item_group;
			//sqlstr = " SELECT " + v_item_display + " FROM TMMSM01  WHERE PONO = @pono " + " GROUP BY  " + v_item_group;

			//Log::Trace("", __FUNCTION__, "sqlstr [{0}] ", sqlstr);

		
			sqlstr_count = "SELECT COUNT(*) FROM ( " + sqlstr + " )";

			//Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

	/*		break;
		}*/

		cmd_inq.Parameters.Set("pono", v_pono);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "SELECT sum(mat_num),sum(mat_act_wt) "
				"		FROM	 TMMSM01 "
				"		WHERE  PONO =  @pono";
			
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("pono", v_pono);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_mat_tube_t = cmd_inq.GetDecimal(1);
			v_mat_wt_t = cmd_inq.GetDecimal(2);
		}
		cmd_inq.Close();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "SELECT sum(mat_num),sum(mat_act_wt) "
				"		FROM	HMMSM01 "
				"		WHERE  PONO =  @pono";

			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("pono", v_pono);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_mat_tube_h = cmd_inq.GetDecimal(1);
			v_mat_wt_h = cmd_inq.GetDecimal(2);
		}
		cmd_inq.Close();

		////Log::Info("", __FUNCTION__, "v_mat_tube_t  =[{0}]", v_mat_tube_t);
		////Log::Info("", __FUNCTION__, "v_mat_wt_t  =[{0}]", v_mat_wt_t);
		////Log::Info("", __FUNCTION__, "v_mat_tube_h  =[{0}]", v_mat_tube_h);
		////Log::Info("", __FUNCTION__, "v_mat_wt_h  =[{0}]", v_mat_wt_h);

		blkNum = bcls_ret->Tables.IndexOf("MMSMTJ");
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("MMSMTJ");
		}

		
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "MAT_TUBE_T");
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "MAT_WT_T");
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "MAT_TUBE_H");
		bcls_ret->Tables["MMSMTJ"].Columns.Add(DT_STRING, "MAT_WT_H");

		bcls_ret->Tables["MMSMTJ"].Rows.Add();
		bcls_ret->Tables["MMSMTJ"].Rows[0]["MAT_TUBE_T"] = v_mat_tube_t;
		bcls_ret->Tables["MMSMTJ"].Rows[0]["MAT_WT_T"] = v_mat_wt_t;
		bcls_ret->Tables["MMSMTJ"].Rows[0]["MAT_TUBE_H"] = v_mat_tube_h;
		bcls_ret->Tables["MMSMTJ"].Rows[0]["MAT_WT_H"] = v_mat_wt_h;

		
	
		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

	 }
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
