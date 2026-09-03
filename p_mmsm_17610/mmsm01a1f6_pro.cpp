/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    向萍
Version:    1.0
Date:       2016-09-24
Description: 方坯物料拆批
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 方坯物料信息拆批
/// <para>
/// * 根据传入的材料号和拆批支数调用物料拆批函数
/// <para>
/// </summary>
/// <param name="MAT_NO">材料号 </param>
/// <param name="MAT_TUBE">拆批根数</param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



//外部函数声明
int f_mmsm81(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

#if defined(_WMS_DEPENDENT_BW)   //非独立仓库
int f_wm00_queue(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
#endif

#if defined _SYS_PES 
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

// service入口
BM2F_ENTERACE(mmsm01a1f6_pro)

int f_mmsm01a1f6_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CDecimal cd_mat_tube_new = 0;
	CString	cs_mat_no_new("");

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm01_new("TMMSM01");
	CModel tmmsm96("TMMSM96");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MMSM81");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM81");
			bcls_rec->Tables["MMSM81"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec->Tables["MMSM81"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
			bcls_rec->Tables["MMSM81"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec->Tables["MMSM81"].Columns.Add(DT_STRING, "FUNC_ID");
			bcls_rec->Tables["MMSM81"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["MMSM81"].Columns.Add(DT_STRING, "MAT_NUM");
			bcls_rec->Tables["MMSM81"].Columns.Add(DT_STRING, "MAT_NO_NEW");
			bcls_rec->Tables["MMSM81"].Rows.Add();
		}

#if defined(_WMS_DEPENDENT_BW)   //非独立仓库
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
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_DECIMAL, "MAT_TUBE");
		}
#endif

		/* 获取输入参数 */
		tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();		//母材料号
		cd_mat_tube_new = bcls_rec->Tables[0].Rows[0]["MAT_NUM_CUT"].ToDecimal();

		Log::Trace("", __FUNCTION__, "tmmsm01.MAT_NO	= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "cd_mat_tube_new	= [{0}]", cd_mat_tube_new.ToInt32());

		/* 检查输入参数合法性 */
		if (tmmsm01["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "数据校验失败，传入的材料号不能为空。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (cd_mat_tube_new <= 0)
		{
			strcpy(s.msg, "数据校验失败，材料支数不能为0。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 拆批条件校验 */
		tmmsm01.Query("MAT_NO");
		tmmsm01.TrimOrBlank();

		if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() == "1")
		{
			strcpy(s.msg, "请选择形态符合要求的材料进行拆批!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["MAT_STATUS"].ToString().Trim() == "24")
		{
			strcpy(s.msg, "材料状态在制品编入计划的不能进行拆批!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["MAT_NUM"].ToDecimal() <= 1)
		{
			strcpy(s.msg, "材料数量为1，不允许拆批!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		/* 该校验可根据业务需求 */
		//if(tmmsm01["IN_FLAG"].ToString().Trim() != "1")
		//{
		//	strcpy(s.msg,"材料未入库,不能拆批!");
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		/* 调用拆批函数并发送电文 */
		bcls_rec->Tables["MMSM81"].Rows[0]["EVENT_ID"] = "MM30";	//材料为画面拆批产生
		bcls_rec->Tables["MMSM81"].Rows[0]["EVENT_LINE_TYPE"] = "00";
		bcls_rec->Tables["MMSM81"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MMSM81"].Rows[0]["FUNC_ID"] = "mmsm01a1f6_pro";
		bcls_rec->Tables["MMSM81"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
		bcls_rec->Tables["MMSM81"].Rows[0]["MAT_NUM"] = cd_mat_tube_new;
		bcls_rec->Tables["MMSM81"].Rows[0]["MAT_NO_NEW"] = "";
		doFlag = f_mmsm81(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "cd_mat_tube_new= [{0}]", cd_mat_tube_new);
		cs_mat_no_new = bcls_ret->Tables["MMSM81"].Rows[0]["MAT_NO_NEW"].ToString().Trim();
		Log::Trace("", __FUNCTION__, "cd_mat_tube_new= [{0}]", cd_mat_tube_new);

		/* 炼钢侧母材料剔料拆批发送电文 */
#if defined _SYS_PES 

		tmmsm01.Query("MAT_NO");
		tmmsm01.TrimOrBlank();
		Log::Trace("", __FUNCTION__, "母坯");

		bcls_rec->Tables["MMSMSND"].Rows.Add();
		bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM3G";
		bcls_rec->Tables["MMSMSND"].Rows[0]["EVENT_ID"] = "MM30";
		bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATOR"] = tmmsm01["REC_CREATOR"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATE_TIME"] = tmmsm01["REC_ERASE_TIME"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_MAT_NO"] = tmmsm01["MAT_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_TUBE"] = tmmsm01["MAT_NUM"];
		doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(doFlag, s.msg, log.Location);
		}
#endif

		/* 炼钢侧母材料剔料拆批产生新材料 发送电文 */
#if defined _SYS_PES 
		Log::Trace("", __FUNCTION__, "子坯");

		tmmsm01_new["MAT_NO"] = cs_mat_no_new;
		tmmsm01_new.Query("MAT_NO");
		tmmsm01_new.TrimOrBlank();

		bcls_rec->Tables["MMSMSND"].Rows.Clear();
		bcls_rec->Tables["MMSMSND"].Rows.Add();
		bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM3G";
		bcls_rec->Tables["MMSMSND"].Rows[0]["EVENT_ID"] = "MM3A";
		bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATOR"] = tmmsm01_new["REC_CREATOR"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["REC_CREATE_TIME"] = tmmsm01_new["REC_ERASE_TIME"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_MAT_NO"] = tmmsm01_new["MAT_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_TUBE"] = tmmsm01_new["MAT_NUM"];
		doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(doFlag, s.msg, log.Location);
		}
#endif	

#if defined(_WMS_DEPENDENT_SM)   //非独立仓库
		//必热装且无库位的，则写入库队列
		if (tmmsm01_new["HOT_CHARGE_FLAG"].ToString().Trim() == "2" && tmmsm01_new["STOCK_PLACE_NO"].ToString().Trim().GetLength() == 0)
		{
			Log::Trace("", __FUNCTION__, "仓管");
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1K";
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER_DIV"] = "";
			bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_KIND"] = "SM";
			bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NO"] = tmmsm01_new["MAT_NO"];		// 材料号
			bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NUM"] = cd_mat_tube_new;		// 支数
			bcls_rec->Tables["WM00QUE"].Rows[0]["PLAN_NO"] = " ";					// 计划号
			bcls_rec->Tables["WM00QUE"].Rows[0]["PLAN_EXEC_SEQ_NO"] = 0;			// 计划顺序号
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_NO"] = tmmsm01["STOCK_NO"];		// 当前库区号
			bcls_rec->Tables["WM00QUE"].Rows[0]["TRANS_TOOL"] = " ";				// 运输工具
			bcls_rec->Tables["WM00QUE"].Rows[0]["UNIT_CODE"] = " ";					// 当前机组号
			bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_DESTION"] = " ";				// 去向
			bcls_rec->Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "I";					// 操作标记："I":新增;"D": 删除
			//doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);//2022-08-12 去头文件时编译报错 暂时注销
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
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
	
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 

