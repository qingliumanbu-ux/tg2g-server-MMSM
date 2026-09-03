/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-09-01
Description: 炼钢钢坯材料信息新增
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢钢坯材料信息新增
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



#if defined _SYS_MES || defined _SYS_MMS

#endif

//外部函数声明
//计算理重
BM2_FUNCTION_EXPORT
int f_mm0012(CDecimal w_length,			/* Length		    (mm)    */
CDecimal w_width,						/* Width		    (mm)	*/
CDecimal w_thick,						/* Thickness	    (mm)	*/
CDecimal w_density,						/* Material density (g/cm3) */
CDecimal w_ctwg,						/* Coating weight   (g/m2)  */
CDecimal& w_weight,						/* Weight 		    (t)		*/
CDbConnection * conn);

BM2_FUNCTION_IMPORT
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#if defined _SYS_MES || defined _SYS_MMS
BM2_FUNCTION_IMPORT
int f_mm000501_proc(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
#endif
BM2_FUNCTION_IMPORT
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//调用仓库接口，进行板坯入库   太钢定制
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天
int f_mmsm_get_density(CString ST_NO, CDecimal& MAT_DENSITY, CDbConnection* conn);//通过钢种计算密度

BM2F_ENTERACE(mmsm01a1f3_ins)

int f_mmsm01a1f3_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_seq_no("");
	CDecimal w_density = 7.85;	/* Material density (g/cm3) */
	CDecimal w_ctwg = 0;		/* Coating weight   (g/m2)  */
	CDecimal MAT_THEORY_WT = 0;
	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	CModel tsi0021("TSI0021");
	CModel tmmsm33("TMMSM33");
	CModel twmsmzd02("TWMSMZD02");
#if defined _SYS_MES || defined _SYS_MMS
	CModel tmm0005("TMM0005");
#endif

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "SURF_QUALITY");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");
	

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm01);
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
			//bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MAT_NO");
			bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行

		}

#if defined _SYS_MES || defined _SYS_MMS
		blkNum = bcls_rec->Tables.IndexOf("MM000501");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM000501");
		}
