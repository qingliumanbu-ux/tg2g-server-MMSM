/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    向萍
Version:    1.0
Date:       2013-05-24
Description: 炼钢材料履历信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢材料履历信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明

BM2F_ENTERACE(mmsm96a1f2_inq)

int f_mmsm96a1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString cs_rec_create_time_from("");	
	CString cs_rec_create_time_to("");	
	CString cs_archive_flag("");	
	CDecimal cd_count	= 0;
	CDecimal cd_count1	= 0;
	int	record_count_per_page	= 0; // 每页记录数
	int	current_page_no			= 0; // 需查询的页号,从0开始计数
	int	start_row				= 0; // 将要压入outBlock的起始行
	int	end_row					= 0; // 将要压入outBlock的结束行
	
	/* 实体类定义 */ 
	CModel tmmsm96("TMMSM96");
	CPageInfo pageInfo;
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		try{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tmmsm96.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm96.TrimOrBlank();
		if(bcls_rec->Tables[0].Columns.Contains("REC_CREATE_TIME_FROM") == true)
		{
			cs_rec_create_time_from = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_FROM"].ToString().Trim();
		}	
		if(bcls_rec->Tables[0].Columns.Contains("REC_CREATE_TIME_TO") == true)
		{
			cs_rec_create_time_to   = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_TO"].ToString().Trim();
		}
		if(bcls_rec->Tables[0].Columns.Contains("ARCHIVE_FLAG") == true)
		{
			cs_archive_flag         = bcls_rec->Tables[0].Rows[0]["ARCHIVE_FLAG"].ToString().Trim();
		}
		if(bcls_rec->Tables[0].Columns.Contains("RECORD_COUNT_PER_PAGE") == true)
		{
			record_count_per_page	= bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];		// 每页记录数 
		}
		if(bcls_rec->Tables[0].Columns.Contains("CURRENT_PAGE_NO") == true)
		{
			current_page_no			= bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];			// 需查询的页号 
		}

		//Log::Trace("",__FUNCTION__,"tmmsm96.RESUME_SEQ_NO	= [{0}]",(const char*)tmmsm96["RESUME_SEQ_NO"].ToString());
		//Log::Trace("",__FUNCTION__,"tmmsm96.MAT_NO			= [{0}]",(const char*)tmmsm96["MAT_NO"].ToString());
		//Log::Trace("",__FUNCTION__,"tmmsm96.MAT_LINE_TYPE	= [{0}]",(const char*)tmmsm96["MAT_LINE_TYPE"].ToString());
		//Log::Trace("",__FUNCTION__,"cs_rec_create_time_from	= [{0}]",(const char*)cs_rec_create_time_from);
		//Log::Trace("",__FUNCTION__,"cs_rec_create_time_to	= [{0}]",(const char*)cs_rec_create_time_to);
		//Log::Trace("",__FUNCTION__,"cs_archive_flag			= [{0}]",(const char*)cs_archive_flag);
		//Log::Trace("",__FUNCTION__,"record_count_per_page	= [{0}]",record_count_per_page);
		//Log::Trace("",__FUNCTION__,"current_page_no			= [{0}]",current_page_no);

		if(tmmsm96["RESUME_SEQ_NO"].ToString().Trim() != "")
		{
			Log::Trace("", __FUNCTION__, "tmmsm96.RESUME_SEQ_NO	= [{0}]", (const char*)tmmsm96["RESUME_SEQ_NO"].ToString());
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " SELECT * "
						     "   FROM TMMSM96 "
							 "  WHERE 1 = 1 ";
					sqlstr	+= " AND RESUME_SEQ_NO = @tmmsm96.RESUME_SEQ_NO "; 
					sqlstr	+= " UNION ALL "; 
					sqlstr	+= " SELECT	* ";
					sqlstr	+= " FROM HMMSM96 WHERE 1 = 1 ";
					sqlstr	+= " AND RESUME_SEQ_NO = @tmmsm96.RESUME_SEQ_NO "; 
					break;	
			}     
			//Log::Trace("",__FUNCTION__,"000 sqlstr	= [{0}]",(const char*)sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr= [{0}]", sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmmsm96.RESUME_SEQ_NO",tmmsm96["RESUME_SEQ_NO"].ToString());
			cmd_inq.ExecuteReader(); 
			while(cmd_inq.Read())
			{
				//Log::Trace("",__FUNCTION__,"222 sqlstr	= [{0}]",(const char*)sqlstr);
				cmd_inq.Fetch(tmmsm96);
				//Log::Trace("",__FUNCTION__,"333 sqlstr	= [{0}]",(const char*)sqlstr);
			}
			cmd_inq.Close();
			tmmsm96.MergeTo(bcls_ret->Tables[0],false);
		}
		else
		{
		
			/* 设置开始时刻和结束时刻 */
			if(cs_rec_create_time_from.Trim() != "")
			{
				cs_rec_create_time_from += "000000";
			}
			if(cs_rec_create_time_to.Trim() != "")
			{
				cs_rec_create_time_to += "235959";
			}
			
			/* 程序处理 */
			if(cs_archive_flag.Trim() == "T" 
			|| cs_archive_flag.Trim() == "H")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =  "SELECT COUNT(1) ";
						if(cs_archive_flag.Trim() == "T")
						{
							sqlstr += " FROM TMMSM96 WHERE 1 = 1 ";
						}
						else if(cs_archive_flag.Trim() == "H")
						{
							sqlstr += " FROM HMMSM96 WHERE 1 = 1 ";
						}
						if(tmmsm96["MAT_NO"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_NO  = @tmmsm96.MAT_NO "; 
						}
						if(tmmsm96["EVENT_ID"].ToString().Trim() != "")		
						{
							sqlstr += " AND EVENT_ID LIKE	@tmmsm96.EVENT_ID||'%' "; 
						}
						if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_LINE_TYPE LIKE @tmmsm96.MAT_LINE_TYPE ||'%' ";
						}
						if(cs_rec_create_time_from.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME >= @cs_rec_create_time_from "; 
						}
						if(cs_rec_create_time_to.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME <= @cs_rec_create_time_to "; 
						}

					break;
				}     
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				if(tmmsm96["MAT_NO"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
				}
				if(tmmsm96["EVENT_ID"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.EVENT_ID",tmmsm96["EVENT_ID"].ToString());
				}				
				if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.MAT_LINE_TYPE", tmmsm96["MAT_LINE_TYPE"].ToString());
				}
				if(cs_rec_create_time_from.Trim() != "")
				{
					cmd_inq.Parameters.Set("cs_rec_create_time_from",cs_rec_create_time_from);
				}
				if(cs_rec_create_time_to.Trim() != "")
				{
					cmd_inq.Parameters.Set("cs_rec_create_time_to",cs_rec_create_time_to);
				}
				cd_count	= cmd_inq.ExecuteScalar();
				cmd_inq.Close();

				//Log::Trace("",__FUNCTION__,"总记录数 cd_count = [{0}]",cd_count.ToInt32());
				start_row = record_count_per_page * (current_page_no - 1) ;
				if(start_row > cd_count.ToInt32())
				{
					start_row = 0;
				}
				end_row = record_count_per_page * current_page_no ;
				if(end_row > cd_count.ToInt32()) 
				{
					end_row = cd_count.ToInt32();
				}

				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =  "SELECT * ";
						if(cs_archive_flag.Trim() == "T")
						{
							sqlstr += " FROM TMMSM96 WHERE 1 = 1 ";
						}
						else if(cs_archive_flag.Trim() == "H")
						{
							sqlstr += " FROM HMMSM96 WHERE 1 = 1 ";
						}
						if(tmmsm96["MAT_NO"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_NO  = @tmmsm96.MAT_NO "; 
						}
						if(tmmsm96["EVENT_ID"].ToString().Trim() != "")		
						{
							sqlstr += " AND EVENT_ID LIKE @tmmsm96.EVENT_ID||'%' "; 
						}
						if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_LINE_TYPE LIKE @tmmsm96.MAT_LINE_TYPE ||'%' ";
						}
						if(cs_rec_create_time_from.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME >= @cs_rec_create_time_from "; 
						}
						if(cs_rec_create_time_to.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME <= @cs_rec_create_time_to "; 
						}					
						sqlstr += "ORDER BY REC_CREATE_TIME DESC"; 
						break;
				}     
				//Log::Trace("",__FUNCTION__,"sqlstr	= [{0}]",(const char*)sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				if(tmmsm96["MAT_NO"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
				}
				if(tmmsm96["EVENT_ID"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.EVENT_ID",tmmsm96["EVENT_ID"].ToString());
				}				
				if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.MAT_LINE_TYPE", tmmsm96["MAT_LINE_TYPE"].ToString());
				}
				if(cs_rec_create_time_from.Trim() != "")
				{
					cmd_inq.Parameters.Set("cs_rec_create_time_from",cs_rec_create_time_from);
				}
				if(cs_rec_create_time_to.Trim() != "")
				{
					cmd_inq.Parameters.Set("cs_rec_create_time_to",cs_rec_create_time_to);
				}
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0],start_row,record_count_per_page);
				cmd_inq.Close();
			}
			else 
			{
				//Log::Trace("",__FUNCTION__,"显示历史数据和在线数据");
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =  "SELECT COUNT(1) " 
								  " FROM TMMSM96 WHERE 1 = 1 ";
						if(tmmsm96["MAT_NO"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_NO  = @tmmsm96.MAT_NO "; 
						}
						if(tmmsm96["EVENT_ID"].ToString().Trim() != "")		
						{
							sqlstr += " AND EVENT_ID LIKE @tmmsm96.EVENT_ID||'%' "; 
						}
						if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_LINE_TYPE LIKE @tmmsm96.MAT_LINE_TYPE ||'%' ";
						}
						if(cs_rec_create_time_from.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME >= @cs_rec_create_time_from "; 
						}
						if(cs_rec_create_time_to.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME <= @cs_rec_create_time_to "; 
						}
						break;
				}     
				cmd_inq.SetCommandText(sqlstr);
				//Log::Trace("",__FUNCTION__,"sqlstr	= [{0}]",(const char*)sqlstr);
				cmd_inq.Parameters.Clear();
				if(tmmsm96["MAT_NO"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
				}
				if(tmmsm96["EVENT_ID"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.EVENT_ID",tmmsm96["EVENT_ID"].ToString());
				}
				if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.MAT_LINE_TYPE", tmmsm96["MAT_LINE_TYPE"].ToString());
				}
				if(cs_rec_create_time_from.Trim() != "")
				{
					cmd_inq.Parameters.Set("cs_rec_create_time_from",cs_rec_create_time_from);
				}
				if(cs_rec_create_time_to.Trim() != "")
				{
					cmd_inq.Parameters.Set("cs_rec_create_time_to",cs_rec_create_time_to);
				}
				cd_count1	= cmd_inq.ExecuteScalar();
				cmd_inq.Close();

				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr = "SELECT COUNT(1) "
								 " FROM HMMSM96 WHERE 1 = 1 ";
						if(tmmsm96["MAT_NO"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_NO  = @tmmsm96.MAT_NO "; 
						}
						if(tmmsm96["EVENT_ID"].ToString().Trim() != "")		
						{
							sqlstr += " AND EVENT_ID LIKE @tmmsm96.EVENT_ID||'%' "; 
						}
						if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_LINE_TYPE LIKE @tmmsm96.MAT_LINE_TYPE ||'%' ";
						}
						if(cs_rec_create_time_from.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME >= @cs_rec_create_time_from "; 
						}
						if(cs_rec_create_time_to.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME <= @cs_rec_create_time_to "; 
						}
						break;							
				}     
				cmd_inq.SetCommandText(sqlstr);
				//Log::Trace("",__FUNCTION__,"sqlstr	= [{0}]",(const char*)sqlstr);
				cmd_inq.Parameters.Clear();
				if(tmmsm96["MAT_NO"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
				}
				if(tmmsm96["EVENT_ID"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.EVENT_ID",tmmsm96["EVENT_ID"].ToString());
				}
				if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.MAT_LINE_TYPE", tmmsm96["MAT_LINE_TYPE"].ToString());
				}
				if(cs_rec_create_time_from.Trim() != "")
				{
					cmd_inq.Parameters.Set("cs_rec_create_time_from",cs_rec_create_time_from);
				}
				if(cs_rec_create_time_to.Trim() != "")
				{
					cmd_inq.Parameters.Set("cs_rec_create_time_to",cs_rec_create_time_to);
				}
				cd_count	= cmd_inq.ExecuteScalar();
				cmd_inq.Close();

				cd_count	= cd_count + cd_count1;
				//Log::Trace("",__FUNCTION__,"总记录数cd_count	= [{0}]",cd_count.ToInt32());
				start_row = record_count_per_page * (current_page_no-1) ;
				if(start_row > cd_count.ToInt32())
				{
					start_row = 0;
				}
				end_row = record_count_per_page * current_page_no ;
				if(end_row > cd_count.ToInt32()) 
				{
					end_row = cd_count.ToInt32();
				}

				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr = "SELECT * "
								" FROM TMMSM96 WHERE 1 = 1 ";
						if(tmmsm96["MAT_NO"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_NO  = @tmmsm96.MAT_NO "; 
						}
						if(tmmsm96["EVENT_ID"].ToString().Trim() != "")		
						{
							sqlstr += " AND EVENT_ID LIKE @tmmsm96.EVENT_ID||'%' "; 
						}
						if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_LINE_TYPE LIKE @tmmsm96.MAT_LINE_TYPE ||'%' ";
						}
						if(cs_rec_create_time_from.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME >= @cs_rec_create_time_from "; 
						}
						if(cs_rec_create_time_to.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME <= @cs_rec_create_time_to "; 
						}
						sqlstr	+= " UNION ALL "; 
						sqlstr	+= " SELECT	* ";
						sqlstr += " FROM HMMSM96 WHERE 1 = 1 ";
						if(tmmsm96["MAT_NO"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_NO  = @tmmsm96.MAT_NO ";  
						}
						if(tmmsm96["EVENT_ID"].ToString().Trim() != "")		
						{
							sqlstr += " AND EVENT_ID LIKE @tmmsm96.EVENT_ID||'%' "; 
						}
						if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
						{
							sqlstr += " AND MAT_LINE_TYPE LIKE @tmmsm96.MAT_LINE_TYPE ||'%' ";
						}
						if(cs_rec_create_time_from.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME >= @cs_rec_create_time_from "; 
						}
						if(cs_rec_create_time_to.Trim() != "")		
						{
							sqlstr += " AND REC_CREATE_TIME <= @cs_rec_create_time_to "; 
						}
						sqlstr = "select * from (" + sqlstr + ") ORDER BY RESUME_SEQ_NO DESC";
						break;	
				}     
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				if(tmmsm96["MAT_NO"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
				}
				if(tmmsm96["EVENT_ID"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.EVENT_ID",tmmsm96["EVENT_ID"].ToString());
				}
				if (tmmsm96["MAT_LINE_TYPE"].ToString().Trim() != "")
				{
					cmd_inq.Parameters.Set("tmmsm96.MAT_LINE_TYPE", tmmsm96["MAT_LINE_TYPE"].ToString());
				}
				if(cs_rec_create_time_from.Trim() != "")
				{
					cmd_inq.Parameters.Set("cs_rec_create_time_from",cs_rec_create_time_from);
				}
				if(cs_rec_create_time_to.Trim() != "")
				{
					cmd_inq.Parameters.Set("cs_rec_create_time_to",cs_rec_create_time_to);
				}
				Log::Trace("", __FUNCTION__, "  sqlstr123		= [{0}]", sqlstr);
				//cmd_inq.ExecuteQuery(bcls_ret->Tables[0],start_row,record_count_per_page);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
				cmd_inq.Close();
				 
			}

			///* 事件名称 */
			//bcls_ret->Tables[0].Columns.Add(DT_STRING,"EVENT_NAME");
			//for(int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
			//{
			//	//Log::Trace("",__FUNCTION__,"111 TMM0097 sqlstr		= [{0}]",sqlstr);	  	
			//	tmmsm96.MAT_KIND	= bcls_ret->Tables[0].Rows[i]["MAT_KIND"].ToString().Trim();
			//	//Log::Trace("",__FUNCTION__,"222 TMM0097 sqlstr		= [{0}]",sqlstr);	  	
			//	tmmsm96.EVENT_ID	= bcls_ret->Tables[0].Rows[i]["EVENT_ID"].ToString().Trim();
			//	//Log::Trace("",__FUNCTION__,"333 TMM0097 sqlstr		= [{0}]",sqlstr);	  	

			//	/* 查询事件信息 */
			//	switch(conn->DatabaseKind)
			//	{
			//		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//		case DB_KIND_MSSQL:				// MS SQL Server数据库
			//		case DB_KIND_ORACLE:	        // Oracle 数据库
			//		default:
			//			sqlstr	= "	SELECT EVENT_NAME "
			//					  "   FROM TMM0097 "
			//					  "  WHERE EVENT_ID = @tmmsm96.EVENT_ID "
			//					  "    AND MAT_KIND = @tmmsm96.MAT_KIND "
			//					  "    AND EVENT_LINE_TYPE = ' ' "
			//					  "  ORDER BY REC_CREATE_TIME DESC";		
			//			break;
			//	}     
			//	//Log::Trace("",__FUNCTION__,"TMM0097 sqlstr		= [{0}]",sqlstr);	  	
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Clear();
			//	cmd_inq.Parameters.Set("tmmsm96.EVENT_ID",tmmsm96["EVENT_ID"].ToString());
			//	cmd_inq.Parameters.Set("tmmsm96.MAT_KIND",tmmsm96["MAT_KIND"].ToString());
			//	cmd_inq.ExecuteReader();			 
			//	if(cmd_inq.Read())
			//	{
			//		tmmsm96.EVENT_DESC	= cmd_inq.GetString(1); 
			//	} 
			//	cmd_inq.Close();
			//	//Log::Trace("",__FUNCTION__,"tmmsm96.EVENT_DESC		= [{0}]",(const char*)tmmsm96["EVENT_DESC"].ToString());	 
			//	bcls_ret->Tables[0].Rows[i]["EVENT_NAME"]	= tmmsm96["EVENT_DESC"];
			//}
			//
			//返回分页信息 
			bcls_ret->Tables.Add("PAGEINFO");	//增加块
			bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL,"TOTAL_RECORD");	//总记录数
			bcls_ret->Tables["PAGEINFO"].Rows.Add();
			bcls_ret->Tables["PAGEINFO"].Rows[0][0]	= cd_count.ToInt32();
		}
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
		
