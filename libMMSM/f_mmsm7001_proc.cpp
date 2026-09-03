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
  
//#include "tmmsm71.h"  
//#include "tpssm05.h"  
//外部函数声明

int f_pssm55_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm7002_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_matno_catch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#if defined(_WMS_DEPENDENT_SM)   //非独立仓库
int f_wm00_queue(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
#endif


BM2_FUNCTION_EXPORT
 int f_mmsm7001_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义
	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	
	/* 业务变量 */
	CString	datetime("");    
	CString v_code = "";
	/* 实体类定义 */
	CModel tmmsm70("TMMSM70");
	//CTMMSM71 tmmsm71(conn);
	//CTPSSM05 tpssm05(conn);
	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";
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
		blkNum = bcls_rec->Tables.IndexOf("MMSM70P");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMSM70P 不存在。");
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
		/* 调用仓库入库队列 */
		blkNum = bcls_rec->AtBlkName("WM00QUE");
		if (blkNum <= 0)
		{
			blkNum = bcls_rec->AddBlock();
			bcls_rec->SetBlkName(blkNum, "WM00QUE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_EXEC_SEQ_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "TRANS_TOOL");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "UNIT_CODE");
			/*bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "NEXT_UNIT_CODE");*/
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_DESTION");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");
			bcls_rec->Tables["WM00QUE"].Rows.Add(); // 创建一行
		}
		#endif
	

		/* 获取输入参数 */
		//获取计划多入口材料号
		if (bcls_rec->Tables["MMSM70P"].Rows.get_Count() > 0)
		{
			CString in_mat_col = "";
			CDecimal r_num = 0;
			bcls_rec->Tables["PSSM"].Rows.Clear();
			//Log::Trace("", __FUNCTION__, "传入计划接口记录= [{0}]", bcls_rec->Tables["MMSM70P"].Rows.get_Count());

			for (int p_count = 0; p_count < bcls_rec->Tables["MMSM70P"].Rows.get_Count(); p_count++)
			{
				r_num = p_count + 1;
				in_mat_col = "MAT_NO_0" + r_num.ToString();
				if (bcls_rec->Tables["MMSM70"].Columns.Contains(in_mat_col))
				{
					//Log::Trace("", __FUNCTION__, "if1");
					bcls_rec->Tables["MMSM70"].Rows[0][in_mat_col] = bcls_rec->Tables["MMSM70P"].Rows[p_count]["IN_MAT_NO"].ToString();
				}
				else if (bcls_rec->Tables["MMSM70"].Columns.Contains("IN_MAT_NO"))
				{
					//Log::Trace("", __FUNCTION__, "if2");
					bcls_rec->Tables["MMSM70"].Rows[0]["IN_MAT_NO"] = bcls_rec->Tables["MMSM70P"].Rows[p_count]["IN_MAT_NO"].ToString();
				}

				//PS interface
				bcls_rec->Tables["PSSM"].Rows.Add();
				bcls_rec->Tables["PSSM"].Rows[p_count]["FACTORY_DIV"] = bcls_rec->Tables["MMSM70P"].Rows[p_count]["FACTORY_DIV"].ToString();			//生产结束
				bcls_rec->Tables["PSSM"].Rows[p_count]["PLAN_BACKLOG_CODE"] = bcls_rec->Tables["MMSM70P"].Rows[p_count]["PLAN_BACKLOG_CODE"].ToString();
				bcls_rec->Tables["PSSM"].Rows[p_count]["MAT_NO"] = bcls_rec->Tables["MMSM70P"].Rows[p_count]["IN_MAT_NO"].ToString();
				bcls_rec->Tables["PSSM"].Rows[p_count]["PLAN_NO"] = bcls_rec->Tables["MMSM70P"].Rows[p_count]["PLAN_NO"].ToString();
				//Log::Trace("", __FUNCTION__, "传入计划接口MAT_NO= [{0}]", bcls_rec->Tables["MMSM70P"].Rows[p_count]["IN_MAT_NO"].ToString());


			}
		}
		//tmmsm70.MergeFrom(bcls_rec->Tables["MMSM70P"].Rows[0]);
		tmmsm70.MergeFrom(bcls_rec->Tables["MMSM70"].Rows[0]);
		tmmsm70.TrimOrBlank();
		tmmsm70["MAT_SHAPE_FLAG"] = "H";
		//tmmsm70["MAT_THEORY_WT"] = bcls_rec->Tables["MMSM70P"].Rows[0]["MAT_ACT_WT"].ToDecimal();
		tmmsm70["PLAN_NO"] = bcls_rec->Tables["MMSM70P"].Rows[0]["PLAN_NO"].ToString();
		tmmsm70["PLAN_BACKLOG_CODE"] = bcls_rec->Tables["MMSM70P"].Rows[0]["PLAN_BACKLOG_CODE"].ToString();
		//Log::Trace("", __FUNCTION__, "tmmsm70["PLAN_BACKLOG_CODE"] = [{0}]", tmmsm70["PLAN_BACKLOG_CODE"].ToString());
		tmmsm70["FACTORY_DIV"] = bcls_rec->Tables["MMSM70P"].Rows[0]["FACTORY_DIV"].ToString();
		tmmsm70["PROD_TIME"] = datetime;
		tmmsm70["ST_NO"] = bcls_rec->Tables["MMSM70P"].Rows[0]["ST_NO"].ToString();
		tmmsm70["SG_SIGN"] = bcls_rec->Tables["MMSM70P"].Rows[0]["SG_SIGN"].ToString();
		tmmsm70["IN_MAT_NO"] = bcls_rec->Tables["MMSM70P"].Rows[0]["IN_MAT_NO"].ToString();
		if (tmmsm70["PROD_TIME"].ToString().Trim() == "")
		{
			tmmsm70["PROD_TIME"] = datetime;
		}
		/*f_epep_get_shift_group("SM", tmmsm70["PROD_TIME"].ToString(), tmmsm70["PROD_SHIFT_NO"].ToString(), tmmsm70["PROD_SHIFT_GROUP"].ToString(), conn);*/
		f_epep_get_shift_group("SM", tmmsm70["PROD_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
		tmmsm70["PROD_TIME"] = PROD_SHIFT_NO;
		tmmsm70["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
		//Log::Trace("", __FUNCTION__, "tmmsm70["PROD_TIME"] ={0},tmmsm70["PROD_SHIFT_NO"] ={1},tmmsm70["PROD_SHIFT_GROUP"] ={2}", tmmsm70["PROD_TIME"].ToString(), tmmsm70["PROD_SHIFT_NO"].ToString(), tmmsm70["PROD_SHIFT_GROUP"].ToString());

		/* 打印输入参数 */
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm70.PRACT_COLL_MODE	= [{0}]", tmmsm70["PRACT_COLL_MODE"].ToString());//实绩收集方式
		//Log::Trace("", __FUNCTION__, "传入参数,tmmsm70["MAT_NO"] = [{0}]", tmmsm70["MAT_NO"].ToString());

		/* 检查输入参数合法性 */
		if (tmmsm70["PRACT_COLL_MODE"].ToString().Trim() == "")
		{
			strcpy(s.msg, "实绩收集方式不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (tmmsm70["MAT_NO"].ToString().Trim() == "")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
				sqlstr = "select CODE_DESC_1_CONTENT from tep0002 where 1=1 and code_class = 'M00M' "
				" AND   CODE_DESC_2_CONTENT =  @tmmsm70.MAT_SHAPE_FLAG "
				" AND CODE_DESC_4_CONTENT = '1' ";

			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "select CODE_DESC_1_CONTENT from tep0002 where 1=1 and code_class = 'M00M' "
					" AND   CODE_DESC_2_CONTENT =  @tmmsm70.MAT_SHAPE_FLAG "
					" AND CODE_DESC_4_CONTENT = '1' ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("tmmsm70.MAT_SHAPE_FLAG", tmmsm70["MAT_SHAPE_FLAG"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				v_code = cmd_inq.GetString(1);
				doFlag = f_mmsm_matno_catch(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				tmmsm70["MAT_NO"] = bcls_ret->Tables[0].Rows[0]["MAT_NO"];
			}
			cmd_inq.Close();
			
		}

		if (tmmsm70["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料号不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//if (tmmsm70["IN_MAT_NO"].ToString().Trim() == "")
		//{
		//	strcpy(s.msg, "入口材料号不能为空。");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}
		if (tmmsm70["PLAN_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "计划号不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm70["PLAN_BACKLOG_CODE"].ToString().Trim() == "")
		{
			strcpy(s.msg, "计划工序代码不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm70["STATION_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "计划工序代码不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		
		/* 新增实绩表 */
		tmmsm70["UNIT_CODE"] = tmmsm70["PLAN_BACKLOG_CODE"].ToString().Trim() + tmmsm70["STATION_NO"].ToString().Trim();
		tmmsm70["REC_CREATOR"] = s.userid;
		tmmsm70["REC_CREATE_TIME"] = datetime;
		tmmsm70.TrimOrBlank();
		tmmsm70.Insert();


		//回写化渣实绩表TMMSM71
		//TMMSM71


		/* 新增热轧材料主档 */
		bcls_rec->Tables["MMSM70"].Rows.Clear();
		tmmsm70.MergeTo(bcls_rec->Tables["MMSM70"], false);
		doFlag = f_mmsm7002_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

	
		/*if (tpssm05.PLAN_STATUS.Trim() != "60")
		{*/
			/* 调用计划跟踪 */
			//bcls_rec->Tables["PSSM"].Rows[0]["FACTORY_DIV"] = tmmsm70["FACTORY_DIV"];			//生产结束
			//bcls_rec->Tables["PSSM"].Rows[0]["PLAN_BACKLOG_CODE"] = tmmsm70["PLAN_BACKLOG_CODE"];
			//bcls_rec->Tables["PSSM"].Rows[0]["MAT_NO"] = tmmsm70["IN_MAT_NO"];
			//bcls_rec->Tables["PSSM"].Rows[0]["PLAN_NO"] = tmmsm70["PLAN_NO"];
			doFlag = f_pssm55_trace(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		//}

		
#if defined(_WMS_DEPENDENT_SM)   //非独立仓库 
			//YM interface

			//Log::Trace("", __FUNCTION__, "调用仓库队列接口tmmsm70.NEXT_UNIT_CODE= [{0}]", tmmsm70.NEXT_UNIT_CODE);
			bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NO"] = tmmsm70["MAT_NO"];
			bcls_rec->Tables["WM00QUE"].Rows[0]["PLAN_NO"] = " ";
			bcls_rec->Tables["WM00QUE"].Rows[0]["PLAN_EXEC_SEQ_NO"] = 0;
			bcls_rec->Tables["WM00QUE"].Rows[0]["TRANS_TOOL"] = " ";	//运输工具
			bcls_rec->Tables["WM00QUE"].Rows[0]["UNIT_CODE"] = tmmsm70["UNIT_CODE"];
			//bcls_rec->Tables["WM00QUE"].Rows[0]["NEXT_UNIT_CODE"] = tmmsm70.NEXT_UNIT_CODE;
			bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_DESTION"] = " ";
			bcls_rec->Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "I";
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1B"; //1B-机组产出入库
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER_DIV"] = " ";
			//Log::Trace("", __FUNCTION__, "调用仓库队列接口tmmsm70["UNIT_CODE"] = [{0}]", tmmsm70["UNIT_CODE"].ToString());
			//doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn); //2022-08-12 去头文件时编译报错 暂时注销
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
#endif

		
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
