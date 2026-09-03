/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    向萍
Version:    1.0
Date:       2016-09-24
Description: 炼钢物料并批
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 方坯物料信息并批
/// <para>
/// * 调用物料并批函数
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

#if defined(_WMS_DEPENDENT_SM)   //非独立仓库
//int f_wmsmsm_stock_out(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn);	//仓库出库函数  

//int f_wm_stock(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn);
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn);
#endif

#if defined _SYS_PES  //PES
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

// service入口
BM2F_ENTERACE(mmsm01a1f7_pro)

int f_mmsm01a1f7_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */ 
	CString	datetime("");    
	CString	cs_main_mat_flag(""); 
	int j = 0;

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm01_main("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
		}

#if defined(_WMS_DEPENDENT_SM)   //非独立仓库
		blkNum = bcls_rec->Tables.IndexOf("WM00QUE");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("WM00QUE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");		//库操作指示
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV"); //业务类型细分 （见代码定义 Y028）
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_KIND");				//物料类型
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");				//物料号
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_DECIMAL, "MAT_NUM");				//入库数量：按件->1;按批->是多少给多少
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_NO");				//计划号
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_DECIMAL, "PLAN_EXEC_SEQ_NO");	//计划执行顺序号
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_NO");				//当前库区号
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "TRANS_TOOL");			//运输工具（见代码 WM11）
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "UNIT_CODE");			//当前机组号
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_DESTION");			//去向
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");			//操作标记："I":新增;"D": 删除
			bcls_rec->Tables["WM00QUE"].Rows.Add();
		}
		blkNum = bcls_rec->Tables.IndexOf("WM00_CONFM");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("WM_STOCK");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_NO");
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

#if defined _SYS_PES 
		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSMSND");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "REC_CREATOR");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "REC_CREATE_TIME");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "AIM_MAT_NO");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_NO");
		}
