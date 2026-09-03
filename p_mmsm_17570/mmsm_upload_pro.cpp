/************************/
/*** 2024-2-20 ********/
/****   wsl **************/
/**** 修磨量  ***********/
/************************/



//框架头文件
#include "stdafx.h"
//程序用头文件
//电文发送头文件
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天


// service入口
BM2F_ENTERACE(mmsm_upload_pro)
BM2_FUNCTION_EXPORT
int f_mmsm_210036_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//2024-03-25
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
//发切废电文
int f_mmsm_210044_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_upload_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CString sqlstr = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal after_wt = 0;
	CString count_ks = "";
	CDbCommand cmd_sql(conn);

	//2024-03-11 定义tmmsm39，生成切废实绩
	CModel tmmsm39("TMMSM39");
	CModel tmmsm34("TMMSM34");
	CModel tmmsm39_1("TMMSM39_1");
	CModel tmmsm34_1("TMMSM34_1");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm3e("TMMSM3E");
	CModel tmmsm33dbsx("TMMSM33DBSX");
	CString v_resume_seq_no = "";//序号

	try
	{
		Log::Trace("", "", "批处理={0}", "开始处理");
		sqlstr = " SELECT MAT_NO,PROD_SEQ_NO,ISUPLOAD  FROM TMMSM34  WHERE 1 = 1  AND  ISUPLOAD not in ('1','2')  ORDER BY REC_CREATE_TIME DESC, MAT_NO ASC ";


		// 2024-03-25 这里设置一下调用事件
		tmmsm96["EVENT_ID"] = "MM3F";
		tmmsm96["EVENT_LINE_TYPE"] = "SM";
		tmmsm96["FUNC_ID"] = "f_mmsm3401_proc";
		tmmsm96["SYSTEM_ID"] = "MMSM";
		tmmsm96["EVENT_DESC"] = "收货修磨切废分切操作标记更改";
		

		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteReader();
		while (cmd_sql.Read())
		{
			CString matNo = cmd_sql.GetString(1);
			CString prodSeqNo = cmd_sql.GetString(2);
			tmmsm34["MAT_NO"] = matNo;
			tmmsm34["PROD_SEQ_NO"] = prodSeqNo;
			tmmsm34.Query();
			Log::Trace("", "", "当前材料={0}", matNo);


			tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
			tmmsm01.Query();
			
			//2024-04-09 这里添加一个判断，如果综判合格：tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim()!="1"
			if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() == "1")
			{
				CString mendFlag = tmmsm34["MEND_FLAG"].ToString();
				if (mendFlag == "1" || mendFlag == "2")
				{

					double afterWeight = tmmsm34["MEND_AFTER_WEIGHT"].ToDouble();
					if (afterWeight > 0)
					{
						CString endTime = tmmsm34["GRINDING_END_TIME"].ToString();
						if (mendFlag == "2")
						{
							//如果是初磨外弧，需要获取外弧修磨结束时间
							endTime = tmmsm34["GRINDING_OUTER_END_TIME"].ToString();
						}
						if (endTime != " ")
						{
							CDateTime end = CDateTime::Parse(endTime);
							CDateTime now = CDateTime::Parse(CDateTime::Now().ToString("yyyyMMddHHmmss"));
							CTimeSpan span = now.Subtract(end);
							if (span.TotalMinutes() - 60 < 0)
							{
								Log::Trace("", "", "查询结果={0}", matNo + "没有符合条件的数据" + endTime);
							}
							else
							{
								tmmsm34["ISUPLOAD"] = 1;//表示发：210036电文--修磨实绩电文
								tmmsm34.Update("ISCONFIRM,ISUPLOAD");
								Log::Trace("", "", "{0}", "开始上传修磨实绩");
								EIClass bcls_rec_210036;
								bcls_rec_210036.Tables[0].set_TableName("210036");
								bcls_rec_210036.Tables[0].Columns.Add(tmmsm34);
								bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
								bcls_rec_210036.Tables[0].Rows.Add();

								bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34);
								bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
								bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "N";

								if (bcls_rec_210036.Tables[0].Rows.get_Count() > 0 && tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal() != 0)
								{
									doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
									if (doFlag < 0)
									{
										throw CApplicationException(-1, s.msg, log.Location);
									}
									//插入实绩表
									Log::Trace("", "", "开始上传", "");
									tmmsm34_1.CopyFrom(tmmsm34);
									tmmsm01["MAT_NO"] = tmmsm34_1["MAT_NO"];
									tmmsm01.Query();

									//region
									tmmsm34_1["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
									tmmsm34_1["MAT_ACT_THICK"] = tmmsm01["MAT_ACT_THICK"];
									tmmsm34_1["MAT_ACT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
									tmmsm34_1["MAT_ACT_LEN"] = tmmsm01["MAT_ACT_LEN"];
									tmmsm34_1["MAT_ACT_WT"] = tmmsm34["MEND_AFTER_WEIGHT"];
									tmmsm34_1.Insert();

									//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
									tmmsm96["MAT_NO"] = tmmsm34["MAT_NO"];
									tmmsm96["RCV_MAT_FLAG"] = "W";
									tmmsm96["MEND_FLAG"] = tmmsm34_1["MEND_FLAG"];
									tmmsm96["MEND_FEEDBACK_FLAG"] = 1;

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
									tmmsm3e["RECEIVE_BACK_STATUS"] = "W";
									tmmsm3e.Insert();
									if (tmmsm01["HOLD_FLAG"].ToString() == "0")
									{
										if (bcls_rec->Tables.Contains("MM0099") == false)
										{
											bcls_rec->Tables.Add("MM0099");
											bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
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
										tmmsm33dbsx["MAT_NO"] = tmmsm34["MAT_NO"];
										tmmsm33dbsx["EVENT_ID"] = "MM12";
										tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm34["RESUME_SEQ_NO"];
										tmmsm33dbsx["SEQ_NO"] = tmmsm33dbsx.QueryCount("MAT_NO") + 1;
										tmmsm33dbsx.Insert();
									}

								}
							}


						}
						else
						{
							Log::Trace("", "", "查询结果={0}", matNo + "没有符合条件的数据");
						}
					}
					else
					{
						Log::Trace("", "", "当前材料={0},没有磨后重量", matNo);
					}

				}
				if (mendFlag == "3" || mendFlag == "4")
				{
					double secondWeight = tmmsm34["MEND_SECOND_WEIGHT"].ToDouble();
					if (secondWeight > 0)
					{
						CString endTime = tmmsm34["GRINDING_END_TIME"].ToString();
						if (mendFlag == "4")
						{
							//如果是再磨外弧，需要获取外弧修磨结束时间
							endTime = tmmsm34["GRINDING_OUTER_END_TIME"].ToString();
						}
						if (endTime != " ")
						{
							CDateTime end = CDateTime::Parse(endTime);
							CDateTime now = CDateTime::Parse(CDateTime::Now().ToString("yyyyMMddHHmmss"));
							CTimeSpan span = now.Subtract(end);
							if (span.TotalMinutes() - 60 < 0)
							{
								Log::Trace("", "", "查询结果={0}", matNo + "没有符合条件的数据" + endTime);
							}
							else
							{
								tmmsm34["ISUPLOAD"] = 2;//表示发：210044电文--切废电文
								tmmsm34.Update("ISCONFIRM,ISUPLOAD");
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
								if (bcls_rec_210044.Tables[0].Rows.get_Count() > 0 && tmmsm34["MEND_SCRAP_WEIGHT"].ToDecimal() != 0)
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
									tmmsm39["RESUME_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
									tmmsm39.Insert();

									tmmsm39_1.CopyFrom(tmmsm39);
									tmmsm39_1.Insert();

									//2024-03-19
									//插入实绩表
									tmmsm34_1.CopyFrom(tmmsm34);
									//region
									tmmsm34_1["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
									tmmsm34_1["MEND_FLAG"] = mendFlag;
									tmmsm34_1.Insert();
									//如果发送电文，需要将TMMSM01表的RCV_MAT_FLAG置成 W
									tmmsm96["MAT_NO"] = tmmsm34["MAT_NO"];
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


									if (tmmsm01["HOLD_FLAG"].ToString() == "0")
									{
										if (bcls_rec->Tables.Contains("MM0099") == false)
										{
											bcls_rec->Tables.Add("MM0099");
											bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
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
										tmmsm33dbsx["MAT_NO"] = tmmsm34["MAT_NO"];
										tmmsm33dbsx["EVENT_ID"] = "MM12";
										tmmsm33dbsx["RESUME_SEQ_NO"] = tmmsm34["RESUME_SEQ_NO"];
										tmmsm33dbsx["SEQ_NO"] = tmmsm33dbsx.QueryCount("MAT_NO") + 1;
										tmmsm33dbsx.Insert();
									}
								}
							}

						}
						else
						{
							Log::Trace("", "", "查询结果={0}", matNo + "没有符合条件的数据");
						}

					}
					else
					{
						Log::Trace("", "", "当前材料={0},没有再磨重量", matNo);
					}

				}

				
			}
			else if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != "1")
			{
				Log::Trace("", "", "当前材料={0},综判不合格", matNo);
			}

		}
		cmd_sql.Close();
		
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
