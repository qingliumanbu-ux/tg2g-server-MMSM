/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description:厚板板坯在制品脱合同
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 厚板板坯在制品脱合同
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
 
#if defined _LINE_HP
 
 
#endif
 

//外部函数声明
#if defined _SYS_MMS || defined _SYS_MES
#if defined _LINE_HP
BM2_FUNCTION_EXPORT
int f_qmtqhp_dele_resv_chg_new(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	//质量代预替换
//int f_pmouhp_del_pmou_slab(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn);	//厚板删四大命令，板坯脱合同时调
#endif
#endif
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_mmsm62(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_sample_lot_no("");
	CString	cs_dele_resv_code("");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	#if defined _LINE_HP
	CModel tmmsm03("TMMSM03");
	CModel tmmsm04("TMMSM04");
	#endif
	CModel tmmsm96("TMMSM96");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_qmtq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MM0099 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 添加并设置块名 */
		//调用物料跟踪
		EIClass inBlock_mm;
		inBlock_mm.Tables.SetTableName(0, "MM0099");
		#if defined _SYS_MMS || defined _SYS_MES
		#if defined _LINE_HP
		blkNum = bcls_rec->Tables.IndexOf("QMZSBlock");//调用代预替换
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("QMZSBlock");
			bcls_rec->Tables["QMZSBlock"].Columns.Add(DT_STRING, "EVENT_ID");		//事件号
			bcls_rec->Tables["QMZSBlock"].Columns.Add(DT_STRING, "EVENT_DESC");		//事件描述
			bcls_rec->Tables["QMZSBlock"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec->Tables["QMZSBlock"].Columns.Add(DT_STRING, "FUNC_ID");
			bcls_rec->Tables["QMZSBlock"].Columns.Add(DT_STRING, "FORM_CODE");
			bcls_rec->Tables["QMZSBlock"].Columns.Add(DT_STRING, "MAT_KIND");
			bcls_rec->Tables["QMZSBlock"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["QMZSBlock"].Rows.Add();
		}
		#endif
		#endif
		//blkNum = bcls_rec->Tables.IndexOf("PMOUHP");//调厚板删四大命令用
		//if (blkNum < 0)
		//{
		//	bcls_rec->Tables.Add("PMOUHP");
		//	bcls_rec->Tables["PMOUHP"].Columns.Add(DT_STRING, "PONO_SLAB");
		//	bcls_rec->Tables["PMOUHP"].Columns.Add(DT_STRING, "PLATE_NO");
		//	bcls_rec->Tables["PMOUHP"].Rows.Add();
		//}

		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables["MM0099"].Rows.get_Count(); i++)
		{
			tmmsm96.MergeFrom(bcls_rec->Tables["MM0099"].Rows[i]);
			tmmsm96.TrimOrBlank();

			/* 打印输入参数 */
			//Log::Trace("", __FUNCTION__, "传入参数,tmmsm96.MAT_NO				= [{0}];", tmmsm96["MAT_NO"].ToString());

			#if defined _LINE_HP     //厚板产线
			/* 查询厚板主档表 */
			tmmsm01["MAT_NO"] = tmmsm96["MAT_NO"];
			tmmsm01.Query("MAT_NO");
			tmmsm01.TrimOrBlank();
			//Log::Trace("", __FUNCTION__, "查询厚板主档表,tmmsm01.SUB_BACKLOG_SEQ		= [{0}];", tmmsm01["SUB_BACKLOG_SEQ"].ToDecimal());
			//Log::Trace("", __FUNCTION__, "查询厚板主档表,tmmsm01.SUB_BACKLOG_CODE		= [{0}];", tmmsm01["SUB_BACKLOG_CODE"].ToString());
			//Log::Trace("", __FUNCTION__, "查询厚板主档表,tmmsm01.NEXT_SUB_BACKLOG_SEQ	= [{0}];", tmmsm01["NEXT_SUB_BACKLOG_SEQ"].ToDecimal());
			//Log::Trace("", __FUNCTION__, "查询厚板主档表,tmmsm01.NEXT_SUB_BACKLOG_CODE	= [{0}];", tmmsm01["NEXT_SUB_BACKLOG_CODE"].ToString());


			/* 查询命令板坯号并删除命令信息 */
			switch (conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr =" SELECT PONO_SLAB "
							"	FROM TMMSM03 "
							"  WHERE MAT_NO = @tmmsm96.MAT_NO "
							"    AND ORDER_NO > ' ' ";
					break;
			}
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmmsm96.MAT_NO", tmmsm96["MAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				tmmsm03["PONO_SLAB"] = cmd_inq.GetString(1);

				/* 按命令板坯号调用质量程序 */
				#if defined _SYS_MMS || defined _SYS_MES
				switch (conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =" SELECT SAMPLE_LOT_NO, "
								"		  DELE_RESV_CODE "
								"	FROM TQMTQJ2A "
								"  WHERE PONO_SLAB = @tmmsm03.PONO_SLAB "
								"    AND DELE_RESV_CODE IN('1', '2') ";
						break;
				}
				//Log::Trace("", __FUNCTION__, "按命令板坯号调用质量程序 sqlstr = [{0}]", sqlstr);
				cmd_inq_qmtq.SetCommandText(sqlstr);
				cmd_inq_qmtq.Parameters.Clear();
				cmd_inq_qmtq.Parameters.Set("tmmsm03.PONO_SLAB", tmmsm03["PONO_SLAB"].ToString());
				cmd_inq_qmtq.ExecuteReader();
				while (cmd_inq_qmtq.Read())
				{
					bcls_rec->Tables["QMZSBlock"].Rows[0]["EVENT_ID"] = tmmsm96["EVENT_ID"];
					bcls_rec->Tables["QMZSBlock"].Rows[0]["EVENT_DESC"] = "板坯脱合同";
					bcls_rec->Tables["QMZSBlock"].Rows[0]["SYSTEM_ID"] = "MMSM";
					bcls_rec->Tables["QMZSBlock"].Rows[0]["FUNC_ID"] = "f_mmsm62";
					bcls_rec->Tables["QMZSBlock"].Rows[0]["FORM_CODE"] = " ";
					bcls_rec->Tables["QMZSBlock"].Rows[0]["MAT_KIND"] = "SM";
					bcls_rec->Tables["QMZSBlock"].Rows[0]["MAT_NO"] = tmmsm96["MAT_NO"];
					doFlag = f_qmtqhp_dele_resv_chg_new(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
				cmd_inq_qmtq.Close();
				#endif

				///* 调厚板删四大命令用 */
				//bcls_rec->Tables["PMOUHP"].Rows[0]["PONO_SLAB"] = tmmsm96["PONO_SLAB"];
				//doFlag = f_pmouhp_del_pmou_slab(bcls_rec, bcls_ret, conn);
				//if (doFlag < 0)
				//{
				//	strcat(s.msg, "调用f_pmouhp_del_pmou_slab函数出错。");
				//	throw CApplicationException(-1, s.msg, s.svc_name);
				//}
			}
			cmd_inq.Close();

			/****** 删除目的档和工序档 ******/
			/* 查询目的材料号 */
			switch (conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr =" SELECT NVL(MIN(AIM_MAT_NO),' ') "
							"	FROM TMMSM03 "
							"  WHERE MAT_NO = @tmmsm96.MAT_NO ";
					break;
			}
			//Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmmsm96.MAT_NO", tmmsm96["MAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm03["AIM_MAT_NO"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();

			if (tmmsm03["AIM_MAT_NO"].ToString().Trim() != "")
			{
				/* 删除目的档 */
				switch (conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =" DELETE FROM TMMSM03 "
								"  WHERE MAT_NO = @tmmsm96.MAT_NO "
								"    AND AIM_MAT_NO <> @tmmsm03.AIM_MAT_NO ";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmmsm96.MAT_NO", tmmsm96["MAT_NO"].ToString());
				cmd_inq.Parameters.Set("tmmsm03.AIM_MAT_NO", tmmsm03["AIM_MAT_NO"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				/* 更新目的档 */
				tmmsm03["REC_REVISOR"] = s.userid;
				tmmsm03["REC_REVISE_TIME"] = datetime;
				switch (conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =" UPDATE TMMSM03 "
								"    SET AIM_MAT_NO			= MAT_NO, "
								"		 ORDER_NO			= ' ', "
								"		 ORDER_REMAIN_DIV	= '0', "
								"		 PONO_SLAB			= ' ', "
								"		 PILE_INDEX			= ' ', "
								"		 HOT_CHARGE_FLAG	= '0', "
								"		 INFUR_SLAB_THICK	= @tmmsm01.MAT_ACT_THICK, "
								"		 INFUR_SLAB_WID		= @tmmsm01.MAT_ACT_WIDTH, "
								"		 INFUR_SLAB_LEN		= @tmmsm01.MAT_ACT_LEN, "
								"		 INFUR_SLAB_WT		= @tmmsm01.MAT_THEORY_WT, "
								"		 INFUR_SLAB_MAX_LEN	= 0, "
								"		 INFUR_SLAB_MIN_LEN	= 0, "
								"		 INFUR_SLAB_MAX_WT	= 0, "
								"		 INFUR_SLAB_MIN_WT	= 0, "
								"		 REC_REVISOR		= @tmmsm03.REC_REVISOR, "
								"		 REC_REVISE_TIME	= @tmmsm03.REC_REVISE_TIME  "
								"  WHERE MAT_NO = @tmmsm96.MAT_NO ";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmmsm01.MAT_ACT_THICK", tmmsm01["MAT_ACT_THICK"].ToDecimal());
				cmd_inq.Parameters.Set("tmmsm01.MAT_ACT_WIDTH", tmmsm01["MAT_ACT_WIDTH"].ToDecimal());
				cmd_inq.Parameters.Set("tmmsm01.MAT_ACT_LEN", tmmsm01["MAT_ACT_LEN"].ToDecimal());
				cmd_inq.Parameters.Set("tmmsm01.MAT_THEORY_WT", tmmsm01["MAT_THEORY_WT"].ToDecimal());
				cmd_inq.Parameters.Set("tmmsm03.REC_REVISOR", tmmsm03["REC_REVISOR"].ToString());
				cmd_inq.Parameters.Set("tmmsm03.REC_REVISE_TIME", tmmsm03["REC_REVISE_TIME"].ToString());
				cmd_inq.Parameters.Set("tmmsm96.MAT_NO", tmmsm96["MAT_NO"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				/* 删除工序档 */
				switch (conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =" DELETE FROM TMMSM04 "
								"  WHERE MAT_NO = @tmmsm96.MAT_NO "
								"    AND (AIM_MAT_NO <> @tmmsm03.AIM_MAT_NO  "
								"		 OR BACKLOG_PASS_TIME = ' ') ";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmmsm96.MAT_NO", tmmsm96["MAT_NO"].ToString());
				cmd_inq.Parameters.Set("tmmsm03.AIM_MAT_NO", tmmsm03["AIM_MAT_NO"].ToString());
				cmd_inq.ExecuteNonQuery();
				cmd_inq.Close();

				/* 更新工序档 */
				tmmsm04["AIM_MAT_NO"]		= tmmsm96["MAT_NO"];
				tmmsm04["MAT_NO"]			= tmmsm96["MAT_NO"];
				tmmsm04["REC_REVISOR"]		= s.userid;
				tmmsm04["REC_REVISE_TIME"] = datetime;
				tmmsm04.Update( "AIM_MAT_NO,"
								"REC_REVISE_TIME,"
								"REC_REVISOR",
								"MAT_NO");

			}
			#endif

			/* 设置物料跟踪的参数 */
			tmmsm96.MergeTo(inBlock_mm.Tables["MM0099"], false);
		}

		/* 调用物料跟踪 */
		doFlag = f_mmsm99(&inBlock_mm, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	cmd_inq_qmtq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
