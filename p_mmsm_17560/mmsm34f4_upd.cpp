/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2011-12-13
Version:1.0
Description: 炼钢板坯修磨实绩修改
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 

 
 

/* ***** 静态函数申明 ***** */

//修改板坯主档信息
int f_mmsm3401_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
 
/*<remark>=========================================================
/// <summary>
/// 炼钢板坯修磨实绩修改
/// <para>
/// 修改炼钢板坯修磨实绩
/// </para>
/// </summary>
/// <param name="tmmsm34">修改板坯修磨实绩信息</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm34f4_upd)

int f_mmsm34f4_upd(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
  	int  n_count = 0;
	CString cutFinFlag = "";
	int mat_seq  = 0;
	int mat_tube = 0;
	int fetchRowCount = 0;
 
	CModel tmmsm34("TMMSM34");
	CModel hmmsm34("TMMSM34");
	CModel tmmsm01("TMMSM01");
 
	CDbCommand cmd_sql(conn); //与DB 建立连接。
	try
	{
		
		tmmsm34.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//2024-03-11
		CDecimal beforeWeight = tmmsm34["MEND_BEFORE_WEIGHT"].ToDecimal();
		


		//20240401---
		CDecimal MEND_RATE = tmmsm34["MEND_RATE"].ToDecimal(); //设定修磨率
		CDecimal MEND_BEFORE_UPPER_TEMP = tmmsm34["MEND_BEFORE_UPPER_TEMP"].ToDecimal();//上表面磨前温度
		CDecimal MEND_BEFORE_BOTTOM_TEMP = tmmsm34["MEND_BEFORE_BOTTOM_TEMP"].ToDecimal();//下表面磨前温度
		CString MEND_BEFORE_QUALITY = tmmsm34["MEND_BEFORE_QUALITY"].ToString();//磨前表面质量
		CString MEND_AFTER_UPPER_TEMP = tmmsm34["MEND_AFTER_UPPER_TEMP"].ToString();//上表面磨后温度
		CString MEND_AFTER_BOTTOM_TEMP = tmmsm34["MEND_AFTER_BOTTOM_TEMP"].ToString();//下表面磨后温度

		CString MEND_AFTER_QUALITY = tmmsm34["MEND_AFTER_QUALITY"].ToString();//磨后表面质量
		CString GRINDING_END_TIME = tmmsm34["GRINDING_END_TIME"].ToString();//内弧结束
		CString GRINDING_OUTER_END_TIME = tmmsm34["GRINDING_OUTER_END_TIME"].ToString();//外弧结束

		CDecimal afterWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal();//磨后重量
		CDecimal secondWeight = tmmsm34["MEND_SECOND_WEIGHT"].ToDecimal();//再磨重量

		CString MEND_INNER_OPERATOR = tmmsm34["MEND_INNER_OPERATOR"].ToString();  //内弧操作者
		CString MEND_INNER_MODE = tmmsm34["MEND_INNER_MODE"].ToString();	 //内弧修磨方式
		CString MEND_INNER_MACHINE = tmmsm34["MEND_INNER_MACHINE"].ToString();  //内弧修磨机号
		CString MEND_OUTER_OPERATOR = tmmsm34["MEND_OUTER_OPERATOR"].ToString();  //外弧操作者
		CString MEND_OUTER_MODE = tmmsm34["MEND_OUTER_MODE"].ToString();  //外弧修磨方式
		CString MEND_OUTER_MACHINE = tmmsm34["MEND_OUTER_MACHINE"].ToString();  //外弧修磨机号
		CString mendSet = tmmsm34["MEND_SET"].ToString(); //内弧修磨机组
		CString mendOutSet = tmmsm34["MEND_OUTER_SET"].ToString(); //外弧修磨机组


		//2024-0407
		CString MEND_SLAG_HOPPER_MACHINE = tmmsm34["MEND_SLAG_HOPPER_MACHINE"].ToString();// 倒渣斗机组
		CString MEND_SLAG_HOPPER_OPERATOR = tmmsm34["MEND_SLAG_HOPPER_OPERATOR"].ToString();// 倒渣斗人员
		CString MEND_SLAG_HOPPER_WEIGHT = tmmsm34["MEND_SLAG_HOPPER_WEIGHT"].ToString();// 渣斗重量

		// 内弧
		CString WHEEL_TYPE_IN = tmmsm34["WHEEL_TYPE_IN"].ToString();
		CString	GRINDSTONE_SUPPLIER_IN = tmmsm34["GRINDSTONE_SUPPLIER_IN"].ToString();
		CString GRINDING_WHEEL_MACHINE = tmmsm34["GRINDING_WHEEL_MACHINE"].ToString();
		CString GRINDING_WHEEL_PEOPLE = tmmsm34["GRINDING_WHEEL_PEOPLE"].ToString();
		CString GRINDING_WHEEL_GRAININESS = tmmsm34["GRINDING_WHEEL_GRAININESS"].ToString();


		//外弧
		CString WHEEL_TYPE_OUT = tmmsm34["WHEEL_TYPE_OUT"].ToString();
		CString	GRINDING_WHEEL_OUTER_GRAININESS = tmmsm34["GRINDING_WHEEL_OUTER_GRAININESS"].ToString();
		CString GRINDING_WHEEL_OUTER_PEOPLE = tmmsm34["GRINDING_WHEEL_OUTER_PEOPLE"].ToString();
		CString GRINDING_WHEEL_OUTER_MACHINE = tmmsm34["GRINDING_WHEEL_OUTER_MACHINE"].ToString();
		CString GRINDSTONE_SUPPLIER_OUT = tmmsm34["GRINDSTONE_SUPPLIER_OUT"].ToString();


		CString REMARK = tmmsm34["REMARK"].ToString();
		
		tmmsm34.Query();
		CString flag = tmmsm34["MEND_FLAG"].ToString();
		Log::Trace("", "", "mendSet={0}", tmmsm34["MEND_FLAG"].ToString());
		//设置磨前重量2024-03-11
		tmmsm34["MEND_BEFORE_WEIGHT"] = beforeWeight;
		tmmsm34["REMARK"] = REMARK;
		tmmsm34["MEND_RATE"] = MEND_RATE;
		tmmsm34["MEND_SET"] = mendSet;
		tmmsm34["MEND_SLAG_HOPPER_MACHINE"] = MEND_SLAG_HOPPER_MACHINE;
		tmmsm34["MEND_SLAG_HOPPER_OPERATOR"] = MEND_SLAG_HOPPER_OPERATOR;
		tmmsm34["MEND_SLAG_HOPPER_WEIGHT"] = MEND_SLAG_HOPPER_WEIGHT;

		if (flag == "1"||flag=="3")
		{
			
			tmmsm34["MEND_OUTER_SET"] = mendOutSet;
			tmmsm34["MEND_BEFORE_UPPER_TEMP"] = MEND_BEFORE_UPPER_TEMP;
			tmmsm34["MEND_BEFORE_QUALITY"] = MEND_BEFORE_QUALITY;
			tmmsm34["MEND_AFTER_UPPER_TEMP"] = MEND_AFTER_UPPER_TEMP;
			tmmsm34["MEND_AFTER_QUALITY"] = MEND_AFTER_QUALITY;
			tmmsm34["GRINDING_END_TIME"] = GRINDING_END_TIME;
			tmmsm34["MEND_AFTER_WEIGHT"] = afterWeight;
			tmmsm34["MEND_SECOND_WEIGHT"] = secondWeight;
			tmmsm34["MEND_INNER_OPERATOR"] = MEND_INNER_OPERATOR;
			tmmsm34["MEND_INNER_MODE"] = MEND_INNER_MODE;
			tmmsm34["MEND_INNER_MACHINE"] = MEND_INNER_MACHINE;
			tmmsm34["MEND_SCRAP_WEIGHT"] = beforeWeight - afterWeight;

			tmmsm34["WHEEL_TYPE_IN"] = WHEEL_TYPE_IN;
			tmmsm34["GRINDSTONE_SUPPLIER_IN"] = GRINDSTONE_SUPPLIER_IN;
			tmmsm34["GRINDING_WHEEL_MACHINE"] = GRINDING_WHEEL_MACHINE;
			tmmsm34["GRINDING_WHEEL_PEOPLE"] = GRINDING_WHEEL_PEOPLE;
			tmmsm34["GRINDING_WHEEL_GRAININESS"] = GRINDING_WHEEL_GRAININESS;




		}
		else if (flag == "2"||flag=="4")
		{
			tmmsm34["MEND_OUTER_SET"] = mendOutSet;
			tmmsm34["MEND_BEFORE_BOTTOM_TEMP"] = MEND_BEFORE_BOTTOM_TEMP;
			tmmsm34["MEND_BEFORE_QUALITY"] = MEND_BEFORE_QUALITY;
			tmmsm34["MEND_AFTER_BOTTOM_TEMP"] = MEND_AFTER_BOTTOM_TEMP;
			tmmsm34["MEND_AFTER_QUALITY"] = MEND_AFTER_QUALITY;
			tmmsm34["GRINDING_OUTER_END_TIME"] = GRINDING_OUTER_END_TIME;
			tmmsm34["MEND_AFTER_WEIGHT"] = afterWeight;
			tmmsm34["MEND_SECOND_WEIGHT"] = secondWeight;
			tmmsm34["MEND_OUTER_OPERATOR"] = MEND_OUTER_OPERATOR;
			tmmsm34["MEND_OUTER_MODE"] = MEND_OUTER_MODE;
			tmmsm34["MEND_OUTER_MACHINE"] = MEND_OUTER_MACHINE;
			tmmsm34["MEND_SCRAP_WEIGHT"] = beforeWeight - afterWeight;

			tmmsm34["WHEEL_TYPE_OUT"] = WHEEL_TYPE_OUT;
			tmmsm34["GRINDING_WHEEL_OUTER_GRAININESS"] = GRINDING_WHEEL_OUTER_GRAININESS;
			tmmsm34["GRINDING_WHEEL_OUTER_PEOPLE"] = GRINDING_WHEEL_OUTER_PEOPLE;
			tmmsm34["GRINDING_WHEEL_OUTER_MACHINE"] = GRINDING_WHEEL_OUTER_MACHINE;
			tmmsm34["GRINDSTONE_SUPPLIER_OUT"] = GRINDSTONE_SUPPLIER_OUT;
			
		}

		double mendRate = tmmsm34["MEND_RATE"].ToDouble();
		CString stNo = tmmsm34["ST_NO"].ToString();
		CString strsql = " SELECT t.CODE_DESC_1_CONTENT FROM TWMSMZD02 t WHERE 1 =1 and t.CODE_CLASS = 'METALRATE'  and t.CODE = '" + stNo + "' ";
		CString content = "";
		double rate = 0.0;
		cmd_sql.SetCommandText(strsql);
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{
			content = cmd_sql.GetString(1);
			try
			{
				rate = CDecimal::Parse(content).ToDouble();
				Log::Trace("", "", "返回数据=[{0}]", rate);
				//金属去除速度
				CDecimal wt = ((afterWeight*(mendRate / 100) * 1000 * (18.3 / afterWeight) / 238 * (10.25 / rate))*afterWeight);
				//Log::Trace("", __FUNCTION__, "折算重量=[{0}]", wt);
				tmmsm34["MEND_WEIGHT"] = wt;
			}
			catch (CException& ex)
			{
				rate = 0.0;
			}

		}
		cmd_sql.Close();



		if (!bcls_rec->Tables.Contains("MMSM34"))
		{
			bcls_rec->Tables.Add("MMSM34");
		}
		if (!bcls_rec->Tables["MMSM34"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSM34"].Columns.Add(DT_STRING, "PROC_DIV");
		}

		tmmsm34.MergeTo(bcls_rec->Tables["MMSM34"], false);
		bcls_rec->Tables["MMSM34"].Rows[0]["PROC_DIV"] = "Edit";/*I:新增 U:修改 D:删除*/		

		doFlag = f_mmsm3401_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
