/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-07-04
Description: 炼钢实绩总表跟踪
***********************************************************************/


/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号

BM2_FUNCTION_EXPORT
int f_mmsm10_trace(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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
	CString sqlstr_ps = "";
	CString sql_temp = "";//查询条件
	CString dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_station_id = "";
	CString v_table_name = "";
	CString v_station_no = "";
	CString v_start_time = "";
	CString v_end_time = "";
	CString v_proc_no = "";
	CString l2_proc_no = "";
	int count = 0;
	CString v_proc_div = "";
	CString v_pono = "";//制造命令号    因存在不锈钢转炉，电炉，中频炉  产出时熔炼号，aod生成熔炼号时反写的情况，故根据PONO做对应处理   mfj   20231123
	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel tmmsm10("TMMSM10");
	CModel tmmsm19("TMMSM19");
	CModel tmmsm20("TMMSM20");
	CModel tmmsm21("TMMSM21");
	CModel tmmsm23("TMMSM23");
	CModel tmmsm24("TMMSM24");
	CModel tmmsm25("TMMSM25");
	CModel tmmsm26("TMMSM26");
	CModel tmmsm27("TMMSM27");
	CModel tmmsm31("TMMSM31");
	EIClass block_pssm11;
	CString v_resume_seq_no = " ";
	CString seq("");
 	try
	{
		blkNum = bcls_rec->Tables.IndexOf("MMSM10");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMSM10 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

	
		if (bcls_rec->Tables["MMSM10"].Columns.Contains("HEAT_NO"))
			tmmsm10["HEAT_NO"] = bcls_rec->Tables["MMSM10"].Rows[0]["HEAT_NO"].ToString();
		if (bcls_rec->Tables["MMSM10"].Columns.Contains("PROC_NO"))
			v_proc_no = bcls_rec->Tables["MMSM10"].Rows[0]["PROC_NO"].ToString();
		if (bcls_rec->Tables["MMSM10"].Columns.Contains("STATION_ID"))
			v_station_id = bcls_rec->Tables["MMSM10"].Rows[0]["STATION_ID"].ToString();
		if (bcls_rec->Tables["MMSM10"].Columns.Contains("PROC_DIV"))
			v_proc_div = bcls_rec->Tables["MMSM10"].Rows[0]["PROC_DIV"].ToString();
		if (bcls_rec->Tables["MMSM10"].Columns.Contains("PONO"))
			v_pono = bcls_rec->Tables["MMSM10"].Rows[0]["PONO"].ToString().Trim();
		if (bcls_rec->Tables["MMSM10"].Columns.Contains("L2_PROC_NO"))
			l2_proc_no = bcls_rec->Tables["MMSM10"].Rows[0]["L2_PROC_NO"].ToString().Trim();
		
		tmmsm10["PONO"] = v_pono;
		if (v_station_id == "B" || v_station_id == "Y")//转炉
		{
			v_table_name = "TMMSM21";
		}
		else if (v_station_id == "E" || v_station_id == "X")//电炉
		{
			
			v_table_name = "TMMSM20";
		}
		else if (v_station_id == "Z")//中频炉
		{
			v_table_name = "TMMSM19";
		}
		else if (v_station_id == "A")//AOD
		{
			v_table_name = "TMMSM27";
		}
		else if (v_station_id == "R")//RH
		{
			v_table_name = "TMMSM23";
		}
		else if (v_station_id == "F")//LF
		{
			v_table_name = "TMMSM24";
		}
		else if (v_station_id == "V")//VOD
		{
			v_table_name = "TMMSM25";
		}
		else if (v_station_id == "C")//连铸
		{
			v_table_name = "TMMSM31";
		}
		else if (v_station_id == "S")//LTS
		{
			v_table_name = "TMMSM26";
		}

		Log::Info("", __FUNCTION__, "v_table_name  =[{0}]", v_table_name);

		doFlag = f_mm0011("TMMSM2A_SEQ", 8, v_resume_seq_no, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", "dateNow", "dateNow = {0}", dateNow);
		Log::Trace("", "v_resume_seq_no", "v_resume_seq_no = {0}", v_resume_seq_no);
		seq = dateNow + v_resume_seq_no;
		tmmsm10["PROD_SEQ_NO"] = seq;
		Log::Trace("", "PROD_SEQ_NO", "PROD_SEQ_NO = {0}", tmmsm10["PROD_SEQ_NO"].ToString());
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:

			sqlstr_mm = " SELECT *  FROM " + v_table_name + " WHERE  L2_PROC_NO = @l2_proc_no ";
			break;
		}
		cmd_sql.SetCommandText(sqlstr_mm);
		cmd_sql.Parameters.Clear();
		cmd_sql.Parameters.Set("l2_proc_no", l2_proc_no);
		cmd_sql.ExecuteReader();
		Log::Trace("", "l2_proc_no", "l2_proc_no = {0}", l2_proc_no);
		if (cmd_sql.Read())
		{
			cmd_sql.Fetch(tmmsm10);
			if (v_station_id == "B" || v_station_id == "Y")
			{
				cmd_sql.Fetch(tmmsm21);
				tmmsm10["SMELT_DEV_CODE"] = v_station_id + tmmsm21["STATION_NO"].ToString().Trim();
				Log::Trace("", "SMELT_DEV_CODE", "SMELT_DEV_CODE = {0}", v_station_id);
				tmmsm10["SMELT_START_TIME"] = tmmsm21["START_TIME"];
				tmmsm10["SMELT_END_TIME"] = tmmsm21["END_TIME"];
				tmmsm10["PROD_TIME"] = tmmsm21["START_TIME"];
				tmmsm10["PROD_SHIFT_GROUP"] = tmmsm21["PROD_SHIFT_GROUP"];
				tmmsm10["PROD_SHIFT_NO"] = tmmsm21["PROD_SHIFT_NO"];
				tmmsm10["PROC_DIV"] = v_proc_div;
				tmmsm10["FORE_IP"] = s.fore_ip;
				/*tmmsm10.Update( "REC_REVISOR, "
								"REC_REVISE_TIME,"
								"SMELT_DEV_CODE,"
								"SMELT_START_TIME,"
								"SMELT_END_TIME,"
								"PROD_TIME,"
								"PROD_SHIFT_GROUP,"
								"PROD_SHIFT_NO");*/
				tmmsm10.Insert();
							   
			}
			else if (v_station_id == "A")//AOD
			{
				cmd_sql.Fetch(tmmsm27);
				tmmsm10["SMELT_DEV_CODE"] = v_station_id + tmmsm27["STATION_NO"].ToString();
				tmmsm10["SMELT_START_TIME"] = tmmsm27["START_TIME"];
				tmmsm10["SMELT_END_TIME"] = tmmsm27["END_TIME"];
				tmmsm10["PROD_TIME"] = tmmsm27["START_TIME"];
				tmmsm10["PROD_SHIFT_GROUP"] = tmmsm27["PROD_SHIFT_GROUP"];
				tmmsm10["PROD_SHIFT_NO"] = tmmsm27["PROD_SHIFT_NO"];
				tmmsm10["PROC_DIV"] = v_proc_div;
				tmmsm10["FORE_IP"] = s.fore_ip;
				/*tmmsm10.Update("REC_REVISOR, "
					"REC_REVISE_TIME,"
					"SMELT_DEV_CODE,"
					"SMELT_START_TIME,"
					"SMELT_END_TIME,"
					"PROD_TIME,"
					"PROD_SHIFT_GROUP,"
					"PROD_SHIFT_NO");*/
				tmmsm10.Insert();
			}
			else if (v_station_id == "R")
			{
				cmd_sql.Fetch(tmmsm23);

				v_refine_num = tmmsm23["REFINE_NUM"].ToDecimal().ToInt32();
				v_station_no = tmmsm23["STATION_NO"];
				v_start_time = tmmsm23["START_TIME"];
				v_end_time = tmmsm23["END_TIME"];


			}
			else if (v_station_id == "F")//LF
			{
				cmd_sql.Fetch(tmmsm24);

				v_refine_num = tmmsm24["REFINE_NUM"].ToDecimal().ToInt32();
				v_station_no = tmmsm24["STATION_NO"];
				v_start_time = tmmsm24["START_TIME"];
				v_end_time = tmmsm24["END_TIME"];

			}
			else if (v_station_id == "E" || v_station_id == "X")// 电炉
			{
				cmd_sql.Fetch(tmmsm20);
				tmmsm10["SMELT_DEV_CODE"] = v_station_id + tmmsm20["STATION_NO"].ToString();
				tmmsm10["SMELT_START_TIME"] = tmmsm20["START_TIME"];
				tmmsm10["SMELT_END_TIME"] = tmmsm20["END_TIME"];
				tmmsm10["PROD_TIME"] = tmmsm20["START_TIME"];
				tmmsm10["PROD_SHIFT_GROUP"] = tmmsm20["PROD_SHIFT_GROUP"];
				tmmsm10["PROD_SHIFT_NO"] = tmmsm20["PROD_SHIFT_NO"];
				tmmsm10["PROC_DIV"] = v_proc_div;
				tmmsm10["FORE_IP"] = s.fore_ip;
				tmmsm10.Insert();
				/*tmmsm10.Update("REC_REVISOR, "
					"REC_REVISE_TIME,"
					"SMELT_DEV_CODE,"
					"SMELT_START_TIME,"
					"SMELT_END_TIME,"
					"PROD_TIME,"
					"PROD_SHIFT_GROUP,"
					"PROD_SHIFT_NO");*/

			}
			else if (v_station_id == "Z")//中频炉
			{
				cmd_sql.Fetch(tmmsm19);
				tmmsm10["SMELT_DEV_CODE"] = v_station_id + tmmsm19["STATION_NO"].ToString();
				tmmsm10["SMELT_START_TIME"] = tmmsm19["START_TIME"];
				tmmsm10["SMELT_END_TIME"] = tmmsm19["END_TIME"];
				tmmsm10["PROD_TIME"] = tmmsm19["START_TIME"];
				tmmsm10["PROD_SHIFT_GROUP"] = tmmsm19["PROD_SHIFT_GROUP"];
				tmmsm10["PROD_SHIFT_NO"] = tmmsm19["PROD_SHIFT_NO"];
				tmmsm10["PROC_DIV"] = v_proc_div;
				tmmsm10["FORE_IP"] = s.fore_ip;
				tmmsm10.Insert();
				/*tmmsm10.Update("REC_REVISOR, "
					"REC_REVISE_TIME,"
					"SMELT_DEV_CODE,"
					"SMELT_START_TIME,"
					"SMELT_END_TIME,"
					"PROD_TIME,"
					"PROD_SHIFT_GROUP,"
					"PROD_SHIFT_NO");*/

			}
			else if (v_station_id == "V")//VOD
			{
				cmd_sql.Fetch(tmmsm25);

				v_refine_num = tmmsm25["REFINE_NUM"].ToDecimal().ToInt32();
				v_station_no = tmmsm25["STATION_NO"];
				v_start_time = tmmsm25["START_TIME"];
				v_end_time = tmmsm25["END_TIME"];

			}
			else if (v_station_id == "S")//LTS
			{
				cmd_sql.Fetch(tmmsm26);

				v_refine_num = tmmsm26["REFINE_NUM"].ToDecimal().ToInt32();
				v_station_no = tmmsm26["STATION_NO"];
				v_start_time = tmmsm26["START_TIME"];
				v_end_time = tmmsm26["END_TIME"];

			}
			else if (v_station_id == "C")
			{
				cmd_sql.Fetch(tmmsm31);

				tmmsm10["CC_MACH_NO"] = tmmsm31["STATION_NO"];
				tmmsm10["CC_START_TIME"] = tmmsm31["START_TIME"];
				tmmsm10["CC_END_TIME"] = tmmsm31["END_TIME"];
				tmmsm10["PROC_DIV"] = v_proc_div;
				tmmsm10["FORE_IP"] = s.fore_ip;
				tmmsm10["PROD_SEQ_NO"] = seq;
				tmmsm10.Print();
				Log::Info("", __FUNCTION__, "like  =[{0}]", __LINE__);
				tmmsm10.Insert();
				/*tmmsm10.Update( "REC_REVISOR, "
								"REC_REVISE_TIME,"
								"CC_MACH_NO,"
								"CC_START_TIME,"
								"CC_END_TIME");*/
			}


			if (v_refine_num == 1)
			{
				tmmsm10["SR1_DEV_CODE"] = v_station_id + v_station_id;
				tmmsm10["SR1_START_TIME"] = v_start_time;
				tmmsm10["SR1_END_TIME"]  = v_end_time;
				tmmsm10["PROC_DIV"] = v_proc_div;
				tmmsm10["FORE_IP"] = s.fore_ip;
				tmmsm10.Insert();
				/*tmmsm10.Update( "REC_REVISOR, "
								"REC_REVISE_TIME,"
								"SR1_DEV_CODE,"
								"SR1_START_TIME,"
								"SR1_END_TIME");*/

			}
			if (v_refine_num == 2)
			{

				tmmsm10["SR2_DEV_CODE"] = v_station_id + v_station_id;
				tmmsm10["SR2_START_TIME"] = v_start_time;
				tmmsm10["SR2_END_TIME"] = v_end_time;
				tmmsm10["PROC_DIV"] = v_proc_div;
				tmmsm10["FORE_IP"] = s.fore_ip;
				tmmsm10.Insert();
				/*tmmsm10.Update( "REC_REVISOR, "
								"REC_REVISE_TIME,"
								"SR2_DEV_CODE,"
								"SR2_START_TIME,"
								"SR2_END_TIME");*/

			}
			if (v_refine_num == 3)
			{
				tmmsm10["SR3_DEV_CODE"] = v_station_id + v_station_id;
				tmmsm10["SR3_START_TIME"] = v_start_time;
				tmmsm10["SR3_END_TIME"] = v_end_time;
				tmmsm10["PROC_DIV"] = v_proc_div;
				tmmsm10["FORE_IP"] = s.fore_ip;
				tmmsm10.Insert();
				/*tmmsm10.Update(	"REC_REVISOR, "
								"REC_REVISE_TIME,"
								"SR3_DEV_CODE,"
								"SR3_START_TIME,"
								"SR3_END_TIME");*/
			}
			if (v_refine_num == 4)
			{
				tmmsm10["SR4_DEV_CODE"] = v_station_id + v_station_id;
				tmmsm10["SR4_START_TIME"] = v_start_time;
				tmmsm10["SR4_END_TIME"] = v_end_time;
				tmmsm10["PROC_DIV"] = v_proc_div;
				tmmsm10["FORE_IP"] = s.fore_ip;
				tmmsm10.Insert();
				/*tmmsm10.Update(	"REC_REVISOR, "
								"REC_REVISE_TIME,"
								"SR4_DEV_CODE,"
								"SR4_START_TIME,"
								"SR4_END_TIME");*/

			}

		}
		else        //如果在实绩表中不存在，表示tmmsm10表中相应的信息需要清空
		{
			if (v_station_id == "B" || v_station_id == "Y" || v_station_id == "E" || v_station_id == "X" || v_station_id == "Z"
				||v_station_id == "A")
			{
				tmmsm10["SMELT_DEV_CODE"] = " ";
				tmmsm10["SMELT_START_TIME"] = " ";
				tmmsm10["SMELT_END_TIME"] = " ";
				tmmsm10["PROD_TIME"] = " ";
				tmmsm10["PROD_SHIFT_GROUP"] = " ";
				tmmsm10["PROD_SHIFT_NO"] = " ";

				tmmsm10.Update("REC_REVISOR, "
					"REC_REVISE_TIME,"
					"SMELT_DEV_CODE,"
					"SMELT_START_TIME,"
					"SMELT_END_TIME,"
					"PROD_TIME,"
					"PROD_SHIFT_GROUP,"
					"PROD_SHIFT_NO");

			}
			else if (v_station_id == "C")
			{
				
				tmmsm10["CC_MACH_NO"] = " ";
				tmmsm10["CC_START_TIME"] = " ";
				tmmsm10["CC_END_TIME"] = " ";

				tmmsm10.Update("REC_REVISOR, "
					"REC_REVISE_TIME,"
					"CC_MACH_NO,"
					"CC_START_TIME,"
					"CC_END_TIME");
			}
			else //精炼
			{
				//返回第几重精炼

				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:	    // Oracle 数据库
				default:

					//判断第几重
					sqlstr_ps = " SELECT PROC_NO FROM TPSSM12"
						" WHERE heat_no= @heat_no"
						" AND area_id = 4"
						" ORDER BY CHARGE_NO asc";

					//Log::Info("", __FUNCTION__, "sqlstr_ps      =[{0}]", sqlstr_ps);
					break;
				}


				cmd_inq.SetCommandText(sqlstr_ps);
				cmd_inq.Parameters.Set("heat_no", tmmsm10["HEAT_NO"].ToString());

				cmd_inq.ExecuteReader();
				count = 0;
				while (cmd_inq.Read())
				{
					count++;
					//Log::Trace("", __FUNCTION__, "count		= [{0}]", count);
					if (cmd_inq.GetString(1) == v_proc_no)
					{
						v_refine_num = count;
						break;
					}

				}

				cmd_inq.Close(); //关闭游标

				if (v_refine_num == 1)
				{
					tmmsm10["SR1_DEV_CODE"] = " ";
					tmmsm10["SR1_START_TIME"] = " ";
					tmmsm10["SR1_END_TIME"] = " ";

					tmmsm10.Update("REC_REVISOR, "
						"REC_REVISE_TIME,"
						"SR1_DEV_CODE,"
						"SR1_START_TIME,"
						"SR1_END_TIME");

				}
				if (v_refine_num == 2)
				{
					tmmsm10["SR2_DEV_CODE"] = " ";
					tmmsm10["SR2_START_TIME"] = " ";
					tmmsm10["SR2_END_TIME"] = " ";

					tmmsm10.Update("REC_REVISOR, "
						"REC_REVISE_TIME,"
						"SR2_DEV_CODE,"
						"SR2_START_TIME,"
						"SR2_END_TIME");

				}
				if (v_refine_num == 3)
				{
					tmmsm10["SR3_DEV_CODE"] = " ";
					tmmsm10["SR3_START_TIME"] = " ";
					tmmsm10["SR3_END_TIME"] = " ";

					tmmsm10.Update("REC_REVISOR, "
						"REC_REVISE_TIME,"
						"SR3_DEV_CODE,"
						"SR3_START_TIME,"
						"SR3_END_TIME");
				}
				if (v_refine_num == 4)
				{
					tmmsm10["SR4_DEV_CODE"] = " ";
					tmmsm10["SR4_START_TIME"] = " ";
					tmmsm10["SR4_END_TIME"] = " ";

					tmmsm10.Update("REC_REVISOR, "
						"REC_REVISE_TIME,"
						"SR4_DEV_CODE,"
						"SR4_START_TIME,"
						"SR4_END_TIME");

				}
			}
		}
		cmd_sql.Close();

		if (v_proc_div == "D" && (v_station_id == "B" || v_station_id == "Y" || v_station_id == "E" || v_station_id == "X" || v_station_id == "Z"
			|| v_station_id == "A"))
		{
			tmmsm10.Delete("PONO");
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
