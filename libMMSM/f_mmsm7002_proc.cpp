/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2016-03-14
Description: 电渣锭实绩产出新增炼钢材料主档&入口材料归档
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 电渣锭实绩产出新增炼钢材料主档&入口材料归档
/// <para>
/// * 校验数据
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

 
//#include "tpssm05.h"  
  
#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES

 
  
#endif

//外部函数声明

BM2_FUNCTION_IMPORT
 int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn); 
 
#if defined(_WMS_DEPENDENT_SM)   //非独立仓库
BM2_FUNCTION_IMPORT
int f_wmxx_stock_out(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection* conn);	//仓库出库函数  
#endif
#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
BM2_FUNCTION_IMPORT
 int f_mm000501_proc(EIClass *bcls_rec,EIClass *bcls_ret,CDbConnection * conn);
BM2_FUNCTION_IMPORT
 int f_pmof99_v3(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
#endif


BM2_FUNCTION_EXPORT
 int f_mmsm7002_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");     
	CDecimal v_out_wt_act = 0;
	CDecimal v_out_wt_thoery = 0;
	CDecimal v_out_wt = 0;
	/* 实体类定义 */
	CModel tmmsm70("TMMSM70");
	CModel old_tmmsm01("TMMSM01");
	CModel tmmsm01("TMMSM01");
	CModel new_tmmsm01("TMMSM01");
	//CTPSSM05 tpssm05(conn);
#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
	CModel tmm0005("TMM0005");
	CModel tpmof01("TPMOF01");
	CModel tpmof03("TPMOF03");
#endif

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MMSM70");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MMSM70 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		/* 添加并设置块名 */
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

		blkNum = bcls_rec->Tables.IndexOf("MM0099");				//调用物料跟踪传入数据块
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
		}
#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
		blkNum = bcls_rec->Tables.IndexOf("PMOF99");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("PMOF99");
		}
		blkNum = bcls_rec->Tables.IndexOf("MM000501");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("MM000501");
		}

