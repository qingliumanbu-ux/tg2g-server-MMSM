/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-01-15
Description: 切废记录维护
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明
int f_mmsm391_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号

BM2F_ENTERACE(mmsm391_pro)

int f_mmsm_210044_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm391_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString procDiv = "";
	CString v_mat_no = "";
	CString v_resume_seq_no = "";//序号


	/* 业务变量 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm39_1("TMMSM39_1");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm33dbsx("TMMSM33DBSX");
	CModel tmmsm3e("TMMSM3E");
	CModel tmmsm96_1("TMMSM96");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */

	try
	{
		EIClass bcls_rec_210044;//发送L4二切实绩电文
		bcls_rec_210044.Tables[0].set_TableName("210044");
		bcls_rec_210044.Tables[0].Columns.Add(tmmsm39_1);
		bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
		bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		bcls_rec_210044.Tables[0].Rows.Add();


		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		bcls_rec->Tables["MM0099"].Rows.Add();


		EIClass bcls_MM0099;//发送L4二切实绩电文
		bcls_MM0099.Tables[0].set_TableName("MM0099");
		bcls_MM0099.Tables[0].Columns.Add(tmmsm96_1);
		bcls_MM0099.Tables[0].Rows.Add();

		procDiv = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().Trim();


		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
			tmmsm01.Query();
			tmmsm39_1.CopyFrom(tmmsm01);
			Log::Info("", __FUNCTION__, "PRINT_NO=[{0}]", tmmsm01["PRINT_NO"].ToString());
		}

		//此处添加校验，根据材料号查询TMMSM01，如果查询不到，表示为母坯信息，则报错，需要先将分切撤销
		if (procDiv == "D")
		{
			if (tmmsm01.QueryCount("MAT_NO") <= 0)
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}已做中板改切，请先操作中板改切撤销，再操作改切撤销！", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		/*if (tmmsm01["LOGISTICS_STATUS"].ToString().Trim() != "0")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已不在现场，不允许改切处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		if (tmmsm01["C_STATESIGN"].ToString().Trim() != "0"&& tmmsm01["C_STATESIGN"].ToString().Trim() != ""
			&& tmmsm01["C_STATESIGN"].ToString().Trim() != "6")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已不在现场，不允许改切处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//当其他反馈标记为空且收货标记不为S时，表示当时未做其他操作，且未收货
		//当其他标记有值，且收货标记不为S，表示等待反馈中，不允许做该操作
		if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S" && tmmsm01["FINISH_FLAG"].ToString().Trim() == ""
			&& tmmsm01["MEND_FEEDBACK_FLAG"].ToString().Trim() == ""&& tmmsm01["DIV_FLAG"].ToString().Trim() == "")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}未收货,不允许分段处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm01["DIV_FLAG"].ToString().Trim() == "1" &&tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做中板改切，当前状态为等待制造管理系统反馈,暂不允许再进行该操作", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["MEND_FEEDBACK_FLAG"].ToString().Trim() == "1" &&tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做修磨，当前状态为等待制造管理系统反馈,暂不允许再进行该操作", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["FINISH_FLAG"].ToString().Trim() == "1" &&tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做改切，当前状态为等待制造管理系统反馈,暂不允许再进行该操作", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm01["LGORT"].ToString().Trim() == "6246")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}已做修磨,请先撤销修磨再进行改切操作！", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm39_1.MergeFrom(bcls_rec->Tables[0].Rows[0]);

