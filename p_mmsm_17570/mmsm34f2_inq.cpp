/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-08-28
Version:1.0
Description: 精整相关信息查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================
/// <summary>
/// 可供精整炼钢板坯信息查询
/// <para>
/// 查询可供精整炼钢板坯信息
/// </para>
/// </summary>
/// <param name="MAT_NO">材料号</param>
/// <param name="PONO">制造命令号</param>
/// <param name="HSF_END_TIME_F">精整结束开始时刻</param>
/// <param name="HSF_END_TIME_T">精整结束结束时刻</param>
/// <param name="MACH_CLEAR_FLAG">精整标记</param>
/// <returns>板坯信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm34f2_inq)


int f_mmsm34f2_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	int rowCount = 0;
	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString ch_hsf_end_time_f = "";
	CString ch_hsf_end_time_t = "";
	CString heat_no = "";
	CString mat_no = "";
	CString in_mat_no = "";
	CString prod_shift_no = "";
	CString prod_shift_group = "";
	CString queryDiv = "";
	CString tableName = "";
	CString factoryDiv = "";
	CString st_no = "";
	CString cc_mach_no = "";
	CString finish_flag = "";
	CString station_id = "";
	CString mend_flag = "";//修磨标记

	//查询标记
	CString flag = "";

	//2024-04-02
	CString pord_seq_no = "";//修磨号
	CDecimal cd_count = 0;

	//2024-03-13
	CString unit_code = "";

	//2024-05-01
	CString mend_set = "";

	CString inner2_c = "";

	//20240419
	CString lgort = "";

	//20240420----添加收货标记查询
	CString rcvFlag = "";

	//2024-05-16--- 修磨班组
	CString mendShift = "";

	//2024-06-12---是否发送
	CString isUpload = "";

	/* 实体类定义 */
	CModel tmmsm34("TMMSM34");

	//CPageInfo pageInfo;

	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	CDbCommand cmd_inq(conn);

	try
	{
		CDateTime datetime = CDateTime::Now();

		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			if (bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_NUM")
				&& bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_SIZE"))
			{
				current_page_no = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_NUM"].ToDecimal().ToInt32() + 1;
				record_count_per_page = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_SIZE"];
			}
			else {
				record_count_per_page = 100000;
				current_page_no = 1;
			}
		}
		else {
			record_count_per_page = 100000;
			current_page_no = 1;
		}

		Log::Trace("", "", "record_count_per_page={0}", record_count_per_page);
		Log::Trace("", "", "current_page_no={0}", current_page_no);

		//try
		//{//获取前台DEV控件传入的分页信息
		//	pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		//}
		//catch (CException& ce)
		//{
		//	pageInfo.RecordFrom = 0;
		//	pageInfo.PageSize = 1000;
		//}


		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_GROUP"))
			prod_shift_group = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_GROUP"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SHIFT_NO"))
			prod_shift_no = bcls_rec->Tables[0].Rows[0]["PROD_SHIFT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();

		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME"))
			ch_hsf_end_time_f = bcls_rec->Tables[0].Rows[0]["PROD_TIME"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME1"))
			ch_hsf_end_time_t = bcls_rec->Tables[0].Rows[0]["PROD_TIME1"].ToString();



		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			factoryDiv = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString();
		//LG1---表示二钢，只对四级起作用
		//factoryDiv = "LG1";
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("CC_MACH_NO"))
			cc_mach_no = bcls_rec->Tables[0].Rows[0]["CC_MACH_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("FINISH_FLAG"))
			finish_flag = bcls_rec->Tables[0].Rows[0]["FINISH_FLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MEND_FLAG"))
			mend_flag = bcls_rec->Tables[0].Rows[0]["MEND_FLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("QUERY_DIV"))
			queryDiv = bcls_rec->Tables[0].Rows[0]["QUERY_DIV"].ToString();

		//2024-03-13
		if (bcls_rec->Tables[0].Columns.Contains("UNIT_CODE"))
		{
			unit_code = bcls_rec->Tables[0].Rows[0]["UNIT_CODE"].ToString();
			mend_set = unit_code;
		}
			

		//2024-04-02 
		if (bcls_rec->Tables[0].Columns.Contains("PROD_SEQ_NO"))
			pord_seq_no = bcls_rec->Tables[0].Rows[0]["PROD_SEQ_NO"].ToString();

		//2024-04-12
		if (bcls_rec->Tables[0].Columns.Contains("FLAG"))
			flag = bcls_rec->Tables[0].Rows[0]["FLAG"].ToString();


		//20240416
		if (bcls_rec->Tables[0].Columns.Contains("INNER2_C"))
			inner2_c = bcls_rec->Tables[0].Rows[0]["INNER2_C"].ToString();

		tableName = queryDiv.Trim();

		//20240419
		if (bcls_rec->Tables[0].Columns.Contains("LGORT"))
		{
			lgort = bcls_rec->Tables[0].Rows[0]["LGORT"].ToString();
		}


		//20240420
		if (bcls_rec->Tables[0].Columns.Contains("RCV_MAT_FLAG"))
		{
			rcvFlag = bcls_rec->Tables[0].Rows[0]["RCV_MAT_FLAG"].ToString();
		}

		//20240515
		if (bcls_rec->Tables[0].Columns.Contains("MEND_SHIFT"))
		{
			mendShift = bcls_rec->Tables[0].Rows[0]["MEND_SHIFT"].ToString();
		}

		//20240612---MMSM37BPELG1S2N（修磨实绩Tab）页面，添加是否发送标记
		if (bcls_rec->Tables[0].Columns.Contains("ISUPLOAD"))
		{
			isUpload = bcls_rec->Tables[0].Rows[0]["ISUPLOAD"].ToString();
		}



		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr_count = " SELECT COUNT(1) FROM " + tableName + " WHERE 1=1 ";
			if (tableName == "TMMSM01")
			{
				sqlstr = " SELECT T1.*,TQ01.APN AS APN_1,TQ01.ORDER_THICK,TQ01.PROD_CLASS_DESC FROM TMMSM01 T1  "
					" LEFT JOIN TQMOM01 TQ01 ON T1.ORDER_NO = TQ01.ORDER_NO "
					" WHERE 1 = 1 ";
			}
			else
			{
				sqlstr = " SELECT * FROM " + tableName + " WHERE 1=1 ";
			}

			if (station_id.Trim() != "")
			{
				if (tableName == "TMMSM01")
				{
					//sqlstr_temp += " AND IC_CC_FLAG = @station_id";
				}
				else if (tableName == "TMMSM34")
				{
					sqlstr_temp += " AND STATION_ID = @station_id";
				}
				else if (tableName == "TPSSM81")
				{

				}
			}

			if (heat_no.Trim() != "")
			{
				sqlstr_temp += " AND HEAT_NO like '%" + heat_no + "%'";
			}
			if (prod_shift_no.Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_NO = @prod_shift_no";
			}
			if (prod_shift_group.Trim() != "")
			{
				sqlstr_temp += " AND PROD_SHIFT_GROUP = @prod_shift_group";
			}
			if (mat_no.Trim() != "")
			{
				sqlstr_temp += " AND MAT_NO like '%" + mat_no + "%'";
			}
			if (ch_hsf_end_time_f.Trim() != "")
			{
				ch_hsf_end_time_f = ch_hsf_end_time_f.Trim() + "000000";
				//这里区分一下，如果是材料信息，时间是生成时刻---20240401：
				if (tableName == "TMMSM01")
				{
					sqlstr_temp += " AND PROD_TIME >= @ch_hsf_end_time_f";
				}
				//这里区分一下，如果是修磨记录，时间是修磨内弧开始时间（暂定）---20240401：：
				else if (tableName == "TMMSM34")
				{
					sqlstr_temp += " AND START_TIME >= @ch_hsf_end_time_f";
				}


			}
			if (ch_hsf_end_time_t.Trim() != "")
			{
				ch_hsf_end_time_t = ch_hsf_end_time_t.Trim() + "235959";
				//这里区分一下，如果是材料信息，时间是生成时刻---20240401：
				if (tableName == "TMMSM01")
				{
					sqlstr_temp += " AND PROD_TIME <= @ch_hsf_end_time_t";
				}
				//这里区分一下，如果是修磨记录，时间是修磨内弧开始时间（暂定）---20240401：：
				else if (tableName == "TMMSM34")
				{
					sqlstr_temp += " AND START_TIME <= @ch_hsf_end_time_t";
				}

			}
			Log::Trace("", __FUNCTION__, "unit_code[{0}]  ", unit_code);
			Log::Trace("", __FUNCTION__, "ch_hsf_end_time_f[{0}]  ", ch_hsf_end_time_f);
			Log::Trace("", __FUNCTION__, "ch_hsf_end_time_t[{0}]  ", ch_hsf_end_time_t);

			if (factoryDiv.Trim() != "")
			{
				sqlstr_temp += " AND factory_div = @factoryDiv";
			}
			if (st_no.Trim() != "")
			{
				sqlstr_temp += " AND st_no = @st_no";

			}
			if (cc_mach_no.Trim() != "")
			{
				if (tableName == "TMMSM01")
				{
					sqlstr_temp += " AND unit_code = @cc_mach_no";
				}
				else if (tableName == "TMMSM34")
				{
					sqlstr_temp += " AND cc_mach_no = @cc_mach_no";
				}
			}
			if (finish_flag.Trim() != "")
			{

				sqlstr_temp += " AND finish_flag = @finish_flag";

			}
			if (mend_flag.Trim() != "")
			{

				sqlstr_temp += " AND MEND_FLAG = @mend_flag";

			}

			//20240402
			if (pord_seq_no != "")
			{
				sqlstr_temp += " AND PROD_SEQ_NO = @pord_seq_no";
			}

			//20240419-----添加TMMSM01表，库存地
			if (lgort != "")
			{
				if (tableName == "TMMSM01")
				{
					sqlstr_temp += " AND LGORT = @lgort";
				}
			}

			//20240420------添加TMMSM01表，收货标记
			if (rcvFlag != "")
			{
				if (tableName == "TMMSM01")
				{
					sqlstr_temp += " AND RCV_MAT_FLAG = @rcvFlag ";
				}
			}

			if (tableName == "TMMSM34")
			{
				//通过IP查，机组，
				CString ip = s.fore_ip;
				CString sqlstrIp = " SELECT t.CODE_DESC_1_CONTENT FROM TWMSMZD02 t  WHERE 1 = 1  AND t.CODE_CLASS = 'MEND_SET_IP'  AND t.CODE = '" + ip + "' ";

				cmd_inq.SetCommandText(sqlstrIp);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					unit_code = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
				//如果小代码MEND_SET_IP，设置IP对应的机组为：ADMIN，表示此电脑能查看所所修磨记录
				/*if (unit_code != "ADMIN")
				{
					sqlstr_temp += " AND MEND_SET = @unit_code ";
				}
				else */
				//if (unit_code == "ADMIN"&&mend_set!=" "&&mend_set!="")
				if (mend_set != " "&&mend_set != "")
				{
					sqlstr_temp += " AND MEND_SET = '" + mend_set + "' ";
				}

				if (unit_code.Trim() != "")
				{
					//sqlstr_temp += " AND MEND_SET = @unit_code ";
				}
			}

			//20240412 flag==7 = 3+4 表示再磨
			if (flag.Trim() != ""&&flag == "7")
			{
				sqlstr_temp += " AND MEND_FLAG IN ('3','4') ";
			}
			//20240412 flag==3 = 1+2 表示初磨
			else if (flag.Trim() != ""&&flag == "3")
			{
				sqlstr_temp += " AND MEND_FLAG IN ('1','2') ";
			}

			//20240416 -----添加这个判断的目的是，过滤再磨材料，保MMSM37BPEINNER2S2N查出的数据，都是可以再磨的
			if (inner2_c.Trim() != ""&&inner2_c == "INNER2_C")
			{
				sqlstr_temp += " AND MEND_FLAG in ('1','2','3','4','9')   ";
			}

			//20240515----添加修磨班组
			if (mendShift.Trim() != ""&&mendShift!=" ")
			{
				if (tableName == "TMMSM34")
				{
					sqlstr_temp += " AND MEND_SHIFT = '" + mendShift + "'  ";
				}
			}


			//COMPLEX_DECIDE_CODE----综判标记
			if (tableName == "TMMSM01")
			{
				//sqlstr_temp += " AND MAT_LINE_TYPE = 'SM' AND IN_FLAG = '1'";
				//修磨条件：收货标记为 rcv_mat_flag = S
				//添加条件，不能查出已装车的坯子  mfj  20240309

				//sqlstr_temp += " AND MAT_LINE_TYPE = 'SM' AND IN_FLAG = '1' AND rcv_mat_flag = 'S' AND  LOGISTICS_STATUS ='0'  AND (C_STATESIGN = '0' OR C_STATESIGN =' ' OR C_STATESIGN ='6')";
				
				//sqlstr_temp += " AND MAT_LINE_TYPE = 'SM' AND IN_FLAG = '1' AND  LOGISTICS_STATUS in ('0','1','4')  AND (C_STATESIGN = '0' OR C_STATESIGN =' ' OR C_STATESIGN ='6')";
				//sqlstr_temp += " AND MAT_LINE_TYPE = 'SM'  AND  LOGISTICS_STATUS in ('0','1','4')  AND (C_STATESIGN = '0' OR C_STATESIGN =' ' OR C_STATESIGN ='6')";
				//sqlstr_temp += " AND MAT_LINE_TYPE = 'SM' AND (IN_FLAG = '1' or IN_FLAG='3') AND  LOGISTICS_STATUS in ('0','1','2','4')  AND (C_STATESIGN = '0' OR C_STATESIGN =' ' OR C_STATESIGN ='6' or  C_STATESIGN ='1' )";
				sqlstr_temp += " AND MAT_LINE_TYPE = 'SM' AND (C_STATESIGN = '0' OR C_STATESIGN =' ' OR C_STATESIGN ='6')";

			}

			if (tableName == "TPSSM81")
			{

				sqlstr_temp += "  AND PLAN_TYPE = 'SMJ'  AND PLAN_STATUS = '08' ";

			}


			sqlstr_count = sqlstr_count + sqlstr_temp;

			if (tableName == "TMMSM01")
			{

				sqlstr_temp += " ORDER BY PROD_TIME desc ";
			}
			else if (tableName == "TMMSM34")
			{
				//sqlstr_temp += " ORDER BY PROD_SEQ_NO DESC,MAT_NO ASC ";
				sqlstr_temp += " order by START_TIME desc  ";
			}
			else if (tableName == "TPSSM81")
			{
				sqlstr_temp += " ORDER BY REC_CREATE_TIME DESC,MAT_NO ASC ";
			}
			else if (tableName == "TMMSM34_1")
			{
				if (isUpload == "-1")
				{
					sqlstr_temp += "  AND ISUPLOAD = '-1'  ";
				}
				sqlstr_temp += " order by REC_CREATE_TIME DESC ";
			}


			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		//cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.Parameters.Set("prod_shift_no", prod_shift_no);
		cmd_inq.Parameters.Set("prod_shift_group", prod_shift_group);
		//cmd_inq.Parameters.Set("mat_no", mat_no);
		cmd_inq.Parameters.Set("ch_hsf_end_time_f", ch_hsf_end_time_f);
		cmd_inq.Parameters.Set("ch_hsf_end_time_t", ch_hsf_end_time_t);
		cmd_inq.Parameters.Set("factoryDiv", factoryDiv);
		cmd_inq.Parameters.Set("st_no", st_no);
		cmd_inq.Parameters.Set("cc_mach_no", cc_mach_no);
		cmd_inq.Parameters.Set("finish_flag", finish_flag);
		cmd_inq.Parameters.Set("mend_flag", mend_flag);
		cmd_inq.Parameters.Set("pord_seq_no", pord_seq_no);
		cmd_inq.Parameters.Set("rcvFlag", rcvFlag);
		

		//2024-04-19 设置库存地
		cmd_inq.Parameters.Set("lgort", lgort);

		if (tableName == "TMMSM34")
		{
			cmd_inq.Parameters.Set("unit_code", unit_code);
		}
		cmd_inq.Parameters.Set("station_id", station_id);

		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}

		Log::Trace("", __FUNCTION__, "sqlstr[{0}]  ", sqlstr);

		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();	

		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count;

		//返回分页总数量信息 
		//bcls_ret->Tables.Add("PageInfo");
		//bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		//bcls_ret->Tables["PageInfo"].Rows.Add();
		//bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

		//返回提示栏信息
		CFormattable arguments[] = { cd_count }; // 定义参数列表的数组
		CMessageFormat::Format(s.msg, "查询到数据[{0}]条。", arguments, 1); //查询到[{0}]条记录。
		CString ts = ((CDecimal)(CDateTime::Now() - datetime).TotalMilliseconds()).Round(0).ToString();
		CFormattable arguments2[] = { ts };
		CMessageFormat::Format(s.sysmsg, "SVC用时[{0}ms]", arguments2, 1);
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
