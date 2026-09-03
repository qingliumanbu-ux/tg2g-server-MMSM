/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      连铸炉次报告
Version:     1.0
Date:        2023-10-26
Description: 实绩接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 连铸炉次报告接收
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
#include "epex.h" 


//外部函数声明
int f_mmsm31_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_tableObjectCheck9999(ITableObject2& obj);	//字段超长检测


int f_mmsm_sj(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE_TELE(cm_e2t8sa_rcv)

int f_cm_e2t8sa_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_inq_code(conn);
	CDbCommand cmd_inq_test(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CModel tmmsm31("TMMSM31");
	CString heat_no = " ";
	CString dev_code = "";
	CString sm_plan_no = " ";

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		tmmsm31.MergeFrom(bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]);
		///工号
		dev_code = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["AGGREGATE_NAME"].ToString();
		//INT_MES_HEAT_REPORT.PLAN_NUMBER
		sm_plan_no = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["PLAN_NUMBER"].ToString();
		//熔炼号
		tmmsm31["HEAT_NO"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["HEAT_NUMBER"].ToString();
		tmmsm31["L2_PROC_NO"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["HEAT_NUMBER"].ToString();
		tmmsm31["ID_SJ"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["ID"].ToString();
		//同工位处理次数
		tmmsm31["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["TREATMENT_COUNTER"].ToString();

		Log::Trace("", "dev_code", "dev_code = {0}sm_plan_no =[{1}]", dev_code, sm_plan_no);
		//根据计划号获取熔炼号和制造命令号
		cmd_inq_code.SetCommandText(" SELECT  PONO, SM_PLAN_NO FROM "
			" (select HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 from TPSSM41 "
			" UNION "
			" SELECT HEAT_NO, PONO, SM_PLAN_NO, SM_PLAN_NOL2 FROM TPSSM11)WHERE SM_PLAN_NOL2 = '" + sm_plan_no + "'");
		cmd_inq_code.ExecuteReader();
		if (cmd_inq_code.Read())
		{
			tmmsm31["PONO"] = cmd_inq_code.GetString(1);
			tmmsm31["SM_PLAN_NO"] = cmd_inq_code.GetString(2);
		}
		cmd_inq_code.Close();

		cmd_inq.SetCommandText(" SELECT T1.PROC_NO,T2.STATION_ID,T2.STATION_NO FROM (  "
			" SELECT PROC_NO, DEV_CODE, DECODE(PRE_SOLUTION_FLAG, '1', '0', '1') C_DIV FROM TPSSM12 WHERE HEAT_NO = '" + tmmsm31["HEAT_NO"].ToString() + "' AND DEV_CODE = '" + dev_code + "' and TREATMENT_COUNTER ='" + tmmsm31["SAME_PROC_NUM"].ToString() + "' ) T1 LEFT JOIN TPSSMD1 T2  "
			" on t1.DEV_CODE = t2.DEV_CODE AND T1.C_DIV = T2.C_DIV");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm31["PROC_NO"] = cmd_inq.GetString(1);
			tmmsm31["STATION_ID"] = cmd_inq.GetString(2);
			tmmsm31["STATION_NO"] = cmd_inq.GetString(3);
		}
		Log::Trace("", "PROC_NO", "PROC_NO = {0}", tmmsm31["PROC_NO"].ToString());
		cmd_inq.Close();
		if (tmmsm31["PROC_NO"].ToString() == " "){
			tmmsm31["PROC_NO"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["HEAT_NUMBER"].ToString();
		}

		tmmsm31["DEV_CODE"] = dev_code;
		//计划号 
		tmmsm31["SM_PLAN_NOL2"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["PLAN_NUMBER"].ToString();
		//分包号
		tmmsm31["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["SPLIT_INDICATION"].ToString();
		//班组
		tmmsm31["PROD_SHIFT_GROUP"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["SHIFT_TEAM"].ToString();
		//操作工
		tmmsm31["ASSISTANT"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["OPERATOR_NAME"].ToString();

	
		
		//浇次第几炉
		tmmsm31["CAST_NO"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["HEAT_IN_CAST"].ToString();
		//浇注次数
		tmmsm31["CAST_DIV_NO"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["CAST_COUNTER"].ToString();
		//内部钢种
		tmmsm31["ST_NO"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["GRADE"].ToString();
		//铁水罐号
		tmmsm31["LADLE_NO"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_NUMBER"].ToString();
		//钢水罐温度
		tmmsm31["LADLE_TEMP"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_TEMP"].ToString();
		//最新钢包测温时间
		//INT_MES_HEAT_REPORT.LADLE_TIME  LADLE_TIME

		//钢水罐到达时间
		tmmsm31["LADLE_ARRIVE_TIME"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_ARRIVE_TIME"].ToString();
		//钢水罐打开时间
		tmmsm31["LADLE_OPEN_TIME"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_OPEN_TIME"].ToString();
		tmmsm31["START_TIME"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_OPEN_TIME"].ToString();
		//钢水罐离开时间
		tmmsm31["LADLE_LEAVE_TIME"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_DEPART_TIME"].ToString();
		tmmsm31["END_TIME"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_DEPART_TIME"].ToString();
		//最后切断时间
		tmmsm31["FIN_CUT_TIME"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LAST_SLAB_CUT_TIME"].ToString();
		//钢包到达重量
		if (bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_ARRIVE_NET_WEIGHT"].ToDecimal() != 0){
			tmmsm31["LADLE_ARRIVE_WT"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_ARRIVE_NET_WEIGHT"].ToDecimal() / 1000;
		}
		//钢包离开重量
		if (bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_DEPART_NET_WEIGHT"].ToDecimal() != 0){
			tmmsm31["LADLE_LEAVE_WT"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_DEPART_NET_WEIGHT"].ToDecimal() / 1000;
		}
		tmmsm31["STEEL_WT"] = tmmsm31["LADLE_ARRIVE_WT"].ToDecimal() - tmmsm31["LADLE_LEAVE_WT"].ToDecimal();
		//保温罩使用炉数
		//INT_MES_HEAT_REPORT.SHROUD_COUNTER SHROUD_COUNTER


		//中间包号1
		tmmsm31["TD_NO_1"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["TUNDISH_NUMBER_1"].ToString();
		//中间包号2
		tmmsm31["TD_NO_2"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["TUNDISH_NUMBER_2"].ToString();
		//1流结晶器号
		tmmsm31["MOLD_NO1"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["MOLD_NUMBER_1"].ToString();
		//2流结晶器号
		tmmsm31["MOLD_NO2"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["MOLD_NUMBER_2"].ToString();



		//LADLE_TIME  返回料装入时刻
		tmmsm31["LADLE_TIME"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_TIME"].ToString();
		//SHROUD_COUNTER
		//TUNDISH_POWDER_TYPE_1 
		 tmmsm31["TD_COVER_MAKER"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["TUNDISH_POWDER_TYPE_1"].ToString();
		//TUNDISH_POWDER_TYPE_2
		 tmmsm31["TD_COVER_MAKER1"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["TUNDISH_POWDER_TYPE_2"].ToString();
		//TUNDISH_POWDER_AMOUNT_1
		 if (bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["TUNDISH_POWDER_AMOUNT_1"].ToDecimal() != 0){
			 tmmsm31["TUNDISH_POWDER_AMOUNT_1"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["TUNDISH_POWDER_AMOUNT_1"].ToDecimal() / 1000;
		 }
		//TUNDISH_POWDER_AMOUNT_2
		 if (bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["TUNDISH_POWDER_AMOUNT_2"].ToDecimal() != 0){
			 tmmsm31["TUNDISH_POWDER_AMOUNT_2"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["TUNDISH_POWDER_AMOUNT_2"].ToDecimal() / 1000;
		 }
		//MOLD_POWDER_TYPE_CAST
		//MOLD_POWDER_TYPE_START
		//MOLD_POWDER_AMOUNT_CAST
		 if (bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["MOLD_POWDER_AMOUNT_CAST"].ToDecimal() != 0){
			 tmmsm31["MOLD_POWDER_AMOUNT_CAST"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["MOLD_POWDER_AMOUNT_CAST"].ToDecimal() / 1000;
		 }
		//MOLD_POWDER_AMOUNT_START
		 if (bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["MOLD_POWDER_AMOUNT_START"].ToDecimal() != 0){
			 tmmsm31["MOLD_POWDER_AMOUNT_START"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["MOLD_POWDER_AMOUNT_START"].ToDecimal() / 1000;
		 }


		//浸入式水口1
		tmmsm31["SEN_COUNTER_1"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["SEN_COUNTER_1"].ToString();
		//浸入式水口2
		tmmsm31["SEN_COUNTER_2"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["SEN_COUNTER_2"].ToString();
		//切断板坯块数
		tmmsm31["CUT_SLAB_NUM"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["SLABS_PRODUCED"].ToString();
		//钢包关闭时间
		tmmsm31["LADLE_CLOSE_TIME"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_CLOSE_TIME"].ToString();
		if (tmmsm31["END_TIME"].ToString() == "19000101000000"){
			tmmsm31["END_TIME"] = bcls_rec->Tables["INT_MES_HEAT_REPORT"].Rows[0]["LADLE_CLOSE_TIME"].ToString();
		}
		//tmmsm31["STATION_ID"] = "C";//设备类型

		tmmsm31["REC_CREATOR"] = s.userid;
		tmmsm31["REC_CREATE_TIME"] = datetime;


		bcls_rec->Tables.Clear();
		tmmsm31.MergeTo(bcls_rec->Tables.Add());
		//检测是否有字段超长
		f_tableObjectCheck9999(tmmsm31);
		/*if (tmmsm31["PROC_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "VOD处理号不能为空");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		if (!bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		}
		if (!bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
		}

		bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"] = "LG1";//厂别

		if (tmmsm31.QueryCount("L2_PROC_NO"))
		{
			Log::Trace("", "", "123456");
			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "U";//标记为修改
			doFlag = f_mmsm31_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else
		{
			Log::Trace("", "", "123");
			bcls_rec->Tables[0].Rows[0]["PROC_DIV"] = "I";//标记为新增
			doFlag = f_mmsm31_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				strcpy(s.msg, "调用函数报错!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		blkNum = bcls_rec->Tables.IndexOf("MMSM31");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM31");
		}

		if (!bcls_rec->Tables["MMSM31"].Columns.Contains("SM_PLAN_NOL2"))
		{
			bcls_rec->Tables["MMSM31"].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
		}
		bcls_rec->Tables["MMSM31"].Rows.Add();
		bcls_rec->Tables["MMSM31"].Rows[0]["SM_PLAN_NOL2"] = tmmsm31["SM_PLAN_NOL2"].ToString();
		//doFlag = f_mmsm_sj(bcls_rec, bcls_ret, conn);
		//sprintf(s.msg, "%d条记录新增成功！请重新查询！");
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

	return doFlag;
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


