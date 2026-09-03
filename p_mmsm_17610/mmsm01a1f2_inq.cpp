/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     周越麒
Version:    1.0
Date:       2023年5月29日
Description: 炼钢钢坯材料信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

//业务头文件

//外部函数声明

BM2F_ENTERACE(mmsm01a1f2_inq) 

int f_mmsm01a1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;

	/*业务变量*/
	CString sqlstr;
	CString sqlstr_count;
	CString sqlstr_condition = "";
	CString sqlstr_order = "";
	int TotalRecordCount = 0;
	CPageInfo pageInfo;

	CDbCommand cmd_inq(conn);

	CModel tmmsm01("TMMSM01");
	CString tbl = "";

	try
	{
		CDateTime datetime = CDateTime::Now();

		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		//获取传入参数
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//判断在线历史类型
		if (tmmsm01["ARCHIVE_FLAG"].ToString().Trim() == "T") tbl = " TMMSM01 A";
		else if (tmmsm01["ARCHIVE_FLAG"].ToString().Trim() == "H") tbl = " HMMSM01 A";
		else tbl = " (SELECT * FROM TMMSM01 UNION SELECT * FROM HMMSM01) A";

		//添加校验， 对在线历史数据一起查的情况下，必须要卡时间条件
		if (tmmsm01["ARCHIVE_FLAG"].ToString().Trim() == "S")
		{
			CString v_prod_time_from = 	bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();
			CString v_prod_time_to = bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();
			if (v_prod_time_from == "" || v_prod_time_to == "")
			{
				sprintf(s.msg, "当查询在线和历史数据时，生产日期不能为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			CDecimal time_span = bcls_rec->Tables[0].Rows[0]["TIME_SPAN"].ToDecimal();

			Log::Trace("", __FUNCTION__, "TIME_SPAN= [{0}]", time_span);

			CDecimal v_days = (CDateTime::Parse(v_prod_time_to) - CDateTime::Parse(v_prod_time_from)).TotalDays();

			Log::Trace("", __FUNCTION__, "v_days= [{0}]", v_days);

			if (time_span >0)
			{
				if (v_days > time_span)
				{
					sprintf(s.msg, "当查询在线和历史数据时，日期范围不能超过[%s]天！", (const char*)bcls_rec->Tables[0].Rows[0]["TIME_SPAN"].ToDecimal().ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			

		}



		/* 查询材料信息 */   
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr_count =
					" SELECT COUNT(1)"
					" FROM" + tbl +
					" WHERE 1 = 1";

				sqlstr =
					" SELECT A.*,DECODE(substr(TQ01.ORDER_NO,0,1),'A',TQ01.TRNP_MODE_CODE,' ') TRNP_MODE_CODE_1,decode(t2.MAT_NO,null,'0','1') need_mend,nvl(t2.MEND_CAUSE,' ')MEND_CAUSE,TQ01.ORDER_THICK,TQ01.PROD_CLASS_DESC,CASE WHEN A.RCV_MAT_FLAG = 'N' THEN ' ' WHEN A.measure_wt = A.receive_weight THEN '1' ELSE '0' END AS avlb_flag1 "
					//" SELECT MAT_NO,MAT_SHAPE_FLAG,MAT_STATUS,MAT_THICK,MAT_WIDTH,MAT_LEN,SLAB_HEAD_WIDTH,SLAB_TAIL_WIDTH,MAT_NUM,MAT_WT,MEASURE_WT_FLAG,PONO,HEAT_NO,STOCK_NO,IN_FLAG,ORDER_NO,HOLD_FLAG,PRODUCT_FLAG,ORIGIN_MAT_NO,ST_NO,SG_SIGN,SG_STD,WHOLE_BACKLOG,MSC,PROD_TIME,PSC,MAT_ACT_THICK,MAT_ACT_WIDTH,MAT_ACT_LEN"
					" FROM" + tbl +
					"  LEFT JOIN TQMOM01 TQ01 ON  A.ORDER_NO =TQ01.ORDER_NO   "
					"  LEFT JOIN get_mend_flag t2 ON  A.mat_no =t2.MAT_NO  "
					" WHERE 1 = 1";

				//获取查询条件
				{
					CModel& model = tmmsm01;
					CString tbl_sname = "A";
					CString col_name = "";

					for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
					{
						col_name = bcls_rec->Tables[0].Columns[i].get_ColumnName();
						if (!model.GetFields().Contains(col_name)) continue;

						//跳过不需要的条件
						if (col_name == "MAT_NO") continue;
						if (col_name == "ARCHIVE_FLAG") continue;
						if (col_name == "UNIT_CODE") continue;

						//Log::Info("", __FUNCTION__, "col_name[{0}][{1}][{2}]", col_name, bcls_rec->Tables[0].Rows[0][col_name].ToString(), model.GetFields()[col_name].ColumnType);
						//2025.11.18 查询条件增加制造命令号,可模糊查询
						if (col_name == "PONO" && bcls_rec->Tables[0].Rows[0][col_name].ToString().Trim() != "")
						{
							sqlstr_condition += " AND " + tbl_sname + "." + col_name + " like '%" + bcls_rec->Tables[0].Rows[0][col_name].ToString().Trim() + "%'";
							cmd_inq.Parameters.Set(col_name, bcls_rec->Tables[0].Rows[0][col_name].ToString().Trim());
						}
						else{
							if (model.GetFields()[col_name].ColumnType == DT_STRING && bcls_rec->Tables[0].Rows[0][col_name].ToString().Trim() != "")
							{
								sqlstr_condition += " AND " + tbl_sname + "." + col_name + " = @" + col_name;
								cmd_inq.Parameters.Set(col_name, bcls_rec->Tables[0].Rows[0][col_name].ToString().Trim());
							}
							else if (model.GetFields()[col_name].ColumnType == DT_DECIMAL && bcls_rec->Tables[0].Rows[0][col_name].ToDecimal() != 0)
							{
								sqlstr_condition += " AND " + tbl_sname + "." + col_name + " = @" + col_name;
								cmd_inq.Parameters.Set(col_name, bcls_rec->Tables[0].Rows[0][col_name].ToDecimal());
							}
						}
					}

					//额外的条件
					sqlstr_condition += " AND A.MAT_LINE_TYPE = 'SM'";

					col_name = "MAT_NO";
					if (bcls_rec->Tables[0].Rows[0][col_name].ToString().Trim() != "")
					{
						Log::Info("", __FUNCTION__, "col_name[{0}][{1}]", col_name, bcls_rec->Tables[0].Rows[0][col_name].ToString());
						sqlstr_condition += " AND A." + col_name + " IN ('" + bcls_rec->Tables[0].Rows[0][col_name].ToString().Trim() + "')";
					}

					col_name = "UNIT_CODE";
					if (bcls_rec->Tables[0].Rows[0][col_name].ToString().Trim() != "")
					{
						bcls_rec->Tables[0].Rows[0][col_name] = bcls_rec->Tables[0].Rows[0][col_name].ToString().Replace(",", "','");
						Log::Info("", __FUNCTION__, "col_name[{0}][{1}]", col_name, bcls_rec->Tables[0].Rows[0][col_name].ToString());
						sqlstr_condition += " AND A." + col_name + " IN ('" + bcls_rec->Tables[0].Rows[0][col_name].ToString().Trim() + "')";
					}

					col_name = "PROD_TIME";
					CString col_name_sub = "_FROM";
					if (bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString().Trim() != "")
					{
						Log::Info("", __FUNCTION__, "col_name[{0}][{1}]", col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
						sqlstr_condition += " AND A." + col_name + " >= @" + col_name + col_name_sub;
						cmd_inq.Parameters.Set(col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
					}
					col_name_sub = "_TO";
					if (bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString().Trim() != "")
					{
						Log::Info("", __FUNCTION__, "col_name[{0}][{1}]", col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
						sqlstr_condition += " AND A." + col_name + " <= @" + col_name + col_name_sub;
						cmd_inq.Parameters.Set(col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
					}

					col_name = "RECV_MAT_TIME";
					 col_name_sub = "_FROM";
					if (bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString().Trim() != "")
					{
						Log::Info("", __FUNCTION__, "col_name[{0}][{1}]", col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
						sqlstr_condition += " AND A." + col_name + " >= @" + col_name + col_name_sub;
						cmd_inq.Parameters.Set(col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString() );
					}
					col_name_sub = "_TO";
					if (bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString().Trim() != "")
					{
						Log::Info("", __FUNCTION__, "col_name[{0}][{1}]", col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
						sqlstr_condition += " AND A." + col_name + " <= @" + col_name + col_name_sub;
						cmd_inq.Parameters.Set(col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
					}

					col_name = "SCRAP_TIME";//有判废时间加条件  AND A.USAGE_DECISION = '3005'
					 col_name_sub = "_FROM";
					if (bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString().Trim() != "")
					{
						Log::Info("", __FUNCTION__, "col_name[{0}][{1}]", col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
						sqlstr_condition += " AND A.USAGE_DECISION = '3005'  AND A." + col_name + " >= @" + col_name + col_name_sub;
						cmd_inq.Parameters.Set(col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
					}
					col_name_sub = "_TO";
					if (bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString().Trim() != "")
					{
						Log::Info("", __FUNCTION__, "col_name[{0}][{1}]", col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
						sqlstr_condition += " AND A.USAGE_DECISION = '3005'  AND A." + col_name + " <= @" + col_name + col_name_sub;
						cmd_inq.Parameters.Set(col_name + col_name_sub, bcls_rec->Tables[0].Rows[0][col_name + col_name_sub].ToString());
					}

					
				}

				sqlstr_order = " ORDER BY A.PROD_TIME DESC";

				sqlstr_count = sqlstr_count + sqlstr_condition;
				sqlstr = sqlstr + sqlstr_condition + sqlstr_order;
				break;
		}

		Log::Info("", __FUNCTION__, "sqlstr[{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		//返回分页总数量信息
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

		//返回提示栏信息
		CFormattable arguments[] = { TotalRecordCount }; // 定义参数列表的数组
		CMessageFormat::Format(s.msg, "查询到信息[{0}]条。", arguments, 1); //查询到[{0}]条记录。
		CString ts = ((CDecimal)(CDateTime::Now() - datetime).TotalMilliseconds()).Round(0).ToString();
		CFormattable arguments2[] = { ts };
		CMessageFormat::Format(s.sysmsg, "SVC用时[{0}ms]", arguments2, 1);
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
		