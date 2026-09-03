/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2016-03-14
Description: 炼钢电渣炉实绩处理
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢电渣炉实绩处理
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件    
  
  
  
#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
#include "tmm0005.h"
#include "tpmof01.h" 
  
#endif

//#include "tmmsm71.h"  
//#include "tpssm05.h"  
//外部函数声明

//int f_pssm55_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_mmsm7002_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_mmsm_matno_catch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#if defined(_WMS_DEPENDENT_SM)   //非独立仓库
//int f_wmxx_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
//int f_wmxx_stock_in(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
#endif

#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
BM2_FUNCTION_IMPORT
int f_pmof99_v3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
BM2_FUNCTION_EXPORT
 int f_mmsm7003_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义
	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	CString change_flag = "";
	/* 业务变量 */
	CString	datetime("");    
	CString v_code = "";
	CString v_func_id = "";
	CString v_fieldstr = "";
	/* 实体类定义 */
	CModel tmmsm70("TMMSM70");
	CModel oldtmmsm70("TMMSM70");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
	
	CModel tpmof03("TPMOF03");
#endif
	//CTMMSM71 tmmsm71(conn);
	//CTPSSM05 tpssm05(conn);
	/* 数据库SQL操作字符串 */
	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MMSM70");
		if(blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMSM70 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		blkNum = bcls_rec->Tables.IndexOf("MM0099");	
		if(blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099"); 
		}
		blkNum = bcls_rec->Tables.IndexOf("PSSM");	
		if(blkNum < 0)
		{
			bcls_rec->Tables.Add("PSSM"); 
			bcls_rec->Tables["PSSM"].Columns.Add(DT_STRING,"FACTORY_DIV");
			bcls_rec->Tables["PSSM"].Columns.Add(DT_STRING,"PLAN_BACKLOG_CODE");
			bcls_rec->Tables["PSSM"].Columns.Add(DT_STRING,"PLAN_NO");
			bcls_rec->Tables["PSSM"].Columns.Add(DT_STRING,"MAT_NO");
			bcls_rec->Tables["PSSM"].Rows.Add();
		}
		
#if defined(_WMS_DEPENDENT_SM)   //非独立仓库
		blkNum = bcls_rec->Tables.IndexOf("WM_STOCK");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("WM_STOCK");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_KIND");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_LINE_TYPE");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_NO");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "ROWNO");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "COLUMN_NO");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_DECIMAL, "LAYERNO");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_POSITION");
			bcls_rec->Tables["WM_STOCK"].Rows.Add();
		}
#endif
#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
		blkNum = bcls_rec->Tables.IndexOf("PMOF99");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("PMOF99");
		}
