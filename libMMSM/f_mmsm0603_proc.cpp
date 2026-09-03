/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2015-04-01
Description: 炼钢缺陷新增
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢缺陷新增
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件 
 
 
 
 
//外部函数声明
#if defined _SYS_PES	//PES
 int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

BM2_FUNCTION_EXPORT
int f_mmsm0603_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	
	/* 业务变量 */
	CString	datetime("");    
	CString	cs_mmsm_defect_id("");			
	CString	cs_mmsm_defect_log_seq("");			

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm06("TMMSM06");
	CModel tmmsm061("TMMSM061");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MMSM0603");
		if(blkNum < 0)
		{
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			strcpy(s.sysmsg,"传入数据块 MMSM0603 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		/* 添加并设置块名 */
		#if defined _SYS_PES	//PES
			blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			if(blkNum < 0)
			{
					bcls_rec->Tables.Add("MMSMSND"); 
			}

			if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("TABLE_TYPE"))
			{
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TABLE_TYPE");
			}

		#endif
		
		for(int i = 0; i < bcls_rec->Tables["MMSM0603"].Rows.get_Count(); i++)
		{
			tmmsm06.Reset();
			tmmsm061.Reset();

			/* 获取输入参数 */
			tmmsm061.MergeFrom(bcls_rec->Tables["MMSM0603"].Rows[i]);
			tmmsm061.TrimOrBlank();
			if(bcls_rec->Tables["MMSM0603"].Columns.Contains("PROD_SEQ_NO") == true)
			{
				tmmsm061["PROD_SEQ_NO"]		= bcls_rec->Tables["MMSM0603"].Rows[0]["PROD_SEQ_NO"].ToString().Trim();
			}	
			if(bcls_rec->Tables["MMSM0603"].Columns.Contains("SUB_BACKLOG_CODE") == true)
			{
				tmmsm061["SUB_BACKLOG_CODE"]	= bcls_rec->Tables["MMSM0603"].Rows[0]["SUB_BACKLOG_CODE"].ToString().Trim();
			}	

			/* 打印输入参数 */
			//Log::Trace("", __FUNCTION__, "新增缺陷 传入参数,tmmsm061.PROD_SEQ_NO			= [{0}]",tmmsm061["PROD_SEQ_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "新增缺陷 传入参数,tmmsm061.MAT_NO					= [{0}]",tmmsm061["MAT_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "新增缺陷 传入参数,tmmsm061.SURFACE_DIV			= [{0}]",tmmsm061["SURFACE_DIV"].ToString());
			//Log::Trace("", __FUNCTION__, "新增缺陷 传入参数,tmmsm061.DEFECT_CODE			= [{0}]",tmmsm061["DEFECT_CODE"].ToString());

			/* 检查输入参数合法性 */
			if(tmmsm061["MAT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"材料号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}	

			/* 获取缺陷序号 */
			switch (conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " SELECT NVL(MAX(DEFECT_NO),0) + 1 "
							 "	 FROM TMMSM06 "
							 "  WHERE MAT_NO = @tmmsm061.MAT_NO ";
					break;
			}
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmmsm061.MAT_NO", tmmsm061["MAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())	//取第1行
			{
				tmmsm061["DEFECT_NO"] = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close();

		#if defined _SYS_PES || _SYS_MES //MMS的缺陷戳与PES一致
			/* 获取缺陷戳 */
			cs_mmsm_defect_id	= EPGetNextSeq("MMSM_DEFECT_ID", conn);
			if (cs_mmsm_defect_id.Trim() == "")
			{
				strcpy(s.msg, "获取 缺陷戳 失败!");/*系统出现异常，请联系系统维护人员。*/
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmmsm061["DEFECT_ID"]	= datetime + cs_mmsm_defect_id;			//缺陷戳

			/* 获取缺陷履历流水号 */
			cs_mmsm_defect_log_seq	= EPGetNextSeq("MMSM_DEFECT_LOG_SEQ", conn);
			if (cs_mmsm_defect_log_seq.Trim() == "")
			{
				strcpy(s.msg, "获取 缺陷履历流水号 失败!");/*系统出现异常，请联系系统维护人员。*/
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmmsm061["DEFECT_LOG_SEQ"]	= datetime + cs_mmsm_defect_log_seq;			//缺陷履历流水号
		#endif

			/* 新增缺陷信息履历表 */
			tmmsm061["CLEAR_DIV"]			= "I";			//I-新增
			tmmsm061["REC_CREATOR"]		= s.userid;
			tmmsm061["REC_CREATE_TIME"]	= datetime;
			tmmsm061.TrimOrBlank();
			tmmsm061.Insert();   

			/* 新增缺陷信息表 */
			tmmsm06.CopyFrom(tmmsm061);

			Log::Trace("", __FUNCTION__, "新增缺陷信息表,tmmsm06.MAT_NO					= [{0}]",tmmsm06["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "新增缺陷信息表,tmmsm06.DEFECT_SURF				= [{0}]",tmmsm06["DEFECT_SURF"].ToString());
			Log::Trace("", __FUNCTION__, "新增缺陷信息表,tmmsm06.DEFECT_CODE				= [{0}]",tmmsm06["DEFECT_CODE"].ToString());

			tmmsm06.TrimOrBlank();
			tmmsm06.Insert();    

			#if defined _SYS_PES	//PES
			/* 设置缺陷电文参数 */
			tmmsm061.MergeTo(bcls_rec->Tables["MMSMSND"],false);
			#endif

		}

		#if defined _SYS_PES	//PES
  	
		bcls_rec->Tables["MMSMSND"].Rows[0]["TABLE_TYPE"] = "MMSM3Y";
				
		doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);

		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		#endif

	
		

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