#endif

		/* 获取输入参数 */
		tmmsm70.MergeFrom(bcls_rec->Tables["MMSM70"].Rows[0]);
		tmmsm70.TrimOrBlank();

		/* 打印输入参数 */
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm70.MAT_NO					= [{0}]", tmmsm70["MAT_NO"].ToString());
		//Log::Trace("",__FUNCTION__,"传入参数 tmmsm70.IN_MAT_NO 				= [{0}]",tmmsm70.IN_MAT_NO );	  				


		/* 检查输入参数合法性 */
		if (tmmsm70["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "出口材料号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//if (tmmsm70["IN_MAT_NO"].ToString().Trim() == "")
		//{
		//	strcpy(s.msg, "入口材料号不能为空!");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		/* 获取入口材料信息 */
		CString in_mat_col = "";
		CDecimal r_num = 0;
		bcls_rec->Tables["WM_STOCK"].Rows.Clear();
		bcls_rec->Tables["MM0099"].Clear();
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");

		for (int p_count = 0; p_count < bcls_rec->Tables["MMSM70P"].Rows.get_Count(); p_count++)
		{
			old_tmmsm01["MAT_NO"] = bcls_rec->Tables["MMSM70P"].Rows[p_count]["IN_MAT_NO"].ToString();
			//Log::Trace("", __FUNCTION__, "传入参数iN_MAT_NO  = [{0}],[{1}]", old_tmmsm01["MAT_NO"].ToString(), p_count);
			if (!old_tmmsm01.Query("MAT_NO"))
			{
				sprintf(s.msg, "母材料不存在!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			v_out_wt_act = v_out_wt_act + old_tmmsm01["MAT_ACT_WT"].ToDecimal();
			v_out_wt_thoery = v_out_wt_thoery + old_tmmsm01["MAT_THEORY_WT"].ToDecimal();
			v_out_wt = v_out_wt + old_tmmsm01["MAT_WT"].ToDecimal();
			bcls_rec->Tables["WM_STOCK"].Rows.Clear();
			bcls_rec->Tables["WM_STOCK"].Rows.Add(); // 创建一行
			bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_NO"] = old_tmmsm01["MAT_NO"];
			bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_KIND"] = "SM";
			bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_LINE_TYPE"] = "SM";
			bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "2B";
			bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = old_tmmsm01["STOCK_NO"];
			bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = " ";
			bcls_rec->Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
			bcls_rec->Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
			bcls_rec->Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
			bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";

			bcls_rec->Tables["MM0099"].Rows.Clear(); // 创建一行
			bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM06";
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
			bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "f_mmsm7002_proc";
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = old_tmmsm01["MAT_NO"];




			/* 入口材料信息归档 */
#if defined(_WMS_DEPENDENT_SM)   //非独立仓库
			/* 入口材料自动出库 */
			//bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_NO"] = old_tmmsm01["MAT_NO"];
			//bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_KIND"] = "SM";
			//bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_LINE_TYPE"] = "SM";
			//bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "2B";
			//bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = old_tmmsm01["STOCK_NO"];
			//bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = " ";
			//bcls_rec->Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
			//bcls_rec->Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
			//bcls_rec->Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
			//bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";
			//doFlag = f_wmxx_stock_out(bcls_rec, bcls_ret, conn); //2022-08-12 去头文件时编译报错 暂时注销
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
#endif

			//bcls_rec->Tables["MM0099"].Clear();
			//bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			//bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			//bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
			//bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			//bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
			//bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
			//bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"]			= "MM06";
			//bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"]	= "SM";
			//bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"]			= "MMSM";
			//bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"]			= "f_mmsm7002_proc";
			//bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"]			= tmmsm70["IN_MAT_NO"];

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		//Log::Trace("", __FUNCTION__, "计算出口重量  = [{0}]【{1}】", bcls_rec->Tables["MMSM70P"].Rows.get_Count(), v_out_wt_act);


		

		/* 按出口材料号新增材料主档 */
		new_tmmsm01.CopyFrom(old_tmmsm01);
		new_tmmsm01.CopyFrom(tmmsm70);

		v_out_wt_act = tmmsm70["MAT_THEORY_WT"];
		v_out_wt_thoery = tmmsm70["MAT_THEORY_WT"];
		//针对合同工序信息，PES信息是从计划表里取的,涉及主副制程，不能从入口材料取;MMS信息取的是实绩表的顺序 
		new_tmmsm01["WHOLE_BACKLOG_ACT"] = old_tmmsm01["WHOLE_BACKLOG_ACT"].ToString() + new_tmmsm01["WHOLE_BACKLOG_CODE"].ToString();	//实际全程工序途径码
		new_tmmsm01["PASS_BACKLOG_SEQ_NO"] = old_tmmsm01["PASS_BACKLOG_SEQ_NO"].ToDecimal() + 1;	// (材料通过工序序列号    
		new_tmmsm01["MAT_THEORY_WT"] = v_out_wt_thoery;
		new_tmmsm01["MAT_ACT_WT"] = v_out_wt_act;
		new_tmmsm01["MAT_LINE_TYPE"] = "SM";
		new_tmmsm01["MAT_KIND"] = "SM";
		//new_tmmsm01["FACTORY_DIV"]			= "H";		//厂别区分	 ???
		new_tmmsm01["MAT_SHAPE_FLAG"] = "H";	// H - 电渣锭
		new_tmmsm01["IC_CC_FLAG"] = "I";	// I - 钢锭
		new_tmmsm01["ORIGIN_MAT_NO"] = old_tmmsm01["ORIGIN_MAT_NO"];	//外购材料号		
		new_tmmsm01["RAW_ORIGIN"] = old_tmmsm01["RAW_ORIGIN"];		//原料来源		
		new_tmmsm01["MAT_ORIGIN"] = "2";							//材料来源大类		
		new_tmmsm01["MAT_ORIGIN_DETAIL"] = "2";							//材料来源细分

		new_tmmsm01["MAT_THICK"] = tmmsm70["MAT_ACT_THICK"];	//材料厚度		
		new_tmmsm01["MAT_WIDTH"] = tmmsm70["MAT_ACT_WIDTH"];	//材料宽度		
		new_tmmsm01["MAT_LEN"] = tmmsm70["MAT_ACT_LEN"];	//材料长度		

		new_tmmsm01["MAT_ACT_WIDTH"] = tmmsm70["MAT_ACT_WIDTH"];	//材料实际宽度		
		new_tmmsm01["MAT_ACT_LEN"] = tmmsm70["MAT_ACT_LEN"];		//材料实际长度		
		new_tmmsm01["MAT_ACT_THICK"] = tmmsm70["MAT_ACT_THICK"];	//材料实际长度		
		if (new_tmmsm01["MAT_THICK"].ToDecimal() <= 0)
		{
			new_tmmsm01["MAT_THICK"] = tmmsm70["MAT_ACT_THICK"];	//材料宽度		
		}
		if (new_tmmsm01["MAT_WIDTH"].ToDecimal() <= 0)
		{
			new_tmmsm01["MAT_WIDTH"] = tmmsm70["MAT_ACT_WIDTH"];	//材料宽度		
		}
		if (new_tmmsm01["MAT_LEN"].ToDecimal() <= 0)
		{
			new_tmmsm01["MAT_LEN"] = tmmsm70["MAT_ACT_LEN"];	//材料长度		
		}

		if (new_tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "0")	//0 - 未称重
		{
			new_tmmsm01["MAT_WT"] = v_out_wt_thoery;
		}
		if (new_tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "1")	//1 - 已称重
		{
			new_tmmsm01["MAT_WT"] = v_out_wt_act;
		}
		new_tmmsm01["DEVO_FLAN_WT"] = v_out_wt;
		new_tmmsm01["PROD_CLASS_CODE"] = old_tmmsm01["PROD_CLASS_CODE"];	//产品大类代码
		new_tmmsm01["PROD_CODE"] = old_tmmsm01["PROD_CODE"];		//品名代码

		new_tmmsm01["MAT_DESTION"] = old_tmmsm01["MAT_DESTION"];//????

		new_tmmsm01["MAT_MATCH_FLAG"] = "";							//材料配比标记
		new_tmmsm01["FIN_CUST_CODE"] = old_tmmsm01["FIN_CUST_CODE"];	//最终用户代码 

		//Log::Trace("", __FUNCTION__, "new_tmmsm01.WHOLE_BACKLOG_SEQ		= [{0}]", new_tmmsm01["WHOLE_BACKLOG_SEQ"].ToDecimal());
		//Log::Trace("", __FUNCTION__, "new_tmmsm01.WHOLE_BACKLOG_CODE		= [{0}]", new_tmmsm01["WHOLE_BACKLOG_CODE"].ToString());
		//Log::Trace("", __FUNCTION__, "new_tmmsm01.NEXT_WHOLE_BACKLOG_SEQ	= [{0}]", new_tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal());
		//Log::Trace("", __FUNCTION__, "new_tmmsm01.NEXT_WHOLE_BACKLOG_CODE	= [{0}]", new_tmmsm01["NEXT_WHOLE_BACKLOG_CODE"].ToString());

		new_tmmsm01["MSC"] = old_tmmsm01["MSC"];				//冶金规范码
		new_tmmsm01["MSC_LINE_NO"] = old_tmmsm01["MSC_LINE_NO"];		//产线号
		new_tmmsm01["PSC"] = old_tmmsm01["PSC"];				//产品规范码
		new_tmmsm01["APN"] = old_tmmsm01["APN"];				//产品最终用途码
		new_tmmsm01["SG_CODE"] = old_tmmsm01["SG_CODE"];			//代表钢种代码 
		new_tmmsm01["STD_SG_CODE"] = old_tmmsm01["STD_SG_CODE"];		//标准牌号(钢级代码

		new_tmmsm01["CONFM_FLAG"] = "0";							//准发确认标记
		new_tmmsm01["APP_DECIDE_FLAG"] = "0";							//现货申报标记
		new_tmmsm01["TRANSFER_FLAG"] = "0";							//转库计划标记 
		new_tmmsm01["PCH_JUDGE_CODE"] = "0";							//性能判定代码
		new_tmmsm01["COMPLEX_DECIDE_CODE"] = "0";							//综合判定代码


		new_tmmsm01["PLAN_NO"] = " ";							//计划号
		new_tmmsm01["OLD_STOCK_PLACE_NO"] = old_tmmsm01["STOCK_PLACE_NO"];	//材料原库位号
		new_tmmsm01["IN_FLAG"] = "0";							//入库标记
		new_tmmsm01["SURFACE_DECIDE_CODE"] = "1";							//表面判定代码(默认合格)  1-已判
		new_tmmsm01["SURFACE_DECIDE_TIME"] = datetime;						//表面判定时间    
		new_tmmsm01["SURFACE_DECIDE_MAKER"] = s.userid;						//表面判定责任者    
		new_tmmsm01["PCH_JUDGE_CODE"] = "0";							//性能判定代码   
		new_tmmsm01["COMPLEX_DECIDE_CODE"] = "0";							//综合判定代码  
		new_tmmsm01["TRANSFER_FLAG"] = "0";							//转库计划标记        
		new_tmmsm01["CONFM_FLAG"] = "0";							//准发确认标记       	 
		new_tmmsm01["APP_DECIDE_FLAG"] = "0";							//现货申报标记      	
		new_tmmsm01["PLAN_NO"] = " ";							//计划号                
		//new_tmmsm01["STOCK_NO"]			= " ";							//库号             
		new_tmmsm01["STOCK_PLACE_NO"] = " ";							//材料库位号    


		new_tmmsm01["WHOLE_BACKLOG"] = old_tmmsm01["WHOLE_BACKLOG"];							//全程工序      	
		new_tmmsm01["WHOLE_BACKLOG_NO"] = old_tmmsm01["WHOLE_BACKLOG_NO"];							//制程号   

		new_tmmsm01["WHOLE_BACKLOG_SEQ"] = old_tmmsm01["WHOLE_BACKLOG_SEQ"].ToDecimal() + 1;					//工序顺序号      
		new_tmmsm01["WHOLE_BACKLOG_CODE"] = old_tmmsm01["NEXT_WHOLE_BACKLOG_CODE"];
		new_tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"] = old_tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal() + 1;
		if (new_tmmsm01["ORDER_NO"].ToString().Trim() != "")	//合同材
		{
			new_tmmsm01["NEXT_WHOLE_BACKLOG_CODE"] = new_tmmsm01["WHOLE_BACKLOG"].ToString().SubstringNE((new_tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"].ToDecimal().ToInt16() * 2) - 2, 2);
		}




		//Log::Trace("", __FUNCTION__, "校验材料形态,new_tmmsm01.MAT_SHAPE_FLAG		= [{0}]", new_tmmsm01["MAT_SHAPE_FLAG"].ToString());

		//Log::Trace("", __FUNCTION__, "new_tmmsm01.WHOLE_BACKLOG_CODE		= [{0}]", new_tmmsm01["WHOLE_BACKLOG_CODE"].ToString());
		/* 新增物料信息 */
		new_tmmsm01["MAT_NUM"] = 1;
		new_tmmsm01["MAT_TUBE"] = 1;
		new_tmmsm01["REC_CREATE_TIME"] = datetime;
		new_tmmsm01["REC_CREATOR"] = s.userid;
		new_tmmsm01.TrimOrBlank();
		new_tmmsm01.Insert();

		/* 调物料跟踪 */
		bcls_rec->Tables["MM0099"].Clear();
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");
		bcls_rec->Tables["MM0099"].Rows.Add();
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM09";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "f_mmsm7002_proc";
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = new_tmmsm01["MAT_NO"];
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "电渣锭产出";
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		new_tmmsm01.MergeFrom(bcls_rec->Tables["NEWMM_TABLE"].Rows[0]);



#if defined _SYS_MMS || defined _SYS_MES     //MMS或MES
		/* 调用 物料跟踪路径新增 */
		tmm0005["MAT_NO"] = tmmsm70["MAT_NO"];
		tmm0005["MAT_KIND"] = "SM";
		tmm0005["IN_MAT_NO"] = tmmsm70["IN_MAT_NO"];
		tmm0005["IN_MAT_ID"] = old_tmmsm01["MAT_ID"];
		tmm0005["IN_MAT_KIND"] = old_tmmsm01["MAT_KIND"];
		tmm0005["MAT_PROD_FLAG"] = "20";		//材料产出标记 10-原料录入;20-机组产出;21-并卷;22-分卷;30-整卷回退;31-半卷回退;50-清盘库
		tmm0005["MAIN_MAT_FLAG"] = "1";		//主材料标记  0-非主材料,1-主材料
		tmm0005["SPECAIL_FLAG"] = "0";		//特殊标记 0-默认值,1-并卷同时分卷

		bcls_rec->Tables["MM000501"].Rows.Clear();
		tmm0005.MergeTo(bcls_rec->Tables["MM000501"], false);
		doFlag = f_mm000501_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		/* 抛合同跟踪 */
		tpmof03["ORDER_NO"] = new_tmmsm01["ORDER_NO"];
		tpmof03["SYSTEM_ID"] = "MMSM";
		tpmof03["FUNC_ID"] = "f_mmsm7002_proc";
		tpmof03["WHOLE_BACKLOG"] = new_tmmsm01["WHOLE_BACKLOG"];
		tpmof03["WHOLE_BACKLOG_NO"] = new_tmmsm01["WHOLE_BACKLOG_NO"];
		tpmof03["WHOLE_BACKLOG_SEQ"] = new_tmmsm01["WHOLE_BACKLOG_SEQ"];
		tpmof03["WHOLE_BACKLOG_CODE"] = new_tmmsm01["WHOLE_BACKLOG_CODE"];
		tpmof03["MAT_NO"] = new_tmmsm01["MAT_NO"];
		tpmof03["MAT_STATUS"] = new_tmmsm01["MAT_STATUS"];

		tpmof03["EVENT_ID"] = "52SM";	//产
		tpmof03["WT"] = new_tmmsm01["MAT_WT"];
		tpmof03["NUM"] = 1;
		tpmof03["PREV_MAT_NO"] = old_tmmsm01["MAT_NO"];
		tpmof03["PREV_MAT_STATUS"] = old_tmmsm01["MAT_STATUS"];


		CDecimal prev_num = bcls_rec->Tables["MMSM70P"].Rows.get_Count();
		tpmof03["PREV_WT"] = v_out_wt;
		tpmof03["PREV_NUM"] = prev_num;
		tpmof03["PREV_MAT_STATUS"] = old_tmmsm01["MAT_STATUS"];

		//Log::Trace("", __FUNCTION__, "tpmof03.ORDER_NO			= [{0}]", tpmof03["ORDER_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "tpmof03.MAT_NO	= [{0}]", tpmof03["MAT_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "tpmof03.PREV_MAT_NO	= [{0}]", tpmof03["PREV_MAT_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "tpmof03.PREV_WT	= [{0}]", tpmof03["PREV_WT"].ToDecimal());
		//Log::Trace("", __FUNCTION__, "tpmof03.NUM			= [{0}]", tpmof03["NUM"].ToDecimal());
		//Log::Trace("", __FUNCTION__, "new_tmmsm01.SHEET_NUM	= [{0}]", new_tmmsm01["MAT_NUM"].ToDecimal());

		tpmof03.MergeTo(bcls_rec->Tables["PMOF99"], false);
		doFlag = f_pmof99_v3(bcls_rec, bcls_ret, conn);
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
