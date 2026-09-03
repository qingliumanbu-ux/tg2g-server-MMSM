/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 厚板侧板坯交换
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 厚板侧板坯交换
/// 只支持短坯替换，长坯替换需补充程序
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  /*炼钢物料主表*/
 
 

//外部函数声明 
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);  

BM2_FUNCTION_EXPORT
 int f_mmsm63(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    
	CString	cs_plan_mat_no("");    
	CString	cs_aim_mat_no("");    

	/* 实体类定义 */
	CModel plan_tmmsm01("TMMSM01");
	CModel aim_tmmsm01("TMMSM01");
	CModel plan_tmmsm03("TMMSM03");
	CModel aim_tmmsm03("TMMSM03");
	CModel plan_tmmsm04("TMMSM04");
	CModel aim_tmmsm04("TMMSM04");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MMSM63");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMSM63 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");				
		if(blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099"); 
		}
		bcls_rec->Tables["MM0099"].Rows.Clear();
		blkNum = bcls_rec->Tables.IndexOf("MMSM63_AIM_TMMSM04");				
		if(blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM63_AIM_TMMSM04"); 
		}
		blkNum = bcls_rec->Tables.IndexOf("MMSM63_PLAN_TMMSM04");				
		if(blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM63_PLAN_TMMSM04"); 
		}

		/* 获取输入参数 */
		plan_tmmsm01["MAT_NO"]			= bcls_rec->Tables["MMSM63"].Rows[0]["PLAN_MAT_NO"].ToString().Trim();
		aim_tmmsm01["MAT_NO"]			= bcls_rec->Tables["MMSM63"].Rows[0]["AIM_MAT_NO"].ToString().Trim();

		/* 打印输入参数 */
		//Log::Trace("", __FUNCTION__, "传入参数,plan_tmmsm01.MAT_NO		= [{0}]",plan_tmmsm01["MAT_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "传入参数,aim_tmmsm01.MAT_NO		= [{0}]",aim_tmmsm01["MAT_NO"].ToString());

		/* 检查输入参数合法性 */
		if(plan_tmmsm01["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,"数据校验失败，计划指定的材料号不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(aim_tmmsm01["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,"数据校验失败，计划要求替换的目标材料号不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 查询板坯主档 */
		if (plan_tmmsm01.Query("MAT_NO") == false)
		{
			sprintf(s.msg, "计划指定的材料号["+ plan_tmmsm01["MAT_NO"].ToString() +"]在主档不存在!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (aim_tmmsm01.Query("MAT_NO") == false)
		{
			sprintf(s.msg, "计划要求替换的目标材料号["+ aim_tmmsm01["MAT_NO"].ToString() +"]在主档不存在!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//Log::Trace("", __FUNCTION__, "调用物料跟踪 修改计划指定的材料号主档信息,plan_tmmsm01.MAT_NO		= [{0}]",plan_tmmsm01["MAT_NO"].ToString());

		/* 调用物料跟踪 修改计划指定的材料号主档信息 */
		bcls_rec->Tables["MM0099"].Clear();
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_LINE_TYPE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SYSTEM_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"FUNC_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"KEYVALUE_1");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"KEYVALUE_1_DESC");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MAT_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"ORDER_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PLAN_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PONO_SLAB");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PONO_SLAB_1");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG_CODE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG_SEQ");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"NEXT_WHOLE_BACKLOG_CODE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"NEXT_WHOLE_BACKLOG_SEQ");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SUB_BACKLOG_CODE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SUB_BACKLOG_SEQ");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"NEXT_SUB_BACKLOG_CODE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"NEXT_SUB_BACKLOG_SEQ");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MSC");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PSC");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MSC_LINE_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"FIN_CUST_CODE");
		bcls_rec->Tables["MM0099"].Rows.Add();
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"]					= "PS71";	
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"]			= "HP"; 
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"]					= "MMHP"; 
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"]					= "f_mmsm62"; 
		bcls_rec->Tables["MM0099"].Rows[0]["KEYVALUE_1"]				= aim_tmmsm01["MAT_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["KEYVALUE_1_DESC"]			= "计划要求替换的目标材料号"; 
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"]					= plan_tmmsm01["MAT_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["ORDER_NO"]					= aim_tmmsm01["ORDER_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PLAN_NO"]					= aim_tmmsm01["PLAN_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB"]					= aim_tmmsm01["PONO_SLAB"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_1"]				= aim_tmmsm01["PONO_SLAB_1"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG"]				= aim_tmmsm01["WHOLE_BACKLOG"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG_NO"]			= aim_tmmsm01["WHOLE_BACKLOG_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG_CODE"]		= aim_tmmsm01["WHOLE_BACKLOG_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG_SEQ"]			= aim_tmmsm01["WHOLE_BACKLOG_SEQ"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["NEXT_WHOLE_BACKLOG_CODE"]	= aim_tmmsm01["NEXT_WHOLE_BACKLOG_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["NEXT_WHOLE_BACKLOG_SEQ"]	= aim_tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["SUB_BACKLOG_CODE"]			= aim_tmmsm01["SUB_BACKLOG_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["SUB_BACKLOG_SEQ"]			= aim_tmmsm01["SUB_BACKLOG_SEQ"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["NEXT_SUB_BACKLOG_CODE"]		= aim_tmmsm01["NEXT_SUB_BACKLOG_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["NEXT_SUB_BACKLOG_SEQ"]		= aim_tmmsm01["NEXT_SUB_BACKLOG_SEQ"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["MSC"]						= aim_tmmsm01["MSC"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PSC"]						= aim_tmmsm01["PSC"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["MSC_LINE_NO"]				= aim_tmmsm01["MSC_LINE_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["FIN_CUST_CODE"]				= aim_tmmsm01["FIN_CUST_CODE"]; 
		doFlag = f_mmsm99(bcls_rec, bcls_ret,conn);
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//Log::Trace("", __FUNCTION__, "调用物料跟踪 计划要求替换的目标材料号主档信息,aim_tmmsm01.MAT_NO		= [{0}]",aim_tmmsm01["MAT_NO"].ToString());

		/* 调用物料跟踪 计划要求替换的目标材料号主档信息 */
		bcls_rec->Tables["MM0099"].Clear();
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_LINE_TYPE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SYSTEM_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"FUNC_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"KEYVALUE_1");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"KEYVALUE_1_DESC");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MAT_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"ORDER_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PLAN_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PONO_SLAB");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PONO_SLAB_1");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG_CODE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG_SEQ");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"NEXT_WHOLE_BACKLOG_CODE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"NEXT_WHOLE_BACKLOG_SEQ");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SUB_BACKLOG_CODE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SUB_BACKLOG_SEQ");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"NEXT_SUB_BACKLOG_CODE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"NEXT_SUB_BACKLOG_SEQ");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MSC");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PSC");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MSC_LINE_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"FIN_CUST_CODE");
		bcls_rec->Tables["MM0099"].Rows.Add();
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"]					= "PS71";	
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"]			= "HP"; 
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"]					= "MMHP"; 
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"]					= "f_mmsm62"; 
		bcls_rec->Tables["MM0099"].Rows[0]["KEYVALUE_1"]				= plan_tmmsm01["MAT_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["KEYVALUE_1_DESC"]			= "计划指定的材料号"; 
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"]					= aim_tmmsm01["MAT_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["ORDER_NO"]					= plan_tmmsm01["ORDER_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PLAN_NO"]					= plan_tmmsm01["PLAN_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB"]					= plan_tmmsm01["PONO_SLAB"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB_1"]				= plan_tmmsm01["PONO_SLAB_1"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG"]				= plan_tmmsm01["WHOLE_BACKLOG"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG_NO"]			= plan_tmmsm01["WHOLE_BACKLOG_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG_CODE"]		= plan_tmmsm01["WHOLE_BACKLOG_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG_SEQ"]			= plan_tmmsm01["WHOLE_BACKLOG_SEQ"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["NEXT_WHOLE_BACKLOG_CODE"]	= plan_tmmsm01["NEXT_WHOLE_BACKLOG_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["NEXT_WHOLE_BACKLOG_SEQ"]	= plan_tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["SUB_BACKLOG_CODE"]			= plan_tmmsm01["SUB_BACKLOG_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["SUB_BACKLOG_SEQ"]			= plan_tmmsm01["SUB_BACKLOG_SEQ"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["NEXT_SUB_BACKLOG_CODE"]		= plan_tmmsm01["NEXT_SUB_BACKLOG_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["NEXT_SUB_BACKLOG_SEQ"]		= plan_tmmsm01["NEXT_SUB_BACKLOG_SEQ"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["MSC"]						= plan_tmmsm01["MSC"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PSC"]						= plan_tmmsm01["PSC"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["MSC_LINE_NO"]				= plan_tmmsm01["MSC_LINE_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["FIN_CUST_CODE"]				= plan_tmmsm01["FIN_CUST_CODE"]; 
		doFlag = f_mmsm99(bcls_rec, bcls_ret,conn);
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//Log::Trace("", __FUNCTION__, "目的板坯信息材料号替换,plan_tmmsm01.MAT_NO		= [{0}]",plan_tmmsm01["MAT_NO"].ToString());

		/***** 目的板坯信息材料号替换 BEGIN *****/
		/* 查询 计划指定的材料号 目的板坯信息 */
		plan_tmmsm03["MAT_NO"] = plan_tmmsm01["MAT_NO"];
		if (plan_tmmsm03.QueryCount("MAT_NO") != 1)
		{
			sprintf(s.msg, "计划指定的材料号["+ plan_tmmsm01["MAT_NO"].ToString() +"]必须是短坯,在目的档中只能有1条记录!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (plan_tmmsm03.Query("MAT_NO") == false)
		{
			sprintf(s.msg, "计划指定的材料号["+ plan_tmmsm01["MAT_NO"].ToString() +"]在目的档不存在!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		/* 查询 计划要求替换的 目的板坯信息 */
		aim_tmmsm03["MAT_NO"] = aim_tmmsm01["MAT_NO"];
		if (aim_tmmsm03.QueryCount("MAT_NO") != 1)
		{
			sprintf(s.msg, "计划要求替换的目标材料号["+ plan_tmmsm01["MAT_NO"].ToString() +"]必须是短坯,在目的档中只能有1条记录!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (aim_tmmsm03.Query("MAT_NO") == false)
		{
			sprintf(s.msg, "计划要求替换的目标材料号["+ aim_tmmsm01["MAT_NO"].ToString() +"]在目的档不存在!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//Log::Trace("", __FUNCTION__, "删除目的板坯信息,plan_tmmsm03.MAT_NO		= [{0}]",plan_tmmsm03["MAT_NO"].ToString());

		/* 删除目的板坯信息 */
		plan_tmmsm03.Delete("MAT_NO");
		aim_tmmsm03.Delete("MAT_NO");

		//Log::Trace("", __FUNCTION__, "新增目的板坯信息,plan_tmmsm03.MAT_NO		= [{0}]",plan_tmmsm03["MAT_NO"].ToString());

		/* 新增目的板坯信息 实现目的板坯替换 */
		plan_tmmsm03["MAT_NO"]		= aim_tmmsm01["MAT_NO"];
		plan_tmmsm03["AIM_MAT_NO"] = aim_tmmsm01["MAT_NO"];
		plan_tmmsm03.TrimOrBlank();
		plan_tmmsm03.Insert();
		aim_tmmsm03["MAT_NO"]		= plan_tmmsm01["MAT_NO"];
		aim_tmmsm03["AIM_MAT_NO"]	= plan_tmmsm01["MAT_NO"];
		aim_tmmsm03.TrimOrBlank();
		aim_tmmsm03.Insert();

		/********** 目的板坯信息材料号替换 END **********/

		//Log::Trace("", __FUNCTION__, "板坯工序信息材料号替换,plan_tmmsm03.MAT_NO		= [{0}]",plan_tmmsm03["MAT_NO"].ToString());

		/********** 板坯工序信息材料号替换 BEGIN **********/
		/* 查询 计划指定的材料号 板坯工序信息 */
		bcls_rec->Tables["MMSM63_AIM_TMMSM04"].Clear();
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
						 "  FROM TMMSM04 "
						 " WHERE MAT_NO = @aim_tmmsm01.MAT_NO ";
				break;
		}  	
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("aim_tmmsm01.MAT_NO",aim_tmmsm01["MAT_NO"].ToString()); 
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(aim_tmmsm04);
			aim_tmmsm04.TrimOrBlank();

			aim_tmmsm04.MergeTo(bcls_rec->Tables["MMSM63_AIM_TMMSM04"],false);
		}
		cmd_inq.Close();

		//Log::Trace("", __FUNCTION__, "查询 计划要求替换的 板坯工序信息,plan_tmmsm01.MAT_NO		= [{0}]",plan_tmmsm01["MAT_NO"].ToString());

		/* 查询 计划要求替换的 板坯工序信息 */
		bcls_rec->Tables["MMSM63_PLAN_TMMSM04"].Clear();
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
						 "  FROM TMMSM04 "
						 " WHERE MAT_NO = @plan_tmmsm01.MAT_NO ";
				break;
		}  	
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("plan_tmmsm01.MAT_NO",plan_tmmsm01["MAT_NO"].ToString()); 
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(plan_tmmsm04);
			plan_tmmsm04.TrimOrBlank();

			plan_tmmsm04.MergeTo(bcls_rec->Tables["MMSM63_PLAN_TMMSM04"],false);

		}
		cmd_inq.Close();

		//Log::Trace("", __FUNCTION__, "删除板坯工序,plan_tmmsm01.MAT_NO		= [{0}]",plan_tmmsm01["MAT_NO"].ToString());

		/* 删除板坯工序 */
		plan_tmmsm04.Delete("MAT_NO");
		aim_tmmsm04.Delete("MAT_NO");

		//Log::Trace("", __FUNCTION__, "新增板坯工序信息 实现板坯工序替换,plan_tmmsm01.MAT_NO		= [{0}]",plan_tmmsm01["MAT_NO"].ToString());

		/* 新增板坯工序信息 实现板坯工序替换 */
		for (int i = 0; i < bcls_rec->Tables["MMSM63_AIM_TMMSM04"].Rows.get_Count(); i++)
		{
			aim_tmmsm04.MergeFrom(bcls_rec->Tables["MMSM63_AIM_TMMSM04"].Rows[i]);

			aim_tmmsm04["MAT_NO"]		= plan_tmmsm01["MAT_NO"];
			aim_tmmsm04["AIM_MAT_NO"]	= plan_tmmsm01["MAT_NO"];
			aim_tmmsm04.TrimOrBlank();
			aim_tmmsm04.Insert();     
		}

		//Log::Trace("", __FUNCTION__, "新增板坯工序信息 实现板坯工序替换,plan_tmmsm01.MAT_NO		= [{0}]",plan_tmmsm01["MAT_NO"].ToString());
		/* 新增板坯工序信息 实现板坯工序替换 */
		for (int i = 0; i < bcls_rec->Tables["MMSM63_PLAN_TMMSM04"].Rows.get_Count(); i++)
		{
			plan_tmmsm04.MergeFrom(bcls_rec->Tables["MMSM63_PLAN_TMMSM04"].Rows[i]);

			plan_tmmsm04["MAT_NO"]		= aim_tmmsm01["MAT_NO"];
			plan_tmmsm04["AIM_MAT_NO"]	= aim_tmmsm01["MAT_NO"];
			plan_tmmsm04.TrimOrBlank();
			plan_tmmsm04.Insert();     
		}
		/********** 板坯工序信息材料号替换 END **********/

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