#endif

		/* 获取输入参数 */
		
		//tmmsm70.MergeFrom(bcls_rec->Tables["MMSM70P"].Rows[0]);
		tmmsm70.MergeFrom(bcls_rec->Tables["MMSM70"].Rows[0]);
		tmmsm70.TrimOrBlank();
		oldtmmsm70["MAT_NO"] = tmmsm70["MAT_NO"];
		/* 打印输入参数 */
		//Log::Trace("", __FUNCTION__, "传入参数,tmmsm70.MAT_NO					= [{0}]", tmmsm70["MAT_NO"].ToString());

		if (bcls_rec->Tables["MMSM70"].Columns.Contains("FUNC_ID"))
		{
			v_func_id = bcls_rec->Tables["MMSM70"].Rows[0]["FUNC_ID"].ToString();
			//Log::Trace("", __FUNCTION__, "传入参数,v_func_id1 = [{0}]", v_func_id);
		}
		

		/* 检查输入参数合法性 */
		if (tmmsm70["PRACT_COLL_MODE"].ToString().Trim() == "")
		{
			strcpy(s.msg, "实绩收集方式不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm70["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料号不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (oldtmmsm70.QueryCount("MAT_NO") <= 0)
		{
			strcpy(s.msg, "电渣锭实绩不存在!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		
		if (v_func_id.Trim() != "")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr = " select ITEM_ENAME from ted54 where func_id = @v_func_id "
					"AND FORM_EDIT_FLAG = '1' AND ITEM_HIDE_FLAG = '0'"
					" ORDER BY SEQ_NO"
					;

				break;
			}

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_func_id", v_func_id);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				v_fieldstr += cmd_inq.GetString(1);
				v_fieldstr += ",";
			}
			cmd_inq.Close();
			
			if (v_fieldstr.Find("REC_REVISOR") < 0)
			{
				v_fieldstr += "REC_REVISOR,";
			}
			if (v_fieldstr.Find("REC_REVISE_TIME") < 0)
			{
				v_fieldstr += "REC_REVISE_TIME,";
			}
			v_fieldstr = v_fieldstr.SubstringNE(0, v_fieldstr.GetLength() - 1);
			//Log::Trace("", __FUNCTION__, "获取配置字段v_field= [{0}]", v_fieldstr);
		}
		else
		{
			//默认字段 从传入块获取
			CString sColName = "";
			v_fieldstr = "REC_REVISOR,REC_REVISE_TIME";
			for (int n; n < bcls_rec->Tables["MMSM70"].Columns.get_Count(); n++)
			{
				sColName = bcls_rec->Tables["MMSM70"].Columns[n].get_ColumnName().ToUpper();
				//Log::Trace("", __FUNCTION__, "sColName= [{0}]", sColName);
				if (tmmsm70.GetFields().Contains(sColName)
					&& sColName!="REC_REVISOR"
					&& sColName != "REC_REVISE_TIME")
				{
					v_fieldstr = v_fieldstr + "," + sColName;
				}
			}
			
			//Log::Trace("", __FUNCTION__, "默认可修改字段v_fieldstr= [{0}]", v_fieldstr);
		}
		/* 新增实绩表 */
		
		tmmsm70["REC_REVISOR"] = s.userid;
		tmmsm70["REC_REVISE_TIME"] = datetime;
		tmmsm70.TrimOrBlank();
		tmmsm70.Update(v_fieldstr,"MAT_NO");
		tmmsm70.Query("MAT_NO");
		tmmsm01["MAT_NO"] = tmmsm70["MAT_NO"];
		if (tmmsm01.Query("MAT_NO") == false)
		{
			change_flag = "0";
			//strcpy(s.msg, "材料号不能为空。");
			//throw CApplicationException(-1, s.msg, s.svc_name);
		}
		else
		{
			if (tmmsm01["MAT_WT"].ToDecimal() != tmmsm70["MAT_THEORY_WT"].ToDecimal())
			{
				change_flag = "1";
			}
			else if (tmmsm01["MAT_ACT_LEN"].ToDecimal() != tmmsm70["MAT_ACT_LEN"].ToDecimal()
				|| tmmsm01["MAT_ACT_THICK"].ToDecimal() != tmmsm70["MAT_ACT_THICK"].ToDecimal()
				|| tmmsm01["MAT_ACT_WIDTH"].ToDecimal() != tmmsm70["MAT_ACT_WIDTH"].ToDecimal())
			{
				change_flag = "2";
			}
			else
			{
				change_flag = "0";
			}
		}
		
		if (change_flag == "1" || change_flag == "2")
		{
#if defined(_WMS_DEPENDENT_SM)   //非独立仓库 

			bcls_rec->Tables["WM_STOCK"].Rows.Clear();
			bcls_rec->Tables["WM_STOCK"].Rows.Add(); // 创建一行
			bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_NO"] = tmmsm70["MAT_NO"];
			bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_KIND"] = "SM";
			bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_LINE_TYPE"] = "SM";
			if (change_flag == "1")
			{
				bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "2J";
			}
			else
			{
				bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "2I";
			}
			bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = tmmsm01["STOCK_NO"];
			bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = " ";
			bcls_rec->Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
			bcls_rec->Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
			bcls_rec->Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
			bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";
			//doFlag = f_wmxx_stock_out(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


#endif
			blkNum = bcls_rec->Tables.IndexOf("MM0099");				//调用物料跟踪传入数据块
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MM0099");
			}

			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "MM10";
			tmmsm96["EVENT_LINE_TYPE"] = "SM";
			tmmsm96["SYSTEM_ID"] = "MMSM";
			tmmsm96["FUNC_ID"] = "f_mmsm7003_proc";
			tmmsm96["EVENT_DESC"] = "电渣锭产出修正";

			tmmsm96["MAT_ACT_THICK"] = tmmsm70["MAT_ACT_THICK"];
			tmmsm96["MAT_ACT_WIDTH"] = tmmsm70["MAT_ACT_WIDTH"];
			tmmsm96["MAT_ACT_LEN"] = tmmsm70["MAT_ACT_LEN"];
			tmmsm96["MAT_THICK"] = tmmsm70["MAT_ACT_THICK"];
			tmmsm96["MAT_WIDTH"] = tmmsm70["MAT_ACT_WIDTH"];
			tmmsm96["MAT_LEN"] = tmmsm70["MAT_ACT_LEN"];

			tmmsm96["MAT_ACT_WT"] = tmmsm70["MAT_THEORY_WT"];
			tmmsm96["MAT_THEORY_WT"] = tmmsm70["MAT_THEORY_WT"];
			tmmsm96["MAT_WT"] = tmmsm70["MAT_THEORY_WT"];
			bcls_rec->Tables["MM0099"].Clear();
			tmmsm96.MergeTo(bcls_rec->Tables["MM0099"],false);
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

#if defined(_WMS_DEPENDENT_SM)   //非独立仓库 
			if (change_flag == "1" || change_flag == "2")
			{
				bcls_rec->Tables["WM_STOCK"].Rows.Clear();
				bcls_rec->Tables["WM_STOCK"].Rows.Add(); // 创建一行
				bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_NO"] = tmmsm70["MAT_NO"];
				bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_KIND"] = "SM";
				bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_LINE_TYPE"] = "SM";
				if (change_flag == "1")
				{
					bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "1J";
				}
				else
				{
					bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "1I";
				}
				bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = tmmsm01["STOCK_NO"];
				bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = " ";
				bcls_rec->Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
				bcls_rec->Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
				bcls_rec->Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
				bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";
				//doFlag = f_wmxx_stock_in(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}

#endif





#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
			/* 抛合同跟踪 */
			if (tmmsm01["ORDER_NO"].ToString().Trim() > ""&&change_flag == "1")
			{
				tpmof03["ORDER_NO"] = tmmsm01["ORDER_NO"];
				tpmof03["SYSTEM_ID"] = "MMSM";
				tpmof03["FUNC_ID"] = "f_mmsm7003_proc";
				tpmof03["WHOLE_BACKLOG"] = tmmsm01["WHOLE_BACKLOG"];
				tpmof03["WHOLE_BACKLOG_NO"] = tmmsm01["WHOLE_BACKLOG_NO"];
				tpmof03["WHOLE_BACKLOG_SEQ"] = tmmsm01["WHOLE_BACKLOG_SEQ"];
				tpmof03["WHOLE_BACKLOG_CODE"] = tmmsm01["WHOLE_BACKLOG_CODE"];
				tpmof03["MAT_NO"] = tmmsm01["MAT_NO"];
				tpmof03["MAT_STATUS"] = tmmsm01["MAT_STATUS"];

				tpmof03["EVENT_ID"] = "54";	//重量修正
				tpmof03["WT"] = tmmsm96["MAT_WT"];
				tpmof03["NUM"] = 1;
				tpmof03["PREV_MAT_NO"] = tmmsm01["MAT_NO"];
				tpmof03["PREV_MAT_STATUS"] = tmmsm01["MAT_STATUS"];

				tpmof03["PREV_WT"] = tmmsm01["MAT_WT"];;
				tpmof03["PREV_NUM"] = 1;
				tpmof03["PREV_MAT_STATUS"] = tmmsm01["MAT_STATUS"];

				//Log::Trace("", __FUNCTION__, "tpmof03.ORDER_NO			= [{0}]", tpmof03["ORDER_NO"].ToString());
				//Log::Trace("", __FUNCTION__, "tpmof03.MAT_NO	= [{0}]", tpmof03["MAT_NO"].ToString());
				//Log::Trace("", __FUNCTION__, "tpmof03.PREV_MAT_NO	= [{0}]", tpmof03["PREV_MAT_NO"].ToString());
				//Log::Trace("", __FUNCTION__, "tpmof03.PREV_WT	= [{0}]", tpmof03["PREV_WT"].ToDecimal());
				//Log::Trace("", __FUNCTION__, "tpmof03.NUM			= [{0}]", tpmof03["NUM"].ToDecimal());

				tpmof03.MergeTo(bcls_rec->Tables["PMOF99"], false);
				doFlag = f_pmof99_v3(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}


#endif
		}

		

		


		
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错,sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台,与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1,事务将被回滚
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
	//返回-1时事务将回滚,返回为0是事务将提交
	return doFlag;
}
