/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2015-02-01
Description: 炼钢缺陷信息管理_材料信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢缺陷信息管理_材料信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
 

//外部函数声明

BM2F_ENTERACE(mmsm06a1f2_inq) 

int f_mmsm06a1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString cs_prod_time_from("");	
	CString cs_prod_time_to("");	

	CDecimal cd_count	= 0;								
	int	record_count_per_page	= 0; /* 每页记录数 */
	int	current_page_no			= 0; /* 需查询的页号,从0开始计数 */
	int	start_row				= 0; /* 将要压入outBlock的起始行 */
    
	/* 实体类定义 */ 
	CModel tmmsm01("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm01.TrimOrBlank();	
		if(bcls_rec->Tables[0].Columns.Contains("PROD_TIME_FROM") == true)
		{
			cs_prod_time_from = bcls_rec->Tables[0].Rows[0]["PROD_TIME_FROM"].ToString().Trim();
		}	
		if(bcls_rec->Tables[0].Columns.Contains("PROD_TIME_TO") == true)
		{
			cs_prod_time_to   = bcls_rec->Tables[0].Rows[0]["PROD_TIME_TO"].ToString().Trim();
		}

		record_count_per_page = bcls_rec->Tables[1].Rows[0]["RECORD_COUNT_PER_PAGE"];	
		current_page_no		  = bcls_rec->Tables[1].Rows[0]["CURRENT_PAGE_NO"];	
		
		//Log::Trace("",__FUNCTION__,"tmmsm01.MAT_NO			= [{0}]",(const char*)tmmsm01["MAT_NO"].ToString());
		//Log::Trace("",__FUNCTION__,"tmmsm01.MAT_STATUS		= [{0}]",(const char*)tmmsm01["MAT_STATUS"].ToString());
		//Log::Trace("",__FUNCTION__,"tmmsm01.PONO			= [{0}]",(const char*)tmmsm01["PONO"].ToString());
		//Log::Trace("",__FUNCTION__,"tmmsm01.IN_FLAG			= [{0}]",(const char*)tmmsm01["IN_FLAG"].ToString());
		//Log::Trace("",__FUNCTION__,"tmmsm01.HEAT_NO			= [{0}]",(const char*)tmmsm01["HEAT_NO"].ToString());
		//Log::Trace("",__FUNCTION__,"tmmsm01.SG_SIGN			= [{0}]",(const char*)tmmsm01["SG_SIGN"].ToString());
		//Log::Trace("",__FUNCTION__,"tmmsm01.ORDER_NO		= [{0}]",(const char*)tmmsm01["ORDER_NO"].ToString());
		//Log::Trace("",__FUNCTION__,"tmmsm01.MAT_SHAPE_FLAG	= [{0}]",(const char*)tmmsm01["MAT_SHAPE_FLAG"].ToString());
		//Log::Trace("",__FUNCTION__,"tmmsm01.ARCHIVE_FLAG	= [{0}]",(const char*)tmmsm01["ARCHIVE_FLAG"].ToString());
		//Log::Trace("",__FUNCTION__,"cs_prod_time_from		= [{0}]",(const char*)cs_prod_time_from);
		//Log::Trace("",__FUNCTION__,"cs_prod_time_to			= [{0}]",(const char*)cs_prod_time_to);

		//Log::Trace("",__FUNCTION__,"record_count_per_page	= [{0}]",record_count_per_page);
		//Log::Trace("",__FUNCTION__,"current_page_no			= [{0}]",current_page_no);

		/* 检查输入参数合法性 */   
		if(tmmsm01["ARCHIVE_FLAG"].ToString().Trim() == "")
		{
			sprintf(s.msg,"记录类型不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}	  
			
		///* 设置开始时刻和结束时刻 */
		//if(cs_prod_time_from.Trim() != "")
		//{
		//	cs_prod_time_from += "000000";
		//}
		//if(cs_prod_time_to.Trim() != "")
		//{
		//	cs_prod_time_to += "235959";
		//}

		/* 查询材料信息 */   
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				if(tmmsm01["ARCHIVE_FLAG"].ToString().Trim() == "T")
				{
					sqlstr_count = " SELECT COUNT(1) "
								   "   FROM TMMSM01 "
								   "  WHERE MAT_LINE_TYPE = 'HP' ";
					sqlstr	= " SELECT * "
							  "   FROM TMMSM01 "
							  "  WHERE MAT_LINE_TYPE = 'HP' ";
				}
				else //tmmsm01["ARCHIVE_FLAG"] = "H"
				{
					sqlstr_count = " SELECT COUNT(1) "
								   "   FROM HMMSM01 "
								   "  WHERE MAT_LINE_TYPE = 'HP' ";
					sqlstr	= " SELECT * "
							  "   FROM HMMSM01 "
							  "  WHERE MAT_LINE_TYPE = 'HP' ";
				}				
				if(tmmsm01["MAT_NO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND MAT_NO LIKE @tmmsm01.MAT_NO ||'%' "; 
				}	
				if(tmmsm01["MAT_STATUS"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND MAT_STATUS LIKE @tmmsm01.MAT_STATUS ||'%' "; 
				}					
				if(tmmsm01["PONO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND PONO LIKE	@tmmsm01.PONO ||'%' "; 
				}					
				if(tmmsm01["IN_FLAG"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND IN_FLAG LIKE @tmmsm01.IN_FLAG ||'%' "; 
				}	
				if(tmmsm01["HEAT_NO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND HEAT_NO LIKE @tmmsm01.HEAT_NO ||'%' "; 
				}	
				if(tmmsm01["SG_SIGN"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND SG_SIGN LIKE @tmmsm01.SG_SIGN ||'%' "; 
				}
				if(tmmsm01["ORDER_NO"].ToString().Trim() != "")
				{
					sqlstr_temp	+= " AND ORDER_NO LIKE @tmmsm01.ORDER_NO ||'%' "; 
				}		
				if(cs_prod_time_from.Trim() != "")
				{
					sqlstr_temp	+= " AND PROD_TIME >=	@cs_prod_time_from"; 
				}	
				if(cs_prod_time_to.Trim() != "")
				{
					sqlstr_temp	+= " AND PROD_TIME <=	@cs_prod_time_to"; 
				}
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr		 = sqlstr + sqlstr_temp + " ORDER BY MAT_NO ASC";
				break;
		}     
		//Log::Trace("",__FUNCTION__,"sqlstr_temp			= [{0}]",(const char*)sqlstr_temp);
		//Log::Trace("",__FUNCTION__,"sqlstr_count		= [{0}]",(const char*)sqlstr_count);
		//Log::Trace("",__FUNCTION__,"sqlstr				= [{0}]",(const char*)sqlstr);
		cmd_inq.Parameters.Clear();	
		if(tmmsm01["MAT_NO"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmmsm01.MAT_NO",tmmsm01["MAT_NO"].ToString()); 
		}	
		if(tmmsm01["MAT_STATUS"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmmsm01.MAT_STATUS",tmmsm01["MAT_STATUS"].ToString());
		}
		if(tmmsm01["PONO"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmmsm01.PONO",tmmsm01["PONO"].ToString()); 
		}
		if(tmmsm01["IN_FLAG"].ToString().Trim() != "")	
		{
			cmd_inq.Parameters.Set("tmmsm01.IN_FLAG",tmmsm01["IN_FLAG"].ToString()); 
		}
		if(tmmsm01["HEAT_NO"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmmsm01.HEAT_NO",tmmsm01["HEAT_NO"].ToString()); 
		}	
		if(tmmsm01["SG_SIGN"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmmsm01.SG_SIGN",tmmsm01["SG_SIGN"].ToString()); 
		}
		if(tmmsm01["ORDER_NO"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("tmmsm01.ORDER_NO",tmmsm01["ORDER_NO"].ToString()); 
		}
		if(cs_prod_time_from.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_prod_time_from",cs_prod_time_from); 
		}	
		if(cs_prod_time_to.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_prod_time_to",cs_prod_time_to); 
		}
		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if(start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],start_row,record_count_per_page);
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
		
