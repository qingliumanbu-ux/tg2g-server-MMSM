/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 厚板侧板坯余材组板充当
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 厚板侧板坯余材组板充当
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  /*炼钢物料主表*/
 
//2022-08-12 去头文件时编译报错 HP函数不存在 暂时未编译
 
 
 

//外部函数声明 
BM2_FUNCTION_IMPORT
 int f_mmhp0004_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);  
BM2_FUNCTION_IMPORT
 int f_mmhp0005_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);  
BM2_FUNCTION_IMPORT
 int f_mmhp0008_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);  
BM2_FUNCTION_IMPORT
 int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);  


BM2_FUNCTION_EXPORT
 int f_mmsm61(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    
	CString com_pono_no[8];
	CString	rem_pono_no("");    
	CString	cs_aim_mat_no("");    
	CString	cs_whole_backlog_sm("");    
	CDecimal cd_prod_density = 0;    
	CDecimal cd_pono_slab_num = 0;    
	int		ord_com_num=0;					//合同材命令板坯数
	int		remain_num				= 0;	//余材数量（无合同）
	int		com_num					= 0;	//命令板坯数量n
	int		i_updown_flag			= 0;	//余材是头或尾标记
	long	sum_pre_clean_slab_len	= 0;
	double	sum_infur_slab_max_wt	= 0.0;
	double	sum_infur_slab_min_wt	= 0.0;
	double	sum_infur_slab_wt		= 0.0;
	long	Len						= 0;	//由各个命令板坯反算原坯总长
	int		slab_cut_gap			= 10;	//切缝宽度
	double	Wt_max					= 0.0;	//由各个命令板坯反算原坯总重（最大）
	double	Wt_min					= 0.0;	//由各个命令板坯反算原坯总重（最小）
	double	Wt						= 0.0;	//由各个命令板坯反算原坯总重
	long	Remain_len				= 0;	//余长坯长度
	double	Remain_wt				= 0.0;	//余长坯重量
	long	L_x;						//原坯长度与各个定尺坯（命令板坯）之和的差(x)
	double	L_w;						//原坯重量与各个定尺坯（命令板坯）之和的差(w)
	int		i_whole_backlog_num		= 0;

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm03("TMMSM03");
	CModel tmmsm04("TMMSM04");
	CModel tmmsm96("TMMSM96");
	CModel tpmouhp30("TPMOUHP30");
	CModel tpmouhp32("TPMOUHP32");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_tmmsm01(conn);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MMSM61");
		if(blkNum < 0)
		{
			strcpy(s.msg,_RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			strcpy(s.sysmsg,"传入数据块 MMSM61 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MMHP0004");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMHP0004"); 
			bcls_rec->Tables["MMHP0004"].Columns.Add(DT_STRING,"MAT_NO");
			bcls_rec->Tables["MMHP0004"].Columns.Add(DT_STRING,"INFUR_SLAB_WT");
			bcls_rec->Tables["MMHP0004"].Columns.Add(DT_STRING,"AIM_MAT_NO");
			bcls_rec->Tables["MMHP0004"].Columns.Add(DT_STRING,"INFUR_SLAB_LEN");
			bcls_rec->Tables["MMHP0004"].Columns.Add(DT_STRING,"INFUR_SLAB_THICK");
			bcls_rec->Tables["MMHP0004"].Columns.Add(DT_STRING,"INFUR_SLAB_WID");
			bcls_rec->Tables["MMHP0004"].Columns.Add(DT_STRING,"ORDER_REMAIN_DIV");
			bcls_rec->Tables["MMHP0004"].Columns.Add(DT_STRING, "PONO_SLAB");
		}
		else
		{
			bcls_rec->Tables["MMHP0004"].Rows.Clear();
		}
		bcls_rec->Tables["MMHP0004"].Rows.Add(); // 创建一行
		blkNum = bcls_rec->Tables.IndexOf("MMHP0005");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMHP0005"); 
			bcls_rec->Tables["MMHP0005"].Columns.Add(DT_STRING,"MAT_NO");
			bcls_rec->Tables["MMHP0005"].Columns.Add(DT_STRING,"PONO_SLAB");
			bcls_rec->Tables["MMHP0005"].Columns.Add(DT_STRING,"FIX_SLAB_NUM");
			bcls_rec->Tables["MMHP0005"].Columns.Add(DT_STRING,"CUT_SEQ");
			bcls_rec->Tables["MMHP0005"].Columns.Add(DT_STRING,"MAT_WT_FLAG");
			bcls_rec->Tables["MMHP0005"].Columns.Add(DT_STRING,"AIM_MAT_NO");
		}
		else
		{
			bcls_rec->Tables["MMHP0005"].Rows.Clear();
		}
		bcls_rec->Tables["MMHP0005"].Rows.Add(); // 创建一行
		blkNum = bcls_rec->Tables.IndexOf("MMHP0008");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMHP0008"); 
			bcls_rec->Tables["MMHP0008"].Columns.Add(DT_STRING,"MAT_NO");
			bcls_rec->Tables["MMHP0008"].Columns.Add(DT_STRING,"CUT_NUM");
			bcls_rec->Tables["MMHP0008"].Columns.Add(DT_STRING,"CUT_SEQ");
		}
		else
		{
			bcls_rec->Tables["MMHP0008"].Rows.Clear();
		}
		bcls_rec->Tables["MMHP0008"].Rows.Add(); // 创建一行
		blkNum = bcls_rec->Tables.IndexOf("MM0099");				
		if(blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099"); 
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MAT_NO");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"ORDER_NO");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG_NO");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"NEXT_WHOLE_BACKLOG_CODE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"NEXT_WHOLE_BACKLOG_SEQ");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PONO_SLAB");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PLATE_DT_CODE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"APN");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"STD_SG_CODE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MSC_LINE_NO");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG_CODE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"WHOLE_BACKLOG_SEQ");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PSC");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MSC");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PROD_CODE_HP");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SG_STD");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SG_SIGN");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"BD_FLAG");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"IF_IN_SECUT_FLAG");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SURFACE_DECIDE_CODE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"PCH_JUDGE_CODE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"COMPLEX_DECIDE_CODE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MAT_DESTION");
		}
		else
		{
			bcls_rec->Tables["MM0099"].Rows.Clear();
		}
		bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行

		/* 获取输入参数 */
		tmmsm96["EVENT_ID"]		= bcls_rec->Tables["MMSM61"].Rows[0]["EVENT_ID"].ToString().Trim();	//调用事件号
		tmmsm96["EVENT_LINE_TYPE"]	= bcls_rec->Tables["MMSM61"].Rows[0]["EVENT_LINE_TYPE"].ToString().Trim();	
		tmmsm96["SYSTEM_ID"]		= bcls_rec->Tables["MMSM61"].Rows[0]["SYSTEM_ID"].ToString().Trim();	
		tmmsm96["FUNC_ID"]			= bcls_rec->Tables["MMSM61"].Rows[0]["FUNC_ID"].ToString().Trim();
		tmmsm96["MAT_NO"]			= bcls_rec->Tables["MMSM61"].Rows[0]["MAT_NO"].ToString().Trim();
		tmmsm96["ORDER_NO"]		= bcls_rec->Tables["MMSM61"].Rows[0]["ORDER_NO"].ToString().Trim();
		tmmsm96["WHOLE_BACKLOG_NO"]		= bcls_rec->Tables["MMSM61"].Rows[0]["WHOLE_BACKLOG_NO"].ToDecimal();
		tmmsm96["NEXT_WHOLE_BACKLOG_CODE"]	= bcls_rec->Tables["MMSM61"].Rows[0]["NEXT_WHOLE_BACKLOG_CODE"].ToString().Trim();
		tmmsm96["NEXT_WHOLE_BACKLOG_SEQ"]	= bcls_rec->Tables["MMSM61"].Rows[0]["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal();

		/* 打印输入参数 */
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm96.EVENT_ID		= [{0}]",tmmsm96["EVENT_ID"].ToString());		//调用事件号
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm96.MAT_NO		= [{0}]",tmmsm96["MAT_NO"].ToString());		//母材料号
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm96.ORDER_NO		= [{0}]",tmmsm96["ORDER_NO"].ToString());		
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm96.WHOLE_BACKLOG_NO		= [{0}]",tmmsm96["WHOLE_BACKLOG_NO"].ToDecimal());		
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm96.NEXT_WHOLE_BACKLOG_CODE	= [{0}]",tmmsm96["NEXT_WHOLE_BACKLOG_CODE"].ToString());		
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm96.NEXT_WHOLE_BACKLOG_SEQ	= [{0}]",tmmsm96["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal());		

		/* 检查输入参数合法性 */
		if(tmmsm96["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,"数据校验失败，材料号不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(tmmsm96["ORDER_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,"数据校验失败，合同号不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(tmmsm96["NEXT_WHOLE_BACKLOG_CODE"].ToString().Trim() == "")
		{
			strcpy(s.msg,"数据校验失败，下全程工序代码不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(tmmsm96["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal() <= 0)
		{
			strcpy(s.msg,"数据校验失败，下全程工序顺序号不能为0。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 删除目的材料表 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " DELETE FROM TMMSM03 "
						 "  WHERE MAT_NO = @tmmsm96.MAT_NO ";
				break;
		}     
		cmd_inq.SetCommandText(sqlstr);
		//Log::Trace("", __FUNCTION__, "删除目的材料表 sqlstr		= [{0}]",sqlstr);	
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		/* 删除材料工序表 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " DELETE FROM TMMSM04 "
						 "  WHERE MAT_NO = @tmmsm96.MAT_NO ";
				break;
		}     
		cmd_inq.SetCommandText(sqlstr);
		//Log::Trace("", __FUNCTION__, "删除材料工序表 sqlstr		= [{0}]",sqlstr);	
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();

		/* 查询命令板坯表 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT MIN(PONO_SLAB) "
						 "	 FROM TPMOUHP30 "
						 "  WHERE ORDER_REMAIN_DIV = '1' "
						 "	  AND SLAB_NO = @tmmsm96.MAT_NO";
				break;
		}     
		//Log::Trace("",__FUNCTION__,"查询命令板坯表 sqlstr = [{0}]",sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
	    cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
		cmd_inq.ExecuteReader(); 
		if(cmd_inq.Read())
		{		
			tmmsm96["PONO_SLAB"] = cmd_inq.GetString(1);
		}
		else
		{
			strcpy(s.msg,"没有命令信息(TPMOUHP30)。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();	
		//Log::Trace("", __FUNCTION__, "tmmsm96.PONO_SLAB		= [{0}]", tmmsm96["PONO_SLAB"].ToString());

		/* 取密度和去向 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT PLATE_DT_CODE, "
						 "		  PROD_DENSITY "
						 "	 FROM TPMOUHP31 "
						 "  WHERE PONO_SLAB = @tmmsm96.PONO_SLAB";
				break;
		}     
		//Log::Trace("",__FUNCTION__,"取密度和去向 sqlstr = [{0}]",sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmmsm96.PONO_SLAB", tmmsm96["PONO_SLAB"].ToString());
		cmd_inq.ExecuteReader(); 
		if(cmd_inq.Read())
		{		
			tmmsm96["PLATE_DT_CODE"] = cmd_inq.GetString(1);
			cd_prod_density			= cmd_inq.GetDecimal(2);
		}
		else
		{
			strcpy(s.msg,"没有命令信息(TPMOUHP31)。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();	
		if (cd_prod_density <= 0) 
		{
			cd_prod_density = 7.85;
		}
		//Log::Trace("", __FUNCTION__, "取密度和去向 tmmsm96.PLATE_DT_CODE		= [{0}]", tmmsm96["PLATE_DT_CODE"].ToString());
		//Log::Trace("", __FUNCTION__, "取密度和去向 cd_prod_density			= [{0}]",cd_prod_density);

		/* 查询命令参数表_厚板 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "	 FROM TPMOUHP32 "
						 "  WHERE SLAB_NO = @tmmsm96.MAT_NO "
						 "    AND ORDER_NO = @tmmsm96.ORDER_NO ";
				break;
		}     
		//Log::Trace("",__FUNCTION__,"查询命令参数表_厚板 sqlstr = [{0}]",sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
	    cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
	    cmd_inq.Parameters.Set("tmmsm96.ORDER_NO",tmmsm96["ORDER_NO"].ToString());
		cmd_inq.ExecuteReader(); 
		if(cmd_inq.Read())
		{		
			cmd_inq.Fetch(tpmouhp32);		
			tpmouhp32.TrimOrBlank();
		}
		cmd_inq.Close();	

		//Log::Trace("",__FUNCTION__,"查询命令参数表_厚板 tpmouhp32["ORDER_NO"] = [{0}]",tpmouhp32["ORDER_NO"].ToString());

		#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
		/****** 按合同号查询合同信息 BEGIN ******/
		/* 查询合同主档表 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "SELECT APN,"
						 "		 STD_SG_CODE "
						 "  FROM TOM01 "
						 " WHERE ORDER_NO = @tmmsm96.ORDER_NO ";
				break; 
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmmsm96.ORDER_NO",tmmsm96["ORDER_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm96["APN"] = cmd_inq.GetString(1);
			tmmsm96["STD_SG_CODE"] = cmd_inq.GetString(2);
		}
		else
		{
			tmmsm96["APN"] = "";
			tmmsm96["STD_SG_CODE"] = "";
		}
		cmd_inq.Close();

		//Log::Trace("", __FUNCTION__, "查询合同主档表 标准牌号(钢级)代码	tmmsm96.APN			= [{0}]", tmmsm96["APN"].ToString());
		//Log::Trace("", __FUNCTION__, "查询合同主档表 标准牌号(钢级)代码	tmmsm96["STD_SG_CODE"] = [{0}]", tmmsm96["STD_SG_CODE"].ToString());

		/* 查询TQMTO02获取全程工序信息 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "SELECT WHOLE_BACKLOG,"
						 "		 MSC_LINE_NO "
						 "  FROM TQMTO02 "
						 " WHERE ORDER_NO		  = @tmmsm96.ORDER_NO "
						 "   AND WHOLE_BACKLOG_NO = @tmmsm96.WHOLE_BACKLOG_NO ";   //合同制程
				break; 
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmmsm96.ORDER_NO",tmmsm96["ORDER_NO"].ToString());
		cmd_inq.Parameters.Set("tmmsm96.WHOLE_BACKLOG_NO",tmmsm96["WHOLE_BACKLOG_NO"].ToDecimal());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm96["WHOLE_BACKLOG"] = cmd_inq.GetString(1);
			tmmsm96["MSC_LINE_NO"] = cmd_inq.GetString(2);  //取主工序产线号
		}
		else
		{
			//找不到工序信息
			sprintf(s.msg, "材料[%s]所属合同[%s]找不到工序信息。", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["ORDER_NO"].ToString());
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		cmd_inq.Close();

		//Log::Trace("", __FUNCTION__, "查询TQMTO02获取工序信息 全程工序途径码	tmmsm96["WHOLE_BACKLOG"] = [{0}]", tmmsm96["WHOLE_BACKLOG"].ToString());
		//Log::Trace("", __FUNCTION__, "查询TQMTO02获取工序信息 制程号			tmmsm96["WHOLE_BACKLOG_NO"] = [{0}]", tmmsm96["WHOLE_BACKLOG_NO"].ToDecimal());
		//Log::Trace("", __FUNCTION__, "查询TQMTO02获取工序信息 产线号			tmmsm96["MSC_LINE_NO"] = [{0}]", tmmsm96["MSC_LINE_NO"].ToString());

		/* 和生产商议后 由生产传入下工序代码和顺序号 下段代码可注释 BEGIN 20170327 */
		/* 获取全程工序代码 */
		///* 全程工序和质量厂别区分有一个对应关系****/
		//switch(conn->DatabaseKind)
		//{
		//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:				// MS SQL Server数据库
		//	case DB_KIND_ORACLE:	        // Oracle 数据库
		//	default:
		//		sqlstr = " SELECT SUBSTR(CODE,1,1) "
		//				 "	 FROM TEP0002 "
		//				 "  WHERE CODE_CLASS 	=  'PM2A' "
		//				 "	  AND CODE_DESC_1_CONTENT = "
		//				 "		  (SELECT FACTORY_DIV FROM TMMSM01 WHERE MAT_NO = @tmmsm96["MAT_NO"].ToString()) ";
		//		break;
		//}     
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Clear();
		//cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
		//cmd_inq.ExecuteReader();
		//if(cmd_inq.Read())
		//{		
		//	cs_whole_backlog_sm = cmd_inq.GetString(1);
		//} 
		//cmd_inq.Close();	

		//Log::Trace("",__FUNCTION__,"获取全程工序代码 cs_whole_backlog_sm = [{0}]",cs_whole_backlog_sm);
		//if (cs_whole_backlog_sm.Trim() == "")
		//{
		//	sprintf(s.msg, "材料[%s]的厂别未在代码[PM2A]找到对应大工序，请对代码进行维护。", (const char*)tmmsm96["MAT_NO"].ToString());
		//	throw CApplicationException(-1, s.msg, log.Location); 
		//}
		//i_whole_backlog_num = tmmsm01["WHOLE_BACKLOG"].ToString().Find(cs_whole_backlog_sm);
		//if(i_whole_backlog_num % 2 != 0)
		//{
		//	sprintf(s.msg, "材料[%s]所属合同[%s]找不到工序信息。", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["ORDER_NO"].ToString());
		//	throw CApplicationException(-1, s.msg, log.Location); 
		//}

		////取炼钢全程工序代码（2位）及当前全程工序顺序号
		//tmmsm01["WHOLE_BACKLOG_CODE"]		= tmmsm01["WHOLE_BACKLOG"].ToString().SubstringNE(i_whole_backlog_num, 2);	//全程工序代码
		//tmmsm01["WHOLE_BACKLOG_SEQ"]		= (tmmsm01["WHOLE_BACKLOG"].ToString().Find(tmmsm01["WHOLE_BACKLOG_CODE"].ToString()) / 2) + 1;	//全程工序顺序号N2
		//tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"]	= tmmsm01["WHOLE_BACKLOG_SEQ"].ToDecimal() + 1;	//后全程工序顺序号N2
		//tmmsm01["NEXT_WHOLE_BACKLOG_CODE"] = tmmsm01["WHOLE_BACKLOG"].ToString().SubstringNE((tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal().ToInt16() * 2) - 2,2);
		/* 和生产商议后 由生产传入下工序代码和顺序号 上段代码可注释 END 20170327 */

		//炼钢侧可能存在需指定大工序组板的情况
		//i_whole_backlog_num = tmmsm01["WHOLE_BACKLOG"].ToString().Find("A");
		//if(i_whole_backlog_num % 2 != 0)
		//{
		//	sprintf(s.msg, "材料[%s]所属合同[%s]找不到工序信息。", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["ORDER_NO"].ToString());
		//	throw CApplicationException(-1, s.msg, log.Location); 
		//}

		tmmsm96["WHOLE_BACKLOG_SEQ"] = tmmsm96["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal() - 1;	//全程工序顺序号N2
		tmmsm96["WHOLE_BACKLOG_CODE"] = tmmsm96["WHOLE_BACKLOG"].ToString().SubstringNE((tmmsm96["WHOLE_BACKLOG_SEQ"].ToDecimal().ToInt16() * 2) - 2, 2);	//全程工序代码

		//Log::Trace("", __FUNCTION__, "获取全程工序信息 全程途径码		tmmsm96.WHOLE_BACKLOG			= [{0}]", tmmsm96["WHOLE_BACKLOG"].ToString());
		//Log::Trace("", __FUNCTION__, "获取全程工序信息 后全程工序代码	tmmsm96.WHOLE_BACKLOG_CODE		= [{0}]", tmmsm96["WHOLE_BACKLOG_CODE"].ToString());
		//Log::Trace("", __FUNCTION__, "获取全程工序信息 后全程工序顺序号	tmmsm96.WHOLE_BACKLOG_SEQ		= [{0}]", tmmsm96["WHOLE_BACKLOG_SEQ"].ToDecimal());
		//Log::Trace("", __FUNCTION__, "获取全程工序信息 后全程工序代码	tmmsm96["NEXT_WHOLE_BACKLOG_CODE"] = [{0}]", tmmsm96["NEXT_WHOLE_BACKLOG_CODE"].ToString());
		//Log::Trace("", __FUNCTION__, "获取全程工序信息 后全程工序顺序号	tmmsm96.NEXT_WHOLE_BACKLOG_SEQ	= [{0}]", tmmsm96["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal());
		//if (tmmsm01["NEXT_WHOLE_BACKLOG_CODE"].ToString().Trim() == "")
		//{
		//	strcpy(s.msg,"余材组板后,材料的下个大工序为空,请检查数据。");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		/****** 按合同号查询合同信息 END ******/
		#endif

		//Log::Trace("",__FUNCTION__,"设置板坯主档设置值并修改主档 sqlstr = [{0}]",sqlstr);
		/* 设置板坯主档设置值并修改主档 */
		tmmsm96["PREC_SLAB_NO"]			= tmmsm96["PONO_SLAB"];
		tmmsm96["PSC"]						= tpmouhp32["PSC"];
		tmmsm96["MSC"]						= tpmouhp32["MSC"];
		tmmsm96["PROD_CODE_HP"]			= tpmouhp32["PROD_CODE_HP"];
		tmmsm96["SG_STD"]					= tpmouhp32["SG_STD"];
		tmmsm96["SG_SIGN"]					= tpmouhp32["SG_SIGN"];
		tmmsm96["BD_FLAG"]					= tpmouhp32["TWO_ROLL_MARK"];
		//Log::Trace("",__FUNCTION__,"设置板坯主档设置值并修改主档 tpmouhp32["TWO_ROLL_MARK"] = [{0}]",tpmouhp32["TWO_ROLL_MARK"].ToString());
		tmmsm96["IF_IN_SECUT_FLAG"]		= "";
		tmmsm96["SURFACE_DECIDE_CODE"]		= "1";
		tmmsm96["PCH_JUDGE_CODE"]			= "1";
		tmmsm96["COMPLEX_DECIDE_CODE"]		= "1";
		tmmsm96["MAT_DESTION"]				= "10";

		//tmmsm01["PREC_SLAB_NO"]			= tmmsm01["PONO_SLAB"];
		//tmmsm01["ORDER_NO"]				= tmmsm96["ORDER_NO"];
		//tmmsm01["WHOLE_BACKLOG_NO"]        = tmmsm96["WHOLE_BACKLOG_NO"];
		//tmmsm01["PSC"]						= tpmouhp32["PSC"];
		//tmmsm01["MSC"]						= tpmouhp32["MSC"];
		//tmmsm01["PROD_CODE_HP"]			= tpmouhp32["PROD_CODE_HP"];
		//tmmsm01["SG_STD"]					= tpmouhp32["SG_STD"];
		//tmmsm01["SG_SIGN"]					= tpmouhp32["SG_SIGN"];
		//tmmsm01["BD_FLAG"]					= tpmouhp32["TWO_ROLL_MARK"];
		//Log::Trace("",__FUNCTION__,"设置板坯主档设置值并修改主档 tpmouhp32["TWO_ROLL_MARK"] = [{0}]",tpmouhp32["TWO_ROLL_MARK"].ToString());
		//tmmsm01["PLAN_NO"]					= "";
		//tmmsm01["IF_IN_SECUT_FLAG"]		= "";
		//tmmsm01["SURFACE_DECIDE_CODE"]		= "1";
		//tmmsm01["PCH_JUDGE_CODE"]			= "1";
		//tmmsm01["COMPLEX_DECIDE_CODE"]		= "1";
		//tmmsm01["SUB_BACKLOG_CODE"]		= " ";
		//tmmsm01["SUB_BACKLOG_SEQ"]			= 0;
		//tmmsm01["NEXT_SUB_BACKLOG_CODE"]	= "";
		//tmmsm01["NEXT_SUB_BACKLOG_SEQ"]	= 0;
		//tmmsm01["MAT_DESTION"]				= "10";
		//tmmsm01["MAT_NO"]					= tmmsm96["MAT_NO"];
		//tmmsm01["REC_REVISOR"]				= s.userid;
		//tmmsm01["REC_REVISE_TIME"]			= datetime;

		//Log::Trace("",__FUNCTION__,"tmmsm01.Update sqlstr = [{0}]",sqlstr);
		//tmmsm01.TrimOrBlank();
		//tmmsm01.Update( "PONO_SLAB,"
		//				"PLATE_DT_CODE,"
		//				"APN,"
		//				"STD_SG_CODE,"
		//				"WHOLE_BACKLOG,"
		//				"WHOLE_BACKLOG_NO,"
		//				"MSC_LINE_NO,"
		//				"WHOLE_BACKLOG_CODE,"
		//				"WHOLE_BACKLOG_SEQ,"
		//				"NEXT_WHOLE_BACKLOG_CODE,"
		//				"NEXT_WHOLE_BACKLOG_SEQ,"
		//				"ORDER_NO,"
		//				"PSC,"
		//				"MSC,"
		//				"PROD_CODE_HP,"
		//				"SG_STD,"
		//				"SG_SIGN,"
		//				"BD_FLAG,"
		//				"PLAN_NO,"
		//				"IF_IN_SECUT_FLAG,"
		//				"SURFACE_DECIDE_CODE,"
		//				"PCH_JUDGE_CODE,"
		//				"COMPLEX_DECIDE_CODE,"
		//				"SUB_BACKLOG_CODE,"
		//				"SUB_BACKLOG_SEQ,"
		//				"NEXT_SUB_BACKLOG_CODE,"
		//				"NEXT_SUB_BACKLOG_SEQ,"
		//				"MAT_DESTION,"
		//				"REC_REVISOR,"
		//				"REC_REVISE_TIME","MAT_NO");

		//Log::Trace("",__FUNCTION__,"重新查询板坯主档信息 sqlstr = [{0}]",sqlstr);
		/* 重新查询板坯主档信息 */
		tmmsm01["MAT_NO"]	= tmmsm96["MAT_NO"];
		tmmsm01.Query("MAT_NO");
		tmmsm01.TrimOrBlank();
			
		//Log::Trace("",__FUNCTION__,"计算板坯理重 sqlstr = [{0}]",sqlstr);
		/* 计算板坯理重 */
		tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_ACT_LEN"].ToDecimal() * tmmsm01["MAT_ACT_WIDTH"].ToDecimal() * tmmsm01["MAT_ACT_THICK"].ToDecimal() * cd_prod_density / 1000000000;
		tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);

		//Log::Trace("",__FUNCTION__,"命令板坯表 sqlstr = [{0}]",sqlstr);
		//命令板坯表
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "	 FROM TPMOUHP30 "
						 "  WHERE SLAB_NO = @tmmsm96.MAT_NO "
						 "  ORDER BY PONO_SLAB ASC";
				break;
		}     
		//Log::Trace("",__FUNCTION__,"sqlstr = [{0}]",sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
	    cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
		cmd_inq.ExecuteReader(); 
		while(cmd_inq.Read())
		{		
			cmd_inq.Fetch(tpmouhp30);		
			tpmouhp30.TrimOrBlank();

			cd_pono_slab_num = cd_pono_slab_num + 1;

			//Log::Trace("",__FUNCTION__,"cd_pono_slab_num = cd_pono_slab_num + 1 sqlstr = [{0}]",sqlstr);
			sqlstr = "";
			sqlstr = "PONO_SLAB_" + cd_pono_slab_num.ToString() + "";
			//Log::Trace("", __FUNCTION__, "111 UPDATE TMMSM01 sqlstr = [{0}]",sqlstr);
			tmmsm01["MAT_NO"]			= tmmsm96["MAT_NO"];
			tmmsm01["PONO_SLAB_1"]		= tpmouhp30["PONO_SLAB"];
			tmmsm01.Update(sqlstr,"MAT_NO");
		
			//switch(conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:
			//		sqlstr = " UPDATE TMMSM01 "
			//				 "    SET PONO_SLAB_" + cd_pono_slab_num.ToString() + " = '" + tpmouhp30["PONO_SLAB"].ToString() + "' "
			//				 "  WHERE MAT_NO = '" + tmmsm96["MAT_NO"].ToString() + "' ";
			//		break;
			//}     
			//cmd_inq_tmmsm01.SetCommandText(sqlstr);
			//Log::Trace("", __FUNCTION__, "UPDATE TMMSM01 sqlstr = [{0}]",sqlstr);
			//Log::Trace("", __FUNCTION__, "UPDATE TMMSM01 tpmouhp30["PONO_SLAB"] = [{0}]",tpmouhp30["PONO_SLAB"].ToString());
			//Log::Trace("", __FUNCTION__, "UPDATE TMMSM01 tmmsm96["MAT_NO"] = [{0}]",tmmsm96["MAT_NO"].ToString());
			////cmd_inq_tmmsm01.Parameters.Clear();
			////cmd_inq_tmmsm01.Parameters.Set("tpmouhp30.PONO_SLAB",tpmouhp30["PONO_SLAB"].ToString());
			////cmd_inq_tmmsm01.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
			//cmd_inq_tmmsm01.ExecuteNonQuery();
			//cmd_inq_tmmsm01.Close();
			//Log::Trace("", __FUNCTION__, "END UPDATE TMMSM01 sqlstr = [{0}]",sqlstr);

			//switch(conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:
			//		sqlstr = " UPDATE TMMSM01 "
			//				 "    SET PONO_SLAB_" + cd_pono_slab_num.ToString() + " = @tpmouhp30.PONO_SLAB "
			//				 "  WHERE MAT_NO = @tmmsm96.MAT_NO ";
			//		break;
			//}     
			//cmd_inq_tmmsm01.SetCommandText(sqlstr);
			//Log::Trace("", __FUNCTION__, "UPDATE TMMSM01 sqlstr = [{0}]",sqlstr);
			//Log::Trace("", __FUNCTION__, "UPDATE TMMSM01 tpmouhp30["PONO_SLAB"] = [{0}]",tpmouhp30["PONO_SLAB"].ToString());
			//Log::Trace("", __FUNCTION__, "UPDATE TMMSM01 tmmsm96["MAT_NO"] = [{0}]",tmmsm96["MAT_NO"].ToString());
			//cmd_inq_tmmsm01.Parameters.Clear();
			//cmd_inq_tmmsm01.Parameters.Set("tpmouhp30.PONO_SLAB",tpmouhp30["PONO_SLAB"].ToString());
			//cmd_inq_tmmsm01.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
			//cmd_inq_tmmsm01.ExecuteNonQuery();
			//cmd_inq_tmmsm01.Close();
			//Log::Trace("", __FUNCTION__, "END UPDATE TMMSM01 sqlstr = [{0}]",sqlstr);

			if(tpmouhp30["ORDER_REMAIN_DIV"].ToString().Trim() == "1")
			{
				ord_com_num = ord_com_num + 1;
				com_pono_no[ord_com_num - 1] = tpmouhp30["PONO_SLAB"];

				sum_pre_clean_slab_len = sum_pre_clean_slab_len + tpmouhp30["PRE_CLEAN_SLAB_LEN"].ToDecimal().ToInt32();;
				sum_infur_slab_min_wt  = sum_infur_slab_min_wt  + tpmouhp30["PRE_CLEAN_SLAB_MIN_WT"].ToDecimal().ToDouble();
				sum_infur_slab_max_wt  = sum_infur_slab_max_wt  + tpmouhp30["PRE_CLEAN_SLAB_MAX_WT"].ToDecimal().ToDouble();
				sum_infur_slab_wt      = sum_infur_slab_wt      + tpmouhp30["PRE_CLEAN_SLAB_WT"].ToDecimal().ToDouble();
			}
			else
			{
				remain_num = remain_num + 1;

				if (remain_num == 1)
				{
					rem_pono_no = tpmouhp30["PONO_SLAB"];
				}
			}

			com_num = com_num + 1;

			if (com_num == 1
			&&  remain_num == 1)
			{
				i_updown_flag = 1;
			}
		}
		cmd_inq.Close();

		//Log::Trace("", __FUNCTION__, "余材命令板坯号rem_pono_no = [{0}]", rem_pono_no);

		/* 优化对板坯尺寸的修约规则 */
	    //D、余才板坯组板时，将板坯对应的目标厚度，目标宽度置换为命令板坯厚度，命令板坯宽度
		//Log::Trace("", __FUNCTION__, "优化对板坯尺寸的修约规则 sqlstr = [{0}]",sqlstr);
		tmmsm01["MAT_TARG_THICK"]	= tpmouhp30["PRE_CLEAN_SLAB_THICK"];
		tmmsm01["MAT_TARG_WIDTH"]	= tpmouhp30["PRE_CLEAN_SLAB_WIDTH"];
		tmmsm01["MAT_NO"]			= tmmsm96["MAT_NO"];
		tmmsm01.Update(	"MAT_TARG_THICK,"
						"MAT_TARG_WIDTH","MAT_NO");

		Len     = sum_pre_clean_slab_len + slab_cut_gap*(com_num - 1);
		Wt_max  = sum_infur_slab_max_wt  + slab_cut_gap * (com_num - 1) * cd_prod_density.ToDouble() * tmmsm01["MAT_ACT_THICK"].ToDecimal().ToDouble() * tmmsm01["MAT_ACT_WIDTH"].ToDecimal().ToDouble() / 1000000000;	
		//EDLog(1,1,"命令板坯最大重量之和（考虑切缝）,Wt_max***[%f]***",Wt_max);			
		Wt_min  = sum_infur_slab_min_wt  + slab_cut_gap * (com_num - 1) * cd_prod_density.ToDouble() * tmmsm01["MAT_ACT_THICK"].ToDecimal().ToDouble() * tmmsm01["MAT_ACT_WIDTH"].ToDecimal().ToDouble() / 1000000000;
		//EDLog(1,1,"命令板坯最小重量之和（考虑切缝）,Wt_min***[%f]***",Wt_min);
		Wt      = sum_infur_slab_wt      + slab_cut_gap * (com_num - 1) * cd_prod_density.ToDouble() * tmmsm01["MAT_ACT_THICK"].ToDecimal().ToDouble() * tmmsm01["MAT_ACT_WIDTH"].ToDecimal().ToDouble() / 1000000000;
		//EDLog(1,1,"命令板坯目标重量之和（考虑切缝）,Wt***[%f]***",Wt);
		Remain_len = tmmsm01["MAT_ACT_LEN"].ToDecimal().ToDouble() - sum_pre_clean_slab_len - slab_cut_gap * ord_com_num;//余长坯长度
		//EDLog(1,1,"余长坯长度,Remain_len***[%ld]***",Remain_len);
		Remain_wt  = cd_prod_density.ToDouble() * Remain_len * tmmsm01["MAT_ACT_THICK"].ToDecimal().ToDouble() * tmmsm01["MAT_ACT_WIDTH"].ToDecimal().ToDouble() / 1000000000;	 //余长坯重量
		//EDLog(1,1,"余长坯重量,Remain_wt***[%f]***",Remain_wt);
		//Log::Trace("",__FUNCTION__,"Remain_len = [{0}] Remain_wt = [{1}]",Remain_len,Remain_wt);		
	
		//开始处理
		if(ord_com_num == 0 
		&& remain_num >= 1)
		{
			//所有命令板坯都无合同
			bcls_rec->Tables["MMHP0004"].Rows[0]["MAT_NO"]				= tmmsm96["MAT_NO"];
			bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_WT"]		= tmmsm01["MAT_THEORY_WT"];
			bcls_rec->Tables["MMHP0004"].Rows[0]["AIM_MAT_NO"]			= tmmsm96["MAT_NO"];
			bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_LEN"]		= tmmsm01["MAT_ACT_LEN"];
			bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_THICK"]	= tmmsm01["MAT_ACT_THICK"];
			bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_WID"]		= tmmsm01["MAT_ACT_WIDTH"];
			bcls_rec->Tables["MMHP0004"].Rows[0]["ORDER_REMAIN_DIV"]	= "0";
			bcls_rec->Tables["MMHP0004"].Rows[0]["PONO_SLAB"]			= rem_pono_no;
			doFlag = f_mmhp0004_proc(bcls_rec, bcls_ret,conn);
			if(doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}	
		if(ord_com_num >= 1 
		&& remain_num  >= 1)
		{
			//有合同。有余材
			tmmsm01["FIX_SLAB_NUM"] = ord_com_num + 1;
			tmmsm01.Update("FIX_SLAB_NUM","MAT_NO");

			if(i_updown_flag == 1) //合同材在尾部Ｂ，则将长坯上的所有余材都置到板坯头部
			{
				//生成余长坯号
				bcls_rec->Tables["MMHP0008"].Rows[0]["MAT_NO"]	= tmmsm96["MAT_NO"];
				bcls_rec->Tables["MMHP0008"].Rows[0]["CUT_NUM"]	= ord_com_num + 1;
				bcls_rec->Tables["MMHP0008"].Rows[0]["CUT_SEQ"]	= "1";
				doFlag = f_mmhp0008_proc(bcls_rec, bcls_ret,conn);
				if(doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				cs_aim_mat_no = bcls_rec->Tables["MMHP0008"].Rows[0]["MAT_NO"].ToString();

				bcls_rec->Tables["MMHP0004"].Rows[0]["MAT_NO"]				= tmmsm96["MAT_NO"];
				bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_WT"]		= Remain_wt;
				bcls_rec->Tables["MMHP0004"].Rows[0]["AIM_MAT_NO"]			= cs_aim_mat_no;
				bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_LEN"]		= Remain_len;
				bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_THICK"]	= tmmsm01["MAT_ACT_THICK"];
				bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_WID"]		= tmmsm01["MAT_ACT_WIDTH"];
				bcls_rec->Tables["MMHP0004"].Rows[0]["ORDER_REMAIN_DIV"]	= "0";
				bcls_rec->Tables["MMHP0004"].Rows[0]["PONO_SLAB"]			= rem_pono_no;
				doFlag = f_mmhp0004_proc(bcls_rec, bcls_ret,conn);
				if(doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				
				//生成合同材的目的档
				for(int i = 0;i < ord_com_num;i++)
				{
					bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_NO"]			= tmmsm96["MAT_NO"];
					bcls_rec->Tables["MMHP0005"].Rows[0]["PONO_SLAB"]		= com_pono_no[i];
					bcls_rec->Tables["MMHP0005"].Rows[0]["FIX_SLAB_NUM"]	= ord_com_num + 1;
					bcls_rec->Tables["MMHP0005"].Rows[0]["CUT_SEQ"]			= i + 2;
					bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_WT_FLAG"]		= "0";
					bcls_rec->Tables["MMHP0005"].Rows[0]["AIM_MAT_NO"]		= "";
					doFlag = f_mmhp0005_proc(bcls_rec, bcls_ret,conn);
					if(doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
			}
			else //余材在尾部
			{
				//生成合同材的目的档
				for(int i = 0;i < ord_com_num;i++)
				{
					bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_NO"]			= tmmsm96["MAT_NO"];
					bcls_rec->Tables["MMHP0005"].Rows[0]["PONO_SLAB"]		= com_pono_no[i];
					bcls_rec->Tables["MMHP0005"].Rows[0]["FIX_SLAB_NUM"]	= ord_com_num+1;
					bcls_rec->Tables["MMHP0005"].Rows[0]["CUT_SEQ"]			= i + 1;
					bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_WT_FLAG"]		= "0";
					doFlag = f_mmhp0005_proc(bcls_rec, bcls_ret,conn);
					if(doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}

				//生成余长坯号
				bcls_rec->Tables["MMHP0008"].Rows[0]["MAT_NO"]	= tmmsm96["MAT_NO"];
				bcls_rec->Tables["MMHP0008"].Rows[0]["CUT_NUM"] = ord_com_num + 1;
				bcls_rec->Tables["MMHP0008"].Rows[0]["CUT_SEQ"] = ord_com_num + 1;
				doFlag = f_mmhp0008_proc(bcls_rec, bcls_ret,conn);
				if(doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				cs_aim_mat_no = bcls_rec->Tables["MMHP0008"].Rows[0]["MAT_NO"].ToString();

				bcls_rec->Tables["MMHP0004"].Rows[0]["MAT_NO"]				= tmmsm96["MAT_NO"];
				bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_WT"]		= Remain_wt;
				bcls_rec->Tables["MMHP0004"].Rows[0]["AIM_MAT_NO"]			= cs_aim_mat_no;
				bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_LEN"]		= Remain_len;
				bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_THICK"]	= tmmsm01["MAT_ACT_THICK"];
				bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_WID"]		= tmmsm01["MAT_ACT_WIDTH"];
				bcls_rec->Tables["MMHP0004"].Rows[0]["ORDER_REMAIN_DIV"]	= "0";
				bcls_rec->Tables["MMHP0004"].Rows[0]["PONO_SLAB"]			= rem_pono_no;
				doFlag = f_mmhp0004_proc(bcls_rec, bcls_ret,conn);
				if(doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		} //有合同材有余材
		

        //都是合同材
		if(ord_com_num >= 1 
		&& remain_num == 0)
		{

			//Log::Trace("",__FUNCTION__,"都是合同材");

			if(tmmsm01["MAT_THEORY_WT"].ToDecimal() >= Wt_min 
			&& tmmsm01["MAT_THEORY_WT"].ToDecimal() <= Wt_max)
			{
				//Log::Trace("",__FUNCTION__,"       合同材重量符合");
				//重量符合
				tmmsm01["FIX_SLAB_NUM"] = ord_com_num;
				tmmsm01.Update("FIX_SLAB_NUM","MAT_NO");
               
			    if(ord_com_num == 1)
				{
					//生成一对一的目的板坯
					bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_NO"]			= tmmsm96["MAT_NO"];
					bcls_rec->Tables["MMHP0005"].Rows[0]["PONO_SLAB"]		= com_pono_no[0];
					bcls_rec->Tables["MMHP0005"].Rows[0]["FIX_SLAB_NUM"]	= ord_com_num;
					bcls_rec->Tables["MMHP0005"].Rows[0]["CUT_SEQ"]			= 1;
					bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_WT_FLAG"]		= "0";
					doFlag = f_mmhp0005_proc(bcls_rec, bcls_ret,conn);
					if(doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
				else
				{
					//生成一对多的目的板坯
					for(int i = 0;i < ord_com_num;i++)
					{
						bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_NO"]			= tmmsm96["MAT_NO"];
						bcls_rec->Tables["MMHP0005"].Rows[0]["PONO_SLAB"]		= com_pono_no[i];
						bcls_rec->Tables["MMHP0005"].Rows[0]["FIX_SLAB_NUM"]	= ord_com_num;
						bcls_rec->Tables["MMHP0005"].Rows[0]["CUT_SEQ"]			= i + 1;
						bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_WT_FLAG"]		= "0";
						doFlag = f_mmhp0005_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
				}
			}
			else if(tmmsm01["MAT_THEORY_WT"].ToDecimal() > Wt_max)
			{
			//Log::Trace("",__FUNCTION__,"         合同材实绩重量大于合同总和");
				//实物重量大于命令板坯最大重量之和
				L_w = tmmsm01["MAT_THEORY_WT"].ToDecimal().ToDouble() - Wt;
				L_x = L_w / cd_prod_density.ToDouble() / tmmsm01["MAT_ACT_THICK"].ToDecimal().ToDouble() / tmmsm01["MAT_ACT_WIDTH"].ToDecimal().ToDouble() * 1000000000;

				if(L_x >= 1500 || (L_x < 1500 && L_x >= 1000 && tmmsm01["MAT_ACT_WIDTH"].ToDecimal().ToDouble() >= 1500))
				{
					//余长>=1500mm或者1000mm<=余长<1500但宽度>=1500，则产生余长坯
					tmmsm01["FIX_SLAB_NUM"] = ord_com_num + 1;
					tmmsm01.Update("FIX_SLAB_NUM","MAT_NO");
					
					//生成合同材的目的档
					for(int i = 0;i < ord_com_num;i++)
					{
						bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_NO"]			= tmmsm96["MAT_NO"];
						bcls_rec->Tables["MMHP0005"].Rows[0]["PONO_SLAB"]		= com_pono_no[i];
						bcls_rec->Tables["MMHP0005"].Rows[0]["FIX_SLAB_NUM"]	= ord_com_num + 1;
						bcls_rec->Tables["MMHP0005"].Rows[0]["CUT_SEQ"]			= i + 1;
						bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_WT_FLAG"]		= "0";
						doFlag = f_mmhp0005_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}

					//生成余长坯号
					bcls_rec->Tables["MMHP0008"].Rows[0]["MAT_NO"] = tmmsm96["MAT_NO"];
					bcls_rec->Tables["MMHP0008"].Rows[0]["CUT_NUM"] = ord_com_num + 1;
					bcls_rec->Tables["MMHP0008"].Rows[0]["CUT_SEQ"] = ord_com_num + 1;
					doFlag = f_mmhp0008_proc(bcls_rec, bcls_ret,conn);
					if(doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					cs_aim_mat_no = bcls_rec->Tables["MMHP0008"].Rows[0]["MAT_NO"].ToString();

					bcls_rec->Tables["MMHP0004"].Rows[0]["MAT_NO"]				= tmmsm96["MAT_NO"];
					bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_WT"]		= Remain_wt;
					bcls_rec->Tables["MMHP0004"].Rows[0]["AIM_MAT_NO"]			= cs_aim_mat_no;
					bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_LEN"]		= Remain_len;
					bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_THICK"]	= tmmsm01["MAT_ACT_THICK"];
					bcls_rec->Tables["MMHP0004"].Rows[0]["INFUR_SLAB_WID"]		= tmmsm01["MAT_ACT_WIDTH"];
					bcls_rec->Tables["MMHP0004"].Rows[0]["ORDER_REMAIN_DIV"]	= "0";
					bcls_rec->Tables["MMHP0004"].Rows[0]["PONO_SLAB"]			= rem_pono_no;
					doFlag = f_mmhp0004_proc(bcls_rec, bcls_ret,conn);
					if(doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
				else //L_x<1500
				{
					tmmsm01["FIX_SLAB_NUM"] = ord_com_num;
					tmmsm01.Update("FIX_SLAB_NUM","MAT_NO");

					if(ord_com_num == 1)
					{
						//生成一对一的目的板坯
						bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_NO"]			= tmmsm96["MAT_NO"];
						bcls_rec->Tables["MMHP0005"].Rows[0]["PONO_SLAB"]		= com_pono_no[0];
						bcls_rec->Tables["MMHP0005"].Rows[0]["FIX_SLAB_NUM"]	= ord_com_num;
						bcls_rec->Tables["MMHP0005"].Rows[0]["CUT_SEQ"]			= 1;
						bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_WT_FLAG"]		= "0";
						doFlag = f_mmhp0005_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
					else
					{
						//生成一对多的目的板坯
						for(int i = 0;i < ord_com_num;i++)
						{
							bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_NO"]			= tmmsm96["MAT_NO"];
							bcls_rec->Tables["MMHP0005"].Rows[0]["PONO_SLAB"]		= com_pono_no[i];
							bcls_rec->Tables["MMHP0005"].Rows[0]["FIX_SLAB_NUM"]	= ord_com_num;
							bcls_rec->Tables["MMHP0005"].Rows[0]["CUT_SEQ"]			= i + 1;
							bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_WT_FLAG"]		= "0";
							doFlag = f_mmhp0005_proc(bcls_rec, bcls_ret,conn);
							if(doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
					}
				}
			}
			else
			{
				//Log::Trace("",__FUNCTION__,"         合同材小于合同重量总和");
				//实物重量小于命令板坯最小重量之和
				tmmsm01["FIX_SLAB_NUM"] = ord_com_num;
				tmmsm01.Update("FIX_SLAB_NUM","MAT_NO");

				if(ord_com_num == 1)
				{
					//生成一对一的目的板坯
					bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_NO"]			= tmmsm96["MAT_NO"];
					bcls_rec->Tables["MMHP0005"].Rows[0]["PONO_SLAB"]		= com_pono_no[0];
					bcls_rec->Tables["MMHP0005"].Rows[0]["FIX_SLAB_NUM"]	= ord_com_num;
					bcls_rec->Tables["MMHP0005"].Rows[0]["CUT_SEQ"]			= 1;
					bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_WT_FLAG"]		= "0";
					bcls_rec->Tables["MMHP0005"].Rows[0]["AIM_MAT_NO"]		= tmmsm96["MAT_NO"];

					doFlag = f_mmhp0005_proc(bcls_rec, bcls_ret,conn);
					if(doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
				else
				{
					//生成一对多的目的板坯
					for(int i = 0;i < ord_com_num;i++)
					{
						bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_NO"]			= tmmsm96["MAT_NO"];
						bcls_rec->Tables["MMHP0005"].Rows[0]["PONO_SLAB"]		= com_pono_no[i];
						bcls_rec->Tables["MMHP0005"].Rows[0]["FIX_SLAB_NUM"]	= ord_com_num;
						bcls_rec->Tables["MMHP0005"].Rows[0]["CUT_SEQ"]			= i + 1;
						bcls_rec->Tables["MMHP0005"].Rows[0]["MAT_WT_FLAG"]		= "0";
						doFlag = f_mmhp0005_proc(bcls_rec, bcls_ret,conn);
						if(doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
				}
			}		
		}  //无余材

		////Log::Info("", __FUNCTION__,"目的材料 tmmsm96["MAT_NO"] = [{0}]",tmmsm96["MAT_NO"].ToString());

		/* 目的材料大于1，则删除工序表中原来余材对应的工序信息 */
		tmmsm03["MAT_NO"] = tmmsm96["MAT_NO"];
		if (tmmsm03.QueryCount("MAT_NO") >= 2)
		{
			tmmsm04["AIM_MAT_NO"] = tmmsm96["MAT_NO"];
			tmmsm04.Delete("AIM_MAT_NO");
		}
		////Log::Info("", __FUNCTION__,"000 修改主档工序 tmmsm96["MAT_NO"] = [{0}]",tmmsm96["MAT_NO"].ToString());

		/* 修改主档工序 */
		tmmsm03["MAT_NO"] = tmmsm96["MAT_NO"];
		tmmsm03["ORDER_REMAIN_DIV"] = "1";
		if (tmmsm03.QueryCount("MAT_NO,ORDER_REMAIN_DIV") > 0)
		{

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " SELECT SUB_BACKLOG_CODE, "
							 "		  SUB_BACKLOG_SEQ "
							 "	 FROM TMMSM04 "
							 "  WHERE AIM_MAT_NO IN (SELECT AIM_MAT_NO "
							 "						   FROM TMMSM03 "
							 "						  WHERE MAT_NO = @tmmsm96.MAT_NO "
							 "							AND ORDER_REMAIN_DIV = '1') "
							 "    AND SUB_BACKLOG_SEQ = (SELECT NVL(MIN(SUB_BACKLOG_SEQ),0) "
							 "							   FROM TMMSM04 "
							 "							  WHERE MAT_NO = @tmmsm96.MAT_NO "
							 "								AND BACKLOG_PASS_TIME > ' ') ";
					break;
			}     
			cmd_inq.SetCommandText(sqlstr);
		////Log::Info("", __FUNCTION__,"111 修改主档工序 sqlstr = [{0}]",sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{		
				tmmsm01["SUB_BACKLOG_CODE"] = cmd_inq.GetString(1);
				tmmsm01["SUB_BACKLOG_SEQ"] = cmd_inq.GetDecimal(2);
			} 
			cmd_inq.Close();	
	
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " SELECT SUB_BACKLOG_CODE, "
							 "		  SUB_BACKLOG_SEQ "
							 "	 FROM TMMSM04 "
							 "  WHERE AIM_MAT_NO IN (SELECT AIM_MAT_NO "
							 "						   FROM TMMSM03 "
							 "						  WHERE MAT_NO = @tmmsm96.MAT_NO "
							 "							AND ORDER_REMAIN_DIV = '1') "
							 "    AND SUB_BACKLOG_SEQ = (SELECT NVL(MIN(SUB_BACKLOG_SEQ),0) "
							 "							   FROM TMMSM04 "
							 "							  WHERE MAT_NO = @tmmsm96.MAT_NO "
							 "								AND BACKLOG_PASS_TIME = ' ') ";
					break;
			}     
			cmd_inq.SetCommandText(sqlstr);
		////Log::Info("", __FUNCTION__,"222 修改主档工序 sqlstr = [{0}]",sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmmsm96.MAT_NO",tmmsm96["MAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{		
				tmmsm01["NEXT_SUB_BACKLOG_CODE"]	= cmd_inq.GetString(1);
				tmmsm01["NEXT_SUB_BACKLOG_SEQ"]	= cmd_inq.GetDecimal(2);
			} 
			cmd_inq.Close();
			
		////Log::Info("", __FUNCTION__,"333 修改主档工序 tmmsm96["MAT_NO"] = [{0}]",tmmsm96["MAT_NO"].ToString());
			/* 修改主档工序 */
			tmmsm01.Update( "SUB_BACKLOG_CODE,"
							"SUB_BACKLOG_SEQ,"
							"NEXT_SUB_BACKLOG_CODE,"
							"NEXT_SUB_BACKLOG_SEQ","MAT_NO");

		}

		/* 记录履历 */
		tmmsm96.TrimOrBlank();
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"]			= tmmsm96["EVENT_ID"];	
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"]	= tmmsm96["EVENT_LINE_TYPE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"]			= tmmsm96["SYSTEM_ID"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"]			= tmmsm96["FUNC_ID"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"]			= tmmsm96["MAT_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["ORDER_NO"]			= tpmouhp32["ORDER_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG_NO"]			= tpmouhp32["WHOLE_BACKLOG_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["NEXT_WHOLE_BACKLOG_CODE"]	= tmmsm96["NEXT_WHOLE_BACKLOG_CODE"];
		bcls_rec->Tables["MM0099"].Rows[0]["NEXT_WHOLE_BACKLOG_SEQ"]	= tmmsm96["NEXT_WHOLE_BACKLOG_SEQ"];
		bcls_rec->Tables["MM0099"].Rows[0]["PONO_SLAB"]			= tmmsm96["PONO_SLAB"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PLATE_DT_CODE"]		= tmmsm96["PLATE_DT_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["APN"]			    = tmmsm96["APN"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["STD_SG_CODE"]		= tmmsm96["STD_SG_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG"]		= tmmsm96["WHOLE_BACKLOG"];
		bcls_rec->Tables["MM0099"].Rows[0]["MSC_LINE_NO"]		= tmmsm96["MSC_LINE_NO"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG_CODE"]= tmmsm96["WHOLE_BACKLOG_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["WHOLE_BACKLOG_SEQ"]	= tmmsm96["WHOLE_BACKLOG_SEQ"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PSC"]			    = tmmsm96["PSC"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["MSC"]				= tmmsm96["MSC"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PROD_CODE_HP"]		= tmmsm96["PROD_CODE_HP"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["SG_STD"]			= tmmsm96["SG_STD"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["SG_SIGN"]			= tmmsm96["SG_SIGN"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["BD_FLAG"]			= tmmsm96["BD_FLAG"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["IF_IN_SECUT_FLAG"]	= tmmsm96["IF_IN_SECUT_FLAG"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["SURFACE_DECIDE_CODE"]	= tmmsm96["SURFACE_DECIDE_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["PCH_JUDGE_CODE"]		= tmmsm96["PCH_JUDGE_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["COMPLEX_DECIDE_CODE"]	= tmmsm96["COMPLEX_DECIDE_CODE"]; 
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_DESTION"]			= tmmsm96["MAT_DESTION"]; 
		doFlag = f_mmsm99(bcls_rec, bcls_ret,conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
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