#endif

		/* 获取输入参数 */
		/* 获取主材料 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01_main["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();	//主材料号
			cs_main_mat_flag = bcls_rec->Tables[0].Rows[i]["MAIN_MAT_FLAG"].ToString().Trim();

			//Log::Trace("", __FUNCTION__, "tmmsm01_main.MAT_NO	= [{0}]", (const char*)tmmsm01_main["MAT_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "cs_main_mat_flag		= [{0}]", (const char*)cs_main_mat_flag);

			/* 判断主材料 */
			if (cs_main_mat_flag.Trim() == "1")
			{
				tmmsm01_main.Query("MAT_NO");
				tmmsm01_main.TrimOrBlank();

				if (tmmsm01_main["MAT_SHAPE_FLAG"].ToString().Trim() == "1")
				{
					sprintf(s.msg, "请选择形态符合要求的材料进行并批!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01_main["MAT_STATUS"].ToString().Trim() == "24")
				{
					sprintf(s.msg, "材料状态在制品编入计划的不能进行并批!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01_main["MAT_NUM"].ToDecimal() <= 0)
				{
					sprintf(s.msg, "材料数量为0，不允许并批!");
					throw CApplicationException(-1, s.msg, log.Location);
				}

#if defined _SYS_PES
				//Log::Trace("", __FUNCTION__, "并批电文发送 tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

				/* 并批电文发送 */
				bcls_rec->Tables["MMSMSND"].Rows.Add();
				bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec->Tables["MMSMSND"].Rows[0]["MAIN_MAT_FLAG"] = cs_main_mat_flag;
#endif
				/* 跳出循环 */
				break;
			}
		}

		/* 获取被并批材料号 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			/* 获取被并批材料 */
			tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();	//被并批材料号
			cs_main_mat_flag = bcls_rec->Tables[0].Rows[i]["MAIN_MAT_FLAG"].ToString().Trim();

			//Log::Trace("", __FUNCTION__, "tmmsm01.MAT_NO	= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "cs_main_mat_flag	= [{0}]", (const char*)cs_main_mat_flag);

			/* 判断被并批材料 */
			if (cs_main_mat_flag.Trim() == "0")
			{
				tmmsm01.Query("MAT_NO");
				tmmsm01.TrimOrBlank();

				/* 校验能否并批 */
				if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "1")
				{
					sprintf(s.msg, "请选择形态符合要求的材料进行并批!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["MAT_STATUS"].ToString().Trim() == "24")
				{
					sprintf(s.msg, "材料状态在制品编入计划的不能进行并批!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["MAT_NUM"].ToDecimal() <= 0)
				{
					sprintf(s.msg, "材料数量为0，不允许并批!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["IN_FLAG"].ToString().Trim() != tmmsm01_main["IN_FLAG"].ToString().Trim())
				{
					sprintf(s.msg, "入库标记不同,不能并批。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["SURFACE_DECIDE_CODE"].ToString().Trim() != tmmsm01_main["SURFACE_DECIDE_CODE"].ToString().Trim())
				{
					sprintf(s.msg, "表面判定代码不同,不能并批。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != tmmsm01_main["COMPLEX_DECIDE_CODE"].ToString().Trim())
				{
					sprintf(s.msg, "综合判定代码不同,不能并批。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["PONO"].ToString().Trim() != tmmsm01_main["PONO"].ToString().Trim())
				{
					sprintf(s.msg, "制造命令号不同,不能并批。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["ROLL_PLAN_NO"].ToString().Trim() != tmmsm01_main["ROLL_PLAN_NO"].ToString().Trim())
				{
					sprintf(s.msg, "轧号不同,不能并批。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["WHOLE_BACKLOG_CODE"].ToString().Trim() != tmmsm01_main["WHOLE_BACKLOG_CODE"].ToString().Trim())
				{
					sprintf(s.msg, "工序不同,不能并批。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["MAT_ACT_THICK"].ToDecimal() != tmmsm01_main["MAT_ACT_THICK"].ToDecimal())
				{
					sprintf(s.msg, "厚度不同,不能并批。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["MAT_ACT_WIDTH"].ToDecimal() != tmmsm01_main["MAT_ACT_WIDTH"].ToDecimal())
				{
					sprintf(s.msg, "宽度不同,不能并批。");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/* 设置母材料的支数和重量 */
				tmmsm01_main["MAT_NUM"] = tmmsm01_main["MAT_NUM"].ToDecimal() + tmmsm01["MAT_NUM"].ToDecimal();
				tmmsm01_main["MAT_WT"] = tmmsm01_main["MAT_WT"].ToDecimal() + tmmsm01["MAT_WT"].ToDecimal();
				tmmsm01_main["MAT_ACT_WT"] = tmmsm01_main["MAT_ACT_WT"].ToDecimal() + tmmsm01["MAT_ACT_WT"].ToDecimal();
				tmmsm01_main["MAT_THEORY_WT"] = tmmsm01_main["MAT_THEORY_WT"].ToDecimal() + tmmsm01["MAT_THEORY_WT"].ToDecimal();

				/* 修改被并材料号的MAT_NO_OLD为主材料号 */
				tmmsm01["MAT_NO_OLD"] = tmmsm01_main["MAT_NO"];
				tmmsm01["REC_REVISOR"] = s.userid;
				tmmsm01["REC_REVISE_TIME"] = datetime;
				tmmsm01.Update(	"MAT_NO_OLD,"
								"REC_REVISOR,"
								"REC_REVISE_TIME",
								"MAT_NO");

				//Log::Trace("", __FUNCTION__, "设置物料跟踪参数 tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

#if defined(_WMS_DEPENDENT_SM)   //非独立仓库
				/* 材料在库，则被并掉的材料需要出库 */
				if (tmmsm01["IN_FLAG"].ToString().Trim() == "1")
				{
					//Log::Trace("", __FUNCTION__, "材料自动出库 tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());
					//材料自动出库
					bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "2L";

				/*	bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = tmmsm01["STOCK_NO"];
					bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = " ";
					bcls_rec->Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
					bcls_rec->Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
					bcls_rec->Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
					bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";*/

					//doFlag = f_wmsmsm_stock_out(bcls_rec, bcls_ret, conn);

					//doFlag = f_wm_stock(bcls_rec, bcls_ret, conn);

					if (doFlag < 0)
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
				else
				{
					//判断被并材料是否在入库队列，如果存在，需要删除
					bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = " ";			//删除不需要库操作指示
					bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER_DIV"] = "";
					bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_KIND"] = "SM";
					bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];			// 材料号
					bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NUM"] = tmmsm01["MAT_NUM"];		// 支数
					bcls_rec->Tables["WM00QUE"].Rows[0]["PLAN_NO"] = " ";					// 计划号
					bcls_rec->Tables["WM00QUE"].Rows[0]["PLAN_EXEC_SEQ_NO"] = 0;			// 计划顺序号
					bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_NO"] = tmmsm01["STOCK_NO"];		// 当前库区号
					bcls_rec->Tables["WM00QUE"].Rows[0]["TRANS_TOOL"] = " ";				// 运输工具
					bcls_rec->Tables["WM00QUE"].Rows[0]["UNIT_CODE"] = " ";					// 当前机组号
					bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_DESTION"] = " ";				// 去向
					bcls_rec->Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "D";					// 操作标记："I":新增;"D": 删除
					//doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);//2022-08-12 去头文件时编译报错 暂时注销
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
#endif

#if defined _SYS_PES
				//Log::Trace("", __FUNCTION__, "并批电文发送 tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

				/* 并批电文发送 */
				j = j + 1;
				bcls_rec->Tables["MMSMSND"].Rows.Add();
				bcls_rec->Tables["MMSMSND"].Rows[j]["MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec->Tables["MMSMSND"].Rows[j]["MAIN_MAT_FLAG"] = cs_main_mat_flag;
#endif

				//Log::Trace("", __FUNCTION__, "调用物料跟踪-被并材料删除 tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

				/* 设置物料跟踪参数-被并材料删除 */
				bcls_rec->Tables["MM0099"].Rows.Clear();
				bcls_rec->Tables["MM0099"].Rows.Add();
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM32";	//材料被并批删除
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm01a1f7_pro";
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
				/* 调用物料跟踪-被并材料删除 */
				doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}

		//Log::Trace("", __FUNCTION__, "修改母材料支数等值 tmmsm01_main.MAT_NO		= [{0}]", (const char*)tmmsm01_main["MAT_NO"].ToString());
		/* 修改母材料支数等值 */
		tmmsm01_main["REC_REVISOR"] = s.userid;
		tmmsm01_main["REC_REVISE_TIME"] = datetime;
		sqlstr = "tmmsm01_main.Update()";
		tmmsm01_main.Update("MAT_NUM,"
							"MAT_WT,"
							"MAT_ACT_WT,"
							"MAT_THEORY_WT,"
							"REC_REVISOR,"
							"REC_REVISE_TIME",
							"MAT_NO");

		//Log::Trace("", __FUNCTION__, "主材料抛物料跟踪履历 tmmsm01_main.MAT_NO		= [{0}]", (const char*)tmmsm01_main["MAT_NO"].ToString());
		/* 主材料抛物料跟踪履历 */
		bcls_rec->Tables["MM0099"].Rows.Clear();
		bcls_rec->Tables["MM0099"].Rows.Add();
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM31";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm01a1f7_pro";
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01_main["MAT_NO"];
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

#if defined _SYS_PES     //分层，并在PES
		/* 向MMS发送并批电文 */
		bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM3H";
		bcls_rec->Tables["MMSMSND"].Rows[0]["EVENT_ID"] = "MM29";
		bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATOR"] = tmmsm01_main["REC_CREATOR"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATE_TIME"] = tmmsm01_main["REC_ERASE_TIME"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_MAT_NO"] = tmmsm01_main["MAT_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_HEAT_NO"] = tmmsm01_main["HEAT_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_PONO"] = tmmsm01_main["PONO"];
		doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(doFlag, s.msg, log.Location);
		}
#endif
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch(CApplicationException& ex)
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

