/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   石咏
Version:    1.0
Date:     2015-07-04
Description: 修磨实绩接收物料主档修改
***********************************************************************/


/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/

//#include "tmmsm01.h" 



#if defined(_SYS_PES)
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_210036_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_pssm81_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//发切废电文
int f_mmsm_210044_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

//2024-03-25
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号

int f_wmsm_t8p301_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2_FUNCTION_EXPORT
int f_mmsm3401_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int atFlag = 0;
	int ret = 0;
	int i;
	int blkNum = 0;
	int fetchRowCount;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_proc_div = "";
	int i_count = 0;
	CString ch_mach_clear_flag = "";
	CString sqlstr = "";
	CString v_factory_div = "";
	CString v_plan_no = "";
	CString v_plan_backlog_code = "";
	CString v_station_id = "";

	CModel tmmsm34("TMMSM34");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm39_1("TMMSM39_1");

	/*
		日期：2024-06-17
		原因：添加MM3J事件，更新材料履历
	*/
	CModel tmmsm963J("TMMSM96");

	//2024-1-31 定义tmmsm01,发送切废电文使用
	CModel tmmsm01("TMMSM01");


	//2024-03-11 定义tmmsm39，生成切废实绩
	CModel tmmsm39("TMMSM39");

	CString flag = "";

	//2024-03-08
	CString mendCancelFlag = "";

	CDbCommand cmd_sql(conn); //与DB 建立连接。

	//2024-02-21 定义一个
	CModel tmmsm34_1("TMMSM34_1");

	//2024-03-25 
	CModel tmmsm3e("TMMSM3E");

	CString v_resume_seq_no = "";//序号

	CModel tmmsm33dbsx("TMMSM33DBSX");

	EIClass tmp;
	tmp.Tables[0].set_TableName("MM0099");
	tmp.Tables[0].Columns.Add(tmmsm96);
	tmp.Tables[0].Rows.Clear();





	try
	{
		
		/*判断是否存在指定块*/
		if (bcls_rec->Tables.Contains("MMSM34") == false)
		{
			strcpy(s.msg, _RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			strcpy(s.sysmsg, "块MMSM34不存在。");
			throw CApplicationException(-1, s.msg, log.Location);
		}	
		if (bcls_rec->Tables.Contains("MM0099") == false)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}	

		/*如果是电文调用需要在电文接收service里对厂别和设备类型进行赋值*/
		if (bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables["PARA"].Columns.Contains("FACTORY_DIV"))  //厂别
			v_factory_div = bcls_rec->Tables["PARA"].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables["PARA"].Columns.Contains("STATION_ID"))  //设备类型
			v_station_id = bcls_rec->Tables["PARA"].Rows[0]["STATION_ID"].ToString().TrimOrBlank().ToUpper();	

		Log::Trace("", "", "Confrim={0}", v_proc_div);
		/*获得传入参数*/
		tmmsm34.MergeFrom(bcls_rec->Tables["MMSM34"].Rows[0]);	
		

		tmmsm34["FACTORY_DIV"] = v_factory_div;
		tmmsm34["STATION_ID"] = v_station_id;
		
		Log::Trace("", "", "第一步={0}", v_proc_div);
		//2024-03-25
		tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
		tmmsm01.Query();

		tmmsm34["REMARK_2"] = tmmsm01["C_DIV"];

		Log::Trace("", "", "第一步={0}", v_proc_div);
		tmmsm3e.CopyFrom(tmmsm01);
		doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;

		//添加产出时刻
		tmmsm3e["PROD_TIME"] = datetime;
		tmmsm3e["REMARK"] = "修磨处理等待反馈";


		// 2024-03-25 这里设置一下调用事件
		tmmsm96["EVENT_ID"] = "MM3F";
		tmmsm96["EVENT_LINE_TYPE"] = "SM";
		tmmsm96["FUNC_ID"] = "f_mmsm3401_proc";
		tmmsm96["SYSTEM_ID"] = "MMSM";
		tmmsm96["EVENT_DESC"] = "收货修磨切废分切操作标记更改";
		tmmsm96["MAT_NO"] = tmmsm34["MAT_NO"];		

		/*
			初始化：tmmsm963J
		*/
		tmmsm963J["EVENT_ID"] = "MM3J";
		tmmsm963J["EVENT_LINE_TYPE"] = "SM";
		tmmsm963J["FUNC_ID"] = "f_mmsm3401_proc";
		tmmsm963J["SYSTEM_ID"] = "MMSM";
		tmmsm963J["MAT_NO"] = tmmsm34["MAT_NO"];
		tmmsm963J["EVENT_DESC"] = "材料修磨事件描述";
	


		//统一判断初磨--设定修磨率
		if (v_proc_div == "INNER_1I"||v_proc_div=="OUTER_1U")
		{
			//2024-1-10 这里开始计算修磨率，(磨后重量-磨前重量)/磨前重量
			double beforeWeight = tmmsm34["MEND_BEFORE_WEIGHT"].ToDouble();
			double afterWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
			if (afterWeight > 0.0 && beforeWeight > 0.0){
		
				//tmmsm34["MEND_CALCULATE_RATE"] = ((beforeWeight - afterWeight) / beforeWeight) * 100;
				tmmsm34["MEND_SCRAP_WEIGHT"] = beforeWeight - afterWeight;
				//这里保留一下三位：
				tmmsm34["MEND_SCRAP_WEIGHT"] = tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal().Round(3);
				Log::Trace("", "", "LINE", __LINE__);
				//2024-3-25 计算折算重量
				//设定修磨率
				double mendRate = tmmsm34["MEND_RATE"].ToDouble();
				CString stNo = tmmsm34["ST_NO"].ToString();
				CString strsql = " SELECT t.CODE_DESC_1_CONTENT FROM TWMSMZD02 t WHERE 1 =1 and t.CODE_CLASS = 'METALRATE'  and t.CODE = '"+stNo+"' ";
				CString content = "";
				double rate = 0.0;
				cmd_sql.SetCommandText(strsql);
				cmd_sql.ExecuteReader();
				Log::Trace("", "", "LINE", __LINE__);
				if (cmd_sql.Read())
				{
					content = cmd_sql.GetString(1);
					try
					{
						rate = CDecimal::Parse(content).ToDouble();
						Log::Trace("", "", "返回数据=[{0}]", rate);
						//金属去除速度
						double wt = ((afterWeight*(mendRate / 100) * 1000 * (18.3 / afterWeight) / 238 * (10.25 / rate))*afterWeight);
						Log::Trace("", __FUNCTION__, "折算重量=[{0}]", wt);
						tmmsm34["MEND_WEIGHT"] = wt;
						//保留三位小数
						tmmsm34["MEND_WEIGHT"] = tmmsm34["MEND_WEIGHT"].ToDecimal().Round(3);
					}
					catch (CException& ex)
					{
						rate = 0.0;
					}

				}
				Log::Trace("", "", "LINE", __LINE__);
				cmd_sql.Close();
			}	
		}
		//内弧初磨
		if (v_proc_div == "INNER_1I")
		{		
			//这里判断一下，如果==""表示新增
			if (tmmsm34["PROD_SEQ_NO"].ToString() == " "){


				int count = tmmsm34.QueryCount("MAT_NO");
				if (count==0)
				{
					tmmsm34["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");;
					tmmsm34["MEND_FLAG"] = "1";//初磨		
					//这里设定一下内弧长度
					CString startString = tmmsm34["GRINDING_START_TIME"].ToString();
					CString endString = tmmsm34["GRINDING_END_TIME"].ToString();

					if (startString != " "&&endString != " ")
					{
						CDateTime start = CDateTime::Parse(startString);
						CDateTime end = CDateTime::Parse(endString);
						CTimeSpan span = end.Subtract(start);
						tmmsm34["MEND_INNER_TOTAL_TIME"] = span.TotalMinutes();
						tmmsm34["MEND_TOTAL_TIME"] = span.TotalMinutes();
					}
					tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
					//20240510----修磨添加机位
					tmmsm34["DEV_CODE"] = tmmsm01["DEV_CODE"];

					tmmsm34.Insert();

					tmmsm963J["EVENT_DESC"] = "新增初磨内弧记录，磨前重量：" + tmmsm34["MEND_BEFORE_WEIGHT"].ToString();

				}
				else
				{
					//Log::Trace("", "", "当前数据库数据返回数据=[{0}]", count);
					CFormattable arguments[] = { tmmsm34["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0}已经存在记录信息,请修改或者删除修磨记录信息", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				
				
				
			}
			//如果!=""表示修改
			else if (tmmsm34["PROD_SEQ_NO"].ToString()!= " ")
			{				
				/*if (tmmsm34["ISUPLOAD"].ToString()=="1")
				{
					CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料号[{0}]，已经上传了修磨实绩，不允许再修改", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				
				

				tmmsm34["MEND_FLAG"] = "1";//初磨		
				//这里设定一下内弧长度
				CString startString = tmmsm34["GRINDING_START_TIME"].ToString();
				CString endString = tmmsm34["GRINDING_END_TIME"].ToString();

				if (startString != " "&&endString != " ")
				{
					CDateTime start = CDateTime::Parse(startString);
					CDateTime end = CDateTime::Parse(endString);
					CTimeSpan span = end.Subtract(start);
					tmmsm34["MEND_INNER_TOTAL_TIME"] = span.TotalMinutes();
					tmmsm34["MEND_TOTAL_TIME"] = span.TotalMinutes();
				}



				tmmsm34.Delete();
				tmmsm34.Insert();

				tmmsm963J["EVENT_DESC"] = "修改初磨内弧记录，磨前重量：" + tmmsm34["MEND_BEFORE_WEIGHT"].ToString();
			}
			
			//2024-05-15
			tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
			//2024-03-25  			
			tmmsm96["MEND_FLAG"] = "1";
			tmmsm96["RCV_MAT_FLAG"] = "W";

		}
		//外弧初磨
		if (v_proc_div == "OUTER_1U")
		{
			/*if (tmmsm34["ISUPLOAD"].ToString() == "1")
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料号[{0}]，已经上传了修磨实绩，不允许再修改", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
			//这里内弧时间
			CString startInnerString = tmmsm34["GRINDING_START_TIME"].ToString();
			CString endInnerString = tmmsm34["GRINDING_END_TIME"].ToString();
			if (startInnerString != " "&&endInnerString!=" ")
			{
				CDateTime start = CDateTime::Parse(startInnerString);
				CDateTime end = CDateTime::Parse(endInnerString);
				CTimeSpan span = end.Subtract(start);
				tmmsm34["MEND_INNER_TOTAL_TIME"] = span.TotalMinutes();
			}
			Log::Trace("","", "LINE", __LINE__);
			//这里外弧时间长度
			CString startOuterString = tmmsm34["GRINDING_OUTER_START_TIME"].ToString();
			CString endOuterString = tmmsm34["GRINDING_OUTER_END_TIME"].ToString();
			if (startOuterString != " "&&endOuterString != " ")
			{
				CDateTime start = CDateTime::Parse(startOuterString);
				CDateTime end = CDateTime::Parse(endOuterString);

				CTimeSpan span = end.Subtract(start);
				tmmsm34["MEND_OUTER_TOTAL_TIME"] = span.TotalMinutes();

				CString t1 = tmmsm34["MEND_TOTAL_TIME"].ToString();
				if (t1!=" ")
				{
					CDecimal innerTotal = CDecimal::Parse(t1);
					CDecimal outerTotal = CDecimal::Parse(tmmsm34["MEND_OUTER_TOTAL_TIME"].ToString());
					tmmsm34["MEND_TOTAL_TIME"] = innerTotal + outerTotal;


				}
			}
			Log::Trace("", "", "LINE", __LINE__);
			CString sql = "  select t.MEND_INNER_MACHINE,t.MEND_INNER_OPERATOR from tmmsm34 t where t.MAT_NO = '" + tmmsm34["MAT_NO"].ToString() + "' and t.PROD_SEQ_NO = '" + tmmsm34["PROD_SEQ_NO"].ToString() + "'  ";
			CString MEND_INNER_MACHINE = "";
			CString MEND_INNER_OPERATOR = "";
			cmd_sql.SetCommandText(sql);
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				MEND_INNER_MACHINE = cmd_sql.GetString(1);
				MEND_INNER_OPERATOR = cmd_sql.GetString(2);
			}
			cmd_sql.Close();
			Log::Trace("", "", "MEND_INNER_MACHINE={0}", MEND_INNER_MACHINE);
			Log::Trace("", "", "MEND_INNER_OPERATOR={0}", MEND_INNER_OPERATOR);

			tmmsm34.Delete();
			tmmsm34["MEND_FLAG"] = "2";//初磨外弧
			tmmsm34["MEND_INNER_MODE"] = tmmsm34["MEND_INNER_MODE"];
			tmmsm34["MEND_INNER_MACHINE"] = MEND_INNER_MACHINE;
			tmmsm34["MEND_INNER_OPERATOR"] = MEND_INNER_OPERATOR;
			tmmsm34["GRINDING_START_TIME"] = tmmsm34["GRINDING_START_TIME"];
			tmmsm34["START_TIME"] = tmmsm34["START_TIME"];
			tmmsm34["GRINDING_END_TIME"] = tmmsm34["GRINDING_END_TIME"];
			tmmsm34["GRINDING_OUTER_START_TIME"] = tmmsm34["GRINDING_OUTER_START_TIME"];
			tmmsm34["GRINDING_OUTER_END_TIME"] = tmmsm34["GRINDING_OUTER_END_TIME"];
			tmmsm34["WHEEL_TYPE_OUT"] = tmmsm34["WHEEL_TYPE_OUT"];
			tmmsm34["GRINDSTONE_SUPPLIER_OUT"] = tmmsm34["GRINDSTONE_SUPPLIER_OUT"];
			tmmsm34["GRINDING_WHEEL_OUTER_CHANGE"] = tmmsm34["GRINDING_WHEEL_OUTER_CHANGE"];
			tmmsm34["GRINDING_WHEEL_OUTER_PEOPLE"] = tmmsm34["GRINDING_WHEEL_OUTER_PEOPLE"];
			//2024-05-10 添加机位
			
			tmmsm34["DEV_CODE"] = tmmsm01["DEV_CODE"];
			tmmsm34.Insert();	

			tmmsm963J["EVENT_DESC"] = "初磨外弧记录，磨前重量：" + tmmsm34["MEND_BEFORE_WEIGHT"].ToString();

			Log::Trace("", "", "LINE", __LINE__);
			//2024-03-25  
			tmmsm96["MEND_FLAG"] = "2";
			tmmsm96["RCV_MAT_FLAG"] = "W";
			//2024-05-15
			tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
		}

		//2024-03-19
		if (v_proc_div == "INNER_5I")
		{
			//这里设置一下“内弧砂轮更换机组”
			tmmsm34["GRINDING_WHEEL_MACHINE"] = tmmsm34["MEND_SET"];
			//这里设定一下时间长度
			CString startString = tmmsm34["GRINDING_START_TIME"].ToString();
			CString endString = tmmsm34["GRINDING_END_TIME"].ToString();
			if (startString != " "&&endString != " ")
			{
				CDateTime start = CDateTime::Parse(startString);
				CDateTime end = CDateTime::Parse(endString);
				CTimeSpan span = end.Subtract(start);
				tmmsm34["MEND_TOTAL_TIME"] = span.TotalMinutes();
			}

			tmmsm34["PROD_SEQ_NO"] = tmmsm34["REC_CREATE_TIME"];
			//2024-1-3
			tmmsm34["MEND_FLAG"] = "1";//初磨
			tmmsm34.Insert();			

			//2024-03-25  
			tmmsm96["MEND_FLAG"] = "1";
			tmmsm96["RCV_MAT_FLAG"] = "W";
		}

		//统一判断再磨--设定修磨率
		if (v_proc_div == "INNER_2I" || v_proc_div=="OUTER_2U")
		{
			//2024-1-10 这里开始计算修磨率，(磨后重量-磨前重量)/磨前重量
			double beforeWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
			double afterWeight = tmmsm34["MEND_SECOND_WEIGHT"].ToDouble();


			if (afterWeight>beforeWeight)
			{
				CFormattable arguments[] = { tmmsm34["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}填入的再磨重量大于磨后重量，请检查输入的再磨重量", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (afterWeight > 0.0 && beforeWeight > 0.0){
				//tmmsm34["MEND_CALCULATE_RATE"] = ((beforeWeight - afterWeight) / beforeWeight) * 100;
				tmmsm34["MEND_SCRAP_WEIGHT"] = beforeWeight - afterWeight;	

				//保留三位
				tmmsm34["MEND_SCRAP_WEIGHT"] = tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal().Round(3);

				//2024-3-25 计算折算重量
				//设定修磨率
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
						//金属去除速度
						double wt = ((afterWeight*(mendRate / 100) * 1000 * (18.3 / afterWeight) / 238 * (10.25 / rate))*afterWeight);			
						tmmsm34["MEND_WEIGHT"] = wt;
						//保留三位
						tmmsm34["MEND_WEIGHT"] = tmmsm34["MEND_WEIGHT"].ToDecimal().Round(3);
					}
					catch (CException& ex)
					{
						rate = 0.0;
					}

				}
				cmd_sql.Close();
			}	


		}
		//内弧再磨
		if (v_proc_div == "INNER_2I"){

			//这里判断一下：
			if (tmmsm34["PROD_SEQ_NO"].ToString() ==" ")
			{
				/*
					日期：2024-05-24
					原因：防止重复接管
					说明：由于再磨内弧（MEND_FLAG==3），再磨外弧(MEND_FLAG==4),
					只查标记为3会出现：已经再磨，但是到了再磨外弧的情况。导致重复接管
				*/
				
				tmmsm34["MEND_FLAG"] = "3";
				int countFlag3 = tmmsm34.QueryCount("MAT_NO,MEND_FLAG");
				Log::Trace("", "", "查材料号和只查标记3={0}", countFlag3);
				tmmsm34["MEND_FLAG"] = "4";
				int countFlag4 = tmmsm34.QueryCount("MAT_NO,MEND_FLAG");
				Log::Trace("", "", "查材料号和只查标记4={0}", countFlag4);				
				
				if (countFlag3 == 0 && countFlag4==0)
				{
					tmmsm34["PROD_SEQ_NO"] = tmmsm34["REC_CREATE_TIME"];
					//2024-1-3
					tmmsm34["MEND_FLAG"] = "3";//再磨内弧

					//这里设定一下内弧长度
					CString startString = tmmsm34["GRINDING_START_TIME"].ToString();
					CString endString = tmmsm34["GRINDING_END_TIME"].ToString();

					Log::Trace("", "", "startString={0}", startString);
					Log::Trace("", "", "endString={0}", endString);
					if (startString != " "&&endString != " ")
					{
						CDateTime start = CDateTime::Parse(startString);
						CDateTime end = CDateTime::Parse(endString);
						CTimeSpan span = end.Subtract(start);
						tmmsm34["MEND_INNER_TOTAL_TIME"] = span.TotalMinutes();
						tmmsm34["MEND_TOTAL_TIME"] = span.TotalMinutes();
					}

					tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
					tmmsm34.Insert();

					tmmsm963J["EVENT_DESC"] = "新增再磨内弧记录，磨后重量：" + tmmsm34["MEND_AFTER_WEIGHT"].ToString();
				}
				else
				{
					CFormattable arguments[] = { tmmsm34["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0}已经存在记录信息,请修改或者删除实绩信息", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				
			}
			else if (tmmsm34["PROD_SEQ_NO"].ToString() != " ")
			{

				/*if (tmmsm34["ISUPLOAD"].ToString() == "1")
				{
					CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料号[{0}]，已经上传了修磨实绩，不允许再修改", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				//2024-1-3
				tmmsm34["MEND_FLAG"] = "3";//再磨内弧

				//这里设定一下内弧长度
				CString startString = tmmsm34["GRINDING_START_TIME"].ToString();
				CString endString = tmmsm34["GRINDING_END_TIME"].ToString();

				Log::Trace("", "", "startString={0}", startString);
				Log::Trace("", "", "endString={0}", endString);
				if (startString != " "&&endString != " ")
				{
					CDateTime start = CDateTime::Parse(startString);
					CDateTime end = CDateTime::Parse(endString);
					CTimeSpan span = end.Subtract(start);
					tmmsm34["MEND_INNER_TOTAL_TIME"] = span.TotalMinutes();
					tmmsm34["MEND_TOTAL_TIME"] = span.TotalMinutes();
				}
				tmmsm34.Delete();
				tmmsm34.Insert();
				tmmsm963J["EVENT_DESC"] = "修改再磨内弧记录，磨后重量：" + tmmsm34["MEND_AFTER_WEIGHT"].ToString();
			}
					


			//2024-03-25  
			tmmsm96["MEND_FLAG"] = "3";
			tmmsm96["RCV_MAT_FLAG"] = "W";
			//2024-05-15
			tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
		}
		//外弧再磨
		if (v_proc_div == "OUTER_2U"){
			//先删除
			tmmsm34.Delete();

			//这里内弧时间
			CString startInnerString = tmmsm34["GRINDING_START_TIME"].ToString();
			CString endInnerString = tmmsm34["GRINDING_END_TIME"].ToString();
			if (startInnerString != " "&&endInnerString != " ")
			{
				CDateTime start = CDateTime::Parse(startInnerString);
				CDateTime end = CDateTime::Parse(endInnerString);
				CTimeSpan span = end.Subtract(start);
				tmmsm34["MEND_INNER_TOTAL_TIME"] = span.TotalMinutes();
			}

			CString startOuterString = tmmsm34["GRINDING_OUTER_START_TIME"].ToString();
			CString endOuterString = tmmsm34["GRINDING_OUTER_END_TIME"].ToString();
			Log::Trace("", "", "startOuterString={0}", startOuterString);
			Log::Trace("", "", "endOuterString={0}", endOuterString);
			if (startOuterString != " "&&endOuterString != " ")
			{
				CDateTime start = CDateTime::Parse(startOuterString);
				CDateTime end = CDateTime::Parse(endOuterString);

				CTimeSpan span = end.Subtract(start);
				tmmsm34["MEND_OUTER_TOTAL_TIME"] = span.TotalMinutes();

				CString t1 = tmmsm34["MEND_TOTAL_TIME"].ToString();
				if (t1 != " ")
				{
					CDecimal innerTotal = CDecimal::Parse(t1);
					CDecimal outerTotal = CDecimal::Parse(tmmsm34["MEND_OUTER_TOTAL_TIME"].ToString());
					tmmsm34["MEND_TOTAL_TIME"] = innerTotal + outerTotal;


				}
			}

			tmmsm34["MEND_FLAG"] = "4";//初磨外弧
			tmmsm34.Insert();
			tmmsm963J["EVENT_DESC"] = "再磨外弧记录，磨后重量：" + tmmsm34["MEND_AFTER_WEIGHT"].ToString();

			//2024-03-25  
			tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
			tmmsm96["MEND_FLAG"] = "4";
			tmmsm96["RCV_MAT_FLAG"] = "W";
		}		
		if (v_proc_div == "SEND")
		{
			Log::Trace("", "", "开始上传", "");
			tmmsm34_1.MergeFrom(bcls_rec->Tables["MMSM34"].Rows[0]);
			tmmsm01["MAT_NO"] = tmmsm34_1["MAT_NO"];
			tmmsm01.Query();
			//region
			tmmsm34_1["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm34_1["MEND_BEFORE_WEIGHT"] = tmmsm01["MAT_ACT_WT"].ToDecimal().Round(3);
			tmmsm34_1["MEND_AFTER_WEIGHT"] = tmmsm01["MAT_ACT_WT"].ToDecimal().Round(3);
			tmmsm34_1["GRINDING_START_TIME"] = tmmsm34["GRINDING_START_TIME"];
			tmmsm34_1["GRINDING_END_TIME"] = tmmsm34["GRINDING_END_TIME"];
			tmmsm34_1["MEND_FLAG"] = "5";
			tmmsm34_1["MEND_CALCULATE_RATE"] = "0";
			tmmsm34_1["HEAT_NO"] = tmmsm01["HEAT_NO"];
			tmmsm34_1["MAT_NO"] = tmmsm01["MAT_NO"];
			tmmsm34_1["MAT_ACT_THICK"] = tmmsm01["MAT_ACT_THICK"];
			tmmsm34_1["MAT_ACT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
			tmmsm34_1["MAT_ACT_LEN"] = tmmsm01["MAT_ACT_LEN"];
			tmmsm34_1["MAT_ACT_WT"] = tmmsm01["MAT_ACT_WT"];
			tmmsm34_1["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"];
			tmmsm34_1.Insert();			


			//2024-03-25  
			tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"];
			tmmsm96["MEND_FLAG"] = "5";
			tmmsm96["RCV_MAT_FLAG"] = "W";
		}
		//修磨实绩删除
		if (v_proc_div == "D")
		{
			flag = tmmsm34["MEND_FLAG"].ToString();	
			tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
			tmmsm34_1["PROD_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
			//表示删除
			tmmsm34_1["ISUPLOAD"] = "3";
			tmmsm34_1.Update("MAT_NO,PROD_SEQ_NO,ISUPLOAD");

			Log::Trace("", "", "{0}", "开始上传修磨实绩");
			if (flag == "1" || flag == "2"||flag=="5")
			{
				EIClass bcls_rec_210036;
				bcls_rec_210036.Tables[0].set_TableName("210036");
				bcls_rec_210036.Tables[0].Columns.Add(tmmsm34);
				bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
				bcls_rec_210036.Tables[0].Rows.Add();

				bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34);
				bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
				bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "D";

				if (bcls_rec_210036.Tables[0].Rows.get_Count()>0 && tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal() != 0)
				{
					doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
					tmmsm96["RCV_MAT_FLAG"] = "W";
					tmmsm96["MEND_FLAG"] = flag;
					tmmsm96["MEND_FEEDBACK_FLAG"] = 1;
					tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
					tmmsm3e.Insert();
				}
			}

		}		
		
		//修磨记录删除 --- // 20240413
		if (v_proc_div == "DELRECORD"){
			
			tmmsm34.Delete();
			tmmsm33dbsx["MAT_NO"] = tmmsm34["MAT_NO"];
			tmmsm33dbsx["EVENT_ID"] = "MM12";
			tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
			if (tmmsm33dbsx.QueryCount("MAT_NO,EVENT_ID,RESUME_SEQ_NO")>0)
			{
				tmmsm33dbsx.Delete("MAT_NO,EVENT_ID,RESUME_SEQ_NO");
			}
			if (tmmsm34["MEND_FLAG"].ToString()=="1"){
				tmmsm96["MEND_FLAG"] = "0";
				tmmsm96["RCV_MAT_FLAG"] = "W";
			}
			else if (tmmsm34["MEND_FLAG"].ToString() == "3")
			{
				tmmsm96["MEND_FLAG"] = "2";
				tmmsm96["RCV_MAT_FLAG"] = "W";
			}
		}

		//F7-修改
		if (v_proc_div == "EDIT")
		{			
			//2024-1-10 这里开始计算修磨率，(磨后重量-磨前重量)/磨前重量
			flag = tmmsm34["MEND_FLAG"].ToString();
			mendCancelFlag = tmmsm34["MEND_CANCEL_FLAG"].ToString();
			if (mendCancelFlag=="1")
			{
				double beforeWeight = tmmsm34["MEND_BEFORE_WEIGHT"].ToDouble();
				double afterWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
				if (afterWeight > 0.0)
				{
					tmmsm34["MEND_CALCULATE_RATE"] = ((beforeWeight - afterWeight) / beforeWeight) * 100;
				}
				tmmsm34["MEND_CANCEL_FLAG"] = "0";
				tmmsm34.Delete();
				tmmsm34.Insert();
			}
			else
			{

				
				//这里设定一下内弧时间长度
				CString startString = tmmsm34["GRINDING_START_TIME"].ToString();
				CString endString = tmmsm34["GRINDING_END_TIME"].ToString();

				if (startString != " "&&endString != " ")
				{
					CDateTime start = CDateTime::Parse(startString);
					CDateTime end = CDateTime::Parse(endString);
					CTimeSpan span = end.Subtract(start);
					tmmsm34["MEND_INNER_TOTAL_TIME"] = span.TotalMinutes();
					tmmsm34["MEND_TOTAL_TIME"] = span.TotalMinutes();
				}

				//这里设定一下外弧时间长度

				//这里外弧时间长度
				CString startOuterString = tmmsm34["GRINDING_OUTER_START_TIME"].ToString();
				CString endOuterString = tmmsm34["GRINDING_OUTER_END_TIME"].ToString();
				if (startOuterString != " "&&endOuterString != " ")
				{
					CDateTime start = CDateTime::Parse(startOuterString);
					CDateTime end = CDateTime::Parse(endOuterString);

					CTimeSpan span = end.Subtract(start);
					tmmsm34["MEND_OUTER_TOTAL_TIME"] = span.TotalMinutes();

					CString t1 = tmmsm34["MEND_TOTAL_TIME"].ToString();
					if (t1 != " ")
					{
						CDecimal innerTotal = CDecimal::Parse(t1);
						CDecimal outerTotal = CDecimal::Parse(tmmsm34["MEND_OUTER_TOTAL_TIME"].ToString());
						tmmsm34["MEND_TOTAL_TIME"] = innerTotal + outerTotal;
					}
				}

				if (tmmsm34["MEND_FLAG"].ToString() == "1" || tmmsm34["MEND_FLAG"].ToString() == "2")
				{
					double beforeWeight = tmmsm34["MEND_BEFORE_WEIGHT"].ToDouble();
					double afterWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
					if (afterWeight > 0.0 && beforeWeight > 0.0){
						tmmsm34["MEND_CALCULATE_RATE"] = ((beforeWeight - afterWeight) / beforeWeight) * 100;						
					}
					tmmsm34.Delete();
					tmmsm34.Insert();		
				}
				else if (tmmsm34["MEND_FLAG"].ToString() == "3" || tmmsm34["MEND_FLAG"].ToString() == "4")
				{
					//2024-1-10 这里开始计算修磨率，(磨后重量-磨前重量)/磨前重量
					double beforeWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
					double afterWeight = tmmsm34["MEND_SECOND_WEIGHT"].ToDouble();		
					if (afterWeight > 0.0 && beforeWeight > 0.0){
						tmmsm34["MEND_CALCULATE_RATE"] = ((beforeWeight - afterWeight) / beforeWeight) * 100;
						tmmsm34["MEND_SCRAP_WEIGHT"] = beforeWeight - afterWeight;
					}					
					tmmsm34.Delete();
					tmmsm34.Insert();
				}
			}
			tmmsm96["MEND_FLAG"] = tmmsm34["MEND_FLAG"];
			tmmsm96["RCV_MAT_FLAG"] = "W";
		}
	
		//F8-确认
		if (v_proc_div == "CONFIRM")
		{			
			tmmsm34.Query();
			CString mendFlag = tmmsm34["MEND_FLAG"].ToString();
			//表示操作人员手工确认
			tmmsm34["ISCONFIRM"] = 1;
			if (mendFlag=="1"||mendFlag=="2")
			{
				tmmsm34["ISUPLOAD"] = 1;//表示发：210036电文--修磨实绩电文
			}
			else if (mendFlag == "3" || mendFlag == "4")
			{
				tmmsm34["ISUPLOAD"] = 2;//表示发：210044电文--切废电文
			}
			tmmsm34.Update("ISCONFIRM,ISUPLOAD");
			if (mendFlag == "1" || mendFlag=="2")
			{
				Log::Trace("", "", "{0}", "开始上传修磨实绩");
				EIClass bcls_rec_210036;
				bcls_rec_210036.Tables[0].set_TableName("210036");
				bcls_rec_210036.Tables[0].Columns.Add(tmmsm34);
				bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
				bcls_rec_210036.Tables[0].Rows.Add();

				bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34);
				bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
				bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "N";				

				if (bcls_rec_210036.Tables[0].Rows.get_Count()>0 && v_proc_div != "D" && tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal() != 0)
				{
					doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//插入实绩表
					Log::Trace("", "", "开始上传", "");
					tmmsm34_1.MergeFrom(bcls_rec->Tables["MMSM34"].Rows[0]);
					tmmsm01["MAT_NO"] = tmmsm34_1["MAT_NO"];
					tmmsm01.Query();
					//region
					tmmsm34_1["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tmmsm34_1["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
					tmmsm34_1["MEND_AFTER_QUALITY"] = tmmsm34["MEND_AFTER_QUALITY"];
					
					tmmsm34_1["MEND_AFTER_WEIGHT"] = tmmsm34["MEND_AFTER_WEIGHT"];
					tmmsm34_1["GRINDING_START_TIME"] = tmmsm34["GRINDING_START_TIME"];
					tmmsm34_1["GRINDING_END_TIME"] = tmmsm34["GRINDING_END_TIME"];
					tmmsm34_1["MEND_FLAG"] = mendFlag;
					tmmsm34_1["MEND_CALCULATE_RATE"] = tmmsm34["MEND_CALCULATE_RATE"];
					tmmsm34_1["HEAT_NO"] = tmmsm34["HEAT_NO"];
					tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
					tmmsm34_1["PROD_SHIFT_NO"] = tmmsm34["PROD_SHIFT_NO"];
					tmmsm34_1["PROD_SHIFT_GROUP"] = tmmsm34["PROD_SHIFT_GROUP"]; 
					tmmsm34_1["BATCH"] = tmmsm34["BATCH"];
					tmmsm34_1["PROC_NO"] = tmmsm34["PROC_NO"]; 
					tmmsm34_1["ST_NO"] = tmmsm34["ST_NO"];
					tmmsm34_1["MEND_OUTER_MODE"] = tmmsm34["MEND_OUTER_MODE"];
					tmmsm34_1["MEND_INNER_MODE"] = tmmsm34["MEND_INNER_MODE"];
					tmmsm34_1["REMARK"] = tmmsm34["REMARK"];
					tmmsm34_1["MEND_SCRAP_WEIGHT"] = tmmsm34["MEND_SCRAP_WEIGHT"];
					tmmsm34_1["GRINDSTONE_SUPPLIER_OUT"] = tmmsm34["GRINDSTONE_SUPPLIER_OUT"];
					tmmsm34_1["GRINDING_WHEEL_OUTER_GRAININESS"] = tmmsm34["GRINDING_WHEEL_OUTER_GRAININESS"];

					tmmsm34_1["GRINDSTONE_SUPPLIER_IN"] = tmmsm34["GRINDSTONE_SUPPLIER_IN"];
					tmmsm34_1["GRINDSTONE_SUPPLIER_OUT"] = tmmsm34["GRINDSTONE_SUPPLIER_OUT"];
					
					
					tmmsm34_1["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"];
					tmmsm34_1["MAT_ACT_THICK"] = tmmsm01["MAT_ACT_THICK"];
					tmmsm34_1["MAT_ACT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
					tmmsm34_1["MAT_ACT_LEN"] = tmmsm01["MAT_ACT_LEN"];
					tmmsm34_1["MAT_ACT_WT"] = tmmsm34["MEND_AFTER_WEIGHT"];
					tmmsm34_1["FORE_IP"] = s.fore_ip;
					tmmsm34_1.Insert();

					//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
					tmmsm96["RCV_MAT_FLAG"] = "W";
					tmmsm96["MEND_FLAG"] = tmmsm34_1["MEND_FLAG"];
					tmmsm96["MEND_FEEDBACK_FLAG"] = 1;
					tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
					tmmsm3e.Insert();
				}
				Log::Trace("", "", "记录数", bcls_rec->Tables["MM0099"].Rows.get_Count());
				if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
				{
					bcls_rec->Tables["MM0099"].Rows.Add();
				}
				bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
				if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
				{
					//发送电文的时候，再调用 99 函数
					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					else
					{

					}
				}
			}
			if (mendFlag == "3" || mendFlag == "4")
			{
				Log::Trace("", "", "{0}", "开始上传切废实绩");
				tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
				tmmsm01.Query();
				EIClass bcls_rec_210044;
				bcls_rec_210044.Tables[0].set_TableName("210044");
				bcls_rec_210044.Tables[0].Columns.Add(tmmsm39);
				bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");				
				bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");


				bcls_rec_210044.Tables[0].Rows.Add();
				bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm39);
				bcls_rec_210044.Tables[0].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
				
				bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "N";

				Log::Trace("", "", "重量={0}", tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal());
				if (bcls_rec_210044.Tables[0].Rows.get_Count()>0 && tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal() != 0)
				{

					tmmsm39.CopyFrom(tmmsm01);
					tmmsm39["CUT_BEFORE_WT"] = tmmsm34["MEND_AFTER_WEIGHT"];
					tmmsm39["CUT_AFTER_WT"] = tmmsm34["MEND_SECOND_WEIGHT"];
					tmmsm39["CUT_SCRAP_WT"] = tmmsm34["MEND_SCRAP_WEIGHT"];

					tmmsm39["CUT_BEFORE_LEN"] = tmmsm34["MAT_ACT_LEN"];
					tmmsm39["CUT_BEFORE_THICK"] = tmmsm39["MAT_ACT_THICK"];
					tmmsm39["CUT_BEFORE_WIDTH"] = tmmsm39["MAT_ACT_WIDTH"];

					tmmsm39["CUT_AFTER_LEN"] = tmmsm34["MAT_ACT_LEN"];
					tmmsm39["CUT_AFTER_THICK"] = tmmsm39["MAT_ACT_THICK"];
					tmmsm39["CUT_AFTER_WIDTH"] = tmmsm39["MAT_ACT_WIDTH"];
				
					tmmsm39["CUTTING_TYPE"] = "20";
					tmmsm39["FINISH_FLAG"] = "1";
					tmmsm39["RECUT_DATE"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tmmsm39["RESUME_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tmmsm39["GRINDING_FLAG"] = "1";
					tmmsm39.Insert();

					tmmsm39_1.CopyFrom(tmmsm39);
					tmmsm39_1.Insert();

					//2024-03-19
					//插入实绩表
					tmmsm34_1.MergeFrom(bcls_rec->Tables["MMSM34"].Rows[0]);
					//region

					tmmsm34_1["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tmmsm34_1["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
					tmmsm34_1["MEND_AFTER_QUALITY"] = tmmsm34["MEND_AFTER_QUALITY"];
					tmmsm34_1["MEND_AFTER_WEIGHT"] = tmmsm34["MEND_AFTER_WEIGHT"];
					tmmsm34_1["MEND_SECOND_WEIGHT"] = tmmsm34["MEND_SECOND_WEIGHT"];
					tmmsm34_1["GRINDING_START_TIME"] = tmmsm34["GRINDING_START_TIME"];
					tmmsm34_1["GRINDING_END_TIME"] = tmmsm34["GRINDING_END_TIME"];
					tmmsm34_1["MEND_FLAG"] = mendFlag;
					tmmsm34_1["MEND_CALCULATE_RATE"] = tmmsm34["MEND_CALCULATE_RATE"];
					tmmsm34_1["PROD_SHIFT_NO"] = tmmsm34["PROD_SHIFT_NO"];
					tmmsm34_1["PROD_SHIFT_GROUP"] = tmmsm34["PROD_SHIFT_GROUP"];
					tmmsm34_1["HEAT_NO"] = tmmsm34["HEAT_NO"];
					tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
					tmmsm34_1["MAT_ACT_THICK"] = tmmsm34["MAT_ACT_THICK"];
					tmmsm34_1["MAT_ACT_WIDTH"] = tmmsm34["MAT_ACT_WIDTH"];
					tmmsm34_1["MAT_ACT_LEN"] = tmmsm34["MAT_ACT_LEN"];
					tmmsm34_1["MAT_ACT_WT"] = tmmsm34["MAT_ACT_WT"];
					tmmsm34_1["BATCH"] = tmmsm34["BATCH"];
					tmmsm34_1["PROC_NO"] = tmmsm34["PROC_NO"];
					tmmsm34_1["ST_NO"] = tmmsm34["ST_NO"];
					tmmsm34_1["MEND_OUTER_MODE"] = tmmsm34["MEND_OUTER_MODE"];
					tmmsm34_1["MEND_INNER_MODE"] = tmmsm34["MEND_INNER_MODE"];
					tmmsm34_1["REMARK"] = tmmsm34["REMARK"];
					tmmsm34_1["MEND_SCRAP_WEIGHT"] = tmmsm34["MEND_SCRAP_WEIGHT"];
					tmmsm34_1["GRINDSTONE_SUPPLIER_OUT"] = tmmsm34["GRINDSTONE_SUPPLIER_OUT"];
					tmmsm34_1["GRINDING_WHEEL_OUTER_GRAININESS"] = tmmsm34["GRINDING_WHEEL_OUTER_GRAININESS"];

					tmmsm34_1["GRINDSTONE_SUPPLIER_IN"] = tmmsm34["GRINDSTONE_SUPPLIER_IN"];
					tmmsm34_1["GRINDSTONE_SUPPLIER_OUT"] = tmmsm34["GRINDSTONE_SUPPLIER_OUT"];
					tmmsm34_1["FORE_IP"] = s.fore_ip;
					tmmsm34_1.Insert();
					//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
					tmmsm96["MEND_FLAG"] = tmmsm34_1["MEND_FLAG"];
					tmmsm96["RCV_MAT_FLAG"] = "W";
					tmmsm96["MEND_FEEDBACK_FLAG"] = 1;
					tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
					tmmsm3e.Insert();
					bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm39);				
					bcls_rec_210044.Tables[0].Rows[0]["PROC_DIV"] = "MMSM34";
					doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}					
				}
			}
			
			
		}

		//F9-修磨取消
		if (v_proc_div == "CANCEL")
		{
			//通过Mat_No获取所有的修磨实绩
			CString matNo = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			CString prodSeqNo = "";
			CDecimal mendBeforeWeight = 0.0;
			tmmsm34["MAT_NO"] = matNo;
			CDbCommand cmd_sql(conn); 
			sqlstr = "SELECT t.PROD_SEQ_NO,t.MAT_NO,t.MEND_FLAG,t.MEND_BEFORE_WEIGHT,t.MEND_AFTER_WEIGHT,t.MEND_SECOND_WEIGHT  "
				" FROM    tmmsm34 t "
				" WHERE t.MAT_NO = @mat_no";
			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("mat_no", matNo);
			cmd_sql.ExecuteReader();
			while (cmd_sql.Read())
			{
				prodSeqNo = cmd_sql.GetString(1);
				mendBeforeWeight = cmd_sql.GetDecimal(4);
				tmmsm34["PROD_SEQ_NO"] = prodSeqNo;
				tmmsm34.Query();				
				CString cancelFlag = tmmsm34["MEND_FLAG"].ToString();
				if (cancelFlag == "1" || cancelFlag=="2")
				{
					//修磨标记为：初磨|并且存在磨后重量，需要发送删除电文 发210036
					if (tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal() > 0)
					{
						EIClass bcls_rec_210036;
						bcls_rec_210036.Tables[0].set_TableName("210036");
						bcls_rec_210036.Tables[0].Columns.Add(tmmsm34);
						bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
						bcls_rec_210036.Tables[0].Rows.Add();

						bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34);
						bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
						bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "D";

						if (bcls_rec_210036.Tables[0].Rows.get_Count()>0 && tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal() != 0)
						{
							doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
					}
				}
				else if (cancelFlag == "3" || cancelFlag == "4")
				{
					//修磨标记为：再磨|并且存在再磨重量，需要发送删除电文 发210044
					if (tmmsm34["MEND_SECOND_WEIGHT"].ToDecimal() > 0)
					{
						Log::Trace("", "", "发送电文={0}", "删除电文--再磨");
						tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
						tmmsm01.Query();
						EIClass bcls_rec_210044;
						bcls_rec_210044.Tables[0].set_TableName("210044");
						bcls_rec_210044.Tables[0].Columns.Add(tmmsm01);
						bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
						bcls_rec_210044.Tables[0].Columns.Add(DT_DECIMAL, "CUT_SCRAP_WT");
						bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");


						bcls_rec_210044.Tables[0].Rows.Add();
						bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm01);
						bcls_rec_210044.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
						bcls_rec_210044.Tables[0].Rows[0]["PROC_DIV"] = "MMSM34";
						bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "D";
						Log::Trace("", "", "重量={0}", tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal());
						if (bcls_rec_210044.Tables[0].Rows.get_Count()>0 && tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal() != 0)
						{
							bcls_rec_210044.Tables[0].Rows[0]["CUT_SCRAP_WT"] = tmmsm34["MEND_SCRAP_WEIGHT"];
							doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
					}
				}

			}
			cmd_sql.Close();

			//修改主档表
			tmmsm01["MAT_NO"] = matNo;
			tmmsm01.Query();
			//修改修磨实绩
			tmmsm34["MAT_NO"] = matNo;
			tmmsm34["MEND_CANCEL_FLAG"] = 1;
			//修改修磨--状态，统一设置成：
			int n = tmmsm34.Update("MEND_CANCEL_FLAG", "MAT_NO");


			//2024-03-25  
			tmmsm96["MEND_FLAG"] = "0";
			tmmsm96["RCV_MAT_FLAG"] = "W";
		}
		//2024-03-18 --- 新增修磨实绩
		if (v_proc_div == "INS_ACHIEVEMENT")
		{

			/*
				日期：2024-06-17
				原因：新增实绩操作的时候 ，材料已经出库归档 -- A040229807，为了防止此问题出现，进行强制校验
			*/
			tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
			int isOnLine = tmmsm01.QueryCount("MAT_NO");
			//表示归档==0
			if (isOnLine == 0)
			{
				Log::Trace("", "", "归档数据={0}", isOnLine);
			}
			else
			{
				Log::Trace("", "", "材料未归档，开始写入实绩表={0}", isOnLine);
				Log::Trace("", "", "{0}", "开始查询相关信息");
				//2024-1-10 这里开始计算修磨率，(磨后重量-磨前重量)/磨前重量
				double beforeWeight = tmmsm34["MEND_BEFORE_WEIGHT"].ToDouble();
				double afterWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
				if (afterWeight > 0.0 && beforeWeight > 0.0){
					tmmsm34["MEND_CALCULATE_RATE"] = ((beforeWeight - afterWeight) / beforeWeight) * 100;

					tmmsm34["MEND_CALCULATE_RATE"] = tmmsm34["MEND_CALCULATE_RATE"].ToDecimal().Round(3);
					tmmsm34["MEND_SCRAP_WEIGHT"] = beforeWeight - afterWeight;

					tmmsm34["MEND_SCRAP_WEIGHT"] = tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal().Round(3);


				}
				//如果存在磨后重量
				tmmsm34["MEND_FLAG"] = 5;
				tmmsm34_1.CopyFrom(tmmsm34);
				tmmsm34_1["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

				/*
					20240701--幂等处理
				*/
				int tmmsm341Count = tmmsm34_1.QueryCount("MAT_NO");

				if (tmmsm341Count>0)
				{
					Log::Trace("", "", "实绩表已经存在，条数为：={0}", tmmsm341Count);
				}
				else if (tmmsm341Count==0)
				{
					Log::Trace("", "", "实绩表不存在此材料，条数为：={0}", tmmsm341Count);
					tmmsm34_1.Insert();

					tmmsm963J["EVENT_DESC"] = "新增板坯修磨实绩，磨前重量：" + tmmsm34["MEND_BEFORE_WEIGHT"].ToString();

					//2024-03-25  
					//2024-03-25  
					tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"];
					tmmsm96["MEND_FLAG"] = "5";
					//tmmsm96["RCV_MAT_FLAG"] = "W";

					//----------------------------------------发送电文-----------------------------------------//
					if (tmmsm01["HOLD_FLAG"].ToString() == "0")
					{
						EIClass bcls_rec_210036;
						bcls_rec_210036.Tables[0].set_TableName("210036");
						bcls_rec_210036.Tables[0].Columns.Add(tmmsm34_1);
						bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
						bcls_rec_210036.Tables[0].Rows.Add();

						bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34_1);
						bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34_1["MAT_NO"];
						bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "N";

						if (bcls_rec_210036.Tables[0].Rows.get_Count()>0 && v_proc_div != "D" && tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal() != 0)
						{
							doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							/*
							tmp.Tables["MM0099"].Rows.Add();
							tmp.Tables["MM0099"].Rows[0]["SIZE_DECIDE_CODE"] = "1001";
							tmp.Tables["MM0099"].Rows[0]["SLAB_CHECK_RESULT"] = "1001";
							if (!tmp.Tables["MM0099"].Columns.Contains("SURFACE_DECIDE_CODE"))
							tmp.Tables["MM0099"].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");
							tmp.Tables["MM0099"].Rows[0]["SURFACE_DECIDE_CODE"] = "1";
							tmp.Tables["MM0099"].Rows[0]["EVENT_ID"] = "QM20";//修改板坯上的最终出钢记号
							tmp.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
							tmp.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "QMTS";
							tmp.Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
							tmp.Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm34_1["MAT_NO"];
							*/
							//2024-03-25 更新事件
							//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
							tmmsm96["RCV_MAT_FLAG"] = "W";
							tmmsm96["MEND_FEEDBACK_FLAG"] = 1;
							tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
							tmmsm3e.Insert();
						}

						if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
						{
							bcls_rec->Tables["MM0099"].Rows.Add();
						}
						bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
						if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
						{
							doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							else
							{

							}
						}
						/*
						日期：2024-05-22
						原因：防止多次上传，需要修改一下tmmsm34
						*/
						//tmmsm34["MAT_NO"]
						//int count = tmmsm01.QueryCount("MAT_NO,MEND_FLAG");
					}
					else
					{

						tmmsm33dbsx["MAT_NO"] = tmmsm34_1["MAT_NO"];
						tmmsm33dbsx["EVENT_ID"] = "MM12";
						tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm34_1["PROD_SEQ_NO"];
						tmmsm33dbsx["SEQ_NO"] = tmmsm33dbsx.QueryCount("MAT_NO") + 1;
						/*
						日期:20240516
						原因：初磨 内弧和外弧，都有磨后量的时候，都会往待办事项里面写入数据，导致待办事项处理的时候，出现多次上传210044
						*/
						int count = tmmsm33dbsx.QueryCount("MAT_NO,EVENT_ID,RESUME_SEQ_NO");
						if (count > 0)
						{
							Log::Trace("", "", "查询结果={0}", count);
						}
						else
						{
							tmmsm33dbsx.Insert();
						}
					}
				}

			}
		

			
		}
		//2024-03-18 --- 修改修磨实绩
		if (v_proc_div == "EDIT_ACHIEVEMENT")
		{
			tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
			tmmsm34_1["PROD_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
			tmmsm34_1.Query();

			Log::Trace("", "", "tmmsm34_1={0}", tmmsm34_1["MEND_BEFORE_WEIGHT"].ToDecimal());
			Log::Trace("", "", "tmmsm34_1={0}", tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal());

			Log::Trace("", "", "tmmsm34MEND_AFTER_WEIGHT={0}", tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal());
			Log::Trace("", "", "tmmsm01MAT_ACT_WT={0}", tmmsm01["MAT_ACT_WT"].ToDecimal());
			//2026.06.15 判断磨后重量和当前重量是否一致
			if (tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal() == tmmsm01["MAT_ACT_WT"].ToDecimal())
			{

				//2024-1-10 这里开始计算修磨率，(磨后重量-磨前重量)/磨前重量
				double beforeWeight = tmmsm34["MEND_BEFORE_WEIGHT"].ToDouble();
				double afterWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
				if (afterWeight > 0.0 && beforeWeight > 0.0){
					tmmsm34_1["MEND_CALCULATE_RATE"] = ((beforeWeight - afterWeight) / beforeWeight) * 100;

					tmmsm34_1["MEND_CALCULATE_RATE"] = tmmsm34_1["MEND_CALCULATE_RATE"].ToDecimal().Round(3);

					tmmsm34_1["MEND_SCRAP_WEIGHT"] = beforeWeight - afterWeight;
					tmmsm34_1["MEND_SCRAP_WEIGHT"] = tmmsm34_1["MEND_SCRAP_WEIGHT"].ToDecimal().Round(3);

					tmmsm34_1["MEND_AFTER_WEIGHT"] = afterWeight;
					tmmsm34_1["MEND_AFTER_WEIGHT"] = tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal().Round(3);

					tmmsm34_1["MEND_BEFORE_WEIGHT"] = beforeWeight;
					tmmsm34_1["MEND_BEFORE_WEIGHT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"].ToDecimal().Round(3);
				}
				//1---初磨发送标记；2---再磨发生标记；3----表示删除；4----表示修改

				tmmsm34_1["ISUPLOAD"] = "4";
				Log::Trace("", "", "get_Count={0}", "到这里了");
				tmmsm34_1.Update("MEND_CALCULATE_RATE,MEND_SCRAP_WEIGHT,MEND_AFTER_WEIGHT,MEND_BEFORE_WEIGHT,ISUPLOAD");
				//tmmsm34_1.Delete();
				//tmmsm34_1.Insert();

				//2024-03-25  
				//2024-03-25  
				tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"];
				tmmsm96["MEND_FLAG"] = tmmsm34_1["MEND_FLAG"];
				tmmsm96["RCV_MAT_FLAG"] = "W";
				Log::Trace("", "", "get_Count={0}", "这一步操作完了");
				tmmsm963J["EVENT_DESC"] = "修改板坯修磨实绩，磨前重量：" + tmmsm34["MEND_BEFORE_WEIGHT"].ToString();
			}
			else
			{
				CFormattable arguments[] = { tmmsm34["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}磨后重量和当前重量不一致，不能修磨。", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		// 2024-03-25 修完操作完毕，更新了修磨状态，这里需要更新一下TMMSM01表的修磨状态	

		//false说明：初磨再磨只是生成记录，不会在发送电文，发送电文通过：“F8确认”按钮
		//if (false){
			//如果是初磨有磨后重量：发210036，修磨实绩电文

		//}


		if (v_proc_div == "ADMIN_EDIT")
		{
			Log::Trace("", "", "标记={0}", "管理员修改");
			Log::Trace("", "", "材料号={0}", tmmsm34["MAT_NO"].ToString());
			Log::Trace("", "", "PROD_SEQ_NO={0}", tmmsm34["PROD_SEQ_NO"].ToString());

			Log::Trace("", "", "修磨机组={0}", tmmsm34["MEND_SET"].ToString());
			Log::Trace("", "", "修磨折算率={0}", tmmsm34["MEND_WEIGHT"].ToString());
			Log::Trace("", "", "设定修磨率={0}", tmmsm34["MEND_RATE"].ToString());

			Log::Trace("", "", "磨前重量={0}", tmmsm34["MEND_BEFORE_WEIGHT"].ToString());
			Log::Trace("", "", "磨后重量={0}", tmmsm34["MEND_AFTER_WEIGHT"].ToString());
			Log::Trace("", "", "修磨率={0}", tmmsm34["MEND_CALCULATE_RATE"].ToString());
			Log::Trace("", "", "修磨班组={0}", tmmsm34["MEND_SHIFT"].ToString());
			
			tmmsm34.Update("MEND_SET,MEND_WEIGHT,MEND_RATE,MEND_BEFORE_WEIGHT,MEND_AFTER_WEIGHT,MEND_CALCULATE_RATE,MEND_SHIFT,MEND_SECOND_WEIGHT", "MAT_NO,PROD_SEQ_NO");

			
		}

		if (v_proc_div == "ADMIN_DELETE")
		{
			Log::Trace("", "", "标记={0}", "管理员删除");
			Log::Trace("", "", "材料号={0}", tmmsm34["MAT_NO"].ToString());
			Log::Trace("", "", "PROD_SEQ_NO={0}", tmmsm34["PROD_SEQ_NO"].ToString());
			tmmsm34.Delete();
		}

		/*
			原因：调用MM函数，记录材料修磨履历
		*/

		if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
		{
			bcls_rec->Tables["MM0099"].Rows.Add();
		}
		bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm963J);
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{
			//发送电文的时候，再调用 99 函数
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{

			}
		}

		//--------------------------------  发送电文  --------------------------------
		
	
		/*
			日期：2024-05-16
			原因：如果材料已经归档，发送电文会报错。==0表示：归档；>0表示
		*/
		tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
		int sendCount = tmmsm01.QueryCount("MAT_NO");
		//表示归档==0
		if (sendCount==0)
		{
			Log::Trace("", "", "归档数据={0}", sendCount);
		}
		else
		{
			if (v_proc_div == "INNER_1I" || v_proc_div == "OUTER_1U")
			{
				//发送电文的时候需要判断一下
				/*
				    日期：20240518
					原因：发送电文的时候，需要卡一下综判是否合格，如果不合格，写入待办事项
					条件：未封锁&&综判合格
				*/
				Log::Trace("", "", "封锁标记={0}", tmmsm01["HOLD_FLAG"].ToString());
				Log::Trace("", "", "综判标记={0}", tmmsm01["COMPLEX_DECIDE_CODE"].ToString());

				/*
					说明：对于有磨后量的修磨记录。
					如果满足以下条件：发送修磨实绩
					1、实绩表（tmmsm34_1）不存在数据，未发送电文
					2、封锁标记HOLD_FLAG==0(未封锁)   
					3、综判标记[COMPLEX_DECIDE_CODE]==1（综判合格）
					4、收货标记[RCV_MAT_FLAG]==S(已经收货)
					5、存在磨后重量
				*/

				tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
				int checkCount = tmmsm34_1.QueryCount("MAT_NO");
				if (checkCount>0)
				{
					Log::Trace("", "", "上传修磨实绩={0}", checkCount);
					Log::Trace("", "", "已经上传了修磨实绩，这里只保存","");
					/*CFormattable arguments[] = { tmmsm34["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0},已经上传修磨实绩，不能再上传修磨实绩", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);*/
				}
				else{
					if (tmmsm01["HOLD_FLAG"].ToString().Trim() == "0"&&
						tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() == "1"&&
						tmmsm01["RCV_MAT_FLAG"].ToString().Trim() == "S")
					{
						double afterWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
						if (afterWeight > 0)
						{
							//现在判断一下，如果是有实际，就不需要上传了 ----2024-03-19
							tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
							int count = tmmsm34_1.QueryCount("MAT_NO");
							if (count > 0)
							{
								Log::Trace("", "", "实绩条数={0}", count);
							}
							else
							{

								/**
									日期：2024-05-27
									原因：对于做了未收货板坯，以MAT_WT为磨前量，但是收货以后，这个MAT_ACT_WT（系统重量）可能和MAT_WT（实时重量）不一致，
									这就导致给产销上传的磨前量有问题，需要再发送电文的时候，强制以三级系统重量（MAT_ACT_WT）为准
								**/
								Log::Trace("", "", "34磨前量={0}", tmmsm34["MEND_BEFORE_WEIGHT"].ToDouble());
								Log::Trace("", "", "01磨前量={0}", tmmsm01["MAT_ACT_WT"].ToDouble());

								tmmsm34["MEND_BEFORE_WEIGHT"] = tmmsm01["MAT_ACT_WT"];

								tmmsm34["ISUPLOAD"] = 1;//表示发：210036电文--修磨实绩电文
								tmmsm34.Update("ISUPLOAD");
								EIClass bcls_rec_210036;
								bcls_rec_210036.Tables[0].set_TableName("210036");
								bcls_rec_210036.Tables[0].Columns.Add(tmmsm34);
								bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
								bcls_rec_210036.Tables[0].Rows.Add();

								bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34);
								bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
								bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "N";

								if (bcls_rec_210036.Tables[0].Rows.get_Count()>0 && v_proc_div != "D" && tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal() != 0)
								{
									doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
									if (doFlag < 0)
									{
										throw CApplicationException(-1, s.msg, log.Location);
									}
									//插入实绩表								
									tmmsm34_1.MergeFrom(bcls_rec->Tables["MMSM34"].Rows[0]);
									tmmsm01["MAT_NO"] = tmmsm34_1["MAT_NO"];
									tmmsm01.Query();
									/*
									日期：2024-05-16
									原因：为了保证实绩表【TMMSM34_1】和【TMMSM34】数据主键一致
									tmmsm34_1["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
									*/
									tmmsm34_1["PROD_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
									tmmsm34_1["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
									tmmsm34_1["MEND_AFTER_QUALITY"] = tmmsm34["MEND_AFTER_QUALITY"];

									tmmsm34_1["MEND_AFTER_WEIGHT"] = tmmsm34["MEND_AFTER_WEIGHT"];
									tmmsm34_1["GRINDING_START_TIME"] = tmmsm34["GRINDING_START_TIME"];
									tmmsm34_1["GRINDING_END_TIME"] = tmmsm34["GRINDING_END_TIME"];

									tmmsm34_1["MEND_FLAG"] = tmmsm34["MEND_FLAG"];
									tmmsm34_1["MEND_CALCULATE_RATE"] = tmmsm34["MEND_CALCULATE_RATE"];
									tmmsm34_1["HEAT_NO"] = tmmsm34["HEAT_NO"];
									tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
									tmmsm34_1["PROD_SHIFT_NO"] = tmmsm34["PROD_SHIFT_NO"];
									tmmsm34_1["PROD_SHIFT_GROUP"] = tmmsm34["PROD_SHIFT_GROUP"];
									tmmsm34_1["BATCH"] = tmmsm34["BATCH"];
									tmmsm34_1["PROC_NO"] = tmmsm34["PROC_NO"];
									tmmsm34_1["ST_NO"] = tmmsm34["ST_NO"];
									tmmsm34_1["MEND_OUTER_MODE"] = tmmsm34["MEND_OUTER_MODE"];
									tmmsm34_1["MEND_INNER_MODE"] = tmmsm34["MEND_INNER_MODE"];
									tmmsm34_1["REMARK"] = tmmsm34["REMARK"];
									tmmsm34_1["MEND_SCRAP_WEIGHT"] = tmmsm34["MEND_SCRAP_WEIGHT"];
									tmmsm34_1["GRINDSTONE_SUPPLIER_OUT"] = tmmsm34["GRINDSTONE_SUPPLIER_OUT"];
									tmmsm34_1["GRINDING_WHEEL_OUTER_GRAININESS"] = tmmsm34["GRINDING_WHEEL_OUTER_GRAININESS"];

									tmmsm34_1["GRINDSTONE_SUPPLIER_IN"] = tmmsm34["GRINDSTONE_SUPPLIER_IN"];
									tmmsm34_1["GRINDSTONE_SUPPLIER_OUT"] = tmmsm34["GRINDSTONE_SUPPLIER_OUT"];


									tmmsm34_1["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"];
									tmmsm34_1["MAT_ACT_THICK"] = tmmsm01["MAT_ACT_THICK"];
									tmmsm34_1["MAT_ACT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
									tmmsm34_1["MAT_ACT_LEN"] = tmmsm01["MAT_ACT_LEN"];
									tmmsm34_1["MAT_ACT_WT"] = tmmsm34["MEND_AFTER_WEIGHT"];
									tmmsm34_1["FORE_IP"] = s.fore_ip;

									tmmsm34_1.Insert();

									//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
									tmmsm96["RCV_MAT_FLAG"] = "W";
									tmmsm96["MEND_FEEDBACK_FLAG"] = 1;
									tmmsm96["MEND_FLAG"] = tmmsm34["MEND_FLAG"];
									tmmsm3e["RECEIVE_BACK_STATUS"] = "W";

									tmmsm3e.Insert();
								}

								if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
								{
									bcls_rec->Tables["MM0099"].Rows.Add();
								}
								bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
								if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
								{
									//发送电文的时候，再调用 99 函数
									doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
									if (doFlag < 0)
									{
										throw CApplicationException(-1, s.msg, log.Location);
									}
									else
									{

									}
								}
								/*
								日期：2024-05-21
								原因：对于已经发送修磨实绩的，需要增加2250板坯【板坯信息电文】
								**/

								tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
								tmmsm01.Query();
								EIClass inblock;
								inblock.Tables[0].Columns.Add(tmmsm01);
								inblock.Tables[0].Rows.Clear();
								tmmsm01.MergeTo(inblock.Tables[0], false);

								doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
								if (doFlag < 0) {
									throw CApplicationException(-1, s.msg, s.svc_name);
								}
								Log::Trace("", "", "f_wmsm_t8p301_snd={0}", "发送2250板坯信息成功");
							}
						}
						else
						{
							Log::Trace("", "", "get_Count={0}", "没有磨后重量，不发送电文");
						}
					}
					else
					{
						Log::Trace("", "", "封锁标记={0} 综判代码[{1}] 磨后量[{2}] 材料号[{3}]", tmmsm01["HOLD_FLAG"].ToString().Trim(), tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim(), tmmsm34["MEND_AFTER_WEIGHT"].ToDouble(), tmmsm34["MAT_NO"].ToString().Trim());
						//现在判断一下，如果是有磨后量，就不需要上传了 ----2024-03-19
						double afterWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
						if (afterWeight>0)
						{
							/*
							日期：2024-05-21
							原因：对于不满足发送实绩的修磨记录，存入待办事项时
							1、封锁标记(!=0)、综判(!=1)存入MM12事件
							*/
							if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() == "S" &&
								(tmmsm01["HOLD_FLAG"].ToString().Trim() != "0" ||
								tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != "1"))
							{
								/*
								日期：2024-05-21
								原因：先判断待办事项是否存在，如果存在不保存，如果不存在保存
								*/
								tmmsm33dbsx["MAT_NO"] = tmmsm34["MAT_NO"];
								tmmsm33dbsx["EVENT_ID"] = "MM12";
								tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
								tmmsm33dbsx["SEQ_NO"] = tmmsm33dbsx.QueryCount("MAT_NO") + 1;
								tmmsm33dbsx["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
								Log::Trace("", "", "封锁标记={0} 综判代码[{1}] 磨后量[{2}] 材料号[{3}]", tmmsm01["HOLD_FLAG"].ToString().Trim(), tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim(), tmmsm34["MEND_AFTER_WEIGHT"].ToDouble(), tmmsm34["MAT_NO"].ToString().Trim());
								/*
								日期:20240516
								原因：初磨 内弧和外弧，都有磨后量的时候，都会往待办事项里面写入数据，导致待办事项处理的时候，出现多次上传210036
								*/
								int count = tmmsm33dbsx.QueryCount("MAT_NO,EVENT_ID,RESUME_SEQ_NO");
								if (count > 0)
								{
									Log::Trace("", "", "查询结果={0}", count);
								}
								else
								{
									tmmsm33dbsx.Insert();
								}
							}
							/*
							日期：2024-05-21
							原因：对于不满足发送实绩的修磨记录，存入待办事项时
							1、未收货[RCV_MAT_FLAG!=S]存入MM13事件
							*/
							else if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
							{
								/*
								日期：2024-05-21
								原因：先判断待办事项是否存在，如果存在不保存，如果不存在保存
								*/
								tmmsm33dbsx["MAT_NO"] = tmmsm34["MAT_NO"];
								tmmsm33dbsx["EVENT_ID"] = "MM13";
								tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
								tmmsm33dbsx["SEQ_NO"] = tmmsm33dbsx.QueryCount("MAT_NO") + 1;
								tmmsm33dbsx["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
								Log::Trace("", "", "封锁标记={0} 综判代码[{1}] 磨后量[{2}] 材料号[{3}]", tmmsm01["HOLD_FLAG"].ToString().Trim(), tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim(), tmmsm34["MEND_AFTER_WEIGHT"].ToDouble(), tmmsm34["MAT_NO"].ToString().Trim());
								/*
								日期:20240516
								原因：初磨 内弧和外弧，都有磨后量的时候，都会往待办事项里面写入数据，导致待办事项处理的时候，出现多次上传210036
								*/
								int count = tmmsm33dbsx.QueryCount("MAT_NO,EVENT_ID,RESUME_SEQ_NO");
								if (count > 0)
								{
									Log::Trace("", "", "查询结果={0}", count);
								}
								else
								{
									tmmsm33dbsx.Insert();
								}
							} else
							{
								Log::Trace("", "", "封锁标记={0} 综判代码[{1}] 磨后量[{2}] 材料号[{3}]", tmmsm01["HOLD_FLAG"].ToString().Trim(), tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim(), tmmsm34["MEND_AFTER_WEIGHT"].ToDouble(), tmmsm34["MAT_NO"].ToString().Trim());
							}


						}
					}
				}


			}
			//如果是再磨有再磨重量：发210044，切废电文
			else if (v_proc_div == "INNER_2I" || v_proc_div == "OUTER_2U")
			{
				//发送电文的时候需要判断一下
				/*
					日期：20240518
					原因：发送电文的时候，需要卡一下综判是否合格，如果不合格，写入待办事项
					条件：未封锁&&综判合格
				*/
				/*
					日期：20260109
					原因：原有逻辑判断，如果出现先切废，后再磨，则不发切废实绩
					条件：新的判断是修磨记录里有已发的再磨记录则跳过
				*/
				tmmsm39_1["MAT_NO"] = tmmsm34["MAT_NO"];
				tmmsm34["ISUPLOAD"] = 2;
				//int checkCount = tmmsm39_1.QueryCount("MAT_NO");
				int checkCount = tmmsm34.QueryCount("MAT_NO,ISUPLOAD");
				if (checkCount > 0)
				{
					Log::Trace("", "", "上传修磨实绩={0}", checkCount);
					Log::Trace("", "", "已经上传了修磨实绩，这里只保存", "");
				}
				else{
					if (tmmsm01["HOLD_FLAG"].ToString() == "0"&&tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() == "1")
				{
					double secondWeight = tmmsm34["MEND_SECOND_WEIGHT"].ToDouble();
					if (secondWeight > 0)
					{
						//现在判断一下，如果是有实际，就不需要上传了 ----2024-03-19
						tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
						int count = tmmsm34_1.QueryCount("MAT_NO");
						if (count > 1)
						{

						}
						else
						{
							tmmsm34["ISUPLOAD"] = 2;//表示发：210044电文--切废电文
							tmmsm34.Update("ISUPLOAD");

							Log::Trace("", "", "{0}", "开始上传切废实绩");
							tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
							tmmsm01.Query();
							EIClass bcls_rec_210044;
							bcls_rec_210044.Tables[0].set_TableName("210044");
							bcls_rec_210044.Tables[0].Columns.Add(tmmsm39);
							bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
							bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");


							bcls_rec_210044.Tables[0].Rows.Add();
							bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm39);
							bcls_rec_210044.Tables[0].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];

							bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "N";


							if (bcls_rec_210044.Tables[0].Rows.get_Count()>0 && tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal() != 0)
							{

								tmmsm39.CopyFrom(tmmsm01);
								tmmsm39["CUT_BEFORE_WT"] = tmmsm34["MEND_AFTER_WEIGHT"];
								tmmsm39["CUT_AFTER_WT"] = tmmsm34["MEND_SECOND_WEIGHT"];
								tmmsm39["CUT_SCRAP_WT"] = tmmsm34["MEND_SCRAP_WEIGHT"];

								tmmsm39["CUT_BEFORE_LEN"] = tmmsm34["MAT_ACT_LEN"];
								tmmsm39["CUT_BEFORE_THICK"] = tmmsm39["MAT_ACT_THICK"];
								tmmsm39["CUT_BEFORE_WIDTH"] = tmmsm39["MAT_ACT_WIDTH"];

								tmmsm39["CUT_AFTER_LEN"] = tmmsm34["MAT_ACT_LEN"];
								tmmsm39["CUT_AFTER_THICK"] = tmmsm39["MAT_ACT_THICK"];
								tmmsm39["CUT_AFTER_WIDTH"] = tmmsm39["MAT_ACT_WIDTH"];

								tmmsm39["CUTTING_TYPE"] = "20";
								tmmsm39["FINISH_FLAG"] = "1";

								tmmsm39["RECUT_DATE"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
								tmmsm39["RESUME_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
								tmmsm39["GRINDING_FLAG"] = "1";
								tmmsm39.Insert();

								tmmsm39_1.CopyFrom(tmmsm39);
								tmmsm39_1.Insert();

								//2024-03-19
								//插入实绩表
								tmmsm34_1.MergeFrom(bcls_rec->Tables["MMSM34"].Rows[0]);
								//region
								/*
								日期：2024-05-16
								原因：为了保证实绩表【TMMSM34_1】和【TMMSM34】数据主键一致
								tmmsm34_1["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
								*/
								tmmsm34_1["PROD_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
								tmmsm34_1["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
								tmmsm34_1["MEND_AFTER_QUALITY"] = tmmsm34["MEND_AFTER_QUALITY"];
								tmmsm34_1["MEND_AFTER_WEIGHT"] = tmmsm34["MEND_AFTER_WEIGHT"];
								tmmsm34_1["MEND_SECOND_WEIGHT"] = tmmsm34["MEND_SECOND_WEIGHT"];
								tmmsm34_1["GRINDING_START_TIME"] = tmmsm34["GRINDING_START_TIME"];
								tmmsm34_1["GRINDING_END_TIME"] = tmmsm34["GRINDING_END_TIME"];
								tmmsm34_1["MEND_FLAG"] = tmmsm34["MEND_FLAG"];
								tmmsm34_1["MEND_CALCULATE_RATE"] = tmmsm34["MEND_CALCULATE_RATE"];
								tmmsm34_1["PROD_SHIFT_NO"] = tmmsm34["PROD_SHIFT_NO"];
								tmmsm34_1["PROD_SHIFT_GROUP"] = tmmsm34["PROD_SHIFT_GROUP"];
								tmmsm34_1["HEAT_NO"] = tmmsm34["HEAT_NO"];
								tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
								tmmsm34_1["MAT_ACT_THICK"] = tmmsm34["MAT_ACT_THICK"];
								tmmsm34_1["MAT_ACT_WIDTH"] = tmmsm34["MAT_ACT_WIDTH"];
								tmmsm34_1["MAT_ACT_LEN"] = tmmsm34["MAT_ACT_LEN"];
								tmmsm34_1["MAT_ACT_WT"] = tmmsm34["MAT_ACT_WT"];
								tmmsm34_1["BATCH"] = tmmsm34["BATCH"];
								tmmsm34_1["PROC_NO"] = tmmsm34["PROC_NO"];
								tmmsm34_1["ST_NO"] = tmmsm34["ST_NO"];
								tmmsm34_1["MEND_OUTER_MODE"] = tmmsm34["MEND_OUTER_MODE"];
								tmmsm34_1["MEND_INNER_MODE"] = tmmsm34["MEND_INNER_MODE"];
								tmmsm34_1["REMARK"] = tmmsm34["REMARK"];
								tmmsm34_1["MEND_SCRAP_WEIGHT"] = tmmsm34["MEND_SCRAP_WEIGHT"];
								tmmsm34_1["GRINDSTONE_SUPPLIER_OUT"] = tmmsm34["GRINDSTONE_SUPPLIER_OUT"];
								tmmsm34_1["GRINDING_WHEEL_OUTER_GRAININESS"] = tmmsm34["GRINDING_WHEEL_OUTER_GRAININESS"];

								tmmsm34_1["GRINDSTONE_SUPPLIER_IN"] = tmmsm34["GRINDSTONE_SUPPLIER_IN"];
								tmmsm34_1["GRINDSTONE_SUPPLIER_OUT"] = tmmsm34["GRINDSTONE_SUPPLIER_OUT"];
								tmmsm34_1["FORE_IP"] = s.fore_ip;
								//tmmsm34_1.Insert();
								//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
								tmmsm96["MEND_FLAG"] = tmmsm34_1["MEND_FLAG"];
								tmmsm96["RCV_MAT_FLAG"] = "W";
								tmmsm96["MEND_FEEDBACK_FLAG"] = 1;

								tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
								tmmsm3e.Insert();
								bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm39);
								bcls_rec_210044.Tables[0].Rows[0]["PROC_DIV"] = "MMSM34";
								doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
								if (doFlag < 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}

								Log::Trace("", "", "发送成功", tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal());
								if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
								{
									bcls_rec->Tables["MM0099"].Rows.Add();
								}
								bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
								if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
								{
									doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
									if (doFlag < 0)
									{
										throw CApplicationException(-1, s.msg, log.Location);
									}
									else
									{

									}
								}
							}
						}
					}
					else
					{
						Log::Trace("", "", "get_Count={0}", "没有再磨重量，不发送电文");
					}
				}
					else
				{
					Log::Trace("", "", "HOLD_FLAG={0}", tmmsm01["HOLD_FLAG"].ToString());
					Log::Trace("", "", "COMPLEX_DECIDE_CODE={0}", tmmsm01["COMPLEX_DECIDE_CODE"].ToString());
					double secondWeight = tmmsm34["MEND_SECOND_WEIGHT"].ToDouble();
					if (secondWeight>0 && tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() == "1")
					{
						tmmsm33dbsx["MAT_NO"] = tmmsm34["MAT_NO"];
						tmmsm33dbsx["EVENT_ID"] = "MM12";
						tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
						tmmsm33dbsx["SEQ_NO"] = tmmsm33dbsx.QueryCount("MAT_NO") + 1;
						/*
						日期:20240516
						原因：初磨 内弧和外弧，都有磨后量的时候，都会往待办事项里面写入数据，导致待办事项处理的时候，出现多次上传210044
						*/
						int count = tmmsm33dbsx.QueryCount("MAT_NO,EVENT_ID,RESUME_SEQ_NO");
						if (count > 0)
						{
							Log::Trace("", "", "查询结果={0}", count);
						}
						else
						{
							tmmsm33dbsx.Insert();
						}
					}

				}
				}


			}
			//2024-02-19
			if (false && v_proc_div == "D")
			{

				if (tmmsm01["HOLD_FLAG"].ToString() == "0")
				{

					//2024-02-19 初磨撤销发送电文：发210036，修磨实绩电文
					if (flag == "1" || flag == "2")
					{
						EIClass bcls_rec_210036;
						bcls_rec_210036.Tables[0].set_TableName("210036");
						bcls_rec_210036.Tables[0].Columns.Add(tmmsm34);
						bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
						bcls_rec_210036.Tables[0].Rows.Add();

						bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34);
						bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
						bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "D";

						if (bcls_rec_210036.Tables[0].Rows.get_Count()>0 && tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal() != 0)
						{
							doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}

					}
					//2024-02-19 再磨撤销发送电文：发210044，切废电文
					else if (flag == "3" || flag == "4")
					{
						Log::Trace("", "", "发送电文={0}", "删除电文--再磨");
						tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
						tmmsm01.Query();
						EIClass bcls_rec_210044;
						bcls_rec_210044.Tables[0].set_TableName("210044");
						bcls_rec_210044.Tables[0].Columns.Add(tmmsm01);
						bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
						bcls_rec_210044.Tables[0].Columns.Add(DT_DECIMAL, "CUT_SCRAP_WT");
						bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");


						bcls_rec_210044.Tables[0].Rows.Add();
						bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm01);
						bcls_rec_210044.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
						bcls_rec_210044.Tables[0].Rows[0]["PROC_DIV"] = "MMSM34";
						bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "D";

						Log::Trace("", "", "重量={0}", tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal());
						if (bcls_rec_210044.Tables[0].Rows.get_Count()>0 && tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal() != 0)
						{
							bcls_rec_210044.Tables[0].Rows[0]["CUT_SCRAP_WT"] = tmmsm34["MEND_SCRAP_WEIGHT"];
							doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
					}
					//2024-03-19
					else if (flag == "5")
					{
						EIClass bcls_rec_210036;
						bcls_rec_210036.Tables[0].set_TableName("210036");
						bcls_rec_210036.Tables[0].Columns.Add(tmmsm34);
						bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
						bcls_rec_210036.Tables[0].Rows.Add();

						bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34);
						bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
						bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "D";

						if (bcls_rec_210036.Tables[0].Rows.get_Count()>0 && tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal() != 0)
						{
							doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
					}

					//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
					tmmsm96["RCV_MAT_FLAG"] = "W";
					tmmsm96["MEND_FEEDBACK_FLAG"] = 1;
					tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
					tmmsm3e.Insert();

					if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
					{
						bcls_rec->Tables["MM0099"].Rows.Add();
					}
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
					if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
					{
						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						else
						{

						}
					}
				}
				else
				{
					tmmsm33dbsx["MAT_NO"] = tmmsm34["MAT_NO"];
					tmmsm33dbsx["EVENT_ID"] = "MM12";
					tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm34["RESUME_SEQ_NO"];
					tmmsm33dbsx["SEQ_NO"] = tmmsm33dbsx.QueryCount("MAT_NO") + 1;
					tmmsm33dbsx.Insert();
				}

			}
			//2024-03-08 只发上传实绩
			//设置一下主档表 
			if (v_proc_div == "SEND")
			{

				if (tmmsm01["HOLD_FLAG"].ToString() == "0")
				{

					EIClass bcls_rec_210036;
					bcls_rec_210036.Tables[0].set_TableName("210036");
					bcls_rec_210036.Tables[0].Columns.Add(tmmsm34_1);
					bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
					bcls_rec_210036.Tables[0].Rows.Add();

					bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34_1);
					bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "N";

					if (bcls_rec_210036.Tables[0].Rows.get_Count()>0)
					{
						doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						/*
						tmp.Tables["MM0099"].Rows.Add();
						tmp.Tables["MM0099"].Rows[0]["SIZE_DECIDE_CODE"] = "1001";
						tmp.Tables["MM0099"].Rows[0]["SLAB_CHECK_RESULT"] = "1001";
						if (!tmp.Tables["MM0099"].Columns.Contains("SURFACE_DECIDE_CODE"))
						tmp.Tables["MM0099"].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");
						tmp.Tables["MM0099"].Rows[0]["SURFACE_DECIDE_CODE"] = "1";
						tmp.Tables["MM0099"].Rows[0]["EVENT_ID"] = "QM20";//修改板坯上的最终出钢记号
						tmp.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
						tmp.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "QMTS";
						tmp.Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
						tmp.Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
						*/
						//2024-03-25 更新事件
						//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
						tmmsm96["RCV_MAT_FLAG"] = "W";
						tmmsm96["MEND_FEEDBACK_FLAG"] = 1;
						tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
						tmmsm3e.Insert();

					}

					if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
					{
						bcls_rec->Tables["MM0099"].Rows.Add();
					}
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
					if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
					{
						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						else
						{

						}
					}
				}
				else
				{

					tmmsm33dbsx["MAT_NO"] = tmmsm34_1["MAT_NO"];
					tmmsm33dbsx["EVENT_ID"] = "MM12";
					tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm34_1["PROD_SEQ_NO"];
					tmmsm33dbsx["SEQ_NO"] = tmmsm33dbsx.QueryCount("MAT_NO") + 1;
					tmmsm33dbsx.Insert();
				}


			}
			//新增实绩---发送电文
			if (v_proc_div == "INS_ACHIEVEMENT")
			{
				Log::Trace("", "", "电文发送={0}", "已经发过了");
				
			}
			if (v_proc_div == "EDIT_ACHIEVEMENT")
			{

				if (tmmsm01["HOLD_FLAG"].ToString() == "0")
				{
					tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
					tmmsm34_1["PROD_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
					tmmsm34_1.Query();
					tmmsm34_1.Print();

					EIClass bcls_rec_210036;
					bcls_rec_210036.Tables[0].set_TableName("210036");
					bcls_rec_210036.Tables[0].Columns.Add(tmmsm34_1);
					bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
					bcls_rec_210036.Tables[0].Rows.Add();

					bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34_1);
					bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34_1["MAT_NO"];
					bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "D";

					if (bcls_rec_210036.Tables[0].Rows.get_Count()>0 && v_proc_div != "D" && tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal() != 0)
					{
						doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						else
						{
							bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "N";
							doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
							else
							{
								/*
								tmp.Tables["MM0099"].Rows.Add();
								tmp.Tables["MM0099"].Rows[0]["SIZE_DECIDE_CODE"] = "1001";
								tmp.Tables["MM0099"].Rows[0]["SLAB_CHECK_RESULT"] = "1001";
								if (!tmp.Tables["MM0099"].Columns.Contains("SURFACE_DECIDE_CODE"))
								tmp.Tables["MM0099"].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");
								tmp.Tables["MM0099"].Rows[0]["SURFACE_DECIDE_CODE"] = "1";
								tmp.Tables["MM0099"].Rows[0]["EVENT_ID"] = "QM20";//修改板坯上的最终出钢记号
								tmp.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
								tmp.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "QMTS";
								tmp.Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
								tmp.Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm34_1["MAT_NO"];
								*/
								//2024-03-25 更新事件
								//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
								tmmsm96["RCV_MAT_FLAG"] = "W";
								tmmsm96["MEND_FEEDBACK_FLAG"] = 1;
								tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
								tmmsm3e.Insert();
							}
						}
					}

					if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
					{
						bcls_rec->Tables["MM0099"].Rows.Add();
					}
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
					if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
					{
						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
						else
						{

						}
					}
				}
				else
				{
					tmmsm33dbsx["MAT_NO"] = tmmsm34_1["MAT_NO"];
					tmmsm33dbsx["EVENT_ID"] = "MM12";
					tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm34_1["PROD_SEQ_NO"];
					tmmsm33dbsx["SEQ_NO"] = tmmsm33dbsx.QueryCount("MAT_NO") + 1;
					/*
					日期:20240516
					原因：初磨 内弧和外弧，都有磨后量的时候，都会往待办事项里面写入数据，导致待办事项处理的时候，出现多次上传210044
					*/
					int count = tmmsm33dbsx.QueryCount("MAT_NO,EVENT_ID,RESUME_SEQ_NO");
					if (count > 0)
					{
						Log::Trace("", "", "查询结果={0}", count);
					}
					else
					{
						tmmsm33dbsx.Insert();
					}
				}

			}
		}



		//先判断材料号是否在计划中存在，如果存在，才调用计划跟踪函数

		//获得输入参数
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	    // Oracle 数据库
		default:
			sqlstr = "SELECT  FACTORY_DIV,PLAN_NO,PLAN_BACKLOG_CODE "
				" FROM    TPSSM81 "
				" WHERE   MAT_NO = @mat_no";
			break;
		}
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Clear();
		cmd_sql.Parameters.Set("mat_no", tmmsm34["MAT_NO"].ToString());
		cmd_sql.ExecuteReader();
		if (cmd_sql.Read())
		{

			v_factory_div = cmd_sql.GetString(1);
			v_plan_no = cmd_sql.GetString(2);
			v_plan_backlog_code = cmd_sql.GetString(3);

			if (bcls_rec->Tables["PSSM"].Rows.get_Count() <= 0)
			{
				bcls_rec->Tables["PSSM"].Rows.Add();
			}
			bcls_rec->Tables["PSSM"].Rows[0]["FACTORY_DIV"] = v_factory_div;
			bcls_rec->Tables["PSSM"].Rows[0]["PLAN_BACKLOG_CODE"] = v_plan_backlog_code;
			bcls_rec->Tables["PSSM"].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
			bcls_rec->Tables["PSSM"].Rows[0]["PLAN_NO"] = v_plan_no;

			doFlag = f_pssm81_trace(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

		}
		cmd_sql.Close();

		
		
	#if defined(_SYS_PES)

			blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			if(blkNum < 0)
			{
				bcls_rec->Tables.Add("MMSMSND"); 
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"PROC_DIV");
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"MAT_NO");
				bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"PROD_SEQ_NO");
			}

			//--------向MMS送电文函数----------------


			/*
			bcls_rec->Tables["MMSMSND"].Rows.Clear();

			if (bcls_rec->Tables["MMSMSND"].Rows.get_Count() <= 0)
				bcls_rec->Tables["MMSMSND"].Rows.Add();
			bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM34";	
			bcls_rec->Tables["MMSMSND"].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROD_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
			bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;   */
			/*1:新增 2:修改 0:删除*/

			//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]]", v_proc_div);

			//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}



	#endif

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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


	return doFlag;

}
