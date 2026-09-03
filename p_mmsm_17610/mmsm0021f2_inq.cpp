/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     郝东炜
Version:    1.0
Date:       2016-10-12
Description: 炼钢钢坯材料信息查询（汇总）
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢钢坯材料信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2F_ENTERACE(mmsm0021f2_inq) 

int f_mmsm0021f2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString	cs_prod_time_from("");	
	CString	cs_prod_time_to("");	
	CString	cs_mat_no("");	
	CString	cs_order_no("");	
	CString	cs_heat_no("");	
	CString	cs_pono("");	
	CString	cs_sg_sign("");	
	CString	cs_mat_status("");	
	CString	cs_in_flag("");
	CString	cs_archive_flag("");	
	CDecimal cd_mat_thick = 0;
	CDecimal cd_count	= 0;

	int	record_count_per_page	= 0; /* 每页记录数 */
	int	current_page_no			= 0; /* 需查询的页号,从0开始计数 */
	int	start_row				= 0; /* 将要压入outBlock的起始行 */
    
	/* 实体类定义 */ 

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString sqlstr_temp;

	CString	cs_func_id("");
	CString	cs_item_ename_d("");
	CString	cs_item_ename_g("");
	CString sqlstr_condition;
	CString sqlstr_item_display;
	CString sqlstr_item_groupby;
	int   fetchRowCount = 0;
	CPageInfo pageInfo;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		cs_prod_time_from	= bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();	//起始时间
		cs_prod_time_to		= bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();	//终止时间
		cs_mat_no			= bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();			//材料号
		cs_order_no			= bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().Trim();		//合同号
		cs_heat_no			= bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();			//熔炼号
		cs_pono				= bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();			//制造命令号
		cs_sg_sign			= bcls_rec->Tables[0].Rows[0]["SG_SIGN"].ToString().Trim();			//牌号
		cs_mat_status		= bcls_rec->Tables[0].Rows[0]["MAT_STATUS"].ToString().Trim();      //材料状态
		cs_in_flag          = bcls_rec->Tables[0].Rows[0]["IN_FLAG"].ToString().Trim();
		cs_archive_flag		= bcls_rec->Tables[0].Rows[0]["ARCHIVE_FLAG"].ToString().Trim();      
		//record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];	
		//current_page_no		= bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];	
		//cs_func_id          = bcls_rec->Tables[0].Rows[0]["FUNC_ID"];  2023/8/31

		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		//Log::Trace("",__FUNCTION__,"cs_prod_time_from		= [{0}]",cs_prod_time_from);
		//Log::Trace("",__FUNCTION__,"cs_prod_time_to			= [{0}]",cs_prod_time_to);
		//Log::Trace("",__FUNCTION__,"cs_mat_no				= [{0}]",cs_mat_no);
		//Log::Trace("",__FUNCTION__,"cs_order_no				= [{0}]",cs_order_no);
		//Log::Trace("",__FUNCTION__,"cs_heat_no				= [{0}]",cs_heat_no);
		//Log::Trace("",__FUNCTION__,"cs_pono					= [{0}]",cs_pono);
		//Log::Trace("",__FUNCTION__,"cs_sg_sign				= [{0}]",cs_sg_sign);
		//Log::Trace("",__FUNCTION__,"cs_mat_status			= [{0}]",cs_mat_status);
		//Log::Trace("",__FUNCTION__,"cs_archive_flag			= [{0}]",cs_archive_flag);
		//Log::Trace("",__FUNCTION__,"record_count_per_page	= [{0}]",record_count_per_page);
		//Log::Trace("",__FUNCTION__,"current_page_no			= [{0}]",current_page_no);
		//Log::Trace("", __FUNCTION__, "cs_func_id		        = [{0}]",cs_func_id);

		/* 检查输入参数合法性 */   
		if(cs_archive_flag.Trim() == "")
		{
			sprintf(s.msg,"记录类型不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}	  
			
		/* 设置开始时刻和结束时刻 */
		if(cs_prod_time_from.Trim() != "")
		{
			cs_prod_time_from += "000000";
		}
		if(cs_prod_time_to.Trim() != "")
		{
			cs_prod_time_to += "235959";
		}

		//2023/8/31 liuyali 信融不再采用功能号
		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:				// MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:

		//	sqlstr_condition = " SELECT ITEM_ENAME "
		//		"   FROM TED54 "
		//		"  WHERE FUNC_ID = @cs_func_id  "
		//		"  ORDER BY CLASS_CODE,SEQ_NO ";
		//	break;
		//}

		//cmd_inq.SetCommandText(sqlstr_condition);// 设置执行的SQL语句
		//// 设置SQL中的变量
		//cmd_inq.Parameters.Set("cs_func_id", cs_func_id); //功能号。
		//cmd_inq.ExecuteReader(); //执行读取

		//while (cmd_inq.Read()) //循环读取
		//{
		//	cs_item_ename_d = cmd_inq.GetString(1);
		//	cs_item_ename_g = cmd_inq.GetString(1);

		//	if (cs_item_ename_d.Trim() == "MAT_NUM" || cs_item_ename_d.Trim() == "MAT_WT" 
		//		|| cs_item_ename_d.Trim() == "MAT_ACT_WT" || cs_item_ename_d.Trim() == "MAT_THEORY_WT")
		//	{
		//		cs_item_ename_d = " SUM(" + cs_item_ename_d + ")" + " " + cs_item_ename_d;
		//		cs_item_ename_g = " ";
		//	}

		//	if (fetchRowCount == 0)
		//	{
		//		sqlstr_item_display = cs_item_ename_d;

		//		if (cs_item_ename_g.Trim() != "")
		//		{
		//			sqlstr_item_groupby = cs_item_ename_g;
		//		}
		//	}
		//	else
		//	{
		//		sqlstr_item_display = sqlstr_item_display + "," + cs_item_ename_d; //需显示字段
		//		if (cs_item_ename_g.Trim() != "")
		//		{
		//			sqlstr_item_groupby = sqlstr_item_groupby + "," + cs_item_ename_g; //分组条件字段
		//		}
		//	}

		//	fetchRowCount++;
		//}
		//cmd_inq.Close(); //关闭游标 



		sqlstr_item_display = " HEAT_NO,ORDER_NO,MAT_LEN,MAT_STATUS,STOCK_PLACE_NO,SUM(MAT_NUM) MAT_NUM,SUM(MAT_WT) MAT_WT,SUM(MAT_ACT_WT) MAT_ACT_WT ";
		sqlstr_item_groupby = " HEAT_NO,ORDER_NO,MAT_LEN,MAT_STATUS,STOCK_PLACE_NO ";
		Log::Trace("", __FUNCTION__, "sqlstr_item_display[{0}]", (const char*)sqlstr_item_display);
		Log::Trace("", __FUNCTION__, "sqlstr_item_groupby[{0}]", (const char*)sqlstr_item_groupby);

		/* 查询材料信息 */   
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				if(cs_archive_flag.Trim() == "T")
				{
					sqlstr =  " SELECT " + sqlstr_item_display +
							  "   FROM TMMSM01 "
							  "  WHERE MAT_LINE_TYPE = 'SM' ";
				}
				else
				{
					sqlstr =  " SELECT " + sqlstr_item_display +
							  "   FROM HMMSM01 "
							  "  WHERE MAT_LINE_TYPE = 'SM' ";
				}				
				if(cs_prod_time_from.Trim() != "")
				{
					sqlstr_temp	+= " AND PROD_TIME >=	@cs_prod_time_from "; 
				}	
				if(cs_prod_time_to.Trim() != "")
				{
					sqlstr_temp	+= " AND PROD_TIME <=	@cs_prod_time_to "; 
				}
				if(cs_mat_no.Trim() != "")
				{
					if (cs_mat_no.Find(",", 0) <= 0)	 //单个材料号时，支持模糊查询
					{
						sqlstr_temp += " AND MAT_NO LIKE @cs_mat_no || '%' ";

						//Log::Trace("", __FUNCTION__, "cs_mat_no.Find <= 0= [{0}]", (const char*)cs_mat_no);
					}
					else
					{
						sqlstr_temp += " AND MAT_NO IN ('" + cs_mat_no.Trim() + "') ";

						//Log::Trace("", __FUNCTION__, "else= [{0}]", (const char*)cs_mat_no);
					}
				}
				if(cs_order_no.Trim() != "")
				{
					sqlstr_temp	+= " AND ORDER_NO LIKE @cs_order_no ||'%' "; 
				}
				if(cs_heat_no.Trim() != "")
				{
					sqlstr_temp	+= " AND HEAT_NO LIKE @cs_heat_no ||'%' ";   
				}	
				if(cs_pono.Trim() != "")
				{
					sqlstr_temp	+= " AND PONO LIKE	@cs_pono ||'%' "; 
				}
				if(cs_sg_sign.Trim() != "")
				{
					sqlstr_temp	+= " AND SG_SIGN LIKE @cs_sg_sign ||'%' "; 
				}	
				if(cs_mat_status.Trim() != "")
				{
					sqlstr_temp	+= " AND MAT_STATUS = @cs_mat_status "; 
				}								
				if (cs_in_flag.Trim() != "")
				{
					sqlstr_temp	+= " AND IN_FLAG = @cs_in_flag "; 
				}
				if (cd_mat_thick > 0)
				{
					sqlstr_temp += " AND MAT_THICK = @cd_mat_thick ";
				}

				sqlstr		 = sqlstr + sqlstr_temp;
				sqlstr       = sqlstr + " GROUP BY " + sqlstr_item_groupby;
				sqlstr_count = "SELECT COUNT (1) FROM (" + sqlstr + ")";

				break;
		}     
		//Log::Trace("",__FUNCTION__,"sqlstr_temp			= [{0}]",(const char*)sqlstr_temp);
		//Log::Trace("",__FUNCTION__,"11sqlstr_count		= [{0}]",(const char*)sqlstr_count);
		//Log::Trace("",__FUNCTION__,"11sqlstr				= [{0}]",(const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if(cs_prod_time_from.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_prod_time_from",cs_prod_time_from); 
		}	
		if(cs_prod_time_to.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_prod_time_to",cs_prod_time_to); 
		}
		if(cs_mat_no.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_mat_no",cs_mat_no); 
		}
		if(cs_order_no.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_order_no",cs_order_no); 
		}
		if(cs_heat_no.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_heat_no",cs_heat_no); 
		}
		if(cs_pono.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_pono",cs_pono); 
		}
		if(cs_sg_sign.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_sg_sign",cs_sg_sign); 
		}		
		if(cs_mat_status.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_mat_status",cs_mat_status); 
		}
		if (cs_in_flag.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_in_flag", cs_in_flag);
		}
		if (cd_mat_thick > 0)
		{
			cmd_inq.Parameters.Set("cd_mat_thick", cd_mat_thick);
		}
		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if(start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		Log::Trace("", __FUNCTION__, "22sqlstr_count		= [{0}]", (const char*)sqlstr_count);
		Log::Trace("",__FUNCTION__,"22sqlstr				= [{0}]",(const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		//cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0]	= cd_count.ToInt32();
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
		