#pragma region   添加校验条件,因01表会根据切后长宽厚更新规格和重量，故，该数据不可为空  mfj  20240118
		if (tmmsm39_1["CUT_BEFORE_LEN"].ToDecimal() <= 0 ||
			tmmsm39_1["CUT_BEFORE_THICK"].ToDecimal() <= 0 ||
			tmmsm39_1["CUT_BEFORE_WIDTH"].ToDecimal() <= 0 || tmmsm39_1["CUT_BEFORE_WT"].ToDecimal() <= 0)
		{
			strcpy(s.sysmsg, "切前规格和重量不可为0！");
			strcpy(s.msg, s.sysmsg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm39_1["CUT_AFTER_LEN"].ToDecimal() <= 0 ||
			tmmsm39_1["CUT_AFTER_THICK"].ToDecimal() <= 0 ||
			tmmsm39_1["CUT_AFTER_WIDTH"].ToDecimal() <= 0 || tmmsm39_1["CUT_AFTER_WT"].ToDecimal() <= 0)
		{
			strcpy(s.sysmsg, "切后规格和重量不可为0！");
			strcpy(s.msg, s.sysmsg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//切前量与切后量不一致，且切废量没有值时，认为忘记录入切废量，这里计算一下
		if ((tmmsm39_1["CUT_AFTER_WT"].ToDecimal() != tmmsm39_1["CUT_BEFORE_WT"].ToDecimal()) && tmmsm39_1["CUT_SCRAP_WT"].ToDecimal() == 0)
		{
			tmmsm39_1["CUT_SCRAP_WT"] = tmmsm39_1["CUT_BEFORE_WT"].ToDecimal() - tmmsm39_1["CUT_AFTER_WT"].ToDecimal();
		}
		

#pragma endregion 



		
		
		//名义规格与实际规格保持一致
		tmmsm39_1["MAT_WIDTH"] = tmmsm39_1["CUT_AFTER_WIDTH"];
		tmmsm39_1["MAT_LEN"] = tmmsm39_1["CUT_AFTER_LEN"];
		tmmsm39_1["MAT_THICK"] = tmmsm39_1["CUT_AFTER_THICK"];

		//画面切废，规格取切后长宽厚
		tmmsm39_1["MAT_ACT_WIDTH"] = tmmsm39_1["CUT_AFTER_WIDTH"];
		tmmsm39_1["MAT_ACT_LEN"] = tmmsm39_1["CUT_AFTER_LEN"];
		tmmsm39_1["MAT_ACT_THICK"] = tmmsm39_1["CUT_AFTER_THICK"];
		tmmsm39_1["MAT_WT"] = tmmsm39_1["CUT_AFTER_WT"];//切后重量
		tmmsm39_1["MAT_ACT_WT"] = tmmsm39_1["CUT_AFTER_WT"];//系统重量即实际重量
		//tmmsm39["REAL_TIME_WT"] = tmmsm39["CUT_AFTER_WT"];//实时重量
		tmmsm39_1["QUALIFIED_WT"] = tmmsm39_1["CUT_AFTER_WT"];//合格产量

		tmmsm39_1["SAP_ERP_MATNR"] = tmmsm39_1["CUTTING_TYPE"];//切割类型 存入 物料编码中


		if (procDiv == "U")
		{
			////判断一下切前量与当前的01实际重量是否一致，如果不一致，表示已经做了改切或是修磨等其他操作。
			//if (tmmsm01["MAT_ACT_WT"].ToDecimal() != tmmsm39_1["CUT_BEFORE_WT"].ToDecimal())
			//{
			//	strcpy(s.sysmsg, "切前的系统重量与录入的切前重量数据不一致，请查看系统重量在此时是否更改了！");
			//	strcpy(s.msg, s.sysmsg);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			if ((v_mat_no.Substring(8, 2) == "00" || v_mat_no.Substring(8, 2) == "99" || v_mat_no.Substring(8, 2) == "AA" || v_mat_no.Substring(8, 2) == "ZZ")
				&& (tmmsm39_1["CUTTING_TYPE"].ToString().Trim() == "8" || tmmsm39_1["CUTTING_TYPE"].ToString().Trim() == "9"))
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}改切为头尾坯且切割类型为切头切尾，不可进行修改操作！", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				if (tmmsm01["HOLD_FLAG"].ToString() != "0")
				{
					CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0}处于封锁状态,不允许修改处理！", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm39_1["FINISH_FLAG"] = "1";
				tmmsm39_1.Update("*", "RESUME_SEQ_NO");
			}
			/*else
			{
				strcpy(s.sysmsg, "切后重量与系统重量不一致！");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
		}
		else if (procDiv == "D")
		{
			if ((v_mat_no.Substring(8, 2) == "00" || v_mat_no.Substring(8, 2) == "99" || v_mat_no.Substring(8, 2) == "AA" || v_mat_no.Substring(8, 2) == "ZZ")
				&& (tmmsm39_1["CUTTING_TYPE"].ToString().Trim() == "8" || tmmsm39_1["CUTTING_TYPE"].ToString().Trim() == "9"))
			{
				CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				CMessageFormat::Format(s.msg, "材料{0}改切为头尾坯且切割类型为切头切尾，不可进行撤销操作！", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			else{
				if (tmmsm01["MAT_ACT_WT"].ToDecimal() == tmmsm39_1["CUT_AFTER_WT"].ToDecimal())
				{
					if (tmmsm01["HOLD_FLAG"].ToString() != "0")
					{
						CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
						CMessageFormat::Format(s.msg, "材料{0}处于封锁状态,不允许删除处理", arguments, 1);
						throw CApplicationException(-1, s.msg, log.Location);
					}

					tmmsm39_1.TrimOrBlank();
					tmmsm39_1["FINISH_FLAG"] = "3";
					tmmsm39_1.Update("FINISH_FLAG", "RESUME_SEQ_NO,MAT_NO");
					//tmmsm39_1.Delete("RESUME_SEQ_NO,MAT_NO");
				}
				else
				{
					strcpy(s.sysmsg, "切后重量与系统重量不一致！");
					strcpy(s.msg, s.sysmsg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			
			
		}


		bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm39_1);

		tmmsm96.CopyFrom(tmmsm01);

		tmmsm96_1.CopyFrom(tmmsm01);
		//添加改切记录履历
		if ( procDiv == "U")
		{
			tmmsm96_1["EVENT_ID"] = "MM3K";
			tmmsm96_1["EVENT_LINE_TYPE"] = "SM";
			tmmsm96_1["FUNC_ID"] = "f_mmsm39_proc";
			tmmsm96_1["SYSTEM_ID"] = "MMSM";
			tmmsm96_1["EVENT_DESC"] = "改切实绩数据修改录入";
		}
		else  if (procDiv == "D")
		{
			tmmsm96_1["EVENT_ID"] = "MM3K";
			tmmsm96_1["EVENT_LINE_TYPE"] = "SM";
			tmmsm96_1["FUNC_ID"] = "f_mmsm39_proc";
			tmmsm96_1["SYSTEM_ID"] = "MMSM";
			tmmsm96_1["EVENT_DESC"] = "改切实绩删除";
		}

		bcls_MM0099.Tables[0].Rows[0].Merge(tmmsm96_1);
		if (bcls_MM0099.Tables[0].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(&bcls_MM0099, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}


		if (tmmsm01["HOLD_FLAG"].ToString() == "0")
		{
			if (procDiv == "U")
			{

				bcls_rec->Tables["MM0099"].Rows.Clear();
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["MAT_NO"] = tmmsm39_1["MAT_NO"];
				tmmsm96["RCV_MAT_FLAG"] = "W";
				tmmsm96["FINISH_FLAG"] = "1";
				tmmsm96["EVENT_ID"] = "MM3F";
				tmmsm96["EVENT_LINE_TYPE"] = "SM";
				tmmsm96["FUNC_ID"] = "mmsm39f3_pro";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_DESC"] = "改切修改处理等待反馈";

				bcls_rec->Tables["MM0099"].Rows.Add();
				bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

				tmmsm3e.CopyFrom(tmmsm96);
				doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;

				tmmsm3e["PROD_TIME"] = datetime;
				tmmsm3e["REMARK"] = "改切修改处理等待反馈";
				tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
				tmmsm3e.Insert();

			}
			else if (procDiv == "D")//当为删除时，需将切前长宽厚和重量赋给01表重量   删除校验添加在了前面
			{
				bcls_rec->Tables["MM0099"].Rows.Clear();
				tmmsm96.Reset();
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["MAT_NO"] = tmmsm39_1["MAT_NO"];
				tmmsm96["RCV_MAT_FLAG"] = "W";
				tmmsm96["FINISH_FLAG"] = "3";
				tmmsm96["EVENT_ID"] = "MM3F";
				tmmsm96["EVENT_LINE_TYPE"] = "SM";
				tmmsm96["FUNC_ID"] = "mmsm39f3_pro";
				tmmsm96["SYSTEM_ID"] = "MMSM";
				tmmsm96["EVENT_DESC"] = "改切删除处理等待反馈";

				bcls_rec->Tables["MM0099"].Rows.Add();
				bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

				tmmsm3e.CopyFrom(tmmsm96);
				doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;

				tmmsm3e["PROD_TIME"] = datetime;
				tmmsm3e["REMARK"] = "改切删除处理等待反馈";
				tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
				tmmsm3e.Insert();
			}

			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else
		{
			tmmsm33dbsx["MAT_NO"] = tmmsm39_1["MAT_NO"];
			tmmsm33dbsx["EVENT_ID"] = "MM37";
			tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm39_1["RESUME_SEQ_NO"];
			int dbsx_count = tmmsm33dbsx.QueryCount("MAT_NO,EVENT_ID");
			if (dbsx_count != 0)//为防止现场在封锁状态下多次操作改切，故改切在封锁下只允许操作一次
			{
				strcpy(s.sysmsg, "该材料已做改切，目前状态为等待处理，等待解除封锁后再操作改切！");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);

			}
			tmmsm33dbsx["SEQ_NO"] = dbsx_count + 1;
			tmmsm33dbsx.Insert();
		}

		//发送电文先删后增
		if (procDiv == "U")
		{
			bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "D";//删除
			bcls_rec_210044.Tables[0].Rows[0]["PROC_DIV"] = "TMMSM39_1";//改切实绩
			/********   太钢定制 发送L4电文 钢坯切废实绩   ***********/
			doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			bcls_rec_210044.Tables[0].Rows[0]["PROC_DIV"] = "TMMSM39_1";//改切实绩
			bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "N";//删除
		}
		else if (procDiv == "D")
		{
			bcls_rec_210044.Tables[0].Rows[0]["PROC_DIV"] = "TMMSM39_1";//改切实绩
			bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "D";//删除
		}


		/********   太钢定制 发送L4电文 钢坯切废实绩   ***********/
		doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


