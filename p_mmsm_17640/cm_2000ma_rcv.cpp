/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      178053
Version:     1.0
Date:        2019-12-13 16:04:17
Description: MMS侧接收炼钢钢坯信息新增(炼钢PES->MMS)电文
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"

/*<remark>=========================================================
/// <summary>
/// MMS侧接收热轧钢卷数据新增电文
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件





//外部函数声明
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(cm_2000ma_rcv)


int f_cm_2000ma_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_seq_no("");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel hmmsm01("HMMSM01");
	CModel tsi0021("TSI0021");

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
		}

		/* 获得传入参数 */
		tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm01.TrimOrBlank();

		/* 打印传入参数 */
		//Log::Trace("", __FUNCTION__, "传入参数 tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

		/* 检查输入参数合法性 */
		if (tmmsm01["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "")
		{
			strcpy(s.msg, "材料形态不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() != "2"
			&& tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() != "3"
			&& tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() != "4")
		{
			sprintf(s.msg, "数据校验出错，材料形态标记错误!2:钢板4:纵切钢带3:钢卷");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm01["PONO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "制造命令号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
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
		if (tmmsm01["MAT_THICK"].ToDecimal() <= 0)
		{
			sprintf(s.msg, "材料号[%s]厚度[%f]不能小于0!", (const char*)tmmsm01["MAT_NO"].ToString(), tmmsm01["MAT_THICK"].ToDecimal().ToDouble());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm01["MAT_WIDTH"].ToDecimal() <= 0)
		{
			sprintf(s.msg, "材料号[%s]宽度[%f]不能小于0!", (const char*)tmmsm01["MAT_NO"].ToString(), tmmsm01["MAT_WIDTH"].ToDecimal().ToDouble());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (tmmsm01["MAT_LEN"].ToDecimal() <= 0)
		{
			sprintf(s.msg, "材料号[%s]长度[%d]不能小于0!", (const char*)tmmsm01["MAT_NO"].ToString(), tmmsm01["MAT_LEN"].ToDecimal().ToInt32());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "3")   //3：钢卷
		//{
		//	if (tmmsm01.MAT_ACT_INNER_DIA == 0)
		//	{
		//		strcpy(s.msg, "材料内径不能为0!");
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//	if (tmmsm01.MAT_ACT_OUTER_DIA == 0)
		//	{
		//		strcpy(s.msg, "材料外径不能为0!");
		//		throw CApplicationException(-1, s.msg, s.svc_name);
		//	}
		//}
		if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "2" || tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "4")
		{
			//2：钢板；4：纵切钢带
			if (tmmsm01["MAT_NUM"].ToDecimal() == 0)
			{
				strcpy(s.msg, "数据校验出错，材料数量不能为0!");
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

		//Log::Trace("", __FUNCTION__, "查询该材料是否存在 tmmsm01.STOCK_NO		= [{0}]", tmmsm01["STOCK_NO"].ToString());

		/* 查询库号信息是否存在 */
		tsi0021["STOCK_NO"] = tmmsm01["STOCK_NO"];
		if (tsi0021.Query("STOCK_NO") == false)
		{
			sprintf(s.msg, "材料号[%s]的库号[%s]信息查询失败!", (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["STOCK_NO"].ToString());
			//throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 设置主档表初始值 */
		tmmsm01["FACTORY_DIV"] = tsi0021["FACTORY_DIV"];
		tmmsm01["FACTORY_STORE"] = tsi0021["FACTORY_DIV"];
		tmmsm01["MAT_ORIGIN"] = "5";				//材料来源大类 5-清盘库
		tmmsm01["MAT_LINE_TYPE"] = "SM";           //物料产线类型
		tmmsm01["MAT_KIND"] = "SM";				//物料种类  
		tmmsm01["RAW_ORIGIN"] = "5";				//原料来源大类 5-清盘库
		doFlag = f_mm0011("MM00_MAT_TRACK_NO", 4, cs_seq_no, conn);
		if (doFlag < 0 || cs_seq_no.Trim() == "")
		{
			sprintf(s.msg, "获取 生产流水号 失败，请查看数据库sequence【MM00_MAT_TRACK_NO】是否正常!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tmmsm01["MAT_TRACK_NO"] = datetime + cs_seq_no.Trim();	//物料跟踪号 流水号
		tmmsm01["FACTORY_DIV"] = "H";              //厂别区分
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
		tmmsm01["SURFACE_DECIDE_MAKER"] = s.userid;			//表面判定责任者   
		//tmmsm01.COLD_HOT_FLAG = "0";			//0-冷;1-热
		//tmmsm01.DUMMY_COIL_FLAG = "0";		//0-非过渡卷
		tmmsm01["PROD_TIME"] = datetime;
		tmmsm01["PROD_MAKER"] = s.userid;
		tmmsm01["CMD_FLAG"] = "0";					/* 吊车命令标志 */
		//tmmsm01.TAIL_FLAG = "0";				/* 尾包标志 */
		tmmsm01["MAT_RETURN_FLAG"] = "0";			/* 材料回退标记 */
		//tmmsm01.BACK_MAT_FLAG = "0";			/* 返品标记 */
		//计算理重实重
		if (tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "0") //称重标记  0 - 未称重
		{
			tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_WT"];
			tmmsm01["MAT_ACT_WT"] = 0;
		}
		if (tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "1")//称重标记  1 - 已称重
		{
			tmmsm01["MAT_ACT_WT"] = tmmsm01["MAT_WT"];
		}
		//Log::Trace("", __FUNCTION__, "tmmsm01.MAT_THEORY_WT		= [{0}]", tmmsm01["MAT_THEORY_WT"].ToDecimal());
		//Log::Trace("", __FUNCTION__, "新增主档表 tmmsm01.Insert	= [{0}]", tsi0021["STOCK_NO"].ToString());

		/* 新增主档表 */
		tmmsm01["REC_CREATE_TIME"] = datetime;		//记录创建时刻         
		tmmsm01["REC_CREATOR"] = s.userid;			//记录创建责任者      
		tmmsm01.TrimOrBlank();
		//tmmsm01.Insert(); 

		/* 设置物料跟踪参数 */
		tmmsm96.CopyFrom(tmmsm01);
		tmmsm96["EVENT_ID"] = "MM02";
		tmmsm96["EVENT_LINE_TYPE"] = "SM";
		tmmsm96["SYSTEM_ID"] = "mmsm";
		tmmsm96["FUNC_ID"] = "cm_2000ma_rcv";
		tmmsm96.TrimOrBlank();
		bcls_rec->Tables["MM0099"].Clear();
		tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(doFlag, s.msg, log.Location);
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


