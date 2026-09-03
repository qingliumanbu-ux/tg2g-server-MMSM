/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:
Date:     2023/3/11
Description: 查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/

// service入口
BM2F_ENTERACE(mmsmgycs_inq1)

int f_mmsmgycs_inq1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	CString sqlstr_order = "";
	int TotalRecordCount = 0;

	CString table_name = "";
	CString v_heat_no = "";
	CString v_st_no = "";
	CString v_dev_code = "";
	CString v_mat_no = "";
	CString rec_create_time = "";
	CString v_prod_time_from = "";
	CString v_prod_time_to = ""; 

	CString vapply = "";
	CString vmatno = "";
	CString vstatus = "";
	CString vcarno = "";
	CString v_area = "";


	//系统的分页类信息。
	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);
	CString rec_create_time_1 = "";

	CModel twmsmzhzll("TWMSMZHZLLL");

	try
	{
		try
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME"))
			table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("START_TIME"))
			rec_create_time = bcls_rec->Tables[0].Rows[0]["START_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("END_TIME"))
			rec_create_time_1 = bcls_rec->Tables[0].Rows[0]["END_TIME"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("DEV_CODE"))
			v_dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_FROM"))
			v_prod_time_from = bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("PROD_TIME_TO"))
			v_prod_time_to = bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("PURCHASEDOCID"))
			vapply = bcls_rec->Tables[0].Rows[0]["PURCHASEDOCID"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
			vmatno = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("STATUS"))
			vstatus = bcls_rec->Tables[0].Rows[0]["STATUS"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("CAR_NO"))
			vcarno = bcls_rec->Tables[0].Rows[0]["CAR_NO"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("AREA"))
			v_area = bcls_rec->Tables[0].Rows[0]["AREA"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "rec_create_time  =[{0}]", rec_create_time);
		//Log::Info("", __FUNCTION__, "prod_time_t  =[{0}]", prod_time_t);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			if (table_name == "TQMTSFGB"){
				//sqlstr_count = " SELECT COUNT(1) "
				//"  FROM TMMSM21 "
				//"  WHERE 1=1 "
				//;
				sqlstr = " SELECT T.REC_CREATE_TIME,T.HEAT_NO,T.SEQ_NO,T.WT_SCARP_RC,T.WT_SCARP_RCS FROM TQMTSFGB T WHERE 1 = 1 "
					;

				if (v_heat_no != ""){
					sqlstr_temp += " AND HEAT_NO = '" + v_heat_no + "' ";
				}

				sqlstr_order = " ORDER BY REC_CREATE_TIME DESC ";

			}

			if (table_name == "TQMTSGZGCCFS2N"){
				sqlstr = " select c.REC_CREATE_TIME, c.ELM_NAME, b.ST_NO, c.MAIN_MAX, c.MAIN_MIN, c.MAIN_AIM, c.SPE_MAX,c.SPE_MIN,b.ELM_STD_IDX_A "
					" from( "
					" select IDX_NO, ELM_NAME from TQMTS02 where 1 = 1 group by IDX_NO, ELM_NAME having count(1) > 1) a "
					" left join tqmts0x b on a.IDX_NO = b.ELM_STD_IDX_A or a.IDX_NO = b.ELM_STD_IDX_B "
					" left join TQMTS02 c on a.IDX_NO = c.IDX_NO and a.ELM_NAME = c.ELM_NAME "
					" where 1 = 1 and b.ST_NO is not null "
					;
				if (v_st_no != ""){
					sqlstr_temp += " AND b.ST_NO = '" + v_st_no + "' ";
				}

				sqlstr_order = " order by a.IDX_NO, a.ELM_NAME ";

			}
			if (table_name == "TMMSMT823SJ"){
				sqlstr = " SELECT * FROM TMMSMT823SJ "
					" WHERE 1 = 1 "
					;
				if (v_dev_code != ""){
					sqlstr_temp += " AND DEV_CODE LIKE '%" + v_dev_code + "%'";
				}
				if (v_heat_no != ""){
					sqlstr_temp += " AND HEAT_NO LIKE '%" + v_heat_no + "%'";
				}
				if (v_st_no != ""){
					sqlstr_temp += " AND ST_NO LIKE '%" + v_st_no + "%'";
				}
				if (rec_create_time != ""){
					sqlstr_temp += " AND REC_CREATE_TIME >='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND REC_CREATE_TIME <='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY REC_CREATE_TIME ";
			}
			if (table_name == "TWMSM64"){
				sqlstr = " SELECT * FROM TWMSM64 "
					" WHERE 1 = 1 "
					;
				if (rec_create_time != ""){
					sqlstr_temp += " AND REC_CREATE_TIME >='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND REC_CREATE_TIME <='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " ORDER BY REC_CREATE_TIME ";
			}
			if (table_name == "TMMSM3E"){
				sqlstr = " SELECT * FROM TMMSM3E "
					" WHERE 1 = 1 "
					;
				if (v_mat_no != ""){
					sqlstr_temp += " AND MAT_NO ='" + v_mat_no + "' ";
				}
				if (v_heat_no != ""){
					sqlstr_temp += " AND HEAT_NO ='" + v_heat_no + "' ";
				}
				if (rec_create_time != ""){
					sqlstr_temp += " AND REC_CREATE_TIME >='" + rec_create_time + "' ";
				}
				if (rec_create_time_1 != ""){
					sqlstr_temp += " AND REC_CREATE_TIME <='" + rec_create_time_1 + "' ";
				}
				sqlstr_order = " order by MAT_NO,RESUME_SEQ_NO";
			}
			if (table_name == "TWMSMZHZLLL"){
				sqlstr = " SELECT * FROM TWMSMZHZLLL t "
					" WHERE 1 = 1 "
					;
				if (v_dev_code != ""){
					sqlstr_temp += " AND DEV_CODE LIKE '%" + v_dev_code + "%'";
				}
				sqlstr_order = " order by TIME_STAMPS desc";
			}

			//2026.03.27 先取系统重量,为0时取三级重量
			//2025.10.20 增加删除废坯记录报表
			//2026.06.23 增加操作人列
			if (table_name == "MMSMSCFPS2N"){
				sqlstr = " SELECT  t.*,re.cname,\
				    CASE WHEN COUNT(t.mat_no) OVER (PARTITION BY t.mat_no) > 1 THEN 1 ELSE 0 END AS is_duplicate,\
					CASE\
					WHEN MEASURE_WT = 0 THEN\
					CASE\
					WHEN t.event_id = 'MM04' THEN t.L3_CALTHEROY_WT\
					WHEN t.event_id = 'MM05' THEN -t.L3_CALTHEROY_WT\
					ELSE t.L3_CALTHEROY_WT\
					END\
					ELSE\
					CASE\
					WHEN t.event_id = 'MM04' THEN t.MEASURE_WT\
					WHEN t.event_id = 'MM05' THEN -t.MEASURE_WT\
					ELSE t.MEASURE_WT\
					END\
					END AS MEASURE_WT_1\
					FROM(SELECT * FROM VMMSM96 WHERE 1 = 1  "
					;
				
				if (v_prod_time_from != ""){
					sqlstr_temp += " AND REC_CREATE_TIME >= '" + v_prod_time_from + "'";
				}
				if (v_prod_time_to != ""){
					sqlstr_temp += " AND REC_CREATE_TIME <= '" + v_prod_time_to + "'";
				}
				if (v_mat_no != ""){
					sqlstr_temp += " AND MAT_NO LIKE '%" + v_mat_no + "%'";
				}

				sqlstr_temp += " )T left join tesuserinfo re on t.rec_creator = re.ename\
					WHERE(EVENT_ID = 'MM04' AND FUNC_ID = 'mmsm01a1f5_del')\
					OR(EVENT_ID = 'MM05' AND FUNC_ID = 'mmsm01a1f6_pro_new') ";

				sqlstr_order = " order by RESUME_SEQ_NO desc";
			}

			//2025.11.17 增加铁料装车实绩
			if (table_name == "MMSM67INQS2N"){
				sqlstr = " SELECT a.*,b.*  "
				"   FROM TMMSM67 a, TWMSM61 b "
				"  WHERE 1=1 and a.purchasedocid = b.plan_no(+)";

				if (v_prod_time_from != ""){
					sqlstr_temp += " AND a.APTIME >= '" + v_prod_time_from + "'";
				}
				if (v_prod_time_to != ""){
					sqlstr_temp += " AND a.APTIME <= '" + v_prod_time_to + "'";
				}
				
				if (vapply.Trim() != "")
				{
					sqlstr_temp += " AND a.PURCHASEDOCID like '%" + vapply + "%'";
				}
				if (vmatno.Trim() != "")
				{
					sqlstr_temp += " AND a.mat_code like '" + vmatno + "%'";
				}
				if (vstatus.Trim() != "")
				{
					sqlstr_temp += " AND a.status = '" + vstatus + "'";
				}
				if (vcarno.Trim() != "")
				{
					sqlstr_temp += " AND  exists (select 1 from twmsm61 t where t.plan_no = a.purchasedocid and t.truck_no like '%" + vcarno.Trim() + "%' and t.archive_flag = '1' )  ";
				}

				sqlstr_order = " order by a.REC_CREATE_TIME DESC";
			}

			//2026.01.21 增加修磨初磨外弧维护
			if (table_name == "TMMSMOUT1WH"){
				sqlstr = " SELECT *  "
					"  FROM TMMSMOUT1WH "
					"  WHERE 1=1 ";

				if (v_prod_time_from != ""){
					sqlstr_temp += " AND SLAB_CUT_TIME >= '" + v_prod_time_from + "'";
				}
				if (v_prod_time_to != ""){
					sqlstr_temp += " AND SLAB_CUT_TIME <= '" + v_prod_time_to + "'";
				}
				if (v_mat_no != ""){
					sqlstr_temp += " AND MAT_NO LIKE '%" + v_mat_no + "%'";
				}
				if (v_st_no != ""){
					sqlstr_temp += " AND ST_NO LIKE '%" + v_st_no + "%'";
				}

				sqlstr_order = " order by SLAB_CUT_TIME DESC";
			}

			//2026.01.21 增加废品明细碳钢导入查询
			if (table_name == "TMMSMFP_TG"){
				sqlstr = " SELECT *  "
					"  FROM TMMSMFP_TG "
					"  WHERE 1=1 ";

				if (v_prod_time_from != ""){
					sqlstr_temp += " AND REC_CREATE_TIME >= '" + v_prod_time_from + "'";
				}
				if (v_prod_time_to != ""){
					sqlstr_temp += " AND REC_CREATE_TIME <= '" + v_prod_time_to + "'";
				}
				if (v_heat_no != ""){
					sqlstr_temp += " AND HEAT_NO1 LIKE '%" + v_heat_no + "%'";
				}
				if (v_area != ""){
					sqlstr_temp += " AND AREA LIKE '%" + v_area + "%'";
				}

				sqlstr_order = " order by REC_CREATE_TIME DESC";
			}

			//2026.01.21 增加废品明细不锈钢导入查询
			if (table_name == "TMMSMFP_BX"){
				sqlstr = " SELECT *  "
					"  FROM TMMSMFP_BX "
					"  WHERE 1=1 ";

				if (v_prod_time_from != ""){
					sqlstr_temp += " AND REC_CREATE_TIME >= '" + v_prod_time_from + "'";
				}
				if (v_prod_time_to != ""){
					sqlstr_temp += " AND REC_CREATE_TIME <= '" + v_prod_time_to + "'";
				}
				if (v_heat_no != ""){
					sqlstr_temp += " AND HEAT_NO LIKE '%" + v_heat_no + "%'";
				}
				if (v_area != ""){
					sqlstr_temp += " AND AREA LIKE '%" + v_area + "%'";
				}

				sqlstr_order = " order by REC_CREATE_TIME DESC";
			}

			//2026.04.08 增加废品统计周报表查询
			if (table_name == "TMMSMFPTJ"){
				sqlstr = " SELECT *  "
					"  FROM TMMSMFPTJ "
					"  WHERE 1=1 ";

				if (v_prod_time_from != ""){
					sqlstr_temp += " AND REC_CREATE_TIME >= '" + v_prod_time_from + "'";
				}
				if (v_prod_time_to != ""){
					sqlstr_temp += " AND REC_CREATE_TIME <= '" + v_prod_time_to + "'";
				}
				if (v_heat_no != ""){
					sqlstr_temp += " AND HEAT_NO LIKE '%" + v_heat_no + "%'";
				}
				if (v_area != ""){
					sqlstr_temp += " AND AREA LIKE '%" + v_area + "%'";
				}

				sqlstr_order = " order by REC_CREATE_TIME DESC";
			}

			sqlstr = sqlstr + sqlstr_temp + sqlstr_order;
			break;
		}
		Log::Info("", __FUNCTION__, "sqlstr  =[{0}]", sqlstr);

		//cmd_inq.SetCommandText(sqlstr_count);
		//TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		//处理报警语音 播报过的VOICE_ALARM_FLAG置1
		if (table_name == "TWMSMZHZLLL")
		{
			bcls_ret->Tables.Add();
			bcls_ret->Tables[1].set_TableName("TableVA");
			bcls_ret->Tables[1].Columns.Add(DT_STRING, "REMARK");
			bcls_ret->Tables[1].Rows.Add();
			if (bcls_ret->Tables[0].Rows[0]["VOICE_ALARM_FLAG"].ToString() != "1")
			{
				twmsmzhzll["IDCARD"] = bcls_ret->Tables[0].Rows[0]["IDCARD"].ToString();
				twmsmzhzll["VOICE_ALARM_FLAG"] = "1";
				twmsmzhzll.Update("VOICE_ALARM_FLAG", "IDCARD");
				Log::Info("", __FUNCTION__, "IDCARD =[{0}]", bcls_ret->Tables[0].Rows[0]["IDCARD"].ToString());
			}
		}
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
