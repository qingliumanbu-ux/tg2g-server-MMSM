/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      178053
Version:     1.0
Date:        2019-11-22 16:20:24
Description: PES侧接收铸坯收货反馈信息(MMS->炼钢PES)电文
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"



//外部函数声明
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);		//物料跟踪函数
int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
int f_mmsm_t80rya_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsmsm_stock_in(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//调用仓库接口，进行板坯入库   太钢定制
int f_mmsm_210044_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送切废电文
int f_mmsm_210034_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送分切电文
int f_mmsm_210036_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送修磨电文
int f_wmsm_t8p303_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//给2250发送板坯去向信息
int f_wmsm_t8p301_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//给2250发送板坯数据信息
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//给2250发送板坯删除信息
int f_wmsm_t80ryd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm_get_density(CString ST_NO, CDecimal& MAT_DENSITY, CDbConnection* conn);//通过钢种计算密度
BM2F_ENTERACE_TELE(cm_0rt801_rcv)

int f_cm_0rt801_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CDecimal matTheoryWt = 0;
	CDecimal len_tm35 = 0;//35表实际长度
	CString v_resume_seq_no = "";//序号
	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString v_event_id = "";//事件号
	CString v_receive_back_status = "";
	CString v_event_type = "";//N  正向流程  D 逆向流程
	CString v_ponoslab = "' '";//匹配完不再匹配的命令坯
	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm3e("TMMSM3E");
	CModel tmmsm01_query("TMMSM01");
	CModel twma0 = CModel("TWMA0");
	CModel tmmsm34("TMMSM34");   //修磨记录表
	CModel tmmsm34_1("TMMSM34_1");//修磨实绩表
	CModel tmmsm35("TMMSM35");//分切表
	CModel tmmsm39("TMMSM39");//切废表
	CModel tmmsm39_1("TMMSM39_1");
	CModel tpssm03("TPSSM03");//
	CModel tmmsm33("TMMSM33");//
	CModel tmmsm01_slab("TMMSM01");
	CModel tmmsm01_zp("TMMSM01");//子坯数据
	CModel tmmsm33shll("TMMSM33SHLL");//收货履历表，原始记录，每次收货新增进表后 不再更改改该数据  主键:材料号，序号
	CModel tqmtst802("TQMTST802");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_1;
	CString sqlstr_st;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn); //与DB 建立连接。
	CDbCommand cmd_sql_1(conn); //与DB 建立连接。
	CDbCommand cmd_sql_st(conn); //与DB 建立连接。
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
	try
	{
		EIClass surf;
		surf.Tables[0].set_TableName("MM0099");
		surf.Tables[0].Columns.Add(tmmsm96);
		surf.Tables[0].Rows.Clear();

		if (bcls_rec->Tables.IndexOf("MM0099") < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
			bcls_rec->Tables["MM0099"].Rows.Clear();
		}
		if (bcls_rec->Tables.IndexOf("T80RYA") < 0)
		{
			bcls_rec->Tables.Add("T80RYA");
			bcls_rec->Tables["T80RYA"].Columns.Add(twma0);
			bcls_rec->Tables["T80RYA"].Rows.Clear();
		}

		EIClass bcls_rec_TMMSM35;
		bcls_rec_TMMSM35.Tables[0].set_TableName("TMMSM35");
		bcls_rec_TMMSM35.Tables[0].Columns.Add(tmmsm35);

		//发送修磨电文给产销
		EIClass bcls_rec_210036;
		bcls_rec_210036.Tables[0].set_TableName("210036");
		bcls_rec_210036.Tables[0].Columns.Add(tmmsm34_1);
		bcls_rec_210036.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");

		//发送切废电文给产销
		EIClass bcls_rec_210044;
		bcls_rec_210044.Tables[0].set_TableName("210044");
		bcls_rec_210044.Tables[0].Columns.Add(tmmsm39);
		bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "PROC_DIV");
		//bcls_rec_210044.Tables[0].Columns.Add(DT_DECIMAL, "CUT_SCRAP_WT");
		bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");

		//发送L4二切实绩电文
		EIClass bcls_rec_210034;
		bcls_rec_210034.Tables[0].set_TableName("210034");
		bcls_rec_210034.Tables[0].Columns.Add(tmmsm01);
		bcls_rec_210034.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");



		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			v_event_id = bcls_rec->Tables[0].Rows[i]["EVENT_ID"].ToString().Trim();
			v_receive_back_status = bcls_rec->Tables[0].Rows[i]["RECEIVE_BACK_STATUS"].ToString().Trim();
			v_event_type = bcls_rec->Tables[0].Rows[i]["EVENT_TYPE"].ToString().Trim();
			tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (!tmmsm01.Query("MAT_NO"))
			{
				//针对分切反馈的是子坯材料号，故这里单独处理
				tmmsm35["MAT_NO"] = tmmsm01["MAT_NO"];
				tmmsm35.Query("MAT_NO");

				sqlstr = "SELECT * FROM VMMSM01 WHERE MAT_NO = '" + tmmsm35["IN_MAT_NO"].ToString().Trim() + "' ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm01);
				}
				else
				{
					sprintf(s.msg, "未查到板坯数据！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				cmd_inq.Close();

			}
			tmmsm3e.CopyFrom(tmmsm01);
			tmmsm3e.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmmsm3e["PROD_TIME"] = datetime;
			doFlag = f_mm0011("TMMSM3E_seq", 8, v_resume_seq_no, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm3e["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
			tmmsm3e.Insert();

			if (v_receive_back_status == "S")
			{


				if (v_event_id == "MM09" && v_event_type == "N")
				{
					tmmsm33shll.Reset();
					sqlstr = "SELECT * FROM TMMSM33SHLL WHERE MAT_NO = '" + tmmsm01["MAT_NO"].ToString() + "' ORDER by RESUME_SEQ_NO desc ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						cmd_inq.Fetch(tmmsm33shll);
					}
					cmd_inq.Close();

					//tmmsm01["RECEIVE_WEIGHT"] = tmmsm33shll["RECEIVE_WEIGHT"];
					tmmsm01.CopyFrom(tmmsm33shll); //获取收货履历中的数据

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

						tmmsm01["PRODUTE_CAL_WT"] = ((tmmsm01["MAT_WIDTH"].ToDecimal() / 1000) * (tmmsm01["MAT_THICK"].ToDecimal() / 1000) * (tmmsm01["MAT_LEN"].ToDecimal() / 1000) * v_code_wt).Round(3);

					}



					tmmsm33["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];
					tmmsm33["RCV_MAT_FLAG"] = "S";  //N 未收货  W等待(等L4的反馈)  S收货成功
					tmmsm33["RECV_MAT_TIME"] = tmmsm01["RECV_MAT_TIME"];
					tmmsm33.TrimOrBlank();
					tmmsm33.Update("RECEIVE_WEIGHT,RCV_MAT_FLAG,RECV_MAT_TIME", "MAT_NO");

					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
					//tmmsm96["RECEIVE_WEIGHT"] = tmmsm01["MAT_WT"];
					tmmsm96["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"].ToDecimal().Round(3);//收货重量，同时更新名义和实际重量，实际重量即老系统的系统重量   mfj  20240113
					tmmsm96["MAT_WT"] = tmmsm01["RECEIVE_WEIGHT"].ToDecimal().Round(3);
					tmmsm96["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"].ToDecimal().Round(3);
					tmmsm96["QUALIFIED_WT"] = tmmsm01["RECEIVE_WEIGHT"].ToDecimal().Round(3);//合格产量
					//tmmsm96["REAL_TIME_WT"] = tmmsm01["MAT_WT"]; //实时重量 


					Log::Trace("", __FUNCTION__, "RECEIVE_WEIGHT[{0}] PRODUTE_CAL_WT [{1}]  ", tmmsm01["RECEIVE_WEIGHT"].ToString(), tmmsm01["PRODUTE_CAL_WT"].ToDecimal());

					//名义规格与实际规格保持一致
					tmmsm96["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
					tmmsm96["MAT_LEN"] = tmmsm01["MAT_LEN"];
					tmmsm96["MAT_THICK"] = tmmsm01["MAT_THICK"];

					//画面修改名义，用名义覆盖实际 保持两套一致
					tmmsm96["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
					tmmsm96["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
					tmmsm96["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];
					tmmsm96["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"];
					tmmsm96["SLAB_HEAD_WIDTH"] = tmmsm01["MAT_WIDTH"];//头宽
					tmmsm96["SLAB_TAIL_WIDTH"] = tmmsm01["MAT_WIDTH"];//尾宽
					tmmsm96["PRODUTE_CAL_WT"] = tmmsm01["PRODUTE_CAL_WT"].ToDecimal().Round(3);//计算重量
					tmmsm96["SLAB_NO"] = tmmsm01["SLAB_NO"];
					//tmmsm96["SG_SIGN"] = tmmsm01["SG_SIGN"];  钢种修改在盘库画面 未收货时才可以修改 mfj  20240416

					//tmmsm96["ACCEP_STLOC"] = "6242";
					tmmsm96["RECV_MAT_TIME"] = tmmsm01["RECV_MAT_TIME"];//收货日期
					tmmsm96["PRODUCT_FLAG"] = tmmsm01["PRODUCT_FLAG"];//成品标记
					tmmsm96["RCV_MAT_FLAG"] = "S";//N 未收货  W等待(等L4的反馈)  S收货成功
					tmmsm96["RCV_NO_STATUS"] = "N"; // 收货取消状态  置为 未取消  mfj  20240309
					tmmsm96["EVENT_ID"] = "MM34";
					tmmsm96["EVENT_LINE_TYPE"] = "SM";
					tmmsm96["FUNC_ID"] = "cm_0rt801_rcv";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_DESC"] = "铸坯收货确认";

					tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

					twma0["MAT_NO"] = tmmsm01["MAT_NO"];
					twma0["STOCK_OPER_ORDER"] = "1B";
					twma0.Query("MAT_NO,STOCK_OPER_ORDER");
					bcls_rec->Tables["WM_STOCK"].Rows.Add();
					bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "1B";					//库业务类型
					bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER_DIV"] = "1";					//业务类型内区分
					bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = "SYA";			//库号
					bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = "SYA";						//材料库位号
					bcls_rec->Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";								//行号
					bcls_rec->Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";							//列号
					bcls_rec->Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;								//层号
					bcls_rec->Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = "1";				//库位内位置
					//doFlag = f_wmsmsm_stock_in(bcls_rec, bcls_ret, conn);  //太钢定制   产出时入库
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					bcls_rec->Tables["T80RYA"].Rows.Clear();
					bcls_rec->Tables["T80RYA"].Rows.Add();
					bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER"] = twma0["STOCK_OPER_ORDER"];
					bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER_DIV"] = twma0["STOCK_OPER_ORDER_DIV"];
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NO"] = twma0["MAT_NO"];
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NUM"] = twma0["MAT_NUM"];
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_LINE_TYPE"] = twma0["MAT_LINE_TYPE"];
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_KIND"] = twma0["MAT_KIND"];
					bcls_rec->Tables["T80RYA"].Rows[0]["FACTORY_DIV"] = "LG1";
					bcls_rec->Tables["T80RYA"].Rows[0]["USER_ID"] = "cm_0rt801_rcv";
					if (!bcls_rec->Tables["T80RYA"].Columns.Contains("STOCK_OPER_TIME"))
						bcls_rec->Tables["T80RYA"].Columns.Add(DT_STRING, "STOCK_OPER_TIME");

					bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_TIME"] = datetime;
					bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_NO"] = "AK3";
					bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_PLACE_NO"] = twma0["TO_STOCK_PLACE_NO"];
					bcls_rec->Tables["T80RYA"].Rows[0]["TO_LAYERNO"] = twma0["TO_LAYERNO"];


					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}


					doFlag = f_mmsm_t80rya_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					CModel tmmsm33dbsx("TMMSM33DBSX");
					EIClass bcls_rec_TMMSM33DBSX;
					bcls_rec_TMMSM33DBSX.Tables[0].set_TableName("TMMSM33DBSX");
					bcls_rec_TMMSM33DBSX.Tables[0].Columns.Add(tmmsm33dbsx);
					sqlstr = "SELECT * FROM TMMSM33DBSX WHERE MAT_NO = '" + tmmsm01["MAT_NO"].ToString() + "' AND EVENT_ID = 'MM13' order by SEQ_NO asc";
					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.ExecuteQuery(bcls_rec_TMMSM33DBSX.Tables[0]);
					cmd_sql.Close();

					//正常情况下  一条材料号 在待办事项中 处理未收货时做的修磨实绩 只有一条记录
					//收货的时候有修磨记录且去向为2250的，不发2250

					if (bcls_rec_TMMSM33DBSX.Tables[0].Rows.get_Count() > 0)
					{
						//先清理一波结构体，防止脏数据
						tmmsm96.Reset();
						tmmsm34.Reset();
						tmmsm34_1.Reset();

						tmmsm33dbsx.MergeFrom(bcls_rec_TMMSM33DBSX.Tables[0].Rows[0]);
						tmmsm34["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
						tmmsm34["PROD_SEQ_NO"] = tmmsm33dbsx["RESUME_SEQ_NO"];
						/*
						日期：2024-06-06
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
								//-1表示生成未上传的修磨实绩
								tmmsm34_1["ISUPLOAD"] = -1;
								tmmsm34_1.Insert();
							}
							else
							{
								tmmsm34_1["MAT_NO"] = tmmsm33dbsx["MAT_NO"];
								tmmsm34_1["PROD_SEQ_NO"] = tmmsm33dbsx["RESUME_SEQ_NO"];
								tmmsm34_1.Query("MAT_NO,PROD_SEQ_NO");
								//-1表示生成未上传的修磨实绩
								tmmsm34_1["ISUPLOAD"] = -1;
								Log::Trace("", "", "341更新开始=[{0}]", "-1");
								tmmsm34_1.Update("MAT_NO,PROD_SEQ_NO,ISUPLOAD");
								Log::Trace("", "", "341更新完毕=[{0}]", "-1");
							}

						}
						else
						{
							Log::Trace("", "", "表磨前重量=[{0}]", "34表和主档表重量一致");
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
									tmmsm34_1["MEND_FLAG"].ToString().Trim() == "2")
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
								tmmsm96["EVENT_DESC"] = "未收货产出的铸坯修磨实绩收货时待办处理产出，等待反馈";

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
							tmmsm33dbsx.Print();
						}
						tmmsm33dbsx.Delete();//处理结束后，将待办事项数据删除
					}
					else
					{
						EIClass inblock;
						inblock.Tables[0].Columns.Add(tmmsm01);
						inblock.Tables[0].Rows.Clear();
						tmmsm01.MergeTo(inblock.Tables[0], false);
						CString v_guide_dest = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
						if (v_guide_dest.Find("1") >= 0)
						{
							doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
							if (doFlag < 0) {
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
						doFlag = f_wmsm_t8p303_snd(&inblock, bcls_ret, conn);
						if (doFlag < 0) {
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}

					tqmtst802["SLAB_NO"] = tmmsm01["SLAB_NO"];
					if (tqmtst802.QueryCount("SLAB_NO")==1)
					{
						EIClass INS;
						INS.Tables[0].Columns.Add(DT_STRING,"SLAB_NO");
						INS.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
						INS.Tables[0].Rows.Add();
						INS.Tables[0].Rows[0]["SLAB_NO"] = tmmsm01["SLAB_NO"];
						INS.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
						doFlag = f_wmsm_t80ryd(&INS, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
				}

				if (v_event_id == "MM09" && v_event_type == "D")
				{
					tmmsm33.Reset();
					tmmsm01["RECV_MAT_TIME"] = " ";
					tmmsm01["RECEIVE_WEIGHT"] = 0;

					tmmsm33.CopyFrom(tmmsm01);
					tmmsm33["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];

					tmmsm33["RCV_MAT_FLAG"] = "N";

					tmmsm33["RECV_MAT_TIME"] = tmmsm01["RECV_MAT_TIME"];

					tmmsm33.Update("RECEIVE_WEIGHT,RCV_MAT_FLAG,RECV_MAT_TIME", "MAT_NO");


					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
					//tmmsm96["RECEIVE_WEIGHT"] = tmmsm01["MAT_WT"];
					//tmmsm01["REAL_TIME_WT"] = tmmsm01["RECEIVE_WEIGHT"];//实时重量 
					tmmsm96["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"].ToDecimal().Round(3);//收货重量，更新实际重量，实际重量即老系统的系统重量   mfj  20240113
					/*tmmsm96["MAT_WT"] = tmmsm01["RECEIVE_WEIGHT"];*/
					tmmsm96["MAT_ACT_WT"] = tmmsm01["RECEIVE_WEIGHT"].ToDecimal().Round(3);

					Log::Trace("", __FUNCTION__, "RECEIVE_WEIGHT[{0}]  ", tmmsm01["RECEIVE_WEIGHT"].ToString());

					//名义规格与实际规格保持一致
					//tmmsm96["MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
					//tmmsm96["MAT_LEN"] = tmmsm01["MAT_LEN"];
					//tmmsm96["MAT_THICK"] = tmmsm01["MAT_THICK"];

					////画面修改名义，用名义覆盖实际 保持两套一致
					//tmmsm96["MAT_ACT_WIDTH"] = tmmsm01["MAT_WIDTH"];
					//tmmsm96["MAT_ACT_LEN"] = tmmsm01["MAT_LEN"];
					//tmmsm96["MAT_ACT_THICK"] = tmmsm01["MAT_THICK"];

					tmmsm96["RECV_MAT_TIME"] = tmmsm01["RECV_MAT_TIME"];//收货日期
					//tmmsm96["PRODUCT_FLAG"] = tmmsm01["PRODUCT_FLAG"];//成品标记
					tmmsm96["RCV_MAT_FLAG"] = "N";//N 未收货  W等待(等L4的反馈)  S收货成功
					tmmsm96["RCV_NO_STATUS"] = "S";//添加收货取消状态，取消后自动收货不再进行收货  mfj   20240309
					tmmsm96["EVENT_ID"] = "MM3B";
					tmmsm96["EVENT_LINE_TYPE"] = "SM";
					tmmsm96["FUNC_ID"] = "mmsmacshf4_pro";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_DESC"] = "铸坯收货确认取消,等待反馈";

					bcls_rec->Tables["MM0099"].Rows.Clear();//先将数据清掉
					bcls_rec->Tables["MM0099"].Rows.Add();
					bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm96);

					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					EIClass inblock;
					inblock.Tables[0].Columns.Add(tmmsm01);
					inblock.Tables[0].Rows.Clear();
					CString v_guide_dest = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
					if (v_guide_dest.Find("1") >= 0)
					{
						tmmsm01.MergeTo(inblock.Tables[0], false);
						doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
						if (doFlag < 0) {
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}

				}

				//修磨
				if (v_event_id == "MM13"&& v_event_type == "N")
				{
					tmmsm34_1["MAT_NO"] = tmmsm01["MAT_NO"];
					//tmmsm34["PROD_SEQ_NO"] = tmmsm33dbsx["RESUME_SEQ_NO"];
					tmmsm34_1["MEND_FLAG"] = tmmsm01["MEND_FLAG"];
					Log::Trace("", "", "MAT_NO = {0} MEND_FLAG ={1}", tmmsm34_1["MAT_NO"].ToString(), tmmsm34_1["MEND_FLAG"].ToString());


					int tmmsm341Count = tmmsm34_1.QueryCount("MAT_NO,MEND_FLAG");
					Log::Trace("", "", "此材料实绩数量 ={0}", tmmsm341Count);


					/*
						对于多条的数据，Query或报错
					*/
					//if (!tmmsm34_1.Query("MAT_NO,MEND_FLAG"))
					//{
					//	//2024-04-24 对于质量封锁的，会生成待办事项，修磨的实绩不会tmmsm34_1实绩表。
					//	tmmsm34["MAT_NO"] = tmmsm01["MAT_NO"];
					//	tmmsm34["MEND_FLAG"] = tmmsm01["MEND_FLAG"];
					//	//修磨记录表中也不存在数据，报错不处理
					//	if (!tmmsm34.Query("MAT_NO,MEND_FLAG"))
					//	{
					//		sprintf(s.msg, "未查到修磨数据！");
					//		throw CApplicationException(-1, s.msg, log.Location);
					//	}
					//	else
					//	{
					//		tmmsm34_1.CopyFrom(tmmsm34);
					//	}

					//}
					/*  tmmsm34_1实绩表,不存在，查询34表 */
					if (tmmsm341Count==0)
					{
						tmmsm34["MAT_NO"] = tmmsm01["MAT_NO"];
						tmmsm34["MEND_FLAG"] = tmmsm01["MEND_FLAG"];
						if (!tmmsm34.Query("MAT_NO,MEND_FLAG"))
						{
							sprintf(s.msg, "未查到修磨数据！");
							throw CApplicationException(-1, s.msg, log.Location);
						}
						else
						{
							tmmsm34_1.CopyFrom(tmmsm34);
						}
					}
					/*  tmmsm34_1实绩表存在且唯一 */
					else if (tmmsm341Count == 1)
					{
						tmmsm34_1.Query("MAT_NO,MEND_FLAG");
					}
					/* tmmsm34_1实绩表存在且不唯一 */
					else if (tmmsm341Count > 1)
					{
						CString matNo = tmmsm01["MAT_NO"].ToString();
						CString sql = "SELECT *  FROM ( SELECT t.* FROM TMMSM34_1 t WHERE t.MAT_NO = '" + matNo + "' ORDER BY t.PROD_SEQ_NO DESC ) temp  WHERE ROWNUM = 1";
						cmd_inq.SetCommandText(sql);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							cmd_inq.Fetch(tmmsm34_1);
						}
						cmd_inq.Close();
					}



					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["MAT_ACT_THICK"] = tmmsm34_1["MAT_ACT_THICK"];
					tmmsm96["MAT_ACT_WIDTH"] = tmmsm34_1["MAT_ACT_WIDTH"];
					tmmsm96["MAT_ACT_LEN"] = tmmsm34_1["MAT_ACT_LEN"];
					tmmsm96["MAT_THICK"] = tmmsm34_1["MAT_ACT_THICK"];
					tmmsm96["MAT_WIDTH"] = tmmsm34_1["MAT_ACT_WIDTH"];
					tmmsm96["MAT_LEN"] = tmmsm34_1["MAT_ACT_LEN"];
					tmmsm96["MAT_THEORY_WT"] = tmmsm34_1["MAT_THEORY_WT"].ToDecimal().Round(3);
					tmmsm96["GRINDING_START_TIME"] = tmmsm34_1["GRINDING_START_TIME"];
					tmmsm96["GRINDING_END_TIME"] = tmmsm34_1["GRINDING_END_TIME"];
					tmmsm96["MEND_FLAG"] = tmmsm34_1["MEND_FLAG"];
					tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"].ToDecimal().Round(3);
					tmmsm96["MEND_AFTER_WEIGHT"] = tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal().Round(3);

					Log::Trace("", "", "MEND_AFTER_WEIGHT={0}", tmmsm96["MEND_AFTER_WEIGHT"].ToString());
					//当为初磨时，用磨后重量字段，当为再磨时，用再磨磨后重量
					if (tmmsm34_1["MEND_FLAG"].ToString().Trim() == "1" ||
						tmmsm34_1["MEND_FLAG"].ToString().Trim() == "2" ||
						tmmsm34_1["MEND_FLAG"].ToString().Trim() == "5")
					{
						//如果磨后量有值，取磨后重量数据，否则取磨前重量
						if (tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal() > 0)
						{
							tmmsm96["LGORT"] = "6246";
							tmmsm96["MAT_WT"] = tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal().Round(3);
							tmmsm96["MAT_ACT_WT"] = tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal().Round(3);
							tmmsm96["COMPLEX_DECIDE_CODE"] = "0";///综判给0  ud给空
					tmmsm96["USAGE_DECISION"] = " ";
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
							tmmsm96["MAT_WT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"].ToDecimal().Round(3);
							tmmsm96["MAT_ACT_WT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"].ToDecimal().Round(3);
						}
					}					else if (tmmsm34_1["MEND_FLAG"].ToString().Trim() == "3" ||
						tmmsm34_1["MEND_FLAG"].ToString().Trim() == "4")
					{
						//如果再磨重量有值，取再磨重量字段的数据，否则取磨后重量字段数据
						if (tmmsm34_1["MEND_SECOND_WEIGHT"].ToDecimal() > 0)
						{
							tmmsm96["LGORT"] = "6246";
							tmmsm96["MAT_WT"] = tmmsm34_1["MEND_SECOND_WEIGHT"].ToDecimal().Round(3);
							tmmsm96["MAT_ACT_WT"] = tmmsm34_1["MEND_SECOND_WEIGHT"].ToDecimal().Round(3);
							tmmsm96["COMPLEX_DECIDE_CODE"] = "0";///综判给0  ud给空
							tmmsm96["USAGE_DECISION"] = " ";

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
							tmmsm96["MAT_WT"] = tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal().Round(3);
							tmmsm96["MAT_ACT_WT"] = tmmsm34_1["MEND_AFTER_WEIGHT"].ToDecimal().Round(3);
						}
					}


					//成品标记根据合同号A开头的 为成品，不是A开头和不带合同号的都是在制品  mfj  杨华确认逻辑  20240409
					// 因为错误率较高，暂不更新，等产销做在制品成品的转换时发送电文时再转换。  mfj  李振  20240409
					if (tmmsm01["ORDER_NO"].ToString().Trim() != "")
					{
						CString pre_whole = Db::QueryCString("select SUBSTR2(WHOLE_BACKLOG, INSTR(WHOLE_BACKLOG, '9A') -2, 2) from tqmom03 WHERE ORDER_NO = '" + tmmsm01["ORDER_NO"].ToString() + "'");
						if (pre_whole.Trim() != "")
						{
							//临钢坯合同按在制品算consign_user_code = '0010000008'
							CString if_lg= Db::QueryCString("select consign_user_code from tqmom01 WHERE ORDER_NO = '" + tmmsm01["ORDER_NO"].ToString() + "'");
							if (if_lg == "0010000008")
							{
								tmmsm96["PRODUCT_FLAG"] = "0";
							}
							else
							{
								if (pre_whole == "A1"|| pre_whole == "A2" || pre_whole == "A5")
								{
									tmmsm96["PRODUCT_FLAG"] = "1";
								}
								else
								{
									tmmsm96["PRODUCT_FLAG"] = "0";
								}
							}
						}
						else
						{
							if (tmmsm01["ORDER_NO"].ToString().Trim().Substring(0, 1) == "A")
							{
								tmmsm96["PRODUCT_FLAG"] = "1";
							}
							else
							{
								tmmsm96["PRODUCT_FLAG"] = "0";
							}
						}
						
					}
					/*else
					{
						tmmsm96["PRODUCT_FLAG"] = "0";
					}*/
					/*if (tmmsm01["ORDER_NO"].ToString().Trim() != "")
					{
						tmmsm96["PRODUCT_FLAG"] = "0";
					}*/

					tmmsm96["RCV_MAT_FLAG"] = "S";
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

					if (false)
					{
						//发送修磨电文
						if (bcls_rec_210036.Tables[0].Rows.get_Count()>0)
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
							bcls_rec_210044.Tables[0].Rows[0]["CUT_SCRAP_WT"] = tmmsm34_1["MEND_SCRAP_WEIGHT"];
							doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
					}

					EIClass inblock;
					inblock.Tables[0].Columns.Add(tmmsm01);
					inblock.Tables[0].Rows.Clear();
					CString v_guide_dest = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
					if (v_guide_dest.Find("1") >= 0)
					{
						//若数据发过，则先发删除，再发新增
						if (tmmsm01["HR_SEND_FLAG"].ToString().Trim() == "1")
						{

							tmmsm01.MergeTo(inblock.Tables[0], false);
							doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
							if (doFlag < 0) {
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
						inblock.Tables[0].Rows.Clear();
						tmmsm01.MergeTo(inblock.Tables[0], false);
						doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
						if (doFlag < 0) {
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
					bcls_rec->Tables["T80RYA"].Rows.Clear();
					bcls_rec->Tables["T80RYA"].Rows.Add();
					bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER"] = "1Q";
					bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER_DIV"] = "1";
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NUM"] = 1;
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_LINE_TYPE"] = tmmsm01["MAT_LINE_TYPE"];
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
					bcls_rec->Tables["T80RYA"].Rows[0]["FACTORY_DIV"] = "LG1";
					bcls_rec->Tables["T80RYA"].Rows[0]["USER_ID"] = "cm_0rt801_rcv";
					if (!bcls_rec->Tables["T80RYA"].Columns.Contains("STOCK_OPER_TIME"))
						bcls_rec->Tables["T80RYA"].Columns.Add(DT_STRING, "STOCK_OPER_TIME");

					bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_TIME"] = datetime;
					bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_NO"] = "AK2";
					bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_PLACE_NO"] = "0";
					bcls_rec->Tables["T80RYA"].Rows[0]["TO_LAYERNO"] = 0;
					doFlag = f_mmsm_t80rya_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				//修磨撤销
				if (v_event_id == "MM13"&& v_event_type == "D"){

					tmmsm34_1["MAT_NO"] = tmmsm01["MAT_NO"];
					tmmsm34_1["MEND_FLAG"] = tmmsm01["MEND_FLAG"];

					Log::Trace("", "", "MAT_NO = {0} MEND_FLAG ={1}", tmmsm34_1["MAT_NO"].ToString(), tmmsm34_1["MEND_FLAG"].ToString());

					if (!tmmsm34_1.Query("MAT_NO,MEND_FLAG"))
					{
						sprintf(s.msg, "未查到修磨数据！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//获取主键
					sqlstr = " select PROD_SEQ_NO ,MAT_NO  from tmmsm34_1 t where  t.MAT_NO = '" + tmmsm34_1["MAT_NO"].ToString() + "' and t.MEND_FLAG = '" + tmmsm34_1["MEND_FLAG"].ToString() + "' ";
					CString PROD_SEQ_NO = "";
					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.ExecuteReader();
					if (cmd_sql.Read())
					{
						PROD_SEQ_NO = cmd_sql.GetString(1);
					}
					cmd_sql.Close();

					tmmsm34_1["PROD_SEQ_NO"] = PROD_SEQ_NO;
					tmmsm34_1.Query();
					CString isUpload = tmmsm34_1["ISUPLOAD"].ToString();


					//修磨撤销时，综判置为合格，因为做修磨实绩之前一定是合格才会做(待办里是先处理待办再置合格)，故撤销时直接将综判置为合格


					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["USAGE_DECISION"] = "3001";
					tmmsm96["COMPLEX_DECIDE_CODE"] = "1";
					tmmsm96["MAT_ACT_THICK"] = tmmsm34_1["MAT_ACT_THICK"];
					tmmsm96["MAT_ACT_WIDTH"] = tmmsm34_1["MAT_ACT_WIDTH"];
					tmmsm96["MAT_ACT_LEN"] = tmmsm34_1["MAT_ACT_LEN"];
					tmmsm96["MAT_THICK"] = tmmsm34_1["MAT_ACT_THICK"];
					tmmsm96["MAT_WIDTH"] = tmmsm34_1["MAT_ACT_WIDTH"];
					tmmsm96["MAT_LEN"] = tmmsm34_1["MAT_ACT_LEN"];
					tmmsm96["MAT_THEORY_WT"] = tmmsm34_1["MAT_THEORY_WT"].ToDecimal().Round(3);

					tmmsm96["GRINDING_START_TIME"] = tmmsm34_1["GRINDING_START_TIME"];
					tmmsm96["GRINDING_END_TIME"] = tmmsm34_1["GRINDING_END_TIME"];


					tmmsm96["MEND_BEFORE_WEIGHT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"];
					tmmsm96["LGORT"] = "6242";
					tmmsm96["MAT_WT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"].ToDecimal().Round(3);
					tmmsm96["MAT_ACT_WT"] = tmmsm34_1["MEND_BEFORE_WEIGHT"].ToDecimal().Round(3);
					tmmsm96["MEND_AFTER_WEIGHT"] = 0;  //磨后量清零
					tmmsm96["MEND_FEEDBACK_FLAG"] = "0";
					tmmsm96["RCV_MAT_FLAG"] = "S";;
					tmmsm96["PRODUCT_FLAG"] = "0";
					tmmsm96["EVENT_ID"] = "MM12";
					tmmsm96["EVENT_LINE_TYPE"] = "SM";
					tmmsm96["FUNC_ID"] = "f_mmsm33dbsx_proc";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_DESC"] = "铸坯修磨实绩撤销处理!";


					if (isUpload == "3")
					{
						tmmsm34_1.Delete();
						tmmsm96["MEND_FLAG"] = "0";

					}
					else  if (isUpload == "4")
					{
						tmmsm96["MEND_FLAG"] = tmmsm34_1["MEND_FLAG"];
					}


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


					EIClass inblock;
					inblock.Tables[0].Columns.Add(tmmsm01);
					inblock.Tables[0].Rows.Clear();
					CString v_guide_dest = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
					if (v_guide_dest.Find("1") >= 0)
					{
						//若数据发过，则先发删除，再发新增
						if (tmmsm01["HR_SEND_FLAG"].ToString().Trim() == "1")
						{

							tmmsm01.MergeTo(inblock.Tables[0], false);
							doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
							if (doFlag < 0) {
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
						inblock.Tables[0].Rows.Clear();
						tmmsm01.MergeTo(inblock.Tables[0], false);
						doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
						if (doFlag < 0) {
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
					bcls_rec->Tables["T80RYA"].Rows.Clear();
					bcls_rec->Tables["T80RYA"].Rows.Add();
					bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER"] = "1Q";
					bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_ORDER_DIV"] = "1";
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_NUM"] = 1;
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_LINE_TYPE"] = tmmsm01["MAT_LINE_TYPE"];
					bcls_rec->Tables["T80RYA"].Rows[0]["MAT_KIND"] = tmmsm01["MAT_KIND"];
					bcls_rec->Tables["T80RYA"].Rows[0]["FACTORY_DIV"] = "LG1";
					bcls_rec->Tables["T80RYA"].Rows[0]["USER_ID"] = "cm_0rt801_rcv";
					if (!bcls_rec->Tables["T80RYA"].Columns.Contains("STOCK_OPER_TIME"))
						bcls_rec->Tables["T80RYA"].Columns.Add(DT_STRING, "STOCK_OPER_TIME");

					bcls_rec->Tables["T80RYA"].Rows[0]["STOCK_OPER_TIME"] = datetime;
					bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_NO"] = "AK3";
					bcls_rec->Tables["T80RYA"].Rows[0]["TO_STOCK_PLACE_NO"] = "0";
					bcls_rec->Tables["T80RYA"].Rows[0]["TO_LAYERNO"] = 0;
					doFlag = f_mmsm_t80rya_snd(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				//切废
				if (v_event_id == "MM19"&& v_event_type == "N")
				{

					tmmsm39["MAT_NO"] = tmmsm01["MAT_NO"];
					tmmsm39["FINISH_FLAG"] = "1";// 1 新增已处理，未反馈   3 删除已处理，未反馈  9 处理成功   
					if (!tmmsm39.Query("MAT_NO,FINISH_FLAG"))
					{

						tmmsm39_1["MAT_NO"] = tmmsm01["MAT_NO"];
						tmmsm39_1["FINISH_FLAG"] = "1";
						if (!tmmsm39_1.Query("MAT_NO,FINISH_FLAG"))
						{
							sprintf(s.msg, "未查到改切数据！");
							throw CApplicationException(-1, s.msg, log.Location);
						}

						tmmsm39.CopyFrom(tmmsm39_1);
					}


					tmmsm96.CopyFrom(tmmsm01);
					//名义规格与实际规格保持一致
					tmmsm96["MAT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
					tmmsm96["MAT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
					tmmsm96["MAT_THICK"] = tmmsm39["CUT_AFTER_THICK"];

					//画面切废，规格取切后长宽厚
					tmmsm96["MAT_NO"] = tmmsm01["MAT_NO"];
					tmmsm96["MAT_ACT_WIDTH"] = tmmsm39["CUT_AFTER_WIDTH"];
					tmmsm96["MAT_ACT_LEN"] = tmmsm39["CUT_AFTER_LEN"];
					tmmsm96["MAT_ACT_THICK"] = tmmsm39["CUT_AFTER_THICK"];

					//获取计算重量    
					//首先判断钢种前两位   系数  1A 7.86  1D 7.76   1F  7.83   1M 7.83
					//若以上判断获取不到，则判断钢种第一位   1 7.85  2 7.82  3 7.82
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

					tmmsm96["MAT_WT"] = tmmsm39["CUT_AFTER_WT"].ToDecimal().Round(3);//切后重量
					tmmsm96["MAT_ACT_WT"] = tmmsm39["CUT_AFTER_WT"].ToDecimal().Round(3);//系统重量即实际重量
					//tmmsm96["REAL_TIME_WT"] = tmmsm39["CUT_AFTER_WT"];//实时重量
					tmmsm96["QUALIFIED_WT"] = tmmsm39["CUT_AFTER_WT"].ToDecimal().Round(3);//合格产量
					tmmsm96["RCV_MAT_FLAG"] = "S";
					tmmsm96["FINISH_FLAG"] = "9";
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

					//处理成功后将标记更改为9
					if (!tmmsm39.Query("MAT_NO,FINISH_FLAG"))
					{
						tmmsm39_1["MAT_NO"] = tmmsm01["MAT_NO"];
						tmmsm39_1["FINISH_FLAG"] = "1";
						if (tmmsm39_1.Query("MAT_NO,FINISH_FLAG"))
						{
							tmmsm39_1["FINISH_FLAG"] = "9";
							tmmsm39_1.Update("FINISH_FLAG", "RESUME_SEQ_NO,MAT_NO");
						}
						
					}
					else
					{
						tmmsm39_1["MAT_NO"] = tmmsm01["MAT_NO"];
						tmmsm39_1["FINISH_FLAG"] = "1";// 1 新增已处理，未反馈   3 删除已处理，未反馈  9 处理成功   
						if (tmmsm39_1.Query("MAT_NO,FINISH_FLAG"))
						{
							tmmsm39_1["FINISH_FLAG"] = "9";
							tmmsm39_1.Update("FINISH_FLAG", "RESUME_SEQ_NO,MAT_NO");
						}
						tmmsm39["FINISH_FLAG"] = "9";// 1 新增已处理，未反馈   3 删除已处理，未反馈  9 处理成功   
						tmmsm39.Update("FINISH_FLAG", "RESUME_SEQ_NO,MAT_NO");
					}


					//此程序为反馈接收，表示已经发过了改切实绩了，不需要发送了
					if (false)
					{
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


					EIClass inblock;
					inblock.Tables[0].Columns.Add(tmmsm01);
					inblock.Tables[0].Rows.Clear();
					CString v_guide_dest = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
					if (v_guide_dest.Find("1") >= 0)
					{
						//若数据发过，则先发删除，再发新增
						if (tmmsm01["HR_SEND_FLAG"].ToString().Trim() == "1")
						{

							tmmsm01.MergeTo(inblock.Tables[0], false);
							doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
							if (doFlag < 0) {
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
						inblock.Tables[0].Rows.Clear();
						tmmsm01.MergeTo(inblock.Tables[0], false);
						doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
						if (doFlag < 0) {
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}


				}

				//分切
				if (v_event_id == "MM11"&& v_event_type == "N")
				{

					int y = 1;

					//反馈中的材料号更改为子坯材料号，根据子坯材料号查找记录   mfj  李振  20240528
					bcls_rec_TMMSM35.Tables[0].Rows.Clear();
					sqlstr = "SELECT * FROM TMMSM35 WHERE MAT_NO = '" + tmmsm35["MAT_NO"].ToString() + "' order by PROD_SEQ_NO asc";
					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.ExecuteQuery(bcls_rec_TMMSM35.Tables[0]);
					cmd_sql.Close();

					//将前面根据电文材料号查询到的数据清除掉，重新获取到数据
					/*tmmsm35.Reset();
					tmmsm35.MergeFrom(bcls_rec_TMMSM35.Tables[0].Rows[0]);*/

					//在前面已经从母坯中获取到数据
					/*tmmsm01["MAT_NO"] = tmmsm01["MAT_NO"];
					tmmsm01.Query();*/
					tmmsm01_slab["MAT_NO"] = tmmsm35["IN_MAT_NO"];
					tmmsm01_slab.Query();


					matTheoryWt = tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);

					if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
					{
						bcls_rec->Tables["MM0099"].Rows.Add();
					}

					//前面已经根据母坯号查询了在线历史档数据
					//若能查询到，则表示母坯未归档，则可以修改标记,给2250的信息也只发一次母坯的删除

					if (tmmsm01_slab.QueryCount("MAT_NO") > 0)
					{
						//调用事件  更新母坯的收货标记
						bcls_rec->Tables["MM0099"].Rows.Clear();
						bcls_rec->Tables["MM0099"].Rows.Add();
						bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm01_slab);
						bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM3F";
						bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
						bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
						bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "f_mmsm33dbsx_proc";
						bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm35["IN_MAT_NO"];
						bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = "S";
						bcls_rec->Tables["MM0099"].Rows[0]["DIV_FLAG"] = "1";

						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}

						
						
						EIClass inblock;
						inblock.Tables[0].Columns.Add(tmmsm01_slab);
						inblock.Tables[0].Rows.Clear();
						CString v_guide_dest = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
						if (v_guide_dest.Find("1") >= 0)
						{
							//分切  先发母坯的删除   再发子坯的新增
							/*if (tmmsm01["HR_SEND_FLAG"].ToString().Trim() == "1")
							{*/
							tmmsm01_slab.MergeTo(inblock.Tables[0], false);
							doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
							if (doFlag < 0) {
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
							//}

						}
						
					}

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
						tmmsm01["MAT_ACT_WT"] = tmmsm35["MAT_WT"].ToDecimal().Round(3);
						tmmsm01["MAT_WT"] = tmmsm35["MAT_WT"].ToDecimal().Round(3);
						tmmsm01["RCV_MAT_FLAG"] = "S";
						len_tm35 = tmmsm35["MAT_LEN"].ToDecimal();

						//批次号与母坯号一致的，保留母坯的收货重量
						if (tmmsm35["BATCH"].ToString().Trim() == tmmsm35["IN_MAT_NO"].ToString().Trim())
						{
							tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["RECEIVE_WEIGHT"];
							tmmsm01["MEND_BEFORE_WEIGHT"] = tmmsm01["MEND_BEFORE_WEIGHT"];
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
							//tmmsm01["ORDER_NO"] = " ";//合同号 子坯是否有合同号根据是否有命令板坯决定  lz 20240920
							tmmsm01["PRINT_NO"] = " ";//喷印号 
							tmmsm01["PONO_SLAB"] = " ";
							int v_pono_count = 1;
							//将命令坯置空
							while (v_pono_count < 13)
							{
								tmmsm01["PONO_SLAB_" + CConvert::ToString(v_pono_count)] = " ";
								v_pono_count++;
							}

							//tmmsm01["LSLAB_NO"] = " ";//长坯号-虚拟板坯号 子坯是否有长坯号根据是否有命令板坯决定  lz 20240920
							tmmsm01["FIX_SLAB_NUM"] = 0;
							//tmmsm01["SLAB_TYPE_OLD"] = "3";

							tmmsm01["SLAB_NO"] = tmmsm01["SLAB_NO"].ToString().SubstringNE(0, 15) + tmmsm01["BATCH"].ToString().Substring(8, 2)
								+ tmmsm01["SLAB_NO"].ToString().SubstringNE(17);
							Log::Trace("", __FUNCTION__, "SLAB_NO=[{0}]", tmmsm01["SLAB_NO"].ToString().Trim());

						}

						//tmmsm01["RECEIVE_WEIGHT"] = tmmsm01["MAT_ACT_WT"];//暂定 收货重量取切后重量，  后面待确认  mfj  20240131  收货重量为0 杨姐确认  mfj  20240319
						tmmsm01["MAT_ACT_WT"] = tmmsm35["MAT_WT"].ToDecimal().Round(3);//系统重量
						//tmmsm01["REAL_TIME_WT"] = tmmsm35["MAT_WT"];//实时重量
						tmmsm01["QUALIFIED_WT"] = tmmsm35["MAT_WT"].ToDecimal().Round(3);//合格产量
						tmmsm01["MAT_THEORY_WT"] = matTheoryWt / tmmsm35["IN_MAT_TUBE"].ToDecimal() / tmmsm35["IN_MAT_LEN"].ToDecimal() * tmmsm35["MAT_TUBE"].ToDecimal() * tmmsm35["MAT_LEN"];
						tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);

						for (int pono_35 = 1; pono_35 <= 12; pono_35++)
						{
							if (pono_35 == 1)
							{
								tmmsm01["PONO_SLAB"] = tmmsm35["PONO_SLAB_" + CConvert::ToString(pono_35)];
							}
							tmmsm01["PONO_SLAB_" + CConvert::ToString(pono_35)] = tmmsm35["PONO_SLAB_" + CConvert::ToString(pono_35)];
						}
						if (tmmsm01["PONO_SLAB"].ToString().Trim()=="")
						{
							tmmsm01["ORDER_NO"] = " ";//合同号
							tmmsm01["LSLAB_NO"] = " ";//长坯号-虚拟板坯号
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

							tmmsm01["PRODUTE_CAL_WT"] = ((tmmsm01["MAT_ACT_WIDTH"].ToDecimal() / 1000) * (tmmsm01["MAT_ACT_LEN"].ToDecimal() / 1000) * (tmmsm01["MAT_ACT_THICK"].ToDecimal() / 1000) * v_code_wt).Round(3);

						}



						//在画面新增的时候就已经将命令坯匹配过，并存入TMMSM35表中
						if (false)
						{
							EIClass bcls_tmmsm01;
							bcls_tmmsm01.Tables[0].Columns.Add(tmmsm01);

							//根据虚拟板坯号查找已使用的命令坯，并将每次循环的上一个循环的命令坯排除掉
							sqlstr = " SELECT * FROM TPSSM03 WHERE LSLAB_NO = '" + tmmsm01_slab["LSLAB_NO"].ToString().Trim() + "' AND  SLAB_PROD_FLAG = '1'"
								" AND  SLAB_NO NOT IN (" + v_ponoslab + ")";;

							Log::Trace("", __FUNCTION__, "v_ponoslab=[{0}]", v_ponoslab);
							Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.ExecuteQuery(bcls_tmmsm01.Tables[0]);
							cmd_inq.Close();

							for (int z = 0; z < bcls_tmmsm01.Tables[0].Rows.get_Count(); z++)
							{
								int v_pono_slabcount = z + 1;
								tpssm03.Reset();
								tpssm03.MergeFrom(bcls_tmmsm01.Tables[0].Rows[z]);
								//Log::Trace("", __FUNCTION__, "55555555=[{0}]", "PONO_SLAB_" + CConvert::ToString(y));


								if (tpssm03["SLAB_NO"].ToString().Trim() == "")//如果没有命令坯号，则跳过
								{
									continue;
								}


								//如果实际长度大于命令坯最大长度，则匹配成功
								//如果实际长度在命令坯范围内，则匹配成功
								//如果实际长度小于命令坯最小值，则匹配失败，跳过
								if ((len_tm35 > tpssm03["SLAB_MAX_LEN"].ToDecimal()) ||
									(len_tm35 >= tpssm03["SLAB_MIN_LEN"].ToDecimal() && len_tm35 <= tpssm03["SLAB_MAX_LEN"].ToDecimal()))
								{
									if (z == 1)
									{
										tmmsm01["PONO_SLAB"] = tpssm03["SLAB_NO"];
									}
									tmmsm01["PONO_SLAB_" + CConvert::ToString(v_pono_slabcount)] = tpssm03["SLAB_NO"];
									v_ponoslab = v_ponoslab + ",'" + tpssm03["SLAB_NO"].ToString().Trim() + "'";
									len_tm35 = len_tm35 - tpssm03["SLAB_LEN"].ToDecimal();

								}
								else if (len_tm35 < tpssm03["SLAB_MIN_LEN"].ToDecimal())
								{
									if (z == 1)
									{
										tmmsm01["PONO_SLAB"] = " ";
									}
									tmmsm01["PONO_SLAB_" + CConvert::ToString(v_pono_slabcount)] = " ";
								}
							}
						}
						tmmsm01["LOGISTICS_STATUS"] = "0";
						tmmsm01["PRE_LOAD_FLAG"] = "0";
						tmmsm01["FACTORY_TO"] = " ";
						tmmsm01["DST_STOCK_CODE"] = " ";
						tmmsm01["UNLOAD_CODE"] = " ";
						tmmsm01["LOAD_SCHEME_NO"] = " ";
						tmmsm01["PRACTICE_NO"] = " ";






						bcls_rec->Tables["MM0099"].Rows.Clear();
						tmmsm96.Reset();
						tmmsm96.CopyFrom(tmmsm01);
						tmmsm96["EVENT_ID"] = "MM15";
						tmmsm96["EVENT_LINE_TYPE"] = "SM";
						tmmsm96["SYSTEM_ID"] = "MMSM";
						tmmsm96["FUNC_ID"] = "mmsm35_cut";
						tmmsm96["EVENT_DESC"] = "材料分切产出";
						tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
						Log::Trace("", __FUNCTION__, "4444");
						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}


						if (false)
						{
							bcls_rec_210034.Tables[0].Rows.Add();
							bcls_rec_210034.Tables[0].Rows[j].Merge(tmmsm01);
							bcls_rec_210034.Tables[0].Rows[j]["MAT_NO"] = tmmsm01["MAT_NO"];
							bcls_rec_210034.Tables[0].Rows[j]["DEAL_FLAG"] = "N";//新增
						}


						//上面先发了母坯的删除   这里处理子坯的新增
						EIClass inblock;
						inblock.Tables[0].Columns.Add(tmmsm01);
						inblock.Tables[0].Rows.Clear();
						CString v_guide_dest = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
						if (v_guide_dest.Find("1") >= 0)
						{
							//若数据发过，则先发删除，再发新增
							inblock.Tables[0].Rows.Clear();
							tmmsm01.MergeTo(inblock.Tables[0], false);
							doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
							if (doFlag < 0) {
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}



					}
					Log::Trace("", __FUNCTION__, "5555");

					
					
					if (tmmsm01_slab.QueryCount("MAT_NO") > 0)
					{
						bcls_rec->Tables["MM0099"].Rows.Clear();
						bcls_rec->Tables["MM0099"].Rows.Add();
						bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm01_slab);
						bcls_rec->Tables["MM0099"].Rows[0]["ARCHIVE_TIME"] = datetime;
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
					Log::Trace("", __FUNCTION__, "6666");

					//此程序为反馈接收，表示已经发过了二切实绩了，不需要发送了
					if (false)
					{
						/********   太钢定制 发送L4电文 二切实绩   ***********/
						if (bcls_rec_210034.Tables[0].Rows.get_Count()>0)
						{
							doFlag = f_mmsm_210034_snd(&bcls_rec_210034, bcls_ret, conn);
							if (doFlag < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}
						}
					}


				}

				//切废撤销
				if (v_event_id == "MM19"&& v_event_type == "D")
				{
					tmmsm39["MAT_NO"] = tmmsm01["MAT_NO"];
					tmmsm39["FINISH_FLAG"] = "3";// 1 新增已处理，未反馈   3 删除已处理，未反馈  9 处理成功   
					if (!tmmsm39.Query("MAT_NO,FINISH_FLAG"))
					{
						Log::Trace("", __FUNCTION__, "未查到TMMSM39的改切数据[{0}] MAT_NO =[{1}] ", tmmsm39["FINISH_FLAG"].ToString(), tmmsm39["MAT_NO"].ToString());
						tmmsm39_1["MAT_NO"] = tmmsm01["MAT_NO"];
						tmmsm39_1["FINISH_FLAG"] = "3";
						if (!tmmsm39_1.Query("MAT_NO,FINISH_FLAG"))
						{
							Log::Trace("", __FUNCTION__, "未查到TMMSM39_1的改切数据[{0}] MAT_NO =[{1}] ", tmmsm39["FINISH_FLAG"].ToString(), tmmsm39["MAT_NO"].ToString());
							continue;//当没有查到则不处理  因为修改时是给1标记，此时不处理
						}

						tmmsm39.CopyFrom(tmmsm39_1);

					}

					tmmsm96.CopyFrom(tmmsm01);
					tmmsm96["MAT_NO"] = tmmsm39["MAT_NO"];
					tmmsm96["MAT_WIDTH"] = tmmsm39["CUT_BEFORE_WIDTH"];
					tmmsm96["MAT_LEN"] = tmmsm39["CUT_BEFORE_LEN"];
					tmmsm96["MAT_THICK"] = tmmsm39["CUT_BEFORE_THICK"];

					//画面切废，规格取切后长宽厚
					tmmsm96["MAT_ACT_WIDTH"] = tmmsm39["CUT_BEFORE_WIDTH"];
					tmmsm96["MAT_ACT_LEN"] = tmmsm39["CUT_BEFORE_LEN"];
					tmmsm96["MAT_ACT_THICK"] = tmmsm39["CUT_BEFORE_THICK"];
					tmmsm96["MAT_WT"] = tmmsm39["CUT_BEFORE_WT"].ToDecimal().Round(3);//切前重量
					tmmsm96["MAT_ACT_WT"] = tmmsm39["CUT_BEFORE_WT"].ToDecimal().Round(3);//系统重量即实际重量
					//tmmsm96["REAL_TIME_WT"] = tmmsm39["CUT_BEFORE_WT"];//实时重量
					tmmsm96["QUALIFIED_WT"] = tmmsm39["CUT_BEFORE_WT"].ToDecimal().Round(3);//合格产量

					//获取计算重量    
					//首先判断钢种前两位   系数  1A 7.86  1D 7.76   1F  7.83   1M 7.83
					//若以上判断获取不到，则判断钢种第一位   1 7.85  2 7.82  3 7.82
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

					tmmsm96["RCV_MAT_FLAG"] = "S";
					tmmsm96["FINISH_FLAG"] = "9";
					tmmsm96["EVENT_ID"] = "MM39";
					tmmsm96["EVENT_LINE_TYPE"] = "SM";
					tmmsm96["FUNC_ID"] = "f_mmsm39_proc";
					tmmsm96["SYSTEM_ID"] = "MMSM";
					tmmsm96["EVENT_DESC"] = "钢坯切废实绩删除";

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

					//前面处理没问题，这里直接删除记录
					if (!tmmsm39.Query("MAT_NO,FINISH_FLAG"))
					{
						tmmsm39_1["MAT_NO"] = tmmsm01["MAT_NO"];
						tmmsm39_1["FINISH_FLAG"] = "3";
						if (tmmsm39_1.Query("MAT_NO,FINISH_FLAG"))
						{
							tmmsm39_1.Delete("FINISH_FLAG,MAT_NO,RESUME_SEQ_NO");
						}
					}
					else
					{
						tmmsm39_1["MAT_NO"] = tmmsm01["MAT_NO"];
						tmmsm39_1["FINISH_FLAG"] = "3";
						if (tmmsm39_1.Query("MAT_NO,FINISH_FLAG"))
						{
							tmmsm39_1.Delete("FINISH_FLAG,MAT_NO,RESUME_SEQ_NO");
						}
						tmmsm39.Delete("FINISH_FLAG,MAT_NO,RESUME_SEQ_NO");
					}

					EIClass inblock;
					inblock.Tables[0].Columns.Add(tmmsm01);
					inblock.Tables[0].Rows.Clear();
					CString v_guide_dest = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
					if (v_guide_dest.Find("1") >= 0)
					{
						//若数据发过，则先发删除，再发新增
						if (tmmsm01["HR_SEND_FLAG"].ToString().Trim() == "1")
						{

							tmmsm01.MergeTo(inblock.Tables[0], false);
							doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
							if (doFlag < 0) {
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
						inblock.Tables[0].Rows.Clear();
						tmmsm01.MergeTo(inblock.Tables[0], false);
						doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
						if (doFlag < 0) {
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}


				}

				//分切撤销
				if (v_event_id == "MM11"&& v_event_type == "D")
				{
					//因为此时子坯还未删除，故前面的查询可以获取到数据，所以此处用TMMSM01的mat_no
					bcls_rec_TMMSM35.Tables[0].Rows.Clear();
					sqlstr = "SELECT * FROM TMMSM35 WHERE MAT_NO = '" + tmmsm01["MAT_NO"].ToString() + "' order by PROD_SEQ_NO asc";
					cmd_sql.SetCommandText(sqlstr);
					cmd_sql.ExecuteQuery(bcls_rec_TMMSM35.Tables[0]);
					cmd_sql.Close();

					

					for (int i = 0; i < bcls_rec_TMMSM35.Tables[0].Rows.get_Count(); i++)
					{
						tmmsm35.Reset();
						tmmsm35.MergeFrom(bcls_rec_TMMSM35.Tables[0].Rows[i]);

						tmmsm01_zp.Reset();
						tmmsm01_zp["MAT_NO"] = bcls_rec_TMMSM35.Tables[0].Rows[i]["MAT_NO"].ToString();
						tmmsm01_zp.Query("MAT_NO");



						//因先调99删坯子，发送电文更新履历时会报错，故先发电文，后删除坯子
						EIClass inblock;
						inblock.Tables[0].Columns.Add(tmmsm01_zp);
						inblock.Tables[0].Rows.Clear();
						CString v_guide_dest = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01_zp["GUIDE_DEST"].ToString() + "' ");
						if (v_guide_dest.Find("1") >= 0)
						{
							//若数据发过，则先发删除，再发新增
							/*if (tmmsm01_zp["HR_SEND_FLAG"].ToString().Trim() == "1")
							{*/

							tmmsm01_zp.MergeTo(inblock.Tables[0], false);
							doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
							if (doFlag < 0) {
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
							//}

						}

						bcls_rec->Tables["MM0099"].Rows.Clear();
						tmmsm96.Reset();
						tmmsm96.CopyFrom(tmmsm01_zp);
						tmmsm96["EVENT_ID"] = "MM18";
						tmmsm96["EVENT_LINE_TYPE"] = "SM";
						tmmsm96["SYSTEM_ID"] = "MMSM";
						tmmsm96["FUNC_ID"] = "mmsm35f9_del";
						tmmsm96["EVENT_DESC"] = "材料分切子坯删除";
						tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}

						//分切表数据删除
						tmmsm35["MAT_NO"] = tmmsm01_zp["MAT_NO"];
						tmmsm35.Delete("MAT_NO");

						

					}

					//将母坯数据重新获取
					sqlstr = "SELECT * FROM VMMSM01 WHERE MAT_NO = '" + tmmsm35["IN_MAT_NO"].ToString().Trim() + "' ";
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						cmd_inq.Fetch(tmmsm01);
					}
					else
					{
						sprintf(s.msg, "未查到板坯数据！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					cmd_inq.Close();
					

					//前面已经根据母坯号查询在线和历史档的母坯信息了
					//若在线表查不到，则表示母坯信息尚未拉回，此时要进行拉回在线档操作
					if (tmmsm01.QueryCount("MAT_NO") <= 0 && tmmsm01_zp["BATCH"].ToString().Trim() == tmmsm01["BATCH"].ToString().Trim())
					{
						bcls_rec->Tables["MM0099"].Rows.Clear();
						tmmsm96.Reset();
						tmmsm96["MAT_NO"] = tmmsm01_zp["IN_MAT_NO"];
						tmmsm96["ARCHIVE_TIME"] = " ";
						tmmsm96["EVENT_ID"] = "MM19";
						tmmsm96["EVENT_LINE_TYPE"] = "SM";
						tmmsm96["SYSTEM_ID"] = "MMSM";
						tmmsm96["FUNC_ID"] = "mmsm35f9_del";
						tmmsm96["EVENT_DESC"] = "材料分切撤销母坯拉回";
						tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);

						doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}

						tmmsm01["DIV_FLAG"] = " ";
						tmmsm01.Update("DIV_FLAG", "MAT_NO");


						EIClass inblock;
						inblock.Tables[0].Columns.Add(tmmsm01);
						inblock.Tables[0].Rows.Clear();
						CString v_guide_dest = Db::QueryCString(" select CODE_DESC_3_CONTENT from TWMSMZD02 where CODE_CLASS = 'WM02' and CODE ='" + tmmsm01["GUIDE_DEST"].ToString() + "' ");
						if (v_guide_dest.Find("1") >= 0)
						{
							//上面先发了子坯的删除   这里发母坯的新增
							/*if (tmmsm01["HR_SEND_FLAG"].ToString().Trim() == "1")
							{

							tmmsm01.MergeTo(inblock.Tables[0], false);
							doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
							if (doFlag < 0) {
							throw CApplicationException(-1, s.msg, s.svc_name);
							}
							}*/
							inblock.Tables[0].Rows.Clear();
							tmmsm01.MergeTo(inblock.Tables[0], false);
							doFlag = f_wmsm_t8p301_snd(&inblock, bcls_ret, conn);
							if (doFlag < 0) {
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
					}
				}

				if (v_event_id == "MM3D")
				{
					//调用事件  //只修改标记   收货取消  将收获标记置为N
					bcls_rec->Tables["MM0099"].Rows.Add();
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM3F";
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
					bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
					bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "cm_0rt801_rcv";
					bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
					bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = "W";
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "废品转正品处理成功！";

					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				if ((v_event_id == "MM09" &&v_event_type == "N") || v_event_id == "MM13"&& v_event_type == "N")
				{
					bcls_rec->Tables["MM0099"].Rows.Clear();
					bcls_rec->Tables["MM0099"].Rows.Add();
					bcls_rec->Tables["MM0099"].Rows[0]["SIZE_DECIDE_CODE"] = "1001";
					
					Log::Trace("", __FUNCTION__, "01表初判=[{0}]", tmmsm01["CASTING_PRE_JUDGMENT"].ToString());
					Log::Trace("", __FUNCTION__, "01表MAT_NO=[{0}]", tmmsm01["MAT_NO"].ToString());
					/*if (tmmsm01["CASTING_PRE_JUDGMENT"].ToString().Trim().GetLength() > 0)
					{
						Log::Trace("", __FUNCTION__, "01表初判开头=[{0}]", tmmsm01["CASTING_PRE_JUDGMENT"].ToString().Trim().Substring(0, 1));
					}
					else
					{
						Log::Trace("", __FUNCTION__, "01表无初判字段=[{0}]", tmmsm01["CASTING_PRE_JUDGMENT"].ToString());
					}*/

					/*//获取主键
					sqlstr_1 = " SELECT T.MAT_NO,T.CASTING_PRE_JUDGMENT,TQ01.ORDER_THICK \
						FROM VMMSM01 T LEFT JOIN TQMOM01 TQ01 ON  T.ORDER_NO = TQ01.ORDER_NO\
						WHERE 1 = 1 AND T.MAT_NO = '" + tmmsm01["MAT_NO"].ToString() + "' ";
					CString CASTING_PRE_JUDGMENT = "";
					CDecimal ORDER_THICK = 0;
					cmd_sql_1.SetCommandText(sqlstr_1);
					cmd_sql_1.ExecuteReader();
					if (cmd_sql_1.Read())
					{
						CASTING_PRE_JUDGMENT = cmd_sql_1.GetString(2);
						ORDER_THICK = cmd_sql_1.GetDecimal(3);
					}
					cmd_sql_1.Close();*/

					////获取配置的订货厚度标准
					//sqlstr_st = " SELECT TO_NUMBER(T.CODE) FROM TWMSMZD02 T WHERE T.CODE_CLASS = 'ORDER_THICK_ST'";
					//CDecimal ORDER_THICK_ST = 0;
					//cmd_sql_st.SetCommandText(sqlstr_st);
					//cmd_sql_st.ExecuteReader();
					//if (cmd_sql_st.Read())
					//{
					//	ORDER_THICK_ST = cmd_sql_st.GetDecimal(1);
					//}
					//cmd_sql_st.Close();

					//Log::Trace("", __FUNCTION__, "CASTING_PRE_JUDGMENT=[{0}]", CASTING_PRE_JUDGMENT);
					//Log::Trace("", __FUNCTION__, "ORDER_THICK=[{0}]", ORDER_THICK);
					//Log::Trace("", __FUNCTION__, "ORDER_THICK_ST=[{0}]", ORDER_THICK_ST);

					//初判为C或D开头,订货厚度不为0且<=0.7时,SLAB_CHECK_RESULT为1005
					/*if (tmmsm01["CASTING_PRE_JUDGMENT"].ToString().Trim().GetLength() > 0)
					{
						if (tmmsm01["CASTING_PRE_JUDGMENT"].ToString().Trim().Substring(0, 1) == "C" || tmmsm01["CASTING_PRE_JUDGMENT"].ToString().Trim().Substring(0, 1) == "D")
						{
							if (ORDER_THICK != 0 && ORDER_THICK <= ORDER_THICK_ST)
							{
								bcls_rec->Tables["MM0099"].Rows[0]["SLAB_CHECK_RESULT"] = "1005";
							}
							else
							{
								bcls_rec->Tables["MM0099"].Rows[0]["SLAB_CHECK_RESULT"] = "1001";
							}
						}
						else
						{
							bcls_rec->Tables["MM0099"].Rows[0]["SLAB_CHECK_RESULT"] = "1001";
						}
					}
					else
					{
						bcls_rec->Tables["MM0099"].Rows[0]["SLAB_CHECK_RESULT"] = "1001";
					}*/
					bcls_rec->Tables["MM0099"].Rows[0]["SLAB_CHECK_RESULT"] = "1001";
					if (!bcls_rec->Tables["MM0099"].Columns.Contains("SURFACE_DECIDE_CODE"))
						bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");
					//bcls_rec->Tables["MM0099"].Rows[0]["SURFACE_DECIDE_CODE"] = "1";
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "QM20";//修改板坯上的最终出钢记号
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
					bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "QMTS";
					bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
					bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];

					doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

			}
			else if (v_receive_back_status == "E")
			{
				//调用事件  只改标记 在反馈里集中处理
				bcls_rec->Tables["MM0099"].Rows.Add();
				bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm01);
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM3F";
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
				bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "cm_0rt801_rcv";
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm3e["MAT_NO"];

				if (v_event_id == "MM09" && v_event_type == "D")//收货撤销报错时，收货标记仍为S
				{
					bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = "S";
				}
				if (v_event_id == "MM19" && v_event_type == "N")//切废报错报错时，39表的完成标记置9
				{
					
					tmmsm39_1["MAT_NO"] = tmmsm01["MAT_NO"];
					tmmsm39_1["FINISH_FLAG"] = "1";// 1 新增已处理，未反馈   3 删除已处理，未反馈  9 处理成功   
				
					CString sql = "SELECT *  FROM ( SELECT t.* FROM TMMSM39_1 t WHERE t.MAT_NO = '" + tmmsm39_1["MAT_NO"].ToString() + "' AND FINISH_FLAG ='"+ tmmsm39_1["FINISH_FLAG"].ToString() +"'  ORDER BY t.RESUME_SEQ_NO DESC ) temp  WHERE ROWNUM = 1";
					cmd_inq.SetCommandText(sql);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						cmd_inq.Fetch(tmmsm39_1);
					}
					cmd_inq.Close();
					tmmsm39_1.Delete("RESUME_SEQ_NO");

					tmmsm39["RESUME_SEQ_NO"] = tmmsm39_1["RESUME_SEQ_NO"];
					tmmsm39["FINISH_FLAG"] = "E";// 1 新增已处理，未反馈   3 删除已处理，未反馈  9 处理成功   
					tmmsm39.Update("FINISH_FLAG", "RESUME_SEQ_NO");

					bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = v_receive_back_status;
				}
				else if (v_event_id == "MM13" && v_event_type == "N")
				{
					
					tmmsm34_1["MAT_NO"] = tmmsm01["MAT_NO"];
					
					CString sql = "SELECT *  FROM ( SELECT t.* FROM TMMSM34_1 t WHERE t.MAT_NO = '" + tmmsm34_1["MAT_NO"].ToString() + "' ORDER BY t.PROD_SEQ_NO DESC ) temp  WHERE ROWNUM = 1";
					cmd_inq.SetCommandText(sql);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						cmd_inq.Fetch(tmmsm34_1);
					}
					cmd_inq.Close();
					tmmsm34_1.Delete("MAT_NO,PROD_SEQ_NO");
					bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = v_receive_back_status;
				}
				else
				{
					bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = v_receive_back_status;
				}
				
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = tmmsm3e["REMARK"];

				doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else if (v_receive_back_status == "F")
			{
				//调用事件  只改标记 在反馈里集中处理
				bcls_rec->Tables["MM0099"].Rows.Add();
				bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm01);
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM3F";
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
				bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "cm_0rt801_rcv";
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm3e["MAT_NO"];
				bcls_rec->Tables["MM0099"].Rows[0]["RCV_MAT_FLAG"] = v_receive_back_status;
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = tmmsm3e["REMARK"];

				doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}


		}



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


