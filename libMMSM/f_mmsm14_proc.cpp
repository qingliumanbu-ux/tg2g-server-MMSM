/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-07-04
Description: 炼钢脱硫实绩处理
***********************************************************************/


/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 

 

int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//L4发送
int f_mmsm009c_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//集控大屏写表
int f_t8f002_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源
int f_t823s2_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量
int f_t823s1_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量(三脱)
BM2_FUNCTION_EXPORT
int f_mmsm14_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_proc_div = "";
	CString v_pract_coll_mode = "";
	CString v_factory_div = "";
	CString pro_seq = "";//处理号后五位流水
	CDbCommand cmd_id(conn);

	
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel tmmsm14("TMMSM14");
	
 
 	try
	{

		if (!bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["PARA"].Columns.Add(DT_STRING, "PROC_DIV");
			Log::Info("", __FUNCTION__, "v_proc_div1=[{0}]", bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString());
		}
		v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString();
		Log::Info("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		v_pract_coll_mode = bcls_rec->Tables["PARA"].Rows[0]["PRACT_COLL_MODE"].ToString();
		v_factory_div = bcls_rec->Tables["PARA"].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm14.Reset();
			if (!bcls_rec->Tables[0].Columns.Contains("ID_SJ"))
			{
				bcls_rec->Tables[0].Columns.Add(DT_STRING, "ID_SJ");
			}
			tmmsm14.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (tmmsm14["L2_PROC_NO"].ToString() == " "){
				tmmsm14["L2_PROC_NO"] = tmmsm14["PROC_NO"];
			}
			if (tmmsm14["PROC_NO"].ToString() == " "){
				tmmsm14["PROC_NO"] = tmmsm14["L2_PROC_NO"];
			}
			tmmsm14["FACTORY_DIV"] = "LG1";
			//如果处理号为空，则生成  规则为  工序ID(1,B/A/E-电炉/D-脱硫/H-倒罐/F-合金融化)+设备站号（1）+年号+5位流水。
			if (tmmsm14["PROC_NO"].ToString().Trim() == "")
			{
				cmd_inq.SetCommandText("SELECT LPAD(TO_CHAR(TMMSM14_PROC.NEXTVAL), 5, '0') FROM DUAL");
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					pro_seq = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
				tmmsm14["PROC_NO"] = tmmsm14["STATION_ID"].ToString() + 
					tmmsm14["STATION_NO"].ToString() + dateNow.Substring(3, 1) + pro_seq;
				Log::Trace("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm14["PROC_NO"].ToString());
			}

			if (tmmsm14["DEV_CODE"].ToString().Trim() == "")
			{
				tmmsm14["DEV_CODE"] = tmmsm14["STATION_ID"].ToString() + tmmsm14["STATION_NO"].ToString();
			}


			tmmsm14["FACTORY_DIV"] = v_factory_div;
			tmmsm14.TrimOrBlank();
			if (tmmsm14["START_TIME"].ToString().GetLength() != 14)
			{
				tmmsm14["START_TIME"] = dateNow;
			}
			if (tmmsm14["END_TIME"].ToString().GetLength() != 14)
			{
				tmmsm14["END_TIME"] = dateNow;
			}

			//生产日期暂时定为取开始时刻。  mfj  20231120
			if (tmmsm14["START_TIME"].ToString().Trim() != "" && tmmsm14["PROD_DATE"].ToString().Trim() == "")
			{
				tmmsm14["PROD_DATE"] = tmmsm14["START_TIME"].ToString().SubstringNE(0,8);
			}

			if (tmmsm14["PROD_DATE"].ToString().Trim() != "")
			{
				tmmsm14["PROD_DATE"] = tmmsm14["PROD_DATE"].ToString().Substring(0, 8);
			}

			if (tmmsm14["PROD_SHIFT_NO"].ToString().Trim() == "" )
			{
				//f_epep_get_shift_group("SM", tmmsm14["START_TIME"].ToString(), tmmsm14["PROD_SHIFT_NO"].ToString(), tmmsm14["PROD_SHIFT_GROUP"].ToString(), conn);
				CString PROD_SHIFT_NO = "";
				CString PROD_SHIFT_GROUP = "";
				f_epep_get_shift_group("SMDD", tmmsm14["END_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
				tmmsm14["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
			}

			//if (tmmsm14["DE_S_DURATION"].ToDecimal() == 0)
			//{
			//	switch (conn->DatabaseKind)
			//	{
			//	case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//	case DB_KIND_ORACLE:        // Oracle 数据库
			//	default:
			//		sqlstr = CString(
			//			"SELECT round((To_date(@end_time , 'yyyy-mm-dd hh24-mi-ss') - To_date(@start_time , 'yyyy-mm-dd hh24-mi-ss'))*24*60) "
			//			"from dual"
			//			);
			//		break;
			//	}
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Set("end_time", tmmsm14["END_TIME"].ToString());
			//	cmd_inq.Parameters.Set("start_time", tmmsm14["START_TIME"].ToString());
			//	cmd_inq.ExecuteReader();
			//	if (cmd_inq.Read())
			//	{
			//		tmmsm14["DE_S_DURATION"] = cmd_inq.GetDecimal(1);
			//	}
			//	cmd_inq.Close();
			//}

			tmmsm14.Print();

			if (v_proc_div == "I")
			{
				tmmsm14["REC_CREATE_TIME"] = dateNow;
				tmmsm14["REC_CREATOR"] = s.userid;
				tmmsm14["STATION_ID"] = "D";//设备类型固定为D   必写，不然加料维护报错  mfj  20231121
				tmmsm14.Insert();
			}
			else if (v_proc_div == "U")
			{
				//取原记录的创建时间和创建人
				sqlstr = " SELECT REC_CREATE_TIME, REC_CREATOR  "
					"		FROM	TMMSM14 "
					"   WHERE  PROC_NO	= @proc_no";

				//Log::Info("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);

				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Clear();
				cmd_sql.Parameters.Set("proc_no", tmmsm14["PROC_NO"].ToString());
				cmd_sql.ExecuteReader();

				if (cmd_sql.Read())
				{
					tmmsm14["REC_CREATE_TIME"] = cmd_sql.GetString(1);
					tmmsm14["REC_CREATOR"] = cmd_sql.GetString(2);
				}
				cmd_sql.Close();

				tmmsm14["REC_REVISE_TIME"] = dateNow;
				tmmsm14["REC_REVISOR"] = s.userid;
				Log::Info("", __FUNCTION__, "PROC_NO=[{0}]", tmmsm14["PROC_NO"].ToString());
				tmmsm14.Delete("PROC_NO");
				tmmsm14.Insert();

			}
			else if (v_proc_div == "D")
			{
				tmmsm14.Delete();
			}


			//调用发送电文
			Log::Trace("", __FUNCTION__, "集控大屏=[{0}]", v_proc_div);
			blkNum = bcls_rec->Tables.IndexOf("JKDP");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("JKDP");
			}

			if (!bcls_rec->Tables["JKDP"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			if (!bcls_rec->Tables["JKDP"].Columns.Contains("PROC_NO"))
			{
				bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "PROC_NO");
			}


			if (!bcls_rec->Tables["JKDP"].Columns.Contains("TABLE_NAME"))
			{
				bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "TABLE_NAME");
			}

			bcls_rec->Tables["JKDP"].Rows.Add();
			bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = v_proc_div;
			bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tmmsm14["PROC_NO"].ToString();
			bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_DES_PRO_SUMMARY";

			doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}


			#if defined(_SYS_PES)
			//调用发送电文
			blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MMSMSND");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_NO"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "PROC_NO");
			}

			bcls_rec->Tables["MMSMSND"].Rows.Add();

			//
			if (tmmsm14["STATION_NO"].ToString().Trim() != " ")
			{
				//三脱
				bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM14";//210013
				
			}
			
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tmmsm14["PROC_NO"];

			doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			#endif

			//调用发送电文
			blkNum = bcls_rec->Tables.IndexOf("T8F");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("T8F");
			}

			if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_NO"))
			{
				bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_NO");
			}

			if (!bcls_rec->Tables["T8F"].Columns.Contains("HEAT_NO"))
			{
				bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "HEAT_NO");
			}

			bcls_rec->Tables["T8F"].Rows.Add();
			bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = v_proc_div;
			bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"] = tmmsm14["PROC_NO"];
			bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tmmsm14["HEAT_NO"];


			doFlag = f_t8f002_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//调用智慧质量发送电文
			if (tmmsm14["HEAT_NO"].ToString() != " "){
				blkNum = bcls_rec->Tables.IndexOf("T823S");
				if (blkNum < 0)
				{
					bcls_rec->Tables.Add("T823S");
				}

				if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
				{
					bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
				}

				if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
				{
					bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
				}

				if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
				{
					bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
				}

				bcls_rec->Tables["T823S"].Rows.Add();
				bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = v_proc_div;
				bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm14["PROC_NO"];
				bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm14["HEAT_NO"];
				if (tmmsm14["STATION_NO"].ToString().Trim() == "1")
				{
					doFlag = f_t823s1_snd(bcls_rec, bcls_ret, conn);
				}
				else{
					doFlag = f_t823s2_snd(bcls_rec, bcls_ret, conn);
				}
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

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
