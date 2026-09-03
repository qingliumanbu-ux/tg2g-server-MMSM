/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-07-04
Description: 炼钢实绩总表处理
***********************************************************************/


/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 


//#include "tmmsm20.h" 

//#include "tmmsm22.h" 
 

//#include "tmmsm25.h" 
 



BM2_FUNCTION_EXPORT
int f_mmsm10_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	int i = 0;
	int v_refine_num = 0;
	CString sqlstr = "";
	CString sqlstr_mm = "";
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_station_id = "";
	CString v_table_name = "";
	CString v_station_no = "";
	CString v_start_time = "";
	CString v_end_time = "";
	CString v_BACKLOG_EA = "";
	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel tmmsm10("TMMSM10");
	CModel tmmsm21("TMMSM21");
	//CTMMSM22 tmmsm22(conn);
	CModel tmmsm23("TMMSM23");
	CModel tmmsm24("TMMSM24");
	CModel tmmsm31("TMMSM31");
	EIClass block_pssm11;
 
 	try
	{
	
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			tmmsm10["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();

		//Log::Info("", __FUNCTION__, "HEAT_NO   =[{0}]", tmmsm10["HEAT_NO"].ToString());
	
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "SELECT * "
					"  FROM TPSSM11 "
				    " WHERE HEAT_NO	= @heat_no";
				
				break;
		}
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.Parameters.Set("heat_no", tmmsm10["HEAT_NO"].ToString());
		block_pssm11.Tables[0].Rows.Clear();
		cmd_sql.ExecuteQuery(block_pssm11.Tables[0]);
		cmd_sql.Close();
		//cmd_sql.ExecuteReader();
		//if (cmd_sql.Read())
		if (block_pssm11.Tables[0].Rows.get_Count()<=0)
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "SELECT * "
					"  FROM TPSSM41 "
					" WHERE HEAT_NO	= @heat_no";

				break;
			}
			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("heat_no", tmmsm10["HEAT_NO"].ToString());
			cmd_sql.ExecuteQuery(block_pssm11.Tables[0]);
			cmd_sql.Close();
		}
		//else
		//{
		//	switch (conn->DatabaseKind)
		//	{
		//	case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//	case DB_KIND_ORACLE:	    // Oracle 数据库
		//	default:
		//		sqlstr = "SELECT * "
		//			"  FROM TPSSM41 "
		//		    " WHERE HEAT_NO	= @heat_no";

		//		break;
		//	}
		//	cmd_sql.SetCommandText(sqlstr);
		//	cmd_sql.Parameters.Clear();
		//	cmd_sql.Parameters.Set("heat_no", tmmsm10["HEAT_NO"].ToString());
		//	cmd_sql.ExecuteReader();
		//	if (cmd_sql.Read())
		//	{
		//		//cmd_sql.Fetch(tpssm11);
		//		cmd_sql.Fetch(tmmsm10);
		//	}
		//}
		//cmd_sql.Close();


		//tmmsm10.CopyFrom(tpssm11);
		tmmsm10.MergeFrom(block_pssm11.Tables[0].Rows[0]);
		tmmsm10["REC_CREATE_TIME"] = dateNow;
		tmmsm10["REC_CREATOR"] = s.userid;

		v_BACKLOG_EA = block_pssm11.Tables[0].Rows[0]["BACKLOG_EA"].ToString();
		//Log::Info("", __FUNCTION__, "tpssm11.BACKLOG_EA  =[{0}]", v_BACKLOG_EA);
	
		for (i = 0; i < 4; i++)
		{
			v_station_id = v_BACKLOG_EA.Substring(i, 1);

			//Log::Info("", __FUNCTION__, "v_station_id =[{0}]", v_station_id);

			if (v_station_id.Trim()==' ')
			{
				break;
			}

			if (v_station_id == "B")
			{
				v_table_name = "TMMSM21";//转炉
			}
			else if (v_station_id == "E")
			{
				v_table_name = "TMMSM20";//电炉
			}
			else if (v_station_id == "A")//吹氩
			{
				v_table_name = "TMMSM22"; 
			}
			else if (v_station_id == "R")
			{
				v_table_name = "TMMSM23"; 
			}
			else if (v_station_id == "L")
			{
				v_table_name = "TMMSM24";
			}
			else if (v_station_id == "V")
			{
				v_table_name = "TMMSM25";
			}
			else if (v_station_id == "C")
			{
				v_table_name = "TMMSM31";
			}

			//Log::Info("", __FUNCTION__, "v_table_name  =[{0}]", v_table_name);


			switch (conn->DatabaseKind)
			{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库
				default:

					sqlstr_mm = " SELECT *  FROM " + v_table_name + " WHERE  HEAT_NO = @heat_no ";

					//Log::Info("", __FUNCTION__, "sqlstr_mm  =[{0}]", sqlstr_mm);
				break;
			}
			cmd_sql.SetCommandText(sqlstr_mm);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("heat_no", tmmsm10["HEAT_NO"].ToString());
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				if (v_station_id == "B")
				{
					cmd_sql.Fetch(tmmsm21);
					tmmsm10["SMELT_DEV_CODE"] = v_station_id + tmmsm21["STATION_NO"].ToString();
					tmmsm10["SMELT_START_TIME"] = tmmsm21["START_TIME"];
					tmmsm10["SMELT_END_TIME"]   = tmmsm21["END_TIME"];
					tmmsm10["PROD_TIME"] = tmmsm21["START_TIME"];
					tmmsm10["PROD_SHIFT_GROUP"] = tmmsm21["PROD_SHIFT_GROUP"];
					tmmsm10["PROD_SHIFT_NO"] = tmmsm21["PROD_SHIFT_NO"];
				}
				else if (v_station_id == "A")//吹氩
				{
					/*cmd_sql.Fetch(tmmsm22);

					v_refine_num = tmmsm22.REFINE_NUM.ToInt32();
					v_station_no = tmmsm22.STATION_NO;
					v_start_time = tmmsm22.START_TIME;
					v_end_time = tmmsm22.END_TIME;*/

				}
				else if (v_station_id == "R")
				{
					cmd_sql.Fetch(tmmsm23);

					v_refine_num = tmmsm23["REFINE_NUM"].ToDecimal().ToInt32();
					v_station_no = tmmsm23["STATION_NO"];
					v_start_time = tmmsm23["START_TIME"];
					v_end_time = tmmsm23["END_TIME"];

				}
				else if (v_station_id == "L")
				{
					cmd_sql.Fetch(tmmsm24);

					v_refine_num = tmmsm24["REFINE_NUM"].ToDecimal().ToInt32();
					v_station_no = tmmsm24["STATION_NO"];
					v_start_time = tmmsm24["START_TIME"];
					v_end_time = tmmsm24["END_TIME"];
				
				}
				else if (v_station_id == "C")
				{
					cmd_sql.Fetch(tmmsm31);
					tmmsm10["CC_MACH_NO"] =  tmmsm31["STATION_NO"];
					tmmsm10["CC_START_TIME"]= tmmsm31["START_TIME"];
					tmmsm10["CC_END_TIME"] = tmmsm21["END_TIME"];
				}

				if (v_refine_num == 1)
				{
					tmmsm10["SR1_DEV_CODE"] = v_station_id + v_station_id;
					tmmsm10["SR1_START_TIME"] = v_start_time;
					tmmsm10["SR1_END_TIME"] = v_end_time;
				}
				if (v_refine_num == 2)
				{
					tmmsm10["SR2_DEV_CODE"] = v_station_id + v_station_id;
					tmmsm10["SR2_START_TIME"] = v_start_time;
					tmmsm10["SR2_END_TIME"] = v_end_time;
				}
				if (v_refine_num == 3)
				{
					tmmsm10["SR3_DEV_CODE"] = v_station_id + v_station_id;
					tmmsm10["SR3_START_TIME"] = v_start_time;
					tmmsm10["SR3_END_TIME"] = v_end_time;
				}
				if (v_refine_num == 4)
				{
					tmmsm10["SR4_DEV_CODE"] = v_station_id + v_station_id;
					tmmsm10["SR4_START_TIME"] = v_start_time;
					tmmsm10["SR4_END_TIME"] = v_end_time;
				}

			}
			cmd_sql.Close();
			
			
		}

		bool isExist = tmmsm10.Query("HEAT_NO");
		//校验要删除的材料是否还在，物料主档中获取材料信息。
		if (!isExist)
		{
			tmmsm10.Insert();
		}
		else
		{
			tmmsm10.Delete("HEAT_NO");
			tmmsm10.Insert();
		}
		

	

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