#endif

		//入库队列生成 
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
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "PRE_UNIT_CODE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "NEXT_UNIT_CODE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_DESTION");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");
			bcls_rec->Tables["WM00QUE"].Rows.Add(); // 创建一行
		}

		/**  调用仓库接口  进行入库操作    太钢定制 **/
		blkNum = bcls_rec->Tables.IndexOf("WM_STOCK");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("WM_STOCK");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");        //库业务类型
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_DECIMAL, "STOCK_OPER_ORDER_DIV");   //业务类型内区分
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_NO");				//库号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_NO");			//材料库位号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "ROWNO");					//行号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "COLUMN_NO");				//列号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "LAYERNO");					//层号
			bcls_rec->Tables["WM_STOCK"].Columns.Add(DT_STRING, "STOCK_PLACE_POSITION");	//库位内位置
		}

		/* 获得传入参数 */
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm01.TrimOrBlank();

		//Log::Trace("",__FUNCTION__,"tmmsm01.MAT_NO		= [{0}]",(const char*)tmmsm01["MAT_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "tmmsm01.MAT_SHAPE_FLAG		= [{0}]", (const char*)tmmsm01["MAT_SHAPE_FLAG"].ToString());

		/* 检查输入参数合法性 */
		if (tmmsm01["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//if(tmmsm01["MAT_NO"].ToString().GetLength() < 12)
		//{
		//	strcpy(s.msg,"材料号长度不能小于12位!");
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}		
		if (tmmsm01["MAT_NO"].ToString().GetLength() > 20)
		{
			sprintf(s.msg, _RES("MMHRS0000242")/*材料号长度不能超过20位*/);
			sprintf(s.sysmsg, _RES("MMHRS0000242")/*材料号长度不能超过20位*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料形态不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		/*if(tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() != "2"
		&& tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() != "3"
		&& tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() != "4")
		{
		sprintf(s.msg,"数据校验出错，材料形态标记错误!2:钢板4:纵切钢带3:钢卷");
		throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		/*if(tmmsm01["PONO"].ToString().Trim() == "")
		{
		strcpy(s.msg, "制造命令号不能为空");
		throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		if (tmmsm01["ST_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "内部钢种不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "")
		{
			strcpy(s.msg, "称重标记不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm01["PRODUCT_FLAG"].ToString().Trim() == "")
		{
			strcpy(s.msg, "成品标记不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//if(tmmsm01["WHOLE_BACKLOG_CODE"].ToString().Trim() !=	"H1"
		//&& tmmsm01["WHOLE_BACKLOG_CODE"].ToString().Trim() !=	"H2"
		//&& tmmsm01["WHOLE_BACKLOG_CODE"].ToString().Trim() !=	"H3"
		//&& tmmsm01["WHOLE_BACKLOG_CODE"].ToString().Trim() != "H0")
		//{ 		 
		//	strcpy(s.msg, _RES("MMHRS0000232")/*全程工序代码值错误!清盘库材料新增选择正确的全程工序代码*/);
		//	sprintf(s.sysmsg,_RES("MMHRS0000232")/*全程工序代码值错误0。*/);
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//	
		//}  	  
		/*if(tmmsm01["MAT_THICK"].ToDecimal() <= 0 )
		{
		sprintf(s.msg,"材料号[%s]厚度[%f]不能小于0!",(const char*)tmmsm01["MAT_NO"].ToString(),tmmsm01["MAT_THICK"].ToDecimal().ToDouble());
		throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(tmmsm01["MAT_WIDTH"].ToDecimal() <= 0 )
		{
		sprintf(s.msg,"材料号[%s]宽度[%f]不能小于0!",(const char*)tmmsm01["MAT_NO"].ToString(),tmmsm01["MAT_WIDTH"].ToDecimal().ToDouble());
		throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(tmmsm01["MAT_LEN"].ToDecimal() <= 0 )
		{
		sprintf(s.msg,"材料号[%s]长度[%d]不能小于0!",(const char*)tmmsm01["MAT_NO"].ToString(),tmmsm01["MAT_LEN"].ToDecimal().ToInt32());
		throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		//if(tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "3")   //3：钢卷
		//{
		//	if(tmmsm01.MAT_ACT_INNER_DIA == 0)
		//	{
		//		strcpy(s.msg, _RES("MM00S0000082")/*材料内径不能为0。*/);
		//		strcpy(s.sysmsg, _RES("MM00S0000082")/*材料内径不能为0。*/);
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//	if(tmmsm01.MAT_ACT_OUTER_DIA == 0)
		//	{
		//		strcpy(s.msg, _RES("MM00S0000083")/*材料外径不能为0。*/);
		//		strcpy(s.sysmsg, _RES("MM00S0000083")/*材料外径不能为0。*/);
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//}
		if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "2" || tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "4")
		{
			//2：钢板；4：纵切钢带
			if (tmmsm01["MAT_NUM"].ToDecimal() == 0)
			{
				strcpy(s.msg, _RES("MM00S0000155")/*数据校验出错，材料数量不能为0*/);
				strcpy(s.sysmsg, _RES("MM00S0000155")/*数据校验出错，材料数量不能为0*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		if (tmmsm01["MAT_WT"].ToDecimal() <= 0)
		{
			strcpy(s.msg, "材料重量不能小于0!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm01["STOCK_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "库号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//太钢定制，通过炉号去获取PONO
		sqlstr = "SELECT PONO FROM TPSSM11 WHERE HEAT_NO = '" + tmmsm01["HEAT_NO"].ToString() + "' UNION ALL "
			" SELECT PONO FROM TPSSM41 WHERE HEAT_NO = '" + tmmsm01["HEAT_NO"].ToString() + "'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm01["PONO"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		CString date_month_max = Db::QueryCString("select max(SLAB_CUT_TIME) from vmmsm01 where HEAT_NO='"+ tmmsm01["HEAT_NO"].ToString() +"'");
		twmsmzd02["CODE_CLASS"] = "ADMIN";
		twmsmzd02["CODE"] = s.userid;
		Log::Trace("", __FUNCTION__, "s.userid	= [{0}]", s.userid);
		if (date_month_max.Trim()!=""&& date_month_max.SubstringNE(0,6)< datetime.SubstringNE(0,6)&&twmsmzd02.QueryCount("CODE_CLASS,CODE")<=0)
		{
			strcpy(s.msg, "所新增熔炼号不在本月，不可新增!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

#pragma region  因前台传入数据部分为复制的原数据，故这里需将数据置空或赋值
		tmmsm01["PREC_SLAB_NO"] = "";           /*预定板坯号*/
		tmmsm01["PONO_SLAB"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_1"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_2"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_3"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_4"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_5"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_6"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_7"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_8"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_9"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_10"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_11"] = " ";           /*命令板坯号*/
		tmmsm01["PONO_SLAB_12"] = " ";           /*命令板坯号*/
		Log::Trace("", __FUNCTION__, "11");
		tmmsm01["FIX_SLAB_NUM"] = 0;
		tmmsm01["LSLAB_NO"] = " ";
		tmmsm01["ORDER_NO"] = " ";
		tmmsm01["INITIAL_ORDER_NO"] = tmmsm01["ORDER_NO"];//初始合同号
		tmmsm01["REPAIR_FLAG"] = "0";                  /*返修标记C1*/
		tmmsm01["HOLD_FLAG"] = "0";                  /*封锁标记C1*/
		tmmsm01["SURFACE_DECIDE_CODE"] = "1";              /*表面判定代码C1*/
		tmmsm01["SURFACE_DECIDE_MAKER"] = " ";             /*表面判定责任者*/
		tmmsm01["PCH_JUDGE_CODE"] = "0";              /*性能判定代码C1*/
		tmmsm01["COMPLEX_DECIDE_CODE"] = "0";              /*综合判定代码C1*/
		tmmsm01["SLABTOP_FLAG"] = "0";              /*板坯TOP点确认标志*/

		tmmsm01["IN_FLAG"] = "0";              /*入库标记*/
		tmmsm01["TRANSFER_FLAG"] = "0";              /*转库计划标记*/
		tmmsm01["PRODUCT_PACK_FLAG"] = "0";              /*成品包装标志*/
		tmmsm01["CONFM_FLAG"] = "0";              /*准发确认标记*/
		tmmsm01["APP_DECIDE_FLAG"] = "0";              /*现货申报标记*/
		tmmsm01["STOCK_PLACE_NO"] = "GD";
		tmmsm01["COE_A"] = 0;
		tmmsm01["COE_B"] = 0;
		tmmsm01["MEND_FLAG"] = "0";
		tmmsm01["LGORT"] = "";
		tmmsm01["LOGISTICS_STATUS"] = "0";
		tmmsm01["DEV_CODE"] = tmmsm01["UNIT_CODE"];

		//余材原因
		tmmsm01["REMAINDER_REASON"] = " ";
		tmmsm01["HOLD_FLAG"] = "0";
		tmmsm01["RCV_MAT_FLAG"] = "N";  //收货标记  N 未收货
		tmmsm01["RECV_MAT_TIME"] = " ";
		tmmsm01["RECEIVE_WEIGHT"] = 0;
		tmmsm01["USAGE_DECISION"] = " ";

		//判废
		tmmsm01["SCRAP_TIME"] = " ";
		tmmsm01["SCRAP_MAKER"] = " ";
		tmmsm01["SCRAP_CAUSE_CODE"] = " ";
		tmmsm01["SCRAP_REMARK"] = " ";

		//物流调拨
		tmmsm01["C_STATESIGN"] = "0";
		tmmsm01["LOGISTICS_STATUS"] = "0";
		tmmsm01["C_DELIVERY_FAC"] = " ";
		tmmsm01["C_DELIVERY_STOCK"] = " ";
		tmmsm01["PRE_LOAD_FLAG"] = " ";
		tmmsm01["FACTORY_TO"] = " ";
		tmmsm01["DST_STOCK_CODE"] = " ";
		tmmsm01["UNLOAD_CODE"] = " ";
		tmmsm01["TRAN_TIME"] = " ";
		tmmsm01["TRAN_END_TIME"] = " ";
		tmmsm01["C_DELIVERYID"] = " ";
		tmmsm01["HAND_OVER_GROUP"] = " ";
		tmmsm01["C_ISHOTSEND"] = " ";
		tmmsm01["OUT_STOCK_TIME"] = " ";
		tmmsm01["LOAD_SCHEME_NO"] = " ";
		tmmsm01["PRACTICE_NO"] = " ";
#pragma endregion

		//tmmsm01["SLAB_NO"] = tmmsm01["PRINT_NO"];

		EIClass bcls_rec_WM02;
		CString v_bmzl = "";

		sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TWMSMZD02 WHERE  CODE_CLASS ='MMBMZL' and code='" + tmmsm01["SURF_QUALITY"].ToString() + "' ";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_bmzl = cmd_inq.GetString(1);
		}
		cmd_inq.Close();

		if (v_bmzl.Find("调宽") >= 0)
		{
			tmmsm01["ADJUST_WIDTH_MARK"] = "1";
		}

		Log::Trace("", __FUNCTION__, "tmmsm01.SURF_QUALITY		= [{0}]", v_bmzl);
		Log::Trace("", __FUNCTION__, "tmmsm01.SURF_QUALITY111		= [{0}]", v_bmzl.Find("调宽"));
		Log::Trace("", __FUNCTION__, "tmmsm01.ADJUST_WIDTH_MARK		= [{0}]", tmmsm01["ADJUST_WIDTH_MARK"].ToString());


		/* 材料是否在当前档 */
		if (tmmsm01.QueryCount("MAT_NO") > 0)
		{
			sprintf(s.msg, "材料[%s]已在当前档存在!", (const char*)tmmsm01["MAT_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		else
		{
			hmmsm01["MAT_NO"] = tmmsm01["MAT_NO"];
			if (0 < hmmsm01.QueryCount("MAT_NO"))
			{
				sprintf(s.msg, "材料号[%s]已存在，但已归档!", (const char*)hmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		//Log::Trace("",__FUNCTION__,"查询该材料是否存在 tmmsm01.STOCK_NO		= [{0}]",tmmsm01["STOCK_NO"].ToString());
		/* 查询库号信息是否存在 */
		/*tsi0021["STOCK_NO"] = tmmsm01["STOCK_NO"];
		if (!tsi0021.Query("STOCK_NO"))
		{
		sprintf(s.msg, "材料号[%s]的库号[%s]信息查询失败!", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["STOCK_NO"].ToString());
		throw CApplicationException(-1, s.msg, s.svc_name);
		}*/

		//Log::Trace("", __FUNCTION__, "查询该材料是否存在 tsi0021.FACTORY_DIV		= [{0}]", tsi0021["FACTORY_DIV"].ToString());

		/* 设置主档表初始值 */
		tmmsm01["FACTORY_DIV"] = "LG1";
		tmmsm01["FACTORY_STORE"] = "LG1";
		tmmsm01["MAT_ORIGIN"] = "5";				//材料来源大类 5-清盘库
		tmmsm01["MAT_LINE_TYPE"] = "SM";             //物料产线类型
		tmmsm01["MAT_KIND"] = "SM";             //物料种类  
		doFlag = f_mm0011("MM00_MAT_TRACK_NO", 4, cs_seq_no, conn);
		if (doFlag < 0 || cs_seq_no.Trim() == "")
		{
			sprintf(s.msg, "获取 生产流水号 失败，请查看数据库sequence【MM00_MAT_TRACK_NO】是否正常!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tmmsm01["MAT_TRACK_NO"] = datetime + cs_seq_no.Trim();	//物料跟踪号 流水号
		tmmsm01["MAT_TRACK_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmssmsff").Substring(0, 18);
		//tmmsm01["FACTORY_DIV"]			= "H";              //厂别区分
		tmmsm01["IN_FLAG"] = "0";
		tmmsm01["HOLD_FLAG"] = "0";
		tmmsm01["TRANSFER_FLAG"] = "0";
		tmmsm01["PCH_JUDGE_CODE"] = "0";
		tmmsm01["COMPLEX_DECIDE_CODE"] = "0";
		tmmsm01["CONFM_FLAG"] = "0";
		tmmsm01["APP_DECIDE_FLAG"] = "0";
		tmmsm01["REPAIR_FLAG"] = "0";
		tmmsm01["SURFACE_DECIDE_CODE"] = "1";					//表面判定代码(默认合格)
		tmmsm01["SURFACE_DECIDE_TIME"] = datetime;				//表面判定时间  
		tmmsm01["SURFACE_DECIDE_MAKER"] = s.userid;				//表面判定责任者   
		//tmmsm01["MAT_ACT_THICK"]		= tmmsm01["MAT_THICK"];
		//      tmmsm01.MAT_ACT_WIDTH		= tmmsm01["MAT_WIDTH"];
		//      tmmsm01.MAT_ACT_LEN			= tmmsm01["MAT_LEN"];
		
		tmmsm01["PROD_MAKER"] = s.userid;

		//赋默认值  头宽尾宽保持一致   mfj  太钢定制  20231227
		
		tmmsm01["SLAB_HEAD_WIDTH"] = tmmsm01["MAT_WIDTH"];
		
		
		tmmsm01["SLAB_TAIL_WIDTH"] = tmmsm01["MAT_WIDTH"];
		
		
		tmmsm01["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];
		
		
		tmmsm01["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
		
	
		tmmsm01["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
		

		if (tmmsm01["SLAB_CUT_TIME"].ToString().Trim() == "")
		{
			tmmsm01["SLAB_CUT_TIME"] = datetime;
		}
		tmmsm01["PROD_TIME"] = tmmsm01["SLAB_CUT_TIME"];

		//计算理重实重
		if (tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "0") //称重标记  0 - 未称重
		{
			tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_WT"].ToDecimal().Round(3);
			//太钢定制   mfj  20240113
			//tmmsm01["MAT_ACT_WT"] = tmmsm01["MAT_WT"];// 收货后才有值  mfj  20240419
			//tmmsm01["REAL_TIME_WT"] = tmmsm01["MAT_WT"]; //实时重量   等收货时有数据，与产销保持一致
			//tmmsm01["QUALIFIED_WT"] = tmmsm01["MAT_WT"];//合格产量 收货后才有值  mfj  20240419
			//tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["MAT_WT"];//收货重量 一起跟着变
			
			
		}
		if (tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "1")//称重标记  1 - 已称重
		{
			//盘库新增的理论重量以用户录入为准   
			//f_mm0012(tmmsm01["MAT_ACT_LEN"].ToDecimal(),		/* Length		    (mm)    */
			//	tmmsm01["MAT_ACT_WIDTH"].ToDecimal(),			/* Width		    (mm)	*/
			//	tmmsm01["MAT_ACT_THICK"].ToDecimal(),			/* Thickness	    (mm)	*/
			//	w_density,						/* Material density (g/cm3) */
			//	w_ctwg,							/* Coating weight   (g/m2)  */
			//	MAT_THEORY_WT,			/* Weight 		    (t)		*/
			//	conn);
			//tmmsm01["MAT_ACT_WT"]		= tmmsm01["MAT_WT"];  收货后才有值  mfj  20240419
			//tmmsm01["QUALIFIED_WT"] = tmmsm01["MAT_WT"];//合格产量  收货后才有值  mfj  20240419
			tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_WT"].ToDecimal().Round(3);
			//tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["MAT_WT"];//收货重量 一起跟着变
		}
		tmmsm01["MAT_ACT_WT"] = 0;
		tmmsm01["MEASURE_WT"] = tmmsm01["MAT_WT"];
		tmmsm01["L2_THEORY_WT"] = tmmsm01["MAT_WT"];
		tmmsm01["L3_CALTHEROY_WT"] = tmmsm01["MAT_WT"];
		// 材料支数不能为0
		if (tmmsm01["MAT_NUM"].ToDecimal() == 0)
		{
			tmmsm01["MAT_NUM"] = 1;
		}

		tmmsm01["MAT_TUBE"] = tmmsm01["MAT_NUM"];


		//根据钢种查询钢牌号，将钢牌号更新掉  工艺卡牌号  跟sg_sign不同，sg_sign从tpssm03表获取
		if (tmmsm01["ST_NO"].ToString() != "")
		{
			sqlstr = "SELECT  SG_GRADE_1,C_DIV  FROM TQMTS0X  WHERE ST_NO = '" + tmmsm01["ST_NO"].ToString().Trim() + "'";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tmmsm01["SG_GRADE_1"] = cmd_inq.GetString(1);
				tmmsm01["C_DIV"] = cmd_inq.GetString(2);
			}
			//para_tmmsm01["ORDER_NO"] = " ";//脱合同，不抛合同跟踪  mfj 潘  20240410
			cmd_inq.Close();
		}

		//将不为12的碳锈区分，改为12
		if (tmmsm01["C_DIV"].ToString().Trim() != "")
		{
			if (tmmsm01["C_DIV"].ToString().Trim() == "4")
			{
				tmmsm01["C_DIV"] = "1";
			}

			if (tmmsm01["C_DIV"].ToString().Trim() == "3" || tmmsm01["C_DIV"].ToString().Trim() == "5")
			{
				tmmsm01["C_DIV"] = "2";
			}
		}


		//获取计算重量    
		//首先判断钢种前两位   系数  1A 7.86  1D 7.76   1F  7.83   1M 7.83
		//若以上判断获取不到，则判断钢种第一位   1 7.85  2 7.82  3 7.82
		if (true)
		{
			CDecimal v_code_wt = 0;//计算重量的系数

			/*if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 3) == "1A6")
			{
				v_code_wt = 7.95;
			}
			else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 3) == "1A9")
			{
				v_code_wt = 7.95;
			}
			else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 2) == "1D")
			{
				v_code_wt = 7.8;
			}
			else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "1")
			{
				v_code_wt = 7.9;
			}
			else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "2")
			{
				v_code_wt = 7.85;
			}
			else if (tmmsm01["ST_NO"].ToString().Trim().Substring(0, 1) == "3")
			{
				v_code_wt = 7.85;
			}*/
			doFlag = f_mmsm_get_density(tmmsm01["ST_NO"].ToString(), v_code_wt,conn);

			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tmmsm01["PRODUTE_CAL_WT"] = ((tmmsm01["MAT_ACT_THICK"].ToDecimal() / 1000) * (tmmsm01["MAT_ACT_WIDTH"].ToDecimal() / 1000) * (tmmsm01["MAT_ACT_LEN"].ToDecimal() / 1000) * v_code_wt).Round(3);

		}


		//获取连铸初判数据，获取不到赋默认值A
		cmd_inq.SetCommandText("SELECT CK_RESULT from tmmsm3f where SLAB_NO = '" + tmmsm01["SLAB_NO"].ToString().Trim() + "'  ORDER BY  ID DESC");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm01["CASTING_PRE_JUDGMENT"] = cmd_inq.GetString(1);
		}
		else//没有获取到时给默认值
		{
			tmmsm01["CASTING_PRE_JUDGMENT"] = "A";
		}
		cmd_inq.Close();


		//Log::Trace("", __FUNCTION__, "tmmsm01.MAT_THEORY_WT		= [{0}]", tmmsm01["MAT_THEORY_WT"].ToDecimal());
		//Log::Trace("",__FUNCTION__,"新增主档表 tmmsm01.Insert		= [{0}]",tsi0021["STOCK_NO"].ToString());
		/* 新增主档表 */
		tmmsm01["REC_CREATE_TIME"] = datetime;			//记录创建时刻         
		tmmsm01["REC_CREATOR"] = s.userid;			//记录创建责任者      
		tmmsm01.TrimOrBlank();
		//tmmsm01.Insert();     //MM02事件已配置insert,此处注释。  20230822修改

		/* 设置物料跟踪参数 */
		bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm01);
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM02";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm01a1f3_ins";
		//bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"]			= tmmsm01["MAT_NO"]; 
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(doFlag, s.msg, log.Location);
		}


		tmmsm33.CopyFrom(tmmsm01);
		tmmsm33.Print();
		if (tmmsm33.QueryCount("MAT_NO"))
		{
			Log::Trace("", __FUNCTION__, "该材料号在33表已存在 [{0}]", (const char*)tmmsm33["MAT_NO"].ToString());
		}
		else
		{
			tmmsm33["ARCHIVE_FLAG"] = "3";//归档标记为3的表示只是为了材料号占位，在33画面不显示  mfj  20240115
			tmmsm33.Insert();
		}




#if defined _SYS_MMS || defined _SYS_MES
		/*调用 物料跟踪路径新增 */
		tmm0005["MAT_NO"] = tmmsm01["MAT_NO"];
		tmm0005["MAT_KIND"] = tmmsm01["MAT_KIND"];
		tmm0005["MAT_PROD_FLAG"] = "50";		//材料产出标记 10-原料录入;20-机组产出;21-并卷;22-分卷;30-整卷回退;31-半卷回退;50-清盘库
		tmm0005["MAIN_MAT_FLAG"] = "1";		//主材料标记  0-非主材料,1-主材料
		tmm0005["SPECAIL_FLAG"] = "0";		//特殊标记 0-默认值,1-并卷同时分卷
		tmm0005.MergeTo(bcls_rec->Tables["MM000501"], false);
		doFlag = f_mm000501_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
#endif

		/*调用仓库入库队列*/
		bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1M"; //1M-盘盈入库
		bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
		bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NUM"] = tmmsm01["MAT_NUM"];
		bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_NO"] = tmmsm01["STOCK_NO"];
		bcls_rec->Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "I";
		doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		bcls_rec->Tables["WM_STOCK"].Rows.Add();
		bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
		bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "1M";					//库业务类型
		bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER_DIV"] = "1";					//业务类型内区分
		bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = "SYA";			//库号
		bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = "SYA";						//材料库位号
		bcls_rec->Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";								//行号
		bcls_rec->Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";							//列号
		bcls_rec->Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;								//层号
		bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = "1";				//库位内位置
		doFlag = f_wmsmsm_stock_in(bcls_rec, bcls_ret, conn);  //太钢定制   产出时入库
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//发送宝武聊天 表面质量封锁 暂不发，还没维护好小代码
		//if (tmmsm01["SURF_QUALITY"].ToString().TrimOrBlank() != "0" || tmmsm01["SURF_QUALITY"].ToString().TrimOrBlank() != " ")
		//{

		//	//查询对应的中文名字  表面质量
		//	CString code_desc_surt = "";
		//	CString strsql = " select CODE_DESC_1_CONTENT from TWMSMZD02  WHERE CODE_CLASS = 'MMBMZL' AND CODE = '" + tmmsm01["SURF_QUALITY"].ToString() + "'";
		//	cmd_inq1.SetCommandText(strsql);
		//	cmd_inq1.ExecuteReader();
		//	if (cmd_inq1.Read())
		//	{
		//		code_desc_surt = cmd_inq1.GetString(1);
		//	}
		//	cmd_inq1.Close();

		//	Log::Trace("", __FUNCTION__, "code_desc_surt[{0}]  ", code_desc_surt);

		//	bcls_rec_s.Tables[0].Rows.Add();
		//	bcls_rec_s.Tables[0].Rows[0]["SURF_QUALITY"] = code_desc_surt;
		//	bcls_rec_s.Tables[0].Rows[0]["CODE"] = "6";
		//	bcls_rec_s.Tables[0].Rows[0]["REMARK"] = tmmsm01["MAT_NO"];
		//	doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
		//	if (doFlag < 0)
		//	{
		//		strcpy(s.msg, "调用函数报错!");
		//		throw CApplicationException(-1, s.msg, log.Location);
		//	}
		//}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

