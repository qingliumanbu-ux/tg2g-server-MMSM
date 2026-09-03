/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:ShiYong
Date:2015-08-13
Version:1.0
Description: 炼钢板坯修磨实绩新增
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/  




/* ***** 静态函数申明 ***** */

//修改板坯主档信息
int f_mmsm3401_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn); 

int f_mm0017(CString strUserId, CString& strShiftNo, CString& strShiftGroup, CString& strShiftDay, CDbConnection * conn);




//int f_mm00_getshift(CString strUserId, CString& strShiftNo, CString& strShiftGroup, CString& strShiftDay, CDbConnection * conn);

//板坯信息电文
//int f_wmsm_t8p301_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

//板坯删除信息电文
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);


/*<remark>=========================================================
/// <summary>
/// 炼钢板坯修磨实绩新增
/// <para>
/// 新增炼钢板坯修磨实绩
/// </para>
/// </summary>
/// <param name="tmmsm34">新增板坯修磨实绩信息</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsm34f3_ins)


int f_mmsm34f3_ins(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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
	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";
	CModel tmmsm34("TMMSM34");
	CModel tmmsm34_1("TMMSM34_1");
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	CString v_proc_div = "";
	CDbCommand cmd_sql(conn); //与DB 建立连接。

	CString endTime = "";
	try
	{
		
		tmmsm34.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		endTime = tmmsm34["GRINDING_END_TIME"].ToString();

		
		/* ***** 打印输入参数 ***** */
		tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
		tmmsm01.Query();
		
		EIClass inblock;
		inblock.Tables[0].Columns.Add(tmmsm01);
		inblock.Tables[0].Rows.Clear();
		tmmsm01.MergeTo(inblock.Tables[0], false);
	

		int count = tmmsm01.QueryCount("MAT_NO");
		
		/*
			日期：20240516
			原因：对于只是电子记录编辑。如果tmmsm01表，count>0不存在，则表示在线；==0表示归档
		*/
		if (count > 0)
		{

			if (bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))  
				v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
			/*
				v_proc_div = INNER_1I   初磨内弧
				v_proc_div = OUTER_1U   初磨外弧
				v_proc_div = INNER_2I   再磨内弧
				v_proc_div = OUTER_2U   再磨外弧
				
				v_proc_div = INS_ACHIEVEMENT 新增实绩
				v_proc_div = EDIT_ACHIEVEMENT 编辑实绩

				v_proc_div = ADMIN_EDIT 管理员修改				
			**/

			/*
				日期：2024-05-27
				原因：对于管理员权限的修改和删除，不卡任何条件
			**/

			if (v_proc_div=="ADMIN_EDIT")
			{
				Log::Trace("", "", "标记={0}", "管理员修改");
			}
			else{
				Log::Trace("", "", "标记={0}", "修磨工操作");
				/*
				日期：20240521
				原因：为了防止初磨内弧接管之后，外弧补全信息的时候，多次发送：板坯丢失电文。加限制，只有初磨内弧接管之后才发板坯miss
				*/
				Log::Trace("", "", "修磨标记={0}", v_proc_div);
				if (v_proc_div == "INNER_1I" || v_proc_div == "INS_ACHIEVEMENT")
				{
					/*
					日期：2024-05-27
					原因：编辑初磨内弧的时候，也会发送miss电文。
					限制：如果已经发过修磨实绩的，则不发miss电文
					*/

					tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
					int count = tmmsm34_1.QueryCount("MAT_NO");
					Log::Trace("", "", "修磨实绩数量={0}", count);
					if (count>0)
					{
						Log::Trace("", "", "修磨实绩数量={0},只是修改修磨电子记录", count);
					}
					else
					{
						doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
						if (doFlag < 0) {
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}

				}

				//CONFIRM----修磨记录--对应的F8按钮；INS_ACHIEVEMENT----修磨实绩--对应F3
				if (v_proc_div == "CONFIRM" || v_proc_div == "INS_ACHIEVEMENT")
				{
					Log::Trace("", "", "综判标记={0}", tmmsm01["COMPLEX_DECIDE_CODE"].ToString());
					//2024-04-09
					//"1"---表示 综判合格
					if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != "1")
					{
						CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
						CMessageFormat::Format(s.msg, "材料" + tmmsm01["MAT_NO"].ToString() + "综判不合格，不允许上传修磨实绩", arguments, 0);
						throw CApplicationException(-1, s.msg, log.Location);
					}
					/*
					日期：2024-05-21
					原因：未收货的材料不允许上传修磨实绩
					**/
					if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S" && tmmsm01["FINISH_FLAG"].ToString().Trim() == ""
						&& tmmsm01["MEND_FEEDBACK_FLAG"].ToString().Trim() == ""&& tmmsm01["DIV_FLAG"].ToString().Trim() == "")
					{
						CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
						CMessageFormat::Format(s.msg, "材料{0}未收货,不允许修磨", arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}
					/*
						强制校验，磨后量小于磨前量，磨后量不能为0
					*/
					double beforeWeight = tmmsm34["MEND_BEFORE_WEIGHT"].ToDouble();
					double afterWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
					if (afterWeight==0)
					{
						CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
						CMessageFormat::Format(s.msg, "磨后量不能为0！", arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (beforeWeight<afterWeight)
					{
						CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
						CMessageFormat::Format(s.msg, "填入的磨后量不能大于磨前量", arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}

				}
				//EDIT_ACHIEVEMENT---修磨实绩--对应F4
				else if (v_proc_div == "EDIT_ACHIEVEMENT")
				{

					tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
					tmmsm34_1["PROD_SEQ_NO"] = tmmsm34["PROD_SEQ_NO"];
					tmmsm34_1.Query();

					if (tmmsm01["MAT_ACT_LEN"].ToString() != tmmsm34_1["MAT_ACT_LEN"].ToString())
					{
						CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
						CMessageFormat::Format(s.msg, "材料号[{0}]，主档表和实绩表，长度不一致，不能修改修磨实绩", arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (tmmsm01["MAT_ACT_WIDTH"].ToString() != tmmsm34_1["MAT_ACT_WIDTH"].ToString())
					{
						CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
						CMessageFormat::Format(s.msg, "材料号[{0}]]，主档表和实绩表，宽度不一致，不能修改修磨实绩", arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (tmmsm01["MAT_ACT_THICK"].ToString() != tmmsm34_1["MAT_ACT_THICK"].ToString())
					{
						CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
						CMessageFormat::Format(s.msg, "材料号[{0}]，主档表和实绩表，厚度不一致，不能修改修磨实绩", arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}
					/*if (tmmsm01["MAT_ACT_WT"].ToString() != tmmsm34_1["MEND_AFTER_WEIGHT"].ToString())
					{
						CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
						CMessageFormat::Format(s.msg, "材料号[{0}]，主档表和实绩表，重量不一致，不能修改", arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}*/

					//2024-04-09
					//"0"---表示 未综判
					/*if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != "0")
					{
					CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料号[{0}]，当前材料状态不是：未综判，不允许修改修磨实绩", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
					}*/
				}

				//2024-03-08  添加一个前置条件
				/*if (tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "0" &&
					tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "1"&&
					tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "4")
				{
					CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0}已不在现场,已经装车，不允许修磨", arguments, 0);
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				if (tmmsm01["C_STATESIGN"].ToString().Trim() != "0" &&
					tmmsm01["C_STATESIGN"].ToString().Trim() != ""&&
					tmmsm01["C_STATESIGN"].ToString().Trim() != "6")
				{
					CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0}已不在现场，已经调拨，不允许修磨", arguments, 0);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (tmmsm01["MAT_LINE_TYPE"].ToString().Trim() != "SM")
				{
					strcpy(s.msg, "材料未在炼钢产线，不能修磨");//格式化字符串
					strcpy(s.sysmsg, s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}


				//-----2024-03-29
				/*
				日期：2024-05-21
				原因：未收货的材料可以维护修磨记录，但是不能发修磨实绩电文，符合条件的进入待办事项。
				将未收货的处理条件屏蔽掉
				**/

				/*if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S" && tmmsm01["FINISH_FLAG"].ToString().Trim() == ""
				&& tmmsm01["MEND_FEEDBACK_FLAG"].ToString().Trim() == ""&& tmmsm01["DIV_FLAG"].ToString().Trim() == "")
				{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}未收货,不允许修磨", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
				}*/

				if (tmmsm01["DIV_FLAG"].ToString().Trim() == "1" &&tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
				{
					CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0}已做中板改切，当前状态为等待制造管理系统反馈,暂不允许再进行修磨", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["MEND_FEEDBACK_FLAG"].ToString().Trim() == "1" &&tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
				{
					CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0}已做修磨，当前状态为等待制造管理系统反馈,暂不允许再进行修磨处理", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (tmmsm01["FINISH_FLAG"].ToString().Trim() == "1" &&tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
				{
					CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0}已做改切，当前状态为等待制造管理系统反馈,暂不允许再进行修磨处理", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			tmmsm34.CopyFrom(tmmsm01);
			tmmsm34.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			tmmsm34["GRINDING_END_TIME"] = endTime;
			tmmsm34["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm34["START_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm34["END_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm34["REC_CREATOR"] = s.userid;
			tmmsm34["PRACT_COLL_MODE"] = "0";
			tmmsm34["IN_MAT_NO"] = tmmsm34["MAT_NO"];

			tmmsm34["BATCH"] = tmmsm01["BATCH"];

			tmmsm34["PROD_SEQ_NO"] = bcls_rec->Tables[0].Rows[0]["PROD_SEQ_NO"].ToString();

			CString div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

			tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal();

			if (tmmsm34["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm34["PROD_SHIFT_GROUP"].ToString().Trim() == "")
			{
				/*--------------------------------------------------------------班次班组计算---------------------------------------------------------------------*/
				CString prodTime = tmmsm34["START_TIME"];//班次班组根据开始时刻计算				
				//f_mm0017(s.userid, tmmsm34["PROD_SHIFT_NO"].ToString(), tmmsm34["PROD_SHIFT_GROUP"].ToString(), prodTime, conn);
				f_epep_get_shift_group("SMCP", prodTime, PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
				//f_mm0017(s.userid, PROD_SHIFT_NO, PROD_SHIFT_GROUP, prodTime, conn);
				tmmsm34["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
				tmmsm34["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
				//f_mm00_getshift(s.userid, tmmsm34["PROD_SHIFT_NO"].ToString(), tmmsm34["PROD_SHIFT_GROUP"].ToString(), prodTime, conn);

				//Log::Trace("","","tmmsm34["HSF_END_TIME"] =[{0}]",tmmsm34["HSF_END_TIME"].ToString());
				//f_epep_get_shift_group("SM",tmmsm34["HSF_END_TIME"].ToString(),tmmsm34["PROD_SHIFT_NO"].ToString(),tmmsm34["PROD_SHIFT_GROUP"].ToString(),conn); 
				//Log::Trace("","","tmmsm34.prod_shift_no=[{0}]",tmmsm34["PROD_SHIFT_NO"].ToString());
				//Log::Trace("","","tmmsm34.prod_shift_group=[{0}]",tmmsm34["PROD_SHIFT_GROUP"].ToString());


				/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
			}

			/*-------------------------------------------------------------过IP计算修磨机组-------------------------------------------------------------------*/

			
			CString ip = s.fore_ip;
			CString mendSet = "";
			sqlstr = " SELECT t.CODE_DESC_1_CONTENT FROM TWMSMZD02 t  WHERE 1 = 1  AND t.CODE_CLASS = 'MEND_SET_IP'  AND t.CODE = '" + ip + "' ";
			Log::Trace("", "", "查询SQL={0}", sqlstr);
			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				mendSet = cmd_sql.GetString(1);
			}
			cmd_sql.Close();
			tmmsm34["FORE_IP"] = ip;
			Log::Trace("", "", "选中的修磨机组={0}", tmmsm34["MEND_SET"].ToString());
			/*
				日期：2024-05-21
				原因：手工选的修磨机组优先级更高
			**/
			/*if (mendSet!="ADMIN")
			{
				CString selectedMendSet = tmmsm34["MEND_SET"].ToString();
				if (selectedMendSet)
				{
				}
				tmmsm34["MEND_SET"] = mendSet;
			}
			else{

			}*/
			

			/*-----------------------------------------------------------------------------------------------------------------------------------------------*/



			//tmmsm34.Insert();

			if (!bcls_rec->Tables.Contains("MMSM34"))
			{
				bcls_rec->Tables.Add("MMSM34");
			}
			if (!bcls_rec->Tables["MMSM34"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["MMSM34"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			tmmsm34.MergeTo(bcls_rec->Tables["MMSM34"], false);
			bcls_rec->Tables["MMSM34"].Rows[0]["PROC_DIV"] = "I";/*I:新增 U:修改 D:删除*/


			doFlag = f_mmsm3401_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			inblock.Tables[0].Rows.Clear();
			tmmsm01.Query("MAT_NO");
			tmmsm01.MergeTo(inblock.Tables[0], false);
			/*
				日期：2024-05-21
				原因：为了防止接管的2250的坯子，没有修磨实绩，先注释掉---板坯信息电文
			**/
			//doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
			//if (doFlag < 0) {
			//	throw CApplicationException(-1, s.msg, s.svc_name);
			//}
		}
		else
		{
			hmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
			int hcount = hmmsm01.QueryCount("MAT_NO");
			hmmsm01.Query();				

			if (bcls_rec->Tables["PARA"].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
				v_proc_div = bcls_rec->Tables["PARA"].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();	
			tmmsm34.CopyFrom(hmmsm01);
			tmmsm34.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			tmmsm34["GRINDING_END_TIME"] = endTime;
			tmmsm34["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm34["START_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm34["END_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tmmsm34["REC_CREATOR"] = s.userid;
			tmmsm34["PRACT_COLL_MODE"] = "0";
			tmmsm34["IN_MAT_NO"] = tmmsm34["MAT_NO"];

			tmmsm34["BATCH"] = tmmsm01["BATCH"];

			tmmsm34["PROD_SEQ_NO"] = bcls_rec->Tables[0].Rows[0]["PROD_SEQ_NO"].ToString();

			CString div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString();

			tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal();

			if (tmmsm34["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm34["PROD_SHIFT_GROUP"].ToString().Trim() == "")
			{
				/*--------------------------------------------------------------班次班组计算---------------------------------------------------------------------*/
				CString prodTime = tmmsm34["START_TIME"];//班次班组根据开始时刻计算		

				f_epep_get_shift_group("SMCP", prodTime, PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);	
				tmmsm34["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
				tmmsm34["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
				/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
			}

			/*-------------------------------------------------------------过IP计算修磨机组-------------------------------------------------------------------*/
						
			CString ip = s.fore_ip;
			CString mendSet = "";
			sqlstr = " SELECT t.CODE_DESC_1_CONTENT FROM TWMSMZD02 t  WHERE 1 = 1  AND t.CODE_CLASS = 'MEND_SET_IP'  AND t.CODE = '" + ip + "' ";
		
			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				mendSet = cmd_sql.GetString(1);
			}
			cmd_sql.Close();
			tmmsm34["FORE_IP"] = ip;
			//tmmsm34["MEND_SET"] = mendSet;
			if (!bcls_rec->Tables.Contains("MMSM34"))
			{
				bcls_rec->Tables.Add("MMSM34");
			}
			if (!bcls_rec->Tables["MMSM34"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["MMSM34"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			tmmsm34.MergeTo(bcls_rec->Tables["MMSM34"], false);
			bcls_rec->Tables["MMSM34"].Rows[0]["PROC_DIV"] = "I";/*I:新增 U:修改 D:删除*/


			doFlag = f_mmsm3401_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}		

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
