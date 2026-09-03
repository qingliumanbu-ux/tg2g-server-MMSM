/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   mfj
Version:    1.0
Date:     2024-01-08
Description: 待办事项处理
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件



int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
int f_mmsm_210044_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送切废电文
int f_mmsm_210034_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送分切电文
int f_mmsm_210036_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送修磨电文
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_get_density(CString ST_NO, CDecimal& MAT_DENSITY, CDbConnection* conn);//通过钢种计算密度

//外部函数声明
BM2_FUNCTION_EXPORT
int f_mmsm33dbsx_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm33dbsx_proc";                //定义函数英文名称  
	CString FunctionCname = "待办事项处理";          //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;

	CString sqlstr = "";
	CString v_proc_div = "";
	CString v_pract_rcv_flag = "";
	CString v_factory_div = "";
	CString v_station_id = "";
	CString v_acjc_relation_id = "";
	CString v_resume_seq_no = "";//序号
	CString v_operate = "";//操作区分	 I 新增    U 修改
	CString v_sap_erp_matnr = "";//物料编码
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_event_id = "";//事件号
	CDecimal matTheoryWt = 0;
	CString v_ponoslab = "' '";//匹配完不再匹配的命令坯
	CDecimal len_tm35 = 0;//35表实际长度
	/*数据库操作类定义 */
	CDbCommand cmd_sql(conn); //与DB 建立连接。
	CDbCommand cmd_inq(conn); // 建立连接。

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm34("TMMSM34");//修磨表
	CModel tmmsm34_1("TMMSM34_1");//修磨表
	CModel tmmsm35("TMMSM35");//分切表
	CModel tmmsm39("TMMSM39");//切废表
	//该表主键为材料号，事件号和履历序号，该序号存储实绩主键，另一个序号为处理序号
	CModel tmmsm33dbsx("TMMSM33DBSX");
	CModel tpssm03("TPSSM03");//
	CModel tmmsm01_slab("TMMSM01");


	EIClass inBlock1;
	EIClass outBlock1;


	try
	{
		CPageInfo pageInfo;


		if (bcls_rec->Tables.Contains("MM0099") == false)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}

		//材料号为必要参数，不可缺少
		if (!bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			sprintf(s.msg, "材料号为必要参数,不可缺少!"); //系统错误信息
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "材料号不可为空!"); //系统错误信息
			throw CApplicationException(-1, s.msg, log.Location);
		}

		EIClass bcls_rec_TMMSM33DBSX;
		bcls_rec_TMMSM33DBSX.Tables[0].set_TableName("TMMSM33DBSX");
		bcls_rec_TMMSM33DBSX.Tables[0].Columns.Add(tmmsm33dbsx);

		EIClass bcls_rec_TMMSM35;
		bcls_rec_TMMSM35.Tables[0].set_TableName("TMMSM35");
		bcls_rec_TMMSM35.Tables[0].Columns.Add(tmmsm35);

		//发送修磨电文给产销
		EIClass bcls_rec_210036;
		bcls_rec_210036.Tables[0].set_TableName("210036");
		bcls_rec_210036.Tables[0].Columns.Add(tmmsm34);
		bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");

		//发送切废电文给产销
		EIClass bcls_rec_210044;
		bcls_rec_210044.Tables[0].set_TableName("210044");
		bcls_rec_210044.Tables[0].Columns.Add(tmmsm39);
		bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");

		//发送L4二切实绩电文
		EIClass bcls_rec_210034;
		bcls_rec_210034.Tables[0].set_TableName("210034");
		bcls_rec_210034.Tables[0].Columns.Add(tmmsm01);
		bcls_rec_210034.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");




		tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();

		sqlstr = "SELECT * FROM TMMSM33DBSX WHERE MAT_NO = '" + tmmsm01["MAT_NO"].ToString() + "' order by SEQ_NO asc";
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.ExecuteQuery(bcls_rec_TMMSM33DBSX.Tables[0]);
		cmd_sql.Close();

		//没有待办事项时，直接返回
		if (bcls_rec_TMMSM33DBSX.Tables[0].Rows.get_Count() == 0)
		{
			return doFlag;
		}

		//对根据材料号查询到的待办事项进行处理
		for (int i = 0; i < bcls_rec_TMMSM33DBSX.Tables[0].Rows.get_Count(); i++)
		{
			tmmsm33dbsx.Reset();
			tmmsm01.Reset();
			tmmsm96.Reset();
			tmmsm34.Reset();
			tmmsm34_1.Reset();
			tmmsm33dbsx.MergeFrom(bcls_rec_TMMSM33DBSX.Tables[0].Rows[i]);
			v_event_id = tmmsm33dbsx["EVENT_ID"].ToString().Trim();

			#pragma region  修磨产出
			if (v_event_id == "MM12")
			{
				tmmsm34["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
				tmmsm34["PROD_SEQ_NO"] = tmmsm33dbsx["RESUME_SEQ_NO"];

				/*
					日期：2024-06-04
					原因：发送电文之前需要，判断一下发送的磨前量和主档表的磨前量是否一致，
					如果一致，发送电文；如果不一致，删除待办，生成一条未上传的实绩，提示操作人员未上传，修改修磨实绩
				*/
				double beforeWeight = 0;
				if (!tmmsm34.Query("MAT_NO,PROD_SEQ_NO"))
				{
					tmmsm34_1["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
					tmmsm34_1["PROD_SEQ_NO"] = tmmsm33dbsx["RESUME_SEQ_NO"];
					tmmsm34_1.Query("MAT_NO,PROD_SEQ_NO");
					beforeWeight = tmmsm34_1["MEND_BEFORE_WEIGHT"].ToDouble();
				}
				else
				{
					beforeWeight = tmmsm34["MEND_BEFORE_WEIGHT"].ToDouble();
				}
				tmmsm01["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
				tmmsm01.Query();

				Log::Trace("", "", "34表磨前重量=[{0}]", beforeWeight);
				Log::Trace("", "", "主档表系统重量=[{0}]", tmmsm01["MAT_ACT_WT"].ToDouble());

				if (beforeWeight != tmmsm01["MAT_ACT_WT"].ToDouble())
				{
					Log::Trace("", "", "[{0}]", "发送电文磨前重量和主档表系统重量不一致，不发送电文，生成修磨实绩");

					tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
					int count = tmmsm34_1.QueryCount("MAT_NO");
					if (count == 0)
					{						
						tmmsm34_1.CopyFrom(tmmsm34);
						tmmsm34_1["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
						//-1表示生成错误的修磨实绩
						tmmsm34_1["ISUPLOAD"] = -1;
						tmmsm34_1.Insert();
					}
					else
					{
						tmmsm34_1["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
						tmmsm34_1["PROD_SEQ_NO"] = tmmsm33dbsx["RESUME_SEQ_NO"];
						tmmsm34_1.Query("MAT_NO,PROD_SEQ_NO");
						//-1表示生成错误的修磨实绩
						tmmsm34_1["ISUPLOAD"] = -1;
						Log::Trace("", "", "341更新开始=[{0}]", "-1");
						tmmsm34_1.Update("MAT_NO,PROD_SEQ_NO,ISUPLOAD");
						Log::Trace("", "", "341更新完毕=[{0}]", "-1");
					}

				}
				else
				{
					///开始
					//当34表查不到时，表示空走修磨，查34_1表数据
					if (!tmmsm34.Query("MAT_NO,PROD_SEQ_NO"))
					{
						tmmsm34_1["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
						tmmsm34_1["PROD_SEQ_NO"] = tmmsm33dbsx["RESUME_SEQ_NO"];
						tmmsm34_1.Query("MAT_NO,PROD_SEQ_NO");

						tmmsm96["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
						tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"];
						tmmsm96["MEND_FEEDBACK_FLAG"] = "1";
						tmmsm96["RCV_MAT_FLAG"] = "W";
						tmmsm96["MEND_FLAG"] = tmmsm34_1["MEND_FLAG"];
						tmmsm96["EVENT_ID"] = "MM3F";
						tmmsm96["EVENT_LINE_TYPE"] = "SM";
						tmmsm96["FUNC_ID"] = "f_mmsm33dbsx_proc";
						tmmsm96["SYSTEM_ID"] = "MMSM";
						tmmsm96["EVENT_DESC"] = "铸坯修磨实绩待办处理产出，等待反馈";

						//先清理一遍，防止遗留的脏数据影响
						bcls_rec->Tables["MM0099"].Rows.Clear();
						if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
						{
							bcls_rec->Tables["MM0099"].Rows.Add();
						}
						bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

						//只在这里调用，修改标记，等待反馈电文时将数据重新写入01表
						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}



						tmmsm96["MAT_ACT_THICK"] = tmmsm34_1["MAT_ACT_THICK"];
						tmmsm96["MAT_ACT_WIDTH"] = tmmsm34_1["MAT_ACT_WIDTH"];
						tmmsm96["MAT_ACT_LEN"] = tmmsm34_1["MAT_ACT_LEN"];
						tmmsm96["MAT_THICK"] = tmmsm34_1["MAT_ACT_THICK"];
						tmmsm96["MAT_WIDTH"] = tmmsm34_1["MAT_ACT_WIDTH"];
						tmmsm96["MAT_LEN"] = tmmsm34_1["MAT_ACT_LEN"];
						tmmsm96["MAT_THEORY_WT"] = tmmsm34_1["MAT_THEORY_WT"];

						tmmsm96["GRINDING_START_TIME"] = tmmsm34_1["GRINDING_START_TIME"];
						tmmsm96["GRINDING_END_TIME"] = tmmsm34_1["GRINDING_END_TIME"];

						tmmsm96["MEND_FLAG"] = tmmsm34_1["MEND_FLAG"];



						//当为初磨时，用磨后重量字段，当为再磨时，用再磨磨后重量
						if (tmmsm34_1["MEND_FLAG"].ToString().Trim() == "1" ||
							tmmsm34_1["MEND_FLAG"].ToString().Trim() == "2"||
							tmmsm34_1["MEND_FLAG"].ToString().Trim() == "5")
						{
							//如果磨后量有值，取磨后重量数据，否则取磨前重量
							if (tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal() > 0)
							{
								tmmsm96["LGORT"] = "6246";
								tmmsm96["MAT_WT"] = tmmsm34_1["MEND_AFTER_WEIGHT"];
								tmmsm96["MAT_ACT_WT"] = tmmsm34_1["MEND_AFTER_WEIGHT"];

								//只有有磨后量时才发送电文
								bcls_rec_210036.Tables[0].Rows.Clear();
								bcls_rec_210036.Tables[0].Rows.Add();
								bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34_1);
								bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34_1["MAT_NO"];
								bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "N";
							}
							else
							{
								tmmsm96["LGORT"] = "6242";
								tmmsm96["MAT_WT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"];
								tmmsm96["MAT_ACT_WT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"];
							}
						}
						else if (tmmsm34_1["MEND_FLAG"].ToString().Trim() == "3" ||
							tmmsm34_1["MEND_FLAG"].ToString().Trim() == "4")
						{
							//如果再磨重量有值，取再磨重量字段的数据，否则取磨后重量字段数据
							if (tmmsm34_1["MEND_SECOND_WEIGHT"].ToDecimal() > 0)
							{
								tmmsm96["LGORT"] = "6246";
								tmmsm96["MAT_WT"] = tmmsm34_1["MEND_SECOND_WEIGHT"];
								tmmsm96["MAT_ACT_WT"] = tmmsm34_1["MEND_SECOND_WEIGHT"];

								tmmsm01["MAT_NO"] = tmmsm34_1["MAT_NO"];
								tmmsm01.Query();

								bcls_rec_210044.Tables[0].Rows.Clear();
								bcls_rec_210044.Tables[0].Rows.Add();
								bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm01);
								bcls_rec_210044.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
								bcls_rec_210044.Tables[0].Rows[0]["PROC_DIV"] = "MMSM34";
								bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "N";
							}
							else
							{
								tmmsm96["LGORT"] = "6242";
								tmmsm96["MAT_WT"] = tmmsm34_1["MEND_AFTER_WEIGHT"];
								tmmsm96["MAT_ACT_WT"] = tmmsm34_1["MEND_AFTER_WEIGHT"];
							}
						}
					}
					else
					{
						tmmsm96["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
						tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
						tmmsm96["MEND_FEEDBACK_FLAG"] = "1";
						tmmsm96["RCV_MAT_FLAG"] = "W";
						tmmsm96["MEND_FLAG"] = tmmsm34["MEND_FLAG"];
						tmmsm96["EVENT_ID"] = "MM3F";
						tmmsm96["EVENT_LINE_TYPE"] = "SM";
						tmmsm96["FUNC_ID"] = "f_mmsm33dbsx_proc";
						tmmsm96["SYSTEM_ID"] = "MMSM";
						tmmsm96["EVENT_DESC"] = "铸坯修磨实绩待办处理产出，等待反馈";

						//先清理一遍，防止遗留的脏数据影响
						bcls_rec->Tables["MM0099"].Rows.Clear();
						if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
						{
							bcls_rec->Tables["MM0099"].Rows.Add();
						}
						bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

						//只在这里调用，修改标记，等待反馈电文时将数据重新写入01表
						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}



						tmmsm96["MAT_ACT_THICK"] = tmmsm34["MAT_ACT_THICK"];
						tmmsm96["MAT_ACT_WIDTH"] = tmmsm34["MAT_ACT_WIDTH"];
						tmmsm96["MAT_ACT_LEN"] = tmmsm34["MAT_ACT_LEN"];
						tmmsm96["MAT_THICK"] = tmmsm34["MAT_ACT_THICK"];
						tmmsm96["MAT_WIDTH"] = tmmsm34["MAT_ACT_WIDTH"];
						tmmsm96["MAT_LEN"] = tmmsm34["MAT_ACT_LEN"];
						tmmsm96["MAT_THEORY_WT"] = tmmsm34["MAT_THEORY_WT"];

						tmmsm96["GRINDING_START_TIME"] = tmmsm34["GRINDING_START_TIME"];
						tmmsm96["GRINDING_END_TIME"] = tmmsm34["GRINDING_END_TIME"];

						tmmsm96["MEND_FLAG"] = tmmsm34["MEND_FLAG"];



						//当为初磨时，用磨后重量字段，当为再磨时，用再磨磨后重量
						if (tmmsm34["MEND_FLAG"].ToString().Trim() == "1" ||
							tmmsm34["MEND_FLAG"].ToString().Trim() == "2")
						{
							//如果磨后量有值，取磨后重量数据，否则取磨前重量
							if (tmmsm34["MEND_AFTER_WEIGHT"].ToDecimal() > 0)
							{
								tmmsm96["LGORT"] = "6246";
								tmmsm96["MAT_WT"] = tmmsm34["MEND_AFTER_WEIGHT"];
								tmmsm96["MAT_ACT_WT"] = tmmsm34["MEND_AFTER_WEIGHT"];

								//只有有磨后量时才发送电文
								bcls_rec_210036.Tables[0].Rows.Clear();
								bcls_rec_210036.Tables[0].Rows.Add();
								bcls_rec_210036.Tables[0].Rows[0].Merge(tmmsm34);
								bcls_rec_210036.Tables[0].Rows[0]["MAT_NO"] = tmmsm34["MAT_NO"];
								bcls_rec_210036.Tables[0].Rows[0]["DEAL_FLAG"] = "N";
							}
							else
							{
								tmmsm96["LGORT"] = "6242";
								tmmsm96["MAT_WT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
								tmmsm96["MAT_ACT_WT"] = tmmsm34["MEND_BEFORE_WEIGHT"];
							}
						}
						else if (tmmsm34["MEND_FLAG"].ToString().Trim() == "3" ||
							tmmsm34["MEND_FLAG"].ToString().Trim() == "4")
						{
							//如果再磨重量有值，取再磨重量字段的数据，否则取磨后重量字段数据
							if (tmmsm34["MEND_SECOND_WEIGHT"].ToDecimal() > 0)
							{
								tmmsm96["LGORT"] = "6246";
								tmmsm96["MAT_WT"] = tmmsm34["MEND_SECOND_WEIGHT"];
								tmmsm96["MAT_ACT_WT"] = tmmsm34["MEND_SECOND_WEIGHT"];

								tmmsm01["MAT_NO"] = tmmsm34["MAT_NO"];
								tmmsm01.Query();

								bcls_rec_210044.Tables[0].Rows.Clear();
								bcls_rec_210044.Tables[0].Rows.Add();
								bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm01);
								bcls_rec_210044.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
								bcls_rec_210044.Tables[0].Rows[0]["PROC_DIV"] = "MMSM34";
								bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "N";
							}
							else
							{
								tmmsm96["LGORT"] = "6242";
								tmmsm96["MAT_WT"] = tmmsm34["MEND_AFTER_WEIGHT"];
								tmmsm96["MAT_ACT_WT"] = tmmsm34["MEND_AFTER_WEIGHT"];
							}
						}


						/*
							日期：2024-05-28
							原因：对于走待办事项的，需要给tmmsm34_1表新增
						**/

						tmmsm34_1["MAT_NO"] = tmmsm34["MAT_NO"];
						int count = tmmsm34_1.QueryCount("MAT_NO");
						if (count == 0)
						{
							Log::Trace("", "", "修磨实绩数据={0}", count);
							tmmsm34_1.CopyFrom(tmmsm34);
							tmmsm34_1["PROD_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
							tmmsm34_1.Insert();
						}

					}
					//发送修磨电文
					if (bcls_rec_210036.Tables[0].Rows.get_Count() > 0)
					{
						doFlag = f_mmsm_210036_snd(&bcls_rec_210036, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}

					//发送切废电文
					if (bcls_rec_210044.Tables[0].Rows.get_Count()>0)
					{
						if (tmmsm34.QueryCount("MAT_NO,PROD_SEQ_NO") > 0)
						{
							bcls_rec_210044.Tables[0].Rows[0]["CUT_SCRAP_WT"] = tmmsm34["MEND_SCRAP_WEIGHT"];
						}
						else
						{
							bcls_rec_210044.Tables[0].Rows[0]["CUT_SCRAP_WT"] = tmmsm34_1["MEND_SCRAP_WEIGHT"];
						}
					
						doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					//结束
				}
				tmmsm33dbsx.Delete();//处理结束后，将待办事项数据删除

				if (false)
				{
					tmmsm96["EVENT_ID"] = "MM12";
					tmmsm96["EVENT_LINE_TYPE"] = "SM";
					tmmsm96["FUNC_ID"] = "f_mmsm33dbsx_proc";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_DESC"] = "铸坯修磨实绩待办处理产出";

					//先清理一遍，防止遗留的脏数据影响
					bcls_rec->Tables["MM0099"].Rows.Clear();
					if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
					{
						bcls_rec->Tables["MM0099"].Rows.Add();
					}
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

			}
			#pragma endregion

			#pragma region  分切产出--只存母坯号在待处理表里，子坯数据根据in_mat_no母坯号 获取
			if (v_event_id == "MM16")
			{
			
				tmmsm01.Reset();//先清理一遍
				bcls_rec_TMMSM35.Tables[0].Rows.Clear();
				sqlstr = "SELECT * FROM TMMSM35 WHERE IN_MAT_NO = '" + tmmsm33dbsx["MAT_NO"].ToString() + "' order by PROD_SEQ_NO asc";
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.ExecuteQuery(bcls_rec_TMMSM35.Tables[0]);
				cmd_sql.Close();

				tmmsm35.MergeFrom(bcls_rec_TMMSM35.Tables[0].Rows[0]);

				tmmsm01["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
				tmmsm01.Query();
				tmmsm01_slab["MAT_NO"] = tmmsm35["IN_MAT_NO"];
				tmmsm01_slab.Query();
				if (tmmsm01["HOLD_FLAG"].ToString() != "0")
				{
					CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
					CMessageFormat::Format(s.msg, "材料{0}处于封锁状态,不允许分段处理", arguments, 1);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				////N 未收货  W等待(等L4的反馈)  S收货成功   当未收货成功时，不允许二切  mfj   20231120  因接口问题，暂定W，等反馈状态就可进行分切   后续接口完善再更改  mfj  20240118
				//if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() != "S")
				//{
				//	CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				//	CMessageFormat::Format(s.msg, "材料{0}未收货,不允许分段处理", arguments, 1);
				//	throw CApplicationException(-1, s.msg, log.Location);
				//}

				//if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != "1")
				//{
				//	CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
				//	CMessageFormat::Format(s.msg, "材料{0}未综判,不允许分段处理", arguments, 1);
				//	throw CApplicationException(-1, s.msg, log.Location);
				//}

				matTheoryWt = tmmsm01["MAT_THEORY_WT"];
				bcls_rec->Tables["MM0099"].Rows.Clear();
				bcls_rec->Tables["MM0099"].Rows.Add();
				//调用事件  只改标记 在反馈里集中处理
				bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm01_slab);
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM3F";
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
				bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "f_mmsm33dbsx_proc";
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm35["IN_MAT_NO"];
				bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = "W";
				bcls_rec->Tables["MM0099"].Rows[0]["DIV_FLAG"] = "1";

				doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}



				int y = 0;
				for (int j = 0; j < bcls_rec_TMMSM35.Tables[0].Rows.get_Count(); j++)
				{
					tmmsm35.MergeFrom(bcls_rec_TMMSM35.Tables[0].Rows[j]);

					tmmsm01["MAT_NO"] = tmmsm35["MAT_NO"];
					tmmsm01["BATCH"] = tmmsm35["BATCH"];
					tmmsm01["PRINT_NO"] = tmmsm35["PRINT_NO"];
					tmmsm01["IN_MAT_NO"] = tmmsm35["IN_MAT_NO"];
					tmmsm01["MAT_ACT_LEN"] = tmmsm35["MAT_LEN"];
					tmmsm01["MAT_LEN"] = tmmsm35["MAT_LEN"];
					tmmsm01["MAT_NUM"] = tmmsm35["MAT_TUBE"];
					tmmsm01["MAT_TUBE"] = tmmsm35["MAT_TUBE"];
					tmmsm01["MAT_ACT_WT"] = tmmsm35["MAT_WT"];
					tmmsm01["MAT_WT"] = tmmsm35["MAT_WT"];
					tmmsm01["RCV_MAT_FLAG"] = "S";
					len_tm35 = tmmsm35["MAT_LEN"].ToDecimal();
					//批次号与母坯号一致的，保留母坯的收货重量
					if (tmmsm35["BATCH"].ToString().Trim() == tmmsm35["IN_MAT_NO"].ToString().Trim())
					{
						tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];
					}
					else
					{
						tmmsm01["RECEIVE_WEIGHT"] = 0;//收货重量
						//将修磨的数据不继承母坯
						tmmsm01["MEND_BEFORE_WEIGHT"] = 0; //磨前量
						tmmsm01["MEND_AFTER_WEIGHT"] = 0;//
						//tmmsm01["MEND_FLAG"] = "0";	//修磨标记  继承，子坯不做修磨  mfj  20240520
						tmmsm01["MEASURE_WT"] = 0;	//称重量
						tmmsm01["REAL_TIME_WT"] = 0;//实时重量
						tmmsm01["ORDER_NO"] = " ";//合同号
						tmmsm01["PRINT_NO"] = " ";//喷印号 
						tmmsm01["PONO_SLAB"] = " ";
						
						tmmsm01["LSLAB_NO"] = " ";//长坯号-虚拟板坯号
						tmmsm01["FIX_SLAB_NUM"] = 0;
						//tmmsm01["SLAB_TYPE_OLD"] = "3";

						tmmsm01["SLAB_NO"] = tmmsm01["SLAB_NO"].ToString().Substring(0, 15) + tmmsm01["BATCH"].ToString().Substring(8, 2)
							+ tmmsm01["SLAB_NO"].ToString().Substring(17);
						Log::Trace("", __FUNCTION__, "SLAB_NO=[{0}]", tmmsm01["SLAB_NO"].ToString().Trim());

					}
					//tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["MAT_ACT_WT"];//暂定 收货重量取切后重量，  后面待确认  mfj  20240131  收货重量为0 杨姐确认  mfj  20240319
					tmmsm01["MAT_ACT_WT"] = tmmsm35["MAT_WT"];//系统重量
					//tmmsm01["REAL_TIME_WT"] = tmmsm35["MAT_WT"];//实时重量
					tmmsm01["QUALIFIED_WT"] = tmmsm35["MAT_WT"];//合格产量
					tmmsm01["MAT_THEORY_WT"] = matTheoryWt / tmmsm35["IN_MAT_TUBE"].ToDecimal() / tmmsm35["IN_MAT_LEN"].ToDecimal() * tmmsm35["MAT_TUBE"].ToDecimal() * tmmsm35["MAT_LEN"];
					tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);


					EIClass bcls_tmmsm01;
					bcls_tmmsm01.Tables[0].Columns.Add(tmmsm01);

					//根据虚拟板坯号查找已使用的命令坯，并将每次循环的上一个循环的命令坯排除掉
					sqlstr = " SELECT * FROM TPSSM03 WHERE LSLAB_NO = '" + tmmsm01_slab["LSLAB_NO"].ToString().Trim() + "' AND  SLAB_PROD_FLAG = '1'"
						" AND  SLAB_NO NOT IN (" + v_ponoslab + ") ORDER BY SLAB_NO ";

					Log::Trace("", __FUNCTION__, "v_ponoslab=[{0}]", v_ponoslab);
					Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_tmmsm01.Tables[0]);
					cmd_inq.Close();
					
					Log::Trace("", __FUNCTION__, "bcls_tmmsm01.Tables[0].Rows.get_Count()=[{0}]", bcls_tmmsm01.Tables[0].Rows.get_Count());
					
					for (int tp03_count = 0; tp03_count < bcls_tmmsm01.Tables[0].Rows.get_Count(); tp03_count++)
					{
						int v_pono_slabcount = tp03_count + 1;
						tpssm03.Reset();
						tpssm03.MergeFrom(bcls_tmmsm01.Tables[0].Rows[tp03_count]);
						Log::Trace("", __FUNCTION__, "v_pono_slabcount=[{0}]", v_pono_slabcount);


						if (tpssm03["SLAB_NO"].ToString().Trim() == "")//如果没有命令坯号，则跳过
						{
							continue;
						}

						Log::Trace("", __FUNCTION__, "v_pono_slabcount=[{0}]", v_pono_slabcount);
						//如果实际长度大于命令坯最大长度，则匹配成功
						//如果实际长度在命令坯范围内，则匹配成功
						//如果实际长度小于命令坯最小值，则匹配失败，跳过
						if ((len_tm35> tpssm03["SLAB_MAX_LEN"].ToDecimal()) ||
							(len_tm35 >= tpssm03["SLAB_MIN_LEN"].ToDecimal() && len_tm35 <= tpssm03["SLAB_MAX_LEN"].ToDecimal()))
						{
							/*if (tp03_count == 0)
							{
							tmmsm35["PONO_SLAB"] = tpssm03["SLAB_NO"];
							}*/


							tmmsm35["PONO_SLAB_" + CConvert::ToString(v_pono_slabcount)] = tpssm03["SLAB_NO"];
							v_ponoslab = v_ponoslab + ",'" + tpssm03["SLAB_NO"].ToString().Trim() + "'";

							len_tm35 = len_tm35 - tpssm03["SLAB_LEN"].ToDecimal();

						}
						else if (len_tm35 < tpssm03["SLAB_MIN_LEN"].ToDecimal())
						{
							/*if (v_pono_slabcount == 1)
							{
							tmmsm35["PONO_SLAB"] = " ";
							}*/

							tmmsm35["PONO_SLAB_" + CConvert::ToString(v_pono_slabcount)] = " ";

						}

					}

					int v_pono_count = 1;
					//将命令坯置空
					while (v_pono_count < 13)
					{
						tmmsm01["PONO_SLAB_" + CConvert::ToString(v_pono_count)] = tmmsm35["PONO_SLAB_" + CConvert::ToString(v_pono_count)];
						v_pono_count++;
					}

					//不对主档表处理，在反馈接收电文里集中处理
					if (false)
					{
						bcls_rec->Tables["MM0099"].Rows.Clear();
						tmmsm96.Reset();
						tmmsm96.CopyFrom(tmmsm01);
						tmmsm96["EVENT_ID"] = "MM15";
						tmmsm96["EVENT_LINE_TYPE"] = "SM";
						tmmsm96["SYSTEM_ID"] = "MMSM";
						tmmsm96["FUNC_ID"] = "mmsm35_cut";
						tmmsm96["EVENT_DESC"] = "材料分切产出";
						tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					

					bcls_rec_210034.Tables[0].Rows.Add();
					bcls_rec_210034.Tables[0].Rows[j].Merge(tmmsm01);
					bcls_rec_210034.Tables[0].Rows[j]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec_210034.Tables[0].Rows[j]["DEAL_FLAG"] = "N";//新增

				}

				/********   太钢定制 发送L4电文 二切实绩   ***********/
				if (bcls_rec_210034.Tables[0].Rows.get_Count()>0)
				{
					doFlag = f_mmsm_210034_snd(&bcls_rec_210034, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}


				tmmsm33dbsx.Delete();//处理结束后，将待办事项数据删除

				if (false)
				{
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM16";
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
					bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
					bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "mmsm35f6_cut";
					bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm35["IN_MAT_NO"];
					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}


					
				}


			}
			#pragma endregion


			#pragma region  切废  
			if (v_event_id == "MM37")
			{
				tmmsm39["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
				tmmsm39["RESUME_SEQ_NO"] = tmmsm33dbsx["RESUME_SEQ_NO"];
				tmmsm39.Query("MAT_NO,RESUME_SEQ_NO");
				CString v_mat_no = tmmsm39["MAT_NO"];

				if ((v_mat_no.Substring(8, 2) == "00" || v_mat_no.Substring(8, 2) == "99" || v_mat_no.Substring(8, 2) == "AA" || v_mat_no.Substring(8, 2) == "ZZ")
					&& (tmmsm39["CUTTING_TYPE"].ToString().Trim() == "8" || tmmsm39["CUTTING_TYPE"].ToString().Trim() == "9"))
				{
					bcls_rec->Tables["MM0099"].Rows.Clear();
					tmmsm01["MAT_NO"] = tmmsm39["MAT_NO"];
					tmmsm01.Query();
					tmmsm96.CopyFrom(tmmsm01);

					//名义规格与实际规格保持一致
					tmmsm96["MAT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
					tmmsm96["MAT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
					tmmsm96["MAT_THICK"] = tmmsm39["CUT_AFTER_THICK"];

					//画面切废，规格取切后长宽厚
					tmmsm96["MAT_ACT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
					tmmsm96["MAT_ACT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
					tmmsm96["MAT_ACT_THICK"] = tmmsm39["CUT_AFTER_THICK"];
					tmmsm96["MAT_WT"] = tmmsm39["CUT_AFTER_WT"];//切后重量
					tmmsm96["MAT_ACT_WT"] = tmmsm39["CUT_AFTER_WT"];//系统重量即实际重量
					//tmmsm96["REAL_TIME_WT"] = tmmsm39["CUT_AFTER_WT"];//实时重量
					tmmsm96["QUALIFIED_WT"] = tmmsm39["CUT_AFTER_WT"];//合格产量
					tmmsm96["RECEIVE_WEIGHT"] = tmmsm96["MAT_ACT_WT"];

					if (true)
					{
						CDecimal v_code_wt = 0;//计算重量的系数

						/*if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 3) == "1A6")
						{
							v_code_wt = 7.95;
						}
						else if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 3) == "1A9")
						{
							v_code_wt = 7.95;
						}
						else if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 2) == "1D")
						{
							v_code_wt = 7.8;
						}
						else if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 1) == "1")
						{
							v_code_wt = 7.9;
						}
						else if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 1) == "2")
						{
							v_code_wt = 7.85;
						}
						else if (tmmsm96["ST_NO"].ToString().Trim().Substring(0, 1) == "3")
						{
							v_code_wt = 7.85;
						}*/

						doFlag = f_mmsm_get_density(tmmsm96["ST_NO"].ToString(), v_code_wt,conn);

						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}

						tmmsm96["PRODUTE_CAL_WT"] = ((tmmsm96["MAT_ACT_WIDTH"].ToDecimal() / 1000) * (tmmsm96["MAT_ACT_LEN"].ToDecimal() / 1000) * (tmmsm96["MAT_ACT_THICK"].ToDecimal() / 1000) * v_code_wt).Round(3);

					}
				
					tmmsm96["MAT_NO"] = tmmsm39["MAT_NO"];
					tmmsm96["EVENT_ID"] = "MM03";
					tmmsm96["EVENT_LINE_TYPE"] = "00";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["FUNC_ID"] = "f_mmsm33dbsx_proc";
					tmmsm96["EVENT_DESC"] = "改切头尾坯且为切头切尾时走物料同步";

					
					if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
					{
						bcls_rec->Tables["MM0099"].Rows.Add();
					}
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

				}
				else
				{
					tmmsm01["MAT_NO"] = tmmsm39["MAT_NO"];
					tmmsm01.Query();
					//调用事件  只改标记 在反馈里集中处理
					bcls_rec->Tables["MM0099"].Rows.Clear();
					bcls_rec->Tables["MM0099"].Rows.Add();
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm01);
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM3F";
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
					bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
					bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "f_mmsm33dbsx_proc";
					bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm39["MAT_NO"];
					bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = "W";
					bcls_rec->Tables["MM0099"].Rows[0]["FINISH_FLAG"] = "1";

					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					bcls_rec_210044.Tables[0].Rows.Clear();
					bcls_rec_210044.Tables[0].Rows.Add();
					bcls_rec_210044.Tables[0].Rows[0].Merge(tmmsm39);
					bcls_rec_210044.Tables[0].Rows[0]["MAT_NO"] = tmmsm39["MAT_NO"];
					bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "N";

					doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

				}

				tmmsm33dbsx.Delete();//处理结束后，将待办事项数据删除

				if (false)
				{
					//名义规格与实际规格保持一致
					tmmsm96["MAT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
					tmmsm96["MAT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
					tmmsm96["MAT_THICK"] = tmmsm39["CUT_AFTER_THICK"];

					//画面切废，规格取切后长宽厚
					tmmsm96["MAT_ACT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
					tmmsm96["MAT_ACT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
					tmmsm96["MAT_ACT_THICK"] = tmmsm39["CUT_AFTER_THICK"];
					tmmsm96["MAT_WT"] = tmmsm39["CUT_AFTER_WT"];//切后重量
					tmmsm96["MAT_ACT_WT"] = tmmsm39["CUT_AFTER_WT"];//系统重量即实际重量
					//tmmsm96["REAL_TIME_WT"] = tmmsm39["CUT_AFTER_WT"];//实时重量
					tmmsm96["QUALIFIED_WT"] = tmmsm39["CUT_AFTER_WT"];//合格产量

					tmmsm96["EVENT_ID"] = "MM37";
					tmmsm96["EVENT_LINE_TYPE"] = "SM";
					tmmsm96["FUNC_ID"] = "f_mmsm39_proc";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_DESC"] = "钢坯切废实绩";

					//先清理一遍，防止遗留的脏数据影响
					bcls_rec->Tables["MM0099"].Rows.Clear();
					if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
					{
						bcls_rec->Tables["MM0099"].Rows.Add();
					}
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);
					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					
				}

			}
			#pragma endregion


			

		}













	}
	/*捕获数据库操作异常*/
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		//LogTrace(1,1,"%s",(const char*)sqlstr);
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "DB error:" + sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

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

	//LogTrace(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	////LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);
	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;

}